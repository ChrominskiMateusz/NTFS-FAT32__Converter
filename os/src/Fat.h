#pragma once
#pragma pack(1)
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>

class Fat
{
public:
	Fat (const std::string& imgName);
	~Fat ();
	std::fstream discImg;

	struct BiosParameterBlock
	{
		uint8_t jump[3];					//  7E00h offset
		uint8_t softName[8];
		uint16_t bytesPerSector;
		uint8_t sectorsPerCluster;
		uint16_t reservedSectors;
		uint8_t fatCopies;
		uint16_t rootDirEntries;
		uint16_t totalSectors;
		uint8_t mediaType;
		uint16_t fatSectorCount;
		uint16_t sectorsPerTrack;
		uint16_t headCount;
		uint32_t hiddenSectors;
		uint32_t totalSectorCount;

		uint32_t tableSize;
		uint16_t extFlags;
		uint16_t fatVersion;
		uint32_t rootCluster;
		uint16_t fatInfo;
		uint16_t backupSector;
		uint8_t reserved0[12];
		uint8_t driveNumber;
		uint8_t reserved;
		uint8_t bootSignature;
		uint32_t volumeId;
		uint8_t volumeLabel[11];
		uint8_t fatTypeLabel[8];

	};

	struct DirectoryEntry
	{
		uint8_t name[8];
		uint8_t ext[3];
		uint8_t attributes;
		uint8_t reserved;
		uint8_t cTimeTenth;
		uint16_t cTime;
		uint16_t cDate;
		uint16_t aTime;
		uint16_t firstClusterHi;
		uint16_t wTime;
		uint16_t wDate;
		uint16_t firstClusterLow;
		uint32_t size;

	};


	BiosParameterBlock bpb;
	void readBlock (const uint32_t& partitionOffset);
};

