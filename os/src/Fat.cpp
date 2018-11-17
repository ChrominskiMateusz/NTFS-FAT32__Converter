#include "Fat.h"

Fat::Fat (const std::string& imgName)
{
	discImg.open (imgName, std::ios::binary | std::ios::in | std::ios::app);
}


Fat::~Fat ()
{
	discImg.close ();
}

void Fat::readPartitionBootSector ()
{
	discImg.read (reinterpret_cast<char *>(&pbs), sizeof (PartitionBootSector));
	
	if (pbs.magicNumber != 0xAA55)
		return;
}

void Fat::readMFT (const int& VCN)
{
	int32_t offset = pbs.clusterNumberMFT * pbs.bytesPerSector * pbs.sectorsPerCluster;
	offset += VCN * MFT_SIZE_B;					// system files skip

	discImg.seekg (offset, discImg.beg);
	discImg.read (reinterpret_cast<char *>(&chp), sizeof (CommonHeaderPart));


}


const uint32_t Fat::MFT_SIZE_B = 1024;









//void Fat::readBlock (const uint32_t& partitionOffset)
//{
//	discImg.seekg (partitionOffset, discImg.beg);
//	discImg.read (reinterpret_cast<char *>(&bpb), sizeof (BiosParameterBlock));
//
//	uint32_t fatStart = partitionOffset + bpb.reservedSectors * 512;
//	uint32_t fatSize = bpb.tableSize * 512 * bpb.fatCopies;
//	
//	uint32_t dataStart = fatStart + fatSize;
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