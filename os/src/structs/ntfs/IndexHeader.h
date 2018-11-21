#pragma once
#pragma pack(1)
#include <stdint.h>

struct IndexHeader
{
	uint8_t magicNumber[4];			// INDX
	uint16_t sequenceOffset;
	uint16_t sequenceSize;			// in words
	uint64_t logFileSequenceNumber;
	uint64_t thisVCN;				// VCN of this INDX buffer in the Index Allocation
	uint32_t entriesOffset;			// from 0x18
	int32_t entriesSize;			// from 0x18
	uint32_t allocatedEntriesSize;	// from 0x18
	uint8_t notLeafFlag;			// 1 if has children
	uint8_t allign[3];
	uint16_t updateSequence;
};