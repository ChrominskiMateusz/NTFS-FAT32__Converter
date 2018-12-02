#pragma once
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "FATWrite.h"

class NTFS
{
public:
	NTFS (const std::string& partitionName, const std::string& fatPartition);
	~NTFS ();

	template<class T>
	void read (T& buffer, const uint32_t& size);
	void getMFTChain ();
	void readPartitionBootSector ();
	void readINDX (const uint32_t& dLvl);
	void moveToMFTChain (CommonHeaderPart&);
	uint64_t getVCNOffset (const uint32_t& VCN);
	void readNonResidentData (uint64_t& clustersAmount, int64_t& leftSize, const int64_t& fileSize);
	void printName (const FileName&, const char* name, const uint32_t& depth);
	void readMFT (const uint32_t& VCN, const uint32_t& dLvl);
	std::pair<uint64_t, uint64_t> decodeChain (uint8_t* chain, uint16_t& chainIndex);
	void readIndexRecord (int32_t& size, uint64_t& lastOffset, const uint32_t& dLvl);
	void readData (const uint32_t& dataLength, uint16_t& chainIndex, const CommonHeaderPart&, const ResidentHeader&, const uint64_t& fileSize);
	void calculateMFTRecordSize ();
	void readMFT (const uint32_t& VCN, MFTHeader&);
	bool readAttributeHeader (CommonHeaderPart&, NonResidentHeader&, ResidentHeader&);
	void readFileName (FileName&, const uint32_t& depth, MFTHeader&);
	void readIndexRoot (int32_t& size, IndexRoot&, const uint32_t& depth);
	void readIndexAllocation (int32_t& dataLength, uint16_t& chainIndex, const uint32_t& depth);
	
	uint16_t mftSizeB;
	uint16_t clusterSize;
	FATWrite *fat;
	uint8_t *MFTChain;
	std::fstream partition;
	PartitionBootSector bootSector;


	static const uint8_t RESERVED_MFT;
	static const uint32_t END_MARKER;
};

template<class T>
inline void NTFS::read (T& buffer, const uint32_t& size)
{
	partition.read (reinterpret_cast<char *>(buffer), size);
}