#pragma once
#pragma pack(1)
#include <stdint.h>

struct PartitionBootSector
{
	uint8_t jump[3];
	uint8_t oemID[8];

	uint16_t bytesPerSector;		// BPB
	uint8_t sectorsPerCluster;
	uint16_t reservedSectors;
	uint8_t alwaysZero_0x10[3];
	uint16_t notUsed_0x13;
	uint8_t mediaType;
	uint16_t alwaysZero_0x16;
	uint16_t sectorsPerTrack;
	uint16_t headCount;
	uint32_t hiddenSectors;
	uint32_t notUsed_0x20;

	uint32_t notUsed_0x24;		// Extended BPB
	uint64_t totalSectors;		// this * bytesPerSector == partiotionSize
	uint64_t clusterNumberMFT;
	uint64_t clusterNumberMFTMirr;
	uint32_t clustersPerFileRecordSegment;
	uint8_t clustersPerIndexBuffer;
	uint8_t notUsed_0x45[3];
	uint64_t volumeSerialNumber;
	uint32_t checksum;

	uint8_t bootstrapCode[426];
	uint16_t magicNumber;		// 0xAA55
};