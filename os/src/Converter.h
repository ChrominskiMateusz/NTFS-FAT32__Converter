#pragma once
#pragma pack(1)
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include "structs/AllStucts.h"

class Converter
{
public:
	~Converter ();
	Converter (const std::string& imgName);
	void readPartitionBootSector ();
	void readMFT (const uint32_t& VCN, const uint32_t& dLvl);
	void getMFTChain ();
	void moveToMFTChain (CommonHeaderPart&);
	uint64_t getVCNOffset (const uint32_t& VCN);
	std::pair<uint64_t, uint64_t> decodeChain (uint8_t* chain, uint16_t& chainIndex);
	void readData (const uint32_t& dataLength, uint16_t& chainIndex, const CommonHeaderPart&, const ResidentHeader&);
	void readINDX (const uint32_t& dLvl);
	void readNonResidentData (uint64_t& clustersAmount);
	void readIndexRecord (int32_t& size, uint64_t& lastOffset, const uint32_t& dLvl);
	void printName (const FileName&, const uint32_t& dLvl);

	uint8_t *MFTChain;
	std::fstream discImg;
	PartitionBootSector bootSector;

	static const uint16_t MFT_SIZE_B;
	static const uint8_t RESERVED_MFT;
	static const uint32_t END_MARKER;
};
