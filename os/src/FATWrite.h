#pragma once
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "structs/AllStucts.h"

class FATWrite
{
public:
	FATWrite (const std::string& partitionName);
	~FATWrite ();

	template <class T>
	void write (T* buffer, const uint32_t& size);
	int32_t nextCluster ();
	int32_t searchCluster ();
	uint32_t calculateClusterNumber ();
	void addToDirectoryEntry (const FileName&, char* name);
	void addToMap (const MFTHeader&);
	void clearCluster (const int32_t& cluusterNumber);
	void readBPB ();
	void setAttributes (const FileName&);
	void setCDateCTime (const FileName&);
	void setClusterEntry (const FileName&);
	void setEntryPointer (const uint32_t& parentNumber);
	void setLADate (const FileName&);
	void setMDateMTime (const FileName&);
	void setName (const FileName&, char* name);
	void setSize (const uint32_t& size);
	void writeData (char* buffer, const uint32_t& bufferSize, int64_t& leftSize, const uint64_t& fileSize);
	void writeEntry (const uint32_t& depth);
	void writeToFAT (const uint32_t& value, const int32_t& clusterNumber);

	BiosParameterBlock bpb;
	DirectoryEntry dEntry;
	std::fstream partition;
	std::map<uint32_t, uint32_t> entryClusters;
	std::map<uint32_t, uint32_t> offsetOfEntries;
	uint16_t bytesPerCluster;
	uint32_t copyOffset;
	uint32_t dataOffset;
	uint32_t fatOffset;
};

template<class T>
inline void FATWrite::write (T* buffer, const uint32_t& size)
{
	partition.write (reinterpret_cast<const char*>(buffer), size);
}