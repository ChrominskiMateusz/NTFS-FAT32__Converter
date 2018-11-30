#pragma once
#include "NTFS.h"

class Converter
{
public:
	~Converter ();
	Converter () = default;
	Converter (const std::string& ntfs, const std::string& fat);

};
