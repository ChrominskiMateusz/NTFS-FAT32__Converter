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
	void printAll ();
	bool isLastCluster (uint32_t& cluster);
	void setPointer (const uint32_t& cluster);
	uint32_t getNextCluster (const uint32_t& prev);
	uint32_t calculateClusterNumber (const DirectoryEntry&);
	void printName (const DirectoryEntry&, const uint32_t& depth);
	void fileRead (const uint32_t& startCluster, int32_t& fileSize);
	bool skipIter (const uint32_t& fileCluster, const DirectoryEntry&);
	void directoryRead (const uint32_t& startCluster, const uint32_t& depth);
	void iterateDirectory (DirectoryEntry*, const uint32_t& bytesPerCluster, const uint32_t& depth);

	uint32_t bytesPerCluster;
	BiosParameterBlock bpb;
	std::fstream partition;
	uint32_t dataOffset;
	uint32_t fatOffset;
};

