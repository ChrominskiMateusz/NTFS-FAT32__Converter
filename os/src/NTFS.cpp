#include "NTFS.h"

NTFS::NTFS (const std::string& partitionName, const std::string& fatPartition)
{
	partition.open (partitionName, std::ios::binary | std::ios::in);
	fat = new FATWrite (fatPartition);
	fat->readBPB ();
	fat->entryClusters[0x05] = fat->bpb.rootCluster;
	fat->offsetOfEntries[0x05] = 0;
}

NTFS::~NTFS ()
{
	delete MFTChain;
	delete fat;
	partition.close ();
}

void NTFS::readPartitionBootSector ()
{
	partition.read (reinterpret_cast<char *>(&bootSector), sizeof (PartitionBootSector));
	if (bootSector.magicNumber != 0xAA55)
		return;

	clusterSize = bootSector.bytesPerSector * bootSector.sectorsPerCluster;
}

void NTFS::readMFT (const uint32_t& VCN, const uint32_t& depth)
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

	partition.seekg (offset, partition.beg);
	partition.read (reinterpret_cast<char *>(&mftH), sizeof MFTHeader);
	partition.seekg (offset + mftH.firstAttributeOffset, partition.beg);

	while (true)
	{
		uint32_t tmpOffset = partition.tellg ();
		partition.read (reinterpret_cast<char *>(&comH), sizeof CommonHeaderPart);
		if (comH.attributeType == END_MARKER)
			break;
		if (comH.residentFlag == 0x01)
			partition.read (reinterpret_cast<char *>(&nResH), sizeof NonResidentHeader);
		else
			partition.read (reinterpret_cast<char *>(&resH), sizeof ResidentHeader);
		partition.seekg (comH.nameLength * 2, partition.cur);

		int32_t dataLength = comH.length - (static_cast<uint32_t>(partition.tellg ()) - tmpOffset);
		uint16_t chainIndex{};

		switch (comH.attributeType)
		{
		case Attributes::StandardInformation:
			partition.read (reinterpret_cast<char *>(&sInf), sizeof StandartInformation);
			break;
		case Attributes::FileName:
		{
			partition.read (reinterpret_cast<char *>(&fName), sizeof FileName);
			char *n = new char[fName.filenameLength * 2];
			partition.read (n, fName.filenameLength * 2);
			printName (fName, n, depth);
			if (depth)
			{
				fat->addToDirectoryEntry (fName, n);
				if(fName.flags == 0x10000000)
					fat->addToMap (mftH);
				fat->writeEntry (fName.parentRecordNumberStart);
			}
			delete n;
		}
		break;
		case Attributes::ObjectID:
			partition.read (reinterpret_cast<char *>(&oID), sizeof ObjectID);
			break;
		case Attributes::Data:
			readData (dataLength, chainIndex, comH, resH, fName.realFileSize);
			break;
		case Attributes::IndexRoot:
		{
			partition.read (reinterpret_cast<char *>(&iRoot), sizeof IndexRoot);
			dataLength -= sizeof IndexRoot;
			while (dataLength > 0)
			{
				uint64_t k = 0;
				readIndexRecord (dataLength, k, depth);
			}
		}
		break;
		case Attributes::IndexAllocation:
		{
			uint8_t *chain = new uint8_t[dataLength + 1];
			chain[dataLength] = 0x00;
			partition.read (reinterpret_cast<char *>(chain), dataLength);
			while (chain[chainIndex] != 0x00)
			{
				std::pair<uint64_t, uint64_t> tmp = decodeChain (chain, chainIndex);
				partition.seekg (tmp.second * 4096);
				readINDX (depth);
			}
			delete chain;
		}
		break;
		}

		partition.seekg (tmpOffset + comH.length);
	}
}

uint64_t NTFS::getVCNOffset (const uint32_t& VCN)
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

std::pair<uint64_t, uint64_t> NTFS::decodeChain (uint8_t* chain, uint16_t& chainIndex)
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

void NTFS::readData (const uint32_t& dataLength, uint16_t& chainIndex, const CommonHeaderPart& comH, const ResidentHeader& resH, const uint64_t& fileSize)
{
	int64_t fSize = fileSize;
	uint8_t *p;
	if (!comH.residentFlag)
	{
		p = new uint8_t[resH.attributeLength];
		partition.read (reinterpret_cast<char *>(p), resH.attributeLength);
		fat->writeData (reinterpret_cast<char *>(p), resH.attributeLength, fSize, fileSize);
	}
	else
	{
		p = new uint8_t[dataLength];
		partition.read (reinterpret_cast<char *>(p), dataLength);
		while (p[chainIndex] != 0x00)
		{
			std::pair<uint64_t, uint64_t> tmp = decodeChain (p, chainIndex);
			partition.seekg (tmp.second * clusterSize);
			readNonResidentData (tmp.first, fSize, fileSize);
		}
	}
	delete p;
}

void NTFS::readINDX (const uint32_t& depth)
{
	IndexHeader iHead;
	partition.read (reinterpret_cast<char *>(&iHead), sizeof IndexHeader);
	partition.seekg (iHead.entriesOffset - 0x12, partition.cur);
	iHead.entriesSize -= iHead.entriesOffset;
	int32_t size = iHead.entriesSize;
	while (size > 0)
	{
		uint64_t tmp = partition.tellg ();
		readIndexRecord (size, tmp, depth);
		partition.seekg (tmp);
	}
}

void NTFS::readNonResidentData (uint64_t& clustersAmount, int64_t& leftSize, const int64_t& fileSize)
{
	char *t = new char[clusterSize];
	while (clustersAmount-- > 0)
	{
		partition.read (t, clusterSize);
		fat->writeData (t, clusterSize, leftSize, fileSize);
	}
	delete t;
}

void NTFS::readIndexRecord (int32_t& size, uint64_t& lastOffset, const uint32_t& depth)
{
	IndexEntry iEntry;
	int32_t tP = partition.tellg ();
	partition.read (reinterpret_cast<char *>(&iEntry), sizeof IndexEntry);
	tP += iEntry.entryLength;
	lastOffset += iEntry.entryLength;
	size -= iEntry.entryLength;
	if (iEntry.recordNumber > 0x23)
		readMFT (iEntry.recordNumber, depth + 1);
	partition.seekg (tP);
}

void NTFS::printName (const FileName& fName, const char* name, const uint32_t& depth)
{
	uint8_t *fn = new uint8_t[fName.filenameLength + 1];
	for (int i = 0, j = 0; i < fName.filenameLength * 2; i += 2, j++)
		fn[j] = name[i];
	fn[fName.filenameLength] = '\0';
	for (int i{}; i < depth; i++)
		std::cout << "-";
	if (fName.flags == 0x10000000)
		std::cout << "Dir:  ";
	std::cout << fn << std::endl;
	delete fn;
}

void NTFS::getMFTChain ()
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
	MFTChain = new uint8_t[sequenceSize + 1];
	partition.seekg (attributeSize, partition.cur);
	partition.read (reinterpret_cast<char *>(MFTChain), sequenceSize);
	MFTChain[sequenceSize] = 0x00;
}

void NTFS::moveToMFTChain (CommonHeaderPart& comH)
{
	MFTHeader mftH;
	uint32_t startOffset = bootSector.clusterNumberMFT * bootSector.bytesPerSector * bootSector.sectorsPerCluster;
	uint32_t attributeSize;

	partition.seekg (startOffset, partition.beg);			// read 0x10 attribute offset
	partition.read (reinterpret_cast<char *>(&mftH), sizeof MFTHeader);

	startOffset += mftH.firstAttributeOffset;
	partition.seekg (startOffset + 0x04, partition.beg);			// read 0x30 attribute offset
	partition.read (reinterpret_cast<char *>(&attributeSize), sizeof uint32_t);

	startOffset += attributeSize;
	partition.seekg (startOffset + 0x04, partition.beg);			// read 0x80 attribute offset
	partition.read (reinterpret_cast<char *>(&attributeSize), sizeof uint32_t);

	startOffset += attributeSize;
	partition.seekg (startOffset, partition.beg);					// read 0x80 header
	partition.read (reinterpret_cast<char *>(&comH), sizeof CommonHeaderPart);
}

const uint16_t NTFS::MFT_SIZE_B = 0x400;
const uint8_t NTFS::RESERVED_MFT = 0x23;
const uint32_t NTFS::END_MARKER = 0xFFFFFFFF;