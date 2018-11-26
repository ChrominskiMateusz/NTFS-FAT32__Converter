#pragma once
#pragma pack(push, 1)
#include <stdint.h>

struct Attributes
{
	static const uint8_t StandardInformation = 0x10;
	static const uint8_t FileName = 0x30;
	static const uint8_t ObjectID = 0x40;
	static const uint8_t Data = 0x80;
	static const uint8_t IndexRoot = 0x90;
	static const uint8_t IndexAllocation = 0xA0;
	static const uint8_t BitMap = 0xB0;
};

#pragma pack(pop)