#pragma once
#pragma pack(1)
#include <stdint.h>

struct CommonHeaderPart
{
	uint32_t attributeType;
	uint32_t length;
	uint8_t residentFlag;
	uint8_t nameLength;
	uint16_t nameOffset;
	uint16_t flags;
	uint16_t attributeID;
};