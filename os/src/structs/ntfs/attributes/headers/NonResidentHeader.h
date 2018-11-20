#pragma once
#pragma pack(1)
#include <stdint.h>

struct NonResidentHeader
{
	uint64_t startingVCN;
	uint64_t lastVCN;
	uint16_t dataRunsOffset;
	uint16_t unitSizeCompression;
	uint32_t padding;
	uint64_t attributeAllocatedSize;
	uint64_t attributeRealSize;
	uint64_t streamDataSize;
	// File name in Unicode if nameLength != 0x00
};