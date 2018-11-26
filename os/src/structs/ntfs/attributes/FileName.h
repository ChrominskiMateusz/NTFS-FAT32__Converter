#pragma once
#pragma pack(push, 1)
#include <stdint.h>

struct FileName		// 0x30
{
	uint32_t parentRecordNumberStart;
	uint16_t parentRecordNumberEnd;
	uint16_t parentSequenceNumber;
	uint64_t creationTime;
	uint64_t alteredTime;
	uint64_t modificationTime;
	uint64_t readTime;
	uint64_t allocatedFileSize;
	uint64_t realFileSize;
	uint32_t flags;				// Directory, hidden, ...
	uint32_t EAandReparse;
	uint8_t filenameLength;		// in characters
	uint8_t filenameNamespace;
	// name ...
};

#pragma pack(pop)