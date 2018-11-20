#include "Converter.h"

Converter::Converter (const std::string& imgName)
{
	discImg.open (imgName, std::ios::binary | std::ios::in | std::ios::app);
}


Converter::~Converter ()
{
	delete MFTChain;
	discImg.close ();
}

void Converter::readPartitionBootSector ()
{
	discImg.read (reinterpret_cast<char *>(&pbs), sizeof (PartitionBootSector));
	
	if (pbs.magicNumber != 0xAA55)
		return;
}

void Converter::readMFT (const uint32_t& VCN)
{
	uint32_t offset = getVCNOffset (VCN);

	discImg.seekg (offset, discImg.beg);
	discImg.read (reinterpret_cast<char *>(&mftHeader), sizeof (MFTHeader));

	discImg.seekg (offset + mftHeader.firstAttributeOffset, discImg.beg);
	do
	{
		discImg.read (reinterpret_cast<char *>(&commonHeader), sizeof (CommonHeaderPart));
		if (commonHeader.residentFlag == 0x01)
			discImg.read (reinterpret_cast<char *>(&nonResidentHeader), sizeof (NonResidentHeader));
		else
			discImg.read (reinterpret_cast<char *>(&residentHeader), sizeof (ResidentHeader));
		discImg.seekg (commonHeader.nameLength * 2, discImg.cur);

		switch (commonHeader.attributeType)
		{
		case Attributes::StandardInformation:

			break;
		}

	} while (commonHeader.attributeType != END_MARKER);
}

void Converter::getMFTChain ()
{
	moveToMFTChain ();
	uint32_t attributeSize = 0;
	
	if (commonHeader.residentFlag == 0x01)
		attributeSize += 0x30;
	else
		attributeSize += 0x08;

	if (commonHeader.nameLength != 0x00)
		attributeSize += 2 * commonHeader.nameLength;

	uint8_t sequenceSize = commonHeader.length - (attributeSize + sizeof (CommonHeaderPart));
	MFTChain = new uint8_t[sequenceSize];

	discImg.seekg (attributeSize, discImg.cur);
	discImg.read (reinterpret_cast<char *>(&MFTChain), sizeof MFTChain);
}

void Converter::moveToMFTChain ()
{
	uint32_t startOffset = pbs.clusterNumberMFT * pbs.bytesPerSector * pbs.sectorsPerCluster;
	uint8_t firstAttribute;
	uint32_t attributeSize;

	discImg.seekg (startOffset + 0x14, discImg.beg);			// read 0x10 attribute offset
	discImg.read (reinterpret_cast<char *>(&firstAttribute), sizeof uint8_t);

	startOffset += firstAttribute;
	discImg.seekg (startOffset + 0x04, discImg.beg);			// read 0x30 attribute offset
	discImg.read (reinterpret_cast<char *>(&attributeSize), sizeof uint32_t);

	startOffset += attributeSize;
	discImg.seekg (startOffset + 0x04, discImg.beg);			// read 0x80 attribute offset
	discImg.read (reinterpret_cast<char *>(&attributeSize), sizeof uint32_t);

	startOffset += attributeSize;
	discImg.seekg (startOffset, discImg.beg);					// read 0x80 header
	discImg.read (reinterpret_cast<char *>(&commonHeader), sizeof CommonHeaderPart);
}

uint64_t Converter::getVCNOffset (const uint32_t& VCN)
{
	uint32_t bytesPerCluster = pbs.bytesPerSector * pbs.sectorsPerCluster;
	uint16_t chainIndex{};
	uint32_t rangeVCN{};
	std::pair<uint64_t, uint64_t> sizeAndOffset;

	while (VCN > rangeVCN)
	{
		sizeAndOffset = decodeChain (MFTChain, chainIndex);
		rangeVCN += sizeAndOffset.first * (bytesPerCluster / MFT_SIZE_B);
	}

	rangeVCN -= sizeAndOffset.first * (bytesPerCluster / MFT_SIZE_B);
	return sizeAndOffset.second + (VCN - rangeVCN) * MFT_SIZE_B;
}

std::pair<uint64_t, uint64_t> Converter::decodeChain (uint8_t* chain, uint16_t& chainIndex)
{
	uint8_t sizeLength;
	uint8_t offsetLength;
	std::pair<uint64_t, uint64_t> sizeAndOffset{};

	sizeLength = chain[chainIndex] & 0x0F;
	offsetLength = (chain[chainIndex] >> 4) & 0x0F;

	chainIndex++;
	for (int i{}; i < sizeLength; i++)
		sizeAndOffset.first |= (chain[chainIndex + i] << (i * 8));
	
	chainIndex += sizeLength;
	for (int i{}; i < offsetLength; i++)
		sizeAndOffset.second |= (chain[chainIndex + i] << (i * 8));

	chainIndex += offsetLength;
	return sizeAndOffset;
}


const uint16_t Converter::MFT_SIZE_B = 0x400;
const uint8_t Converter::RESERVED_MFT = 0x23;
const uint16_t Converter::END_MARKER = 0xFFFF;







//void Converter::readBlock (const uint32_t& partitionOffset)
//{
//	discImg.seekg (partitionOffset, discImg.beg);
//	discImg.read (reinterpret_cast<char *>(&bpb), sizeof (BiosParameterBlock));
//
//	uint32_t ConverterStart = partitionOffset + bpb.reservedSectors * 512;
//	uint32_t ConverterSize = bpb.tableSize * 512 * bpb.ConverterCopies;
//	
//	uint32_t dataStart = ConverterStart + ConverterSize;
//	uint32_t rootStart = dataStart + 512 * bpb.sectorsPerCluster * (bpb.rootCluster - 2);
//
//	DirectoryEntry dirent[16];
//	discImg.seekg (rootStart, discImg.beg);
//	discImg.read (reinterpret_cast<char *>(dirent), 16 * sizeof (DirectoryEntry));
//
//	for (int i{}; i < 16; i++)
//	{
//		if (dirent[i].name[0] == 0x00)
//			break;
//
//		if ((dirent[i].attributes & 0x0F) == 0x0F)
//			continue;
//
//		char foo[] = "        \0";
//		for (int j{}; j < 8; j++)
//			foo[j] = dirent[i].name[j];
//		std::cout << foo << std::endl;
//
//		if ((dirent[i].attributes & 0x10) == 0x10)				// dir
//			continue;
//
//		if (dirent[i].size == 4975)
//		{
//			uint32_t fileCluster = (static_cast<uint32_t>(dirent[i].firstClusterHi)) << 16
//				| (static_cast<uint32_t>(dirent[i].firstClusterLow));
//
//			uint32_t fileSector = dataStart + bpb.sectorsPerCluster * (fileCluster - 2) * 512;
//
//			uint8_t buffer[512];
//
//			discImg.seekg (fileSector, discImg.beg);
//			discImg.read (reinterpret_cast<char *>(buffer), 512);
//
//			buffer[dirent[i].size] = '\0';
//			std::cout << buffer;
//		}
//	}
//}