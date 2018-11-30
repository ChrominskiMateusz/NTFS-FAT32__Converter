#include "FATRead.h"

FATRead::FATRead (const std::string& partitionName)
{
	partition.open (partitionName, std::ios::binary | std::ios::in);
}


FATRead::~FATRead ()
{
	partition.close ();
}

void FATRead::readBPB ()
{
	partition.read (reinterpret_cast<char*>(&bpb), sizeof (BiosParameterBlock));

	fatOffset = bpb.reservedSectors * bpb.bytesPerSector;
	uint32_t fatSize = bpb.tableSize * bpb.bytesPerSector * bpb.fatCopies;

	dataOffset = fatOffset + fatSize;
}

void FATRead::fileRead (const uint32_t& startCluster, int32_t& fileSize)
{
	uint32_t nextCluster = startCluster;
	uint32_t bytesPerCluster = bpb.bytesPerSector * bpb.sectorsPerCluster;
	uint8_t *buffer = new uint8_t[bytesPerCluster + 1];

	while (fileSize > 0)
	{
		setPointer (nextCluster);
		partition.read (reinterpret_cast<char*>(buffer), bytesPerCluster);

		buffer[fileSize >= bytesPerCluster ? bytesPerCluster : fileSize] = '\0';
		std::cout << buffer;
		fileSize -= bytesPerCluster;

		nextCluster = getNextCluster (nextCluster);
	}

	delete[] buffer;
}

void FATRead::directoryRead (const uint32_t& startCluster, const uint32_t& depth)
{
	uint32_t nextCluster = startCluster;
	uint32_t bytesPerCluster = bpb.bytesPerSector * bpb.sectorsPerCluster;
	DirectoryEntry *dEntr = new DirectoryEntry[bytesPerCluster / sizeof DirectoryEntry + 1];

	while (nextCluster != 0x0FFFFFFF)
	{
		setPointer (nextCluster);
		partition.read (reinterpret_cast<char*>(dEntr), bytesPerCluster);

		for (uint32_t i{}; i < bytesPerCluster / sizeof DirectoryEntry; i++)
		{
			if (dEntr[i].name[0] == 0x00)
				break;
			if ((dEntr[i].attributes & 0x02) == 0x02 || (dEntr[i].attributes & 0x04) == 0x04 || 
				(dEntr[i].attributes & 0x0F) == 0x0F)
				continue;

			uint32_t fileCluster = (static_cast<uint32_t>(dEntr[i].firstClusterHi)) << 16
				| (static_cast<uint32_t>(dEntr[i].firstClusterLow));
			
			if (getNextCluster (fileCluster) == 0xFFFFFFFF || getNextCluster (fileCluster) == 0x0FFFFFF8)
				continue;

			if (dEntr[i].name[6] == '~')
				continue;

			if (dEntr[i].name[0] == 0x2E || dEntr[i].name[0] == 0xE5)
				continue;
			
			for (int k{}; k < depth; k++)
				std::cout << '-';

			char foo[9]{};
			for (uint32_t j{}; j < 8; j++)
				foo[j] = dEntr[i].name[j];
			std::cout << foo << std::endl;


			if ((dEntr[i].attributes & 0x10) == 0x10)
				directoryRead (fileCluster, depth + 1);
			else
			{
				int32_t size = dEntr[i].size;
				fileRead (fileCluster, size);
			}
			std::cout << std::endl;
		}

		nextCluster = getNextCluster (nextCluster);
		if (!nextCluster)
			break;
		if (nextCluster == 0xFFFFFFFF)
			break;
		if (nextCluster == 0xFFFFFF80)
			break;
	}

	delete[] dEntr;
}

uint32_t FATRead::getNextCluster (const uint32_t& prev)
{
	uint32_t next;
	partition.seekg (fatOffset + prev * sizeof uint32_t);
	partition.read (reinterpret_cast<char*>(&next), sizeof uint32_t);

	return next;
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
