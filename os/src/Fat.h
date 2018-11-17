#pragma once
#pragma pack(1)
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include "structs/AllStucts.h"

class Fat
{
public:
	~Fat ();
	Fat (const std::string& imgName);
	void readPartitionBootSector ();
	void readMFT (const int& VCN);	



	std::fstream discImg;
	PartitionBootSector pbs;
	CommonHeaderPart chp;
	static const uint32_t MFT_SIZE_B;
};

