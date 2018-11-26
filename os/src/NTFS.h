#pragma once
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "structs/AllStucts.h"
#include "FAT.h"

class NTFS
{
public:
	FAT f;
	NTFS (const std::string& partitionName);
	~NTFS ();

	void getMFTChain ();
	void readPartitionBootSector ();
	void readINDX (const uint32_t& dLvl);
	void moveToMFTChain (CommonHeaderPart&);
	uint64_t getVCNOffset (const uint32_t& VCN);
	void readNonResidentData (uint64_t& clustersAmount);
	void printName (const FileName&, const uint32_t& dLvl);
	void readMFT (const uint32_t& VCN, const uint32_t& dLvl);
	std::pair<uint64_t, uint64_t> decodeChain (uint8_t* chain, uint16_t& chainIndex);
	void readIndexRecord (int32_t& size, uint64_t& lastOffset, const uint32_t& dLvl);
	void readData (const uint32_t& dataLength, uint16_t& chainIndex, const CommonHeaderPart&, const ResidentHeader&);
	
	uint8_t *MFTChain;
	std::fstream partition;
	PartitionBootSector bootSector;

	static const uint16_t MFT_SIZE_B;
	static const uint8_t RESERVED_MFT;
	static const uint32_t END_MARKER;
};

