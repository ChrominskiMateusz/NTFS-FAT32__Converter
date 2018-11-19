#pragma once
#pragma pack(1)
#include <stdint.h>

struct PartitionTableEntry
{
	uint8_t bootable;

	uint8_t startHead;
	uint16_t startSectorCylinder;

	uint8_t partitionId;

	uint8_t endHead;
	uint16_t endSectorCylinder;
	
	uint32_t startLba;

	uint32_t length;
};