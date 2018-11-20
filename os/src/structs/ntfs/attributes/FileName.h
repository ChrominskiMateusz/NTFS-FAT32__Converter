#pragma once
#pragma pack(1)
#include <stdint.h>

struct FileName		// 0x30
{
	uint32_t parentRecordNumberStart;
	uint16_t parentRecordNumberEnd;
	uint16_t parentSequenceNumber;
	uint8_t creationTime[8];
	uint8_t alteredTime[8];
	uint8_t modificationTime[8];
	uint8_t readTime[8];
	uint64_t allocatedFileSize;
	uint64_t realFileSize;
	uint32_t flags;				// Directory, hidden, ...
	uint32_t EAandReparse;
	uint8_t filenameLength;		// in characters
	uint8_t filenameNamespace;
	// name ...
};