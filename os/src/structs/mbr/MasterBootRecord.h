#pragma once
#pragma pack(1)
#include "PartitionTableEntry.h"

struct MasterBootRecord
{
	uint8_t bootLoader[440];
	uint32_t signature;
	uint16_t unused;

	PartitionTableEntry primaryPartition[4];		// 510B -> 
	uint16_t magicNumber;
};