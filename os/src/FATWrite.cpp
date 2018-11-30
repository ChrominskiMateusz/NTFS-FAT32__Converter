#include "FATWrite.h"
#include <Windows.h>
#include <stdio.h>

FATWrite::FATWrite (const std::string& partitionName)
{
	partition.open (partitionName, std::ios::binary | std::ios::out | std::ios::trunc);
}

FATWrite::~FATWrite ()
{
	partition.close ();
}

int32_t FATWrite::searchCluster ()				// returning first empty cluster number
{
	partition.seekg (fatOffset + (5 * sizeof uint32_t));
	uint32_t value;
	int32_t index = 4;
	do
	{
		partition.read (reinterpret_cast<char *>(&value), sizeof uint32_t);
		index++;
	} while (value != 0x00000000);

	return index;
}

void FATWrite::addToDirectoryEntry (const FileName& fName, char* name)
{
	dEntry = {};
	setName (fName, name);
	setClusterEntry (fName);
	setSize (fName);
	setCDateCTime (fName);
	setMDateMTime (fName);
	setLADate (fName);
	setAttributes (fName);
}

void FATWrite::setName (const FileName& fName, char * name)
{
	if (fName.flags == 0x10000000)					// if dir
		for (int i{}; i < fName.filenameLength && i < 8; i++)		// copy name
			dEntry.name[i] = name[i * 2];
	else
	{
		for (int i{}; name[i] != '.' && i < 8; i++)			// copy name
			dEntry.name[i] = name[i * 2];

		for (int i{ fName.filenameLength - 3 }, j{}; i < fName.filenameLength; i++, j++)	// and ext
			dEntry.ext[j] = name[i * 2];
	}
}

void FATWrite::setClusterEntry (const FileName& fName)
{
	uint32_t clusterNbr = searchCluster ();						// find first cluster
	dEntry.firstClusterHi = static_cast<uint16_t>((clusterNbr & 0xFFFF0000) >> 16);
	dEntry.firstClusterLow = static_cast<uint16_t>(clusterNbr & 0x0000FFFF);
}

void FATWrite::setSize (const FileName& fName)
{
	dEntry.size = static_cast<uint32_t>(fName.realFileSize);
}

void FATWrite::setCDateCTime (const FileName& fName)
{
	FILETIME ftCreate;
	SYSTEMTIME stUTC;
	ftCreate.dwLowDateTime = DWORD (fName.creationTime & 0x00000000FFFFFFFF);
	ftCreate.dwHighDateTime = DWORD ((fName.creationTime & 0xFFFFFFFF00000000) >> 32);
	FileTimeToSystemTime (&ftCreate, &stUTC);
	//printf ("Created on: %02d/%02d/%d %02d:%02d\n", stUTC.wDay, stUTC.wMonth, stUTC.wYear, stUTC.wHour, stUTC.wMinute);

	dEntry.cDate |= stUTC.wDay & 0x001F;
	dEntry.cDate |= stUTC.wMonth & 0x01E0;
	dEntry.cDate |= stUTC.wYear & 0xFE00;

	dEntry.cTime |= (stUTC.wSecond / 2) & 0x001F;
	dEntry.cTime |= stUTC.wMinute & 0x07E0;
	dEntry.cTime |= stUTC.wHour & 0xF800;
}

void FATWrite::setMDateMTime (const FileName& fName)
{
	FILETIME ftCreate;
	SYSTEMTIME stUTC;
	ftCreate.dwLowDateTime = DWORD (fName.modificationTime & 0x00000000FFFFFFFF);
	ftCreate.dwHighDateTime = DWORD ((fName.modificationTime & 0xFFFFFFFF00000000) >> 32);
	FileTimeToSystemTime (&ftCreate, &stUTC);

	dEntry.wDate |= stUTC.wDay & 0x001F;
	dEntry.wDate |= stUTC.wMonth & 0x01E0;
	dEntry.wDate |= stUTC.wYear & 0xFE00;

	dEntry.wTime |= (stUTC.wSecond / 2) & 0x001F;
	dEntry.wTime |= stUTC.wMinute & 0x07E0;
	dEntry.wTime |= stUTC.wHour & 0xF800;
}

void FATWrite::setLADate (const FileName& fName)
{
	FILETIME ftCreate;
	SYSTEMTIME stUTC;
	ftCreate.dwLowDateTime = DWORD (fName.readTime & 0x00000000FFFFFFFF);
	ftCreate.dwHighDateTime = DWORD ((fName.readTime & 0xFFFFFFFF00000000) >> 32);
	FileTimeToSystemTime (&ftCreate, &stUTC);

	dEntry.aTime |= stUTC.wDay & 0x001F;
	dEntry.aTime |= stUTC.wMonth & 0x01E0;
	dEntry.aTime |= stUTC.wYear & 0xFE00;
}

void FATWrite::setAttributes (const FileName& fName)
{
	// READ_ONLY = 0x01, HIDDEN = 0x02, SYSTEM = 0x04, DIRECTORY = 0x10, ARCHIVE = 0x20

	if (fName.flags & 0x0001)
		dEntry.attributes |= 0x01;
	if (fName.flags & 0x0002)
		dEntry.attributes |= 0x02;
	if (fName.flags & 0x0004)
		dEntry.attributes |= 0x04;
	if (fName.flags & 0x0020)
		dEntry.attributes |= 0x20;
	if (fName.flags & 0x10000000)
		dEntry.attributes |= 0x10;
}

void FATWrite::writeEntry ()
{
	partition.write (reinterpret_cast<char*>(&dEntry), sizeof dEntry);
}

void FATWrite::writeData (char* buffer, const uint32_t& size, int32_t& fileSize)			// max size of buffer is bytesPerCluster
{
	uint16_t bytesPerCluster = bpb.sectorsPerCluster * bpb.bytesPerSector;
	int32_t clusterNumber = searchCluster ();

	partition.seekg (dataOffset + bytesPerCluster * (clusterNumber - 2));
	partition.write (buffer, size);
	fileSize -= bytesPerCluster;
	
	if (fileSize <= 0)
		writeToFAT (0xFFFFFF0F, clusterNumber);
	else
		writeToFAT (nextCluster (), clusterNumber);
}

void FATWrite::writeToFAT (const uint32_t& value, int32_t& clusterNumber)
{
	partition.seekg (fatOffset + clusterNumber * sizeof int32_t);
	partition.write (reinterpret_cast<char*>(value), sizeof uint32_t);

	partition.seekg (copyOffset + clusterNumber * sizeof int32_t);
	partition.write (reinterpret_cast<char*>(value), sizeof uint32_t);
}

int32_t FATWrite::nextCluster ()
{
	int32_t first = searchCluster();
	int32_t next;

	writeToFAT (0xFFFFFFFF, first);
	next = searchCluster ();
	writeToFAT (0x00000000, first);

	return next;
}