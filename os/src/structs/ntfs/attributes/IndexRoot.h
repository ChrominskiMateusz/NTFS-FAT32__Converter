#pragma once
#pragma pack(push, 1)
#include <stdint.h>
#include "headers/IndexNodeHeader.h"

struct IndexRoot		// 0x90
{
	uint32_t attributeType;
	uint32_t collationRule;
	uint32_t bytesPerIndexRecord;
	uint8_t clustersPerIndexRecord;
	uint8_t align[3];
	IndexNodeHeader indexNodeHeader;
	//	Index Entries ...
};

#pragma pack(pop)