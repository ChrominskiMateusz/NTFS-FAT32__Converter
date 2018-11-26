#pragma once
#pragma pack(push, 1)
#include <stdint.h>

struct IndexEntry
{
	uint32_t recordNumber;
	uint16_t recordNumberEnd;
	uint16_t sequenceNumber;
	uint16_t entryLength;
	uint16_t streamLength;
	uint16_t flags;
	uint16_t align;
};

#pragma pack(pop)