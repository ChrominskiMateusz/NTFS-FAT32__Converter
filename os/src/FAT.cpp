#include "FAT.h"

FAT::FAT (const std::string& partitionName)
{
	partition.open (partitionName, std::ios::binary | std::ios::out | std::ios::trunc);
}

FAT::~FAT ()
{
	partition.close ();
}

int32_t FAT::searchCluster ()				// returning first empty cluster number
{
	partition.seekg (fatOffset);
	uint32_t value;
	int32_t index = -1;
	do
	{
		partition.read (reinterpret_cast<char *>(&value), sizeof uint32_t);
		index++;
	} while (value != 0x00000000);

	return index;
}

void FAT::addToDirectoryEntry (const FileName& fName, char* name)
{
	if (fName.flags == 0x10000000)					// if dir
		for (int i{}; i < fName.filenameLength && i < 8; i++)		// copy name
			dEntry.name[i] = name[i * 2];
	else
	{
		for (int i{}; name[i] != '.' && i < 8; i++)			// copy name
			dEntry.name[i] = name[i * 2];
	
		for (int i{ fName.filenameLength - 3 }, j{}; i < fName.filenameLength; i++, j++)	// and ext
			dEntry.ext[j] = name[i * 2];
	}

	uint32_t clusterNbr = searchCluster ();						// find first cluster
	dEntry.firstClusterHi = static_cast<uint16_t>((clusterNbr & 0xFFFF0000) >> 16);
	dEntry.firstClusterLow = static_cast<uint16_t>(clusterNbr & 0x0000FFFF);

	dEntry.size = static_cast<uint32_t>(fName.realFileSize);


	// TO DO: TIME, ATTRIBUTES

}

