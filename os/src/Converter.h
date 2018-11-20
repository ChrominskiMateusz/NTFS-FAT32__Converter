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
	void readMFT (const uint32_t& VCN);
	void getMFTChain ();
	void moveToMFTChain ();
	uint64_t getVCNOffset (const uint32_t& VCN);
	std::pair<uint64_t, uint64_t> decodeChain (uint8_t* chain, uint16_t& chainIndex);
	void readBigData ();
	void readINDX ();
	void readIndexRecords ();

	uint8_t MFTChain[20];

	std::fstream discImg;

	PartitionBootSector pbs;
	MFTHeader mftHeader;
	CommonHeaderPart commonHeader;
	NonResidentHeader nonResidentHeader;
	ResidentHeader residentHeader;
	FileName fileName;
	IndexRoot indexRoot;
	ObjectID objectID;
	StandartInformation standardInfo;

	IndexEntry indexEntry;
	IndexHeader indexHeader;
	
	static const uint16_t MFT_SIZE_B;

	static const uint8_t RESERVED_MFT;
	static const uint16_t END_MARKER;
};

