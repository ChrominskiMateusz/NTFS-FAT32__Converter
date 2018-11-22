#include "Converter.h"

Converter::Converter (const std::string& imgName)
{
	discImg.open (imgName, std::ios::binary | std::ios::in);
}

Converter::~Converter ()
{
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
		uint32_t tmpOffset = discImg.tellg ();
		discImg.read (reinterpret_cast<char *>(&commonHeader), sizeof CommonHeaderPart);
		if (commonHeader.residentFlag == 0x01)
			discImg.read (reinterpret_cast<char *>(&nonResidentHeader), sizeof NonResidentHeader);
		else
			discImg.read (reinterpret_cast<char *>(&residentHeader), sizeof ResidentHeader);
		discImg.seekg (commonHeader.nameLength * 2, discImg.cur);

		int32_t dataLength = commonHeader.length - (static_cast<uint32_t>(discImg.tellg ()) - tmpOffset);
		uint16_t chainIndex{};

		switch (commonHeader.attributeType)
		{
		case Attributes::StandardInformation:
			discImg.read (reinterpret_cast<char *>(&standardInfo), sizeof StandartInformation);
			break;
		case Attributes::FileName:
			discImg.read (reinterpret_cast<char *>(&fileName), sizeof FileName);
			break;
		case Attributes::ObjectID:
			discImg.read (reinterpret_cast<char *>(&objectID), sizeof ObjectID);
			break;
		case Attributes::Data:

			break;
		case Attributes::IndexRoot:
			discImg.read (reinterpret_cast<char *>(&indexRoot), sizeof IndexRoot);
			dataLength -= sizeof IndexRoot;
			while (dataLength > 0)
			{
				readIndexRecord ();
				dataLength -= indexEntry.entryLength;
			}
			break;
		case Attributes::IndexAllocation:
			uint8_t chain[20];
			discImg.read (reinterpret_cast<char *>(chain), 8);
			while (chain[chainIndex] != 0x00)
			{
				std::pair<uint64_t, uint64_t> tmp = decodeChain (chain, chainIndex);
				discImg.seekg (tmp.second * 4096);
				readINDX ();
			}
			break;
		case Attributes::BitMap:			// not needed in school project, U can add it if U'd like to
			break;
		default:
			break;
		}

		discImg.seekg (tmpOffset + commonHeader.length);
	} while (commonHeader.attributeType != END_MARKER);
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
	return sizeAndOffset.second * bytesPerCluster + (VCN - rangeVCN) * MFT_SIZE_B;
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

void Converter::readBigData ()
{

}

void Converter::readINDX ()
{
	discImg.read (reinterpret_cast<char *>(&indexHeader), sizeof IndexHeader);
	discImg.seekg (indexHeader.entriesOffset - 0x12, discImg.cur);
	indexHeader.entriesSize -= indexHeader.entriesOffset;

	while (indexHeader.entriesSize > 0)
	{
		readIndexRecord ();
		indexHeader.entriesSize -= indexEntry.entryLength;
	}
}

void Converter::readIndexRecord ()
{
	discImg.read (reinterpret_cast<char *>(&indexEntry), sizeof IndexEntry);
	discImg.seekg (indexEntry.entryLength - sizeof (IndexEntry), discImg.cur);
	if(indexEntry.recordNumber != 0x0000)
		std::cout << indexEntry.recordNumber << std::endl;
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

	discImg.seekg (attributeSize, discImg.cur);
	discImg.read (reinterpret_cast<char *>(MFTChain), sequenceSize);
}

void Converter::moveToMFTChain ()
{
	uint32_t startOffset = pbs.clusterNumberMFT * pbs.bytesPerSector * pbs.sectorsPerCluster;
	uint32_t attributeSize;

	discImg.seekg (startOffset, discImg.beg);			// read 0x10 attribute offset
	discImg.read (reinterpret_cast<char *>(&mftHeader), sizeof MFTHeader);

	startOffset += mftHeader.firstAttributeOffset;
	discImg.seekg (startOffset + 0x04, discImg.beg);			// read 0x30 attribute offset
	discImg.read (reinterpret_cast<char *>(&attributeSize), sizeof uint32_t);

	startOffset += attributeSize;
	discImg.seekg (startOffset + 0x04, discImg.beg);			// read 0x80 attribute offset
	discImg.read (reinterpret_cast<char *>(&attributeSize), sizeof uint32_t);

	startOffset += attributeSize;
	discImg.seekg (startOffset, discImg.beg);					// read 0x80 header
	discImg.read (reinterpret_cast<char *>(&commonHeader), sizeof CommonHeaderPart);
}

const uint16_t Converter::MFT_SIZE_B = 0x400;
const uint8_t Converter::RESERVED_MFT = 0x23;
const uint32_t Converter::END_MARKER = 0xFFFFFFFF;







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