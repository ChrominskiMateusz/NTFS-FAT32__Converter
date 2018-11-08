#include "Fat.h"

Fat::Fat (const std::string& imgName)
{
	discImg.open (imgName, std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
}


Fat::~Fat ()
{

}

void Fat::readBlock (const uint32_t& partitionOffset)
{
	discImg.seekg (0x7E00, discImg.beg);
	discImg.read (reinterpret_cast<char *>(&bpb), sizeof (BiosParameterBlock));
}
