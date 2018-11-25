#pragma once
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "structs/AllStucts.h"

class FAT
{
public:
	FAT (const std::string& partitionName);
	~FAT ();

	int32_t searchCluster ();


	std::fstream partition;
	uint64_t dataOffset;
	uint64_t fatOffset;
	uint64_t copyOffset;
};

