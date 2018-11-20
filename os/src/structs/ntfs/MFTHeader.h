#pragma once
#pragma pack(1)
#include <stdint.h>

struct MFTHeader
{
	uint8_t magicNumber[4];					// FILE
	uint16_t sequenceOffset;
	uint16_t sequenceSize;					// in words
	uint64_t logFileSequenceNumber;
	uint16_t sequenceNumber;
	uint16_t hardLinkCount;
	uint16_t firstAttributeOffset;
	uint16_t flags;
	uint32_t FILERealSize;
	uint32_t FILEAllocatedSize;
	uint64_t baseFILEReference;
	uint16_t nextAttributeID;
	uint16_t align;
	uint32_t MFTRecordNumber;
	uint16_t updateSequenceNumber;
};