#pragma once
#include "NTFS.h"
#include "FAT.h"

class Converter
{
public:
	~Converter ();
	Converter (const std::string& ntfs, const std::string& fat);


};
