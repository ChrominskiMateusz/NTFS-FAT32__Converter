#pragma once
#pragma pack(1)
#include <stdint.h>

struct ResidentHeader
{
	uint32_t attributeLength;
	uint16_t attributeOffset;
	uint8_t indexedFlag;
	uint8_t padding;
};