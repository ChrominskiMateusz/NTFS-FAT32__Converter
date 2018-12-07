#pragma once
#include <iostream>
#include <fstream>
#include "FATWrite.h"

class NTFS
{
public:
	NTFS (const std::string& partitionName, const std::string& fatPartition);
	~NTFS ();
	void readMFT (const uint32_t& VCN, const uint32_t& dLvl);

private:
	template<class T>
	void read (T* buffer, const uint32_t& size);
	bool readAttributeHeader (CommonHeaderPart&, NonResidentHeader&, ResidentHeader&);
	std::pair<uint64_t, uint64_t> decodeChain (uint8_t* chain, uint16_t& chainIndex);
	uint64_t getVCNOffset (const uint32_t& VCN);
	void calculateMFTRecordSize ();
	void getMFTChain ();
	void moveToMFTChain (CommonHeaderPart&);
	void printName (const FileName&, const char* name, const uint32_t& depth);
	void readData (const uint32_t& dataLength, uint16_t& chainIndex, const CommonHeaderPart&, const ResidentHeader&, const uint64_t& fileSize);
	void readFileName (FileName&, const uint32_t& depth, MFTHeader&);
	void readIndexAllocation (int32_t& dataLength, uint16_t& chainIndex, const uint32_t& depth);
	void readIndexRecord (int32_t& size, uint64_t& lastOffset, const uint32_t& dLvl);
	void readIndexRoot (int32_t& size, IndexRoot&, const uint32_t& depth);
	void readINDX (const uint32_t& dLvl);
	void readMFTHeader (const uint32_t& VCN, MFTHeader&);
	void readNonResidentData (uint64_t& clustersAmount, int64_t& leftSize, const int64_t& fileSize);
	void readPartitionBootSector ();

	FATWrite *fat;
	PartitionBootSector bootSector;
	std::fstream partition;
	uint16_t clusterSize;
	uint16_t mftSizeB;
	uint8_t *MFTChain;

	static const uint8_t RESERVED_MFT;
	static const uint32_t END_MARKER;
};

template<class T>
void NTFS::read (T* buffer, const uint32_t& size)
{
	partition.read (reinterpret_cast<char *>(buffer), size);
}