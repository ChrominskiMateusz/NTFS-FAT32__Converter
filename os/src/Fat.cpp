#include "Fat.h"

Fat::Fat (const std::string& imgName)
{
	discImg.open (imgName, std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
}


Fat::~Fat ()
{

}

void Fat::readPartition ()
{
	discImg.read (reinterpret_cast<char *>(&mbr), sizeof (MasterBootRecord));
	
	if (mbr.magicNumber != 0xAA55)
		return;

	for (int i{}; i < 4; i++)
		if (mbr.primaryPartition[i].partitionId != 0x00)
			readBlock (mbr.primaryPartition[i].startLba);
}

void Fat::readBlock (const uint32_t& partitionOffset)
{
	std::cout << partitionOffset << std::endl;
	discImg.seekg (partitionOffset, discImg.beg);
	discImg.read (reinterpret_cast<char *>(&bpb), sizeof (BiosParameterBlock));

	uint32_t fatStart = partitionOffset + bpb.reservedSectors;
	uint32_t fatSize = bpb.tableSize;

	uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;

	uint32_t rootStart = dataStart + bpb.sectorsPerCluster * (bpb.rootCluster - 2);

	DirectoryEntry dirent[16];
	discImg.seekg (rootStart, discImg.beg);
	discImg.read (reinterpret_cast<char *>(dirent), 16 * sizeof (DirectoryEntry));

	for (int i{ 0 }; i < 16; i++)
	{
		if (dirent[i].name[0] == 0x00)
			break;

		std::cout << dirent[i].name << std::endl;
	}
}