#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include "structs/AllStucts.h"

class FATWrite
{
public:
	FATWrite (const std::string& partitionName);
	~FATWrite ();
	void addToDirectoryEntry (const FileName&, char* name);
	void addToMap (const MFTHeader&);
	void readBPB ();
	void writeData (char* buffer, const uint32_t& bufferSize, int64_t& leftSize, const uint64_t& fileSize);
	void writeEntry (const uint32_t& depth);
	void setSize (const uint32_t& size);

	BiosParameterBlock bpb;
	std::map<uint32_t, uint32_t> entryClusters;
	std::map<uint32_t, uint32_t> offsetOfEntries;

private:
	template <class T>
	void write (T* buffer, const uint32_t& size);
	int32_t nextCluster ();
	int32_t searchCluster ();
	uint32_t calculateClusterNumber ();
	void clearCluster (const int32_t& cluusterNumber);
	void setAttributes (const FileName&);
	void setCDateCTime (const FileName&);
	void setClusterEntry (const FileName&);
	void setEntryPointer (const uint32_t& parentNumber);
	void setLADate (const FileName&);
	void setMDateMTime (const FileName&);
	void setName (const FileName&, char* name);
	void writeToFAT (const uint32_t& value, const int32_t& clusterNumber);

	DirectoryEntry dEntry;
	std::fstream partition;
	uint16_t bytesPerCluster;
	uint32_t copyOffset;
	uint32_t dataOffset;
	uint32_t fatOffset;
	uint32_t writeOffset;
};

template<class T>
void FATWrite::write (T* buffer, const uint32_t& size)
{
	partition.write (reinterpret_cast<const char*>(buffer), size);
}