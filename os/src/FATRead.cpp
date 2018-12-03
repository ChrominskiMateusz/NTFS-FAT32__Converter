#include "FATRead.h"

FATRead::FATRead (const std::string& partitionName)
{
	partition.open (partitionName, std::ios::binary | std::ios::in);
	readBPB ();
}

FATRead::~FATRead ()
{
	partition.close ();
}

void FATRead::readBPB ()
{
	read (&bpb, sizeof BiosParameterBlock);

	fatOffset = bpb.reservedSectors * bpb.bytesPerSector;
	uint32_t fatSize = bpb.tableSize * bpb.bytesPerSector * bpb.fatCopies;

	dataOffset = fatOffset + fatSize;
	bytesPerCluster = bpb.sectorsPerCluster * bpb.bytesPerSector;
}

void FATRead::fileRead (const uint32_t& startCluster, int32_t& fileSize)
{
	uint32_t nextCluster = startCluster;
	uint8_t *buffer = new uint8_t[bytesPerCluster + 1];

	while (fileSize > 0)
	{
		setPointer (nextCluster);
		read (buffer, bytesPerCluster);

		buffer[fileSize >= bytesPerCluster ? bytesPerCluster : fileSize] = '\0';
		if(doPrint)
			std::cout << buffer;
		fileSize -= bytesPerCluster;

		nextCluster = getNextCluster (nextCluster);
	}
	std::cout << std::endl;
	delete[] buffer;
}

void FATRead::directoryRead (const uint32_t& startCluster, const uint32_t& depth)
{
	uint32_t nextCluster = startCluster;
	DirectoryEntry *dEntr = new DirectoryEntry[bytesPerCluster / sizeof DirectoryEntry + 1];

	do
	{
		setPointer (nextCluster);
		read (dEntr, bytesPerCluster);
		iterateDirectory (dEntr, bytesPerCluster, depth);
	} while (isLastCluster (nextCluster));

	delete[] dEntr;
}

uint32_t FATRead::getNextCluster (const uint32_t& prev)
{
	uint32_t next;
	partition.seekg (fatOffset + prev * sizeof uint32_t);
	read (&next, sizeof uint32_t);

	return next;
}

bool FATRead::isLastCluster (uint32_t& cluster)
{
	cluster = getNextCluster (cluster);
	if (!cluster || cluster == 0xFFFFFFFF || cluster == 0xFFFFFF80 || cluster != 0x0FFFFFFF)
		return false;
	return true;
}

bool FATRead::skipIter (const uint32_t& fileCluster, const DirectoryEntry& dEntry)
{
	if ((dEntry.attributes & 0x02) == 0x02 || (dEntry.attributes & 0x04) == 0x04 ||
		(dEntry.attributes & 0x0F) == 0x0F)
		return true;

	if (getNextCluster (fileCluster) == 0xFFFFFFFF)
		return true;
	
	if (dEntry.name[0] == 0x2E || dEntry.name[0] == 0xE5)
		return true;

	if (dEntry.name[6] == '~')
		return true;

	return false;
}

uint32_t FATRead::calculateClusterNumber (const DirectoryEntry& dEntry)
{
	return (static_cast<uint32_t>(dEntry.firstClusterHi)) << 16 | (static_cast<uint32_t>(dEntry.firstClusterLow));
}

void FATRead::printName (const DirectoryEntry& dEntry, const uint32_t& depth)
{
	for (int k{}; k < depth; k++)
		std::cout << '-';

	std::cout << 1980 + ((dEntry.wDate & 0xFE00) >> 9) << '.';
	std::cout << ((dEntry.wDate & 0x01E0) >> 5) << '.';
	std::cout << ((dEntry.wDate & 0x001F)) << "  ";

	std::cout << ((dEntry.wTime & 0xF800) >> 11) << ':';
	std::cout << ((dEntry.wTime & 0x07E0) >> 5) << ':';
	std::cout << ((dEntry.wTime & 0x001F)) * 2 << "  ";

	char foo[9]{};
	for (uint32_t j{}; j < 8; j++)
		foo[j] = dEntry.name[j];
	std::cout << foo;
	if ((dEntry.attributes & 0x10) == 0x10)
		std::cout << "  <DIR>";
	std::cout << std::endl;
}

void FATRead::iterateDirectory (DirectoryEntry* dEntr, const uint32_t& bytesPerCluster, const uint32_t& depth)
{
	for (uint32_t i{}; i < bytesPerCluster / sizeof DirectoryEntry; i++)
	{
		if (dEntr[i].name[0] == 0x00)
			break;

		uint32_t fileCluster = calculateClusterNumber (dEntr[i]);
		int32_t size = dEntr[i].size;
		
		if (skipIter (fileCluster, dEntr[i]))
			continue;

		printName (dEntr[i], depth);
		if ((dEntr[i].attributes & 0x10) == 0x10)			// is directory
			directoryRead (fileCluster, depth + 1);
		else
			fileRead (fileCluster, size);
	}
}

void FATRead::setPointer (const uint32_t& cluster)
{
	uint32_t clusterOffset = dataOffset + bpb.bytesPerSector * bpb.sectorsPerCluster * (cluster - 2);
	partition.seekg (clusterOffset);
}

void FATRead::printAll ()
{
	directoryRead (bpb.rootCluster, 0);
}
