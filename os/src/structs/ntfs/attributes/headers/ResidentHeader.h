#pragma once
#pragma pack(push, 1)
#include <stdint.h>

struct ResidentHeader
{
	uint32_t attributeLength;
	uint16_t attributeOffset;
	uint8_t indexedFlag;
	uint8_t padding;
	// File name in Unicode if nameLength != 0x00
};

#pragma pack(pop)