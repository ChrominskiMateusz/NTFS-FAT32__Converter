#pragma once
#include "structs/AllStucts.h"
#include <fstream>
#include <iostream>

class FATRead
{
public:
	FATRead (const std::string& partitionName);
	~FATRead ();

	void readBPB ();
	void fileRead (const uint32_t& startCluster, int32_t& fileSize);
	void directoryRead (const uint32_t& startCluster, const uint32_t& depth);
	void setPointer (const uint32_t& cluster);
	uint32_t getNextCluster (const uint32_t& prev);
	void printAll ();

	BiosParameterBlock bpb;
	std::fstream partition;
	uint32_t dataOffset;
	uint32_t fatOffset;
};

