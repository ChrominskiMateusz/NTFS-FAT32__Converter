#pragma once
#pragma pack(push, 1)
#include <stdint.h>

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

#pragma pack(pop)