#pragma once
#pragma pack(1)
#include <stdint.h>

struct ObjectID		// 0x40  ==  GUIDs
{
	uint8_t objectID[16];
	uint8_t birthVolumeID[16];
	uint8_t birthObjectID[16];
	uint8_t domainID[16];
};