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
	int32_t nextCluster ();
	void addToDirectoryEntry (const FileName&, char* name);
	void setName (const FileName&, char* name);
	void setClusterEntry (const FileName&);
	void setSize (const FileName&);
	void setCDateCTime (const FileName&);
	void setMDateMTime (const FileName&);
	void setLADate (const FileName&);
	void setAttributes (const FileName&);
	void writeEntry ();
	void writeData (char* buffer, const uint32_t& bufferSize, int32_t& fileSize);
	void writeToFAT (const uint32_t& value, int32_t& clusterNumber);

	BiosParameterBlock bpb;
	DirectoryEntry dEntry;
	std::fstream partition;
	uint64_t dataOffset;
	uint64_t fatOffset;
	uint64_t copyOffset;

};

