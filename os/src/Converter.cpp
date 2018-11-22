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
	discImg.read (reinterpret_cast<char *>(&bootSector), sizeof (PartitionBootSector));
	
	if (bootSector.magicNumber != 0xAA55)
		return;
}

void Converter::readMFT (const uint32_t& VCN)
{
	MFTHeader mftH;
	CommonHeaderPart comH;
	NonResidentHeader nResH;
	ResidentHeader resH;
	StandartInformation sInf;
	FileName fName;
	ObjectID oID;
	IndexRoot iRoot;

	uint32_t offset = getVCNOffset (VCN);

	discImg.seekg (offset, discImg.beg);
	discImg.read (reinterpret_cast<char *>(&mftH), sizeof MFTHeader);
	discImg.seekg (offset + mftH.firstAttributeOffset, discImg.beg);

	while (true)
	{
		uint32_t tmpOffset = discImg.tellg ();
		discImg.read (reinterpret_cast<char *>(&comH), sizeof CommonHeaderPart);
		if (comH.attributeType == END_MARKER)
			break;
		if (comH.residentFlag == 0x01)
			discImg.read (reinterpret_cast<char *>(&nResH), sizeof NonResidentHeader);
		else
			discImg.read (reinterpret_cast<char *>(&resH), sizeof ResidentHeader);
		discImg.seekg (comH.nameLength * 2, discImg.cur);

		int32_t dataLength = comH.length - (static_cast<uint32_t>(discImg.tellg ()) - tmpOffset);
		uint16_t chainIndex{};

		switch (comH.attributeType)
		{
		case Attributes::StandardInformation:
			discImg.read (reinterpret_cast<char *>(&sInf), sizeof StandartInformation);
			break;
		case Attributes::FileName:
			discImg.read (reinterpret_cast<char *>(&fName), sizeof FileName);
			break;
		case Attributes::ObjectID:
			discImg.read (reinterpret_cast<char *>(&oID), sizeof ObjectID);
			break;
		case Attributes::Data:
			readData (dataLength, chainIndex, comH, resH);
			break;
		case Attributes::IndexRoot:
			discImg.read (reinterpret_cast<char *>(&iRoot), sizeof IndexRoot);
			dataLength -= sizeof IndexRoot;
			while (dataLength > 0)
			{
				uint64_t k = 0;
				readIndexRecord (dataLength, k);
			}
			break;
		case Attributes::IndexAllocation:
			uint8_t *chain = new uint8_t[dataLength + 1];
			chain[dataLength] = 0x00;
			discImg.read (reinterpret_cast<char *>(chain), dataLength);
			while (chain[chainIndex] != 0x00)
			{
				std::pair<uint64_t, uint64_t> tmp = decodeChain (chain, chainIndex);
				discImg.seekg (tmp.second * 4096);
				readINDX ();
			}
			delete chain;
			break;
		}

		discImg.seekg (tmpOffset + comH.length);
	}
}

uint64_t Converter::getVCNOffset (const uint32_t& VCN)
{
	uint32_t bytesPerCluster = bootSector.bytesPerSector * bootSector.sectorsPerCluster;
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

void Converter::readData (const uint32_t& dataLength, uint16_t& chainIndex, const CommonHeaderPart& comH, const ResidentHeader& resH)
{
	uint8_t *p;
	if (!comH.residentFlag)
	{
		p = new uint8_t[resH.attributeLength + 1];
		discImg.read (reinterpret_cast<char *>(p), resH.attributeLength);
		p[resH.attributeLength] = '\0';
		std::cout << p;
	}
	else
	{
		p = new uint8_t[dataLength];
		discImg.read (reinterpret_cast<char *>(p), dataLength);
		while (p[chainIndex] != 0x00)
		{
			std::pair<uint64_t, uint64_t> tmp = decodeChain (p, chainIndex);
			discImg.seekg (tmp.second * 4096);
			readNonResidentData (tmp.first);
		}
	}
	delete p;
}

void Converter::readINDX ()
{
	IndexHeader iHead;
	discImg.read (reinterpret_cast<char *>(&iHead), sizeof IndexHeader);
	discImg.seekg (iHead.entriesOffset - 0x12, discImg.cur);
	iHead.entriesSize -= iHead.entriesOffset;
	int32_t size = iHead.entriesSize;
	while (size > 0)
	{
		uint64_t tmp = discImg.tellg ();
		readIndexRecord (size, tmp);
		discImg.seekg (tmp);
	}
}

void Converter::readNonResidentData (uint64_t& clustersAmount)
{
	uint16_t clusterSize = bootSector.bytesPerSector * bootSector.sectorsPerCluster;
	uint8_t *t = new uint8_t[clusterSize + 1];
	while (clustersAmount-- > 0)
	{
		discImg.read (reinterpret_cast<char *>(t), clusterSize);
		t[clusterSize] = '\0';
		std::cout << "Big file right here" << std::endl;
	}
	delete t;
}

void Converter::readIndexRecord (int32_t& size, uint64_t& lastOffset)
{
	IndexEntry iEntry;
	int tP = static_cast<uint64_t>(discImg.tellg ());
	discImg.read (reinterpret_cast<char *>(&iEntry), sizeof IndexEntry);
	tP += iEntry.entryLength;
	lastOffset += iEntry.entryLength;
	size -= iEntry.entryLength;
	discImg.seekg (iEntry.entryLength - sizeof IndexEntry, discImg.cur);
	if (iEntry.recordNumber > 0x23)
	{
		std::cout << iEntry.recordNumber << std::endl;
		if (iEntry.recordNumber == 62)
		{
			std::cout << "";
		}
		readMFT (iEntry.recordNumber);
	}
	discImg.seekg (tP);
}

void Converter::getMFTChain ()
{
	CommonHeaderPart comH;
	moveToMFTChain (comH);
	uint32_t attributeSize = 0;
	
	if (comH.residentFlag == 0x01)
		attributeSize += 0x30;
	else
		attributeSize += 0x08;

	if (comH.nameLength != 0x00)
		attributeSize += 2 * comH.nameLength;

	uint8_t sequenceSize = comH.length - (attributeSize + sizeof CommonHeaderPart);

	discImg.seekg (attributeSize, discImg.cur);
	discImg.read (reinterpret_cast<char *>(MFTChain), sequenceSize);
}

void Converter::moveToMFTChain (CommonHeaderPart& comH)
{
	MFTHeader mftH;
	uint32_t startOffset = bootSector.clusterNumberMFT * bootSector.bytesPerSector * bootSector.sectorsPerCluster;
	uint32_t attributeSize;

	discImg.seekg (startOffset, discImg.beg);			// read 0x10 attribute offset
	discImg.read (reinterpret_cast<char *>(&mftH), sizeof MFTHeader);

	startOffset += mftH.firstAttributeOffset;
	discImg.seekg (startOffset + 0x04, discImg.beg);			// read 0x30 attribute offset
	discImg.read (reinterpret_cast<char *>(&attributeSize), sizeof uint32_t);

	startOffset += attributeSize;
	discImg.seekg (startOffset + 0x04, discImg.beg);			// read 0x80 attribute offset
	discImg.read (reinterpret_cast<char *>(&attributeSize), sizeof uint32_t);

	startOffset += attributeSize;
	discImg.seekg (startOffset, discImg.beg);					// read 0x80 header
	discImg.read (reinterpret_cast<char *>(&comH), sizeof CommonHeaderPart);
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