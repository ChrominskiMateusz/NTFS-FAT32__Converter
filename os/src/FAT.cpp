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
	} while (value != 0x00);

	return index;
}

