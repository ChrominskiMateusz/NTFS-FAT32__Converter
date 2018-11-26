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
	FAT () = default;
	~FAT ();

	int32_t searchCluster ();
	void addToDirectoryEntry (const FileName&, char* name);


	DirectoryEntry dEntry{};
	std::fstream partition;
	uint64_t dataOffset;
	uint64_t fatOffset;
	uint64_t copyOffset;

};

