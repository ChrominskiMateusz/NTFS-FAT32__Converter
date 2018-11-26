#pragma once
#pragma pack(push, 1)
#include <stdint.h>

struct IndexNodeHeader		// 
{
	uint32_t firstIndexOffset;	// Index == IndexEntry
	uint32_t totalIndexEntriesSize;
	uint32_t allocatedNodeSize;
	uint8_t nonLeafFlag;
	uint8_t allign[3];
};

#pragma pack(pop)