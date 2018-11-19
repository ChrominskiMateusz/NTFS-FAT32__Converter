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
	void readMFT (const int& VCN);	



	std::fstream discImg;
	PartitionBootSector pbs;
	CommonHeaderPart chp;

	static const uint16_t MFT_SIZE_B;
	static const uint8_t RESERVED_MFT;
	static const uint16_t END_MARKER;
};

