#pragma once
#pragma pack(1)
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include "structs/BiosParameterBlock.h"
#include "structs/DirectoryEntry.h"
#include "structs/MasterBootRecord.h"

class Fat
{
public:
	~Fat ();
	void readPartition ();
	Fat (const std::string& imgName);
	void readBlock (const uint32_t& partitionOffset);
	
	std::fstream discImg;
	MasterBootRecord mbr;
	BiosParameterBlock bpb;
};

