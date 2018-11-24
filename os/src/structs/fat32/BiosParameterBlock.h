#pragma once
#pragma pack(1)
#include <stdint.h>

struct BiosParameterBlock
{
	uint8_t jump[3];				// common in FATs
	uint8_t softName[8];

	uint16_t bytesPerSector;
	uint8_t sectorsPerCluster;
	uint16_t reservedSectors;
	uint8_t fatCopies;
	uint16_t rootDirEntries;
	uint16_t totalSectors;
	uint8_t mediaType;				// is it hard drive or so
	uint16_t fatSectorCount;
	uint16_t sectorsPerTrack;
	uint16_t headCount;
	uint32_t hiddenSectors;
	uint32_t totalSectorCount;

	uint32_t tableSize;				// FAT size				// FAT 32 part
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