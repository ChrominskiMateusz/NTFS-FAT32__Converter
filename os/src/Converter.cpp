#include "Converter.h"

Converter::Converter (const std::string& imgName)
{
	discImg.open (imgName, std::ios::binary | std::ios::in | std::ios::app);
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

void Converter::readMFT (const int& VCN)
{
	int32_t offset = pbs.clusterNumberMFT * pbs.bytesPerSector * pbs.sectorsPerCluster;			// MFT start
	offset += VCN * MFT_SIZE_B;

	discImg.seekg (offset, discImg.beg);
	discImg.read (reinterpret_cast<char *>(&chp), sizeof (CommonHeaderPart));


}


const uint16_t Converter::MFT_SIZE_B = 0x400;
const uint8_t Converter::RESERVED_MFT = 0x23;
const uint8_t Converter::END_MARKER = 0xFF;







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