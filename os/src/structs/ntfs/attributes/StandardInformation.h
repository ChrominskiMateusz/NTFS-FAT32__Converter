#pragma once
#pragma pack(push, 1)
#include <stdint.h>

struct StandartInformation		// 0x10
{
	uint64_t creationTime;
	uint64_t alteredTime;
	uint64_t modificationTime;
	uint64_t readTime;
	uint32_t filePermissions;
	uint32_t maximumVersionsNumber;
	uint32_t versionNumber;
	uint32_t classID;
	uint32_t ownerID;
	uint32_t securityID;
	uint64_t quotaCharged;
	uint64_t updateSequenceNumber;
};

#pragma pack(pop)