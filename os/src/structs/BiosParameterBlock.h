#pragma once
#pragma pack(1)
#include <stdint.h>

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