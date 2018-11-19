#pragma once
#pragma pack(1)
#include <stdint.h>

struct StandartInformation		// 0x10
{
	uint8_t creationTime[8];
	uint8_t alteredTime[8];
	uint8_t modificationTime[8];
	uint8_t readTime[8];
	uint32_t filePermissions;
	uint32_t maximumVersionsNumber;
	uint32_t versionNumber;
	uint32_t classID;
	uint32_t ownerID;
	uint32_t securityID;
	uint64_t quotaCharged;
	uint64_t updateSequenceNumber;
};