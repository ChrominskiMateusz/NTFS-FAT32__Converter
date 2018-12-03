#pragma once
#include "structs/AllStucts.h"
#include <fstream>
#include <iostream>

class FATRead
{
public:
	FATRead (const std::string& partitionName);
	~FATRead ();

	template<class T>
	void read (T* buffer, const uint32_t& size);
	bool isLastCluster (uint32_t& cluster);
	bool skipIter (const uint32_t& fileCluster, const DirectoryEntry&);
	uint32_t calculateClusterNumber (const DirectoryEntry&);
	uint32_t getNextCluster (const uint32_t& prev);
	void directoryRead (const uint32_t& startCluster, const uint32_t& depth);
	void fileRead (const uint32_t& startCluster, int32_t& fileSize);
	void iterateDirectory (DirectoryEntry*, const uint32_t& bytesPerCluster, const uint32_t& depth);
	void printAll ();
	void printName (const DirectoryEntry&, const uint32_t& depth);
	void readBPB ();
	void setPointer (const uint32_t& cluster);

	bool doPrint;
	BiosParameterBlock bpb;
	std::fstream partition;
	uint32_t bytesPerCluster;
	uint32_t dataOffset;
	uint32_t fatOffset;
};

template<class T>
inline void FATRead::read (T* buffer, const uint32_t& size)
{
	partition.read (reinterpret_cast<char *>(buffer), size);
}