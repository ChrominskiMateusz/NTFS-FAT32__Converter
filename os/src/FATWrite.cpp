#include "FATWrite.h"
#include <Windows.h>
#include <stdio.h>

FATWrite::FATWrite (const std::string& partitionName)
{
	partition.open (partitionName, std::ios::binary | std::ios::out | std::ios::in);
}

FATWrite::~FATWrite ()
{
	partition.close ();
}

void FATWrite::readBPB ()
{
	partition.read (reinterpret_cast<char*>(&bpb), sizeof (BiosParameterBlock));
	uint32_t fatSize = bpb.tableSize * bpb.bytesPerSector;

	fatOffset = bpb.reservedSectors * bpb.bytesPerSector;
	copyOffset = fatOffset + fatSize;
	dataOffset = fatOffset + fatSize * bpb.fatCopies;

	bytesPerCluster = bpb.sectorsPerCluster * bpb.bytesPerSector;
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
	setSize (fName.realFileSize);
	setCDateCTime (fName);
	setMDateMTime (fName);
	setLADate (fName);
	setAttributes (fName);
}

void FATWrite::setName (const FileName& fName, char* name)			// file_names, exts in FAT -> BIG LETTERS
{																	// empty places != 0x00 -> == 0x20
	int i{};
	if (fName.flags == 0x10000000)					// if dir
	{
		for (; i < fName.filenameLength && i < 8; i++)		// copy name
			if (name[i * 2] > 0x60 && name[i * 2] < 0x7B)
				dEntry.name[i] = name[i * 2] - 0x20;
			else
				dEntry.name[i] = name[i * 2];

		for (int j{}; j < 3; j++)
			dEntry.ext[j] = 0x20;
	}
	else
	{
		for (; name[i * 2] != '.' && i < 8; i++)	// copy name
			if (name[i * 2] > 0x60 && name[i * 2] < 0x7B)
				dEntry.name[i] = name[i * 2] - 0x20;
			else
				dEntry.name[i] = name[i * 2];

		for (int k{ fName.filenameLength - 3 }, j{}; k < fName.filenameLength; k++, j++)	// and ext
			dEntry.ext[j] = name[k * 2] - 0x20;
	}

	for (; i < 8; i++)
		dEntry.name[i] = 0x20;
}

void FATWrite::setClusterEntry (const FileName& fName)
{
	int32_t clusterNbr = searchCluster ();						// find first cluster
	dEntry.firstClusterHi = static_cast<uint16_t>((clusterNbr & 0xFFFF0000) >> 16);
	dEntry.firstClusterLow = static_cast<uint16_t>(clusterNbr & 0x0000FFFF);

	writeToFAT (0x0FFFFFFF, clusterNbr);
	clearCluster (clusterNbr);
}

void FATWrite::setSize (const uint32_t& size)
{
	dEntry.size = size;
}

void FATWrite::setCDateCTime (const FileName& fName)
{
	FILETIME ftCreate;
	SYSTEMTIME stUTC;
	ftCreate.dwLowDateTime = DWORD (fName.creationTime & 0x00000000FFFFFFFF);
	ftCreate.dwHighDateTime = DWORD ((fName.creationTime & 0xFFFFFFFF00000000) >> 32);
	FileTimeToSystemTime (&ftCreate, &stUTC);
	//printf ("Created on: %02d/%02d/%d %02d:%02d\n", stUTC.wDay, stUTC.wMonth, stUTC.wYear, stUTC.wHour, stUTC.wMinute);

	dEntry.cDate |= stUTC.wDay;
	dEntry.cDate |= stUTC.wMonth << 5;
	dEntry.cDate |= (stUTC.wYear - 1980) << 9;
					
	dEntry.cTime |= stUTC.wSecond / 2;
	dEntry.cTime |= stUTC.wMinute << 5;
	dEntry.cTime |= stUTC.wHour << 11;
}

void FATWrite::setMDateMTime (const FileName& fName)
{
	FILETIME ftCreate;
	SYSTEMTIME stUTC;
	ftCreate.dwLowDateTime = DWORD (fName.modificationTime & 0x00000000FFFFFFFF);
	ftCreate.dwHighDateTime = DWORD ((fName.modificationTime & 0xFFFFFFFF00000000) >> 32);
	FileTimeToSystemTime (&ftCreate, &stUTC);

	dEntry.wDate |= stUTC.wDay;
	dEntry.wDate |= stUTC.wMonth << 5;
	dEntry.wDate |= (stUTC.wYear - 1980) << 9;

	dEntry.wTime |= stUTC.wSecond / 2;
	dEntry.wTime |= stUTC.wMinute << 5;
	dEntry.wTime |= stUTC.wHour << 11;
}

void FATWrite::setLADate (const FileName& fName)
{
	FILETIME ftCreate;
	SYSTEMTIME stUTC;
	ftCreate.dwLowDateTime = DWORD (fName.readTime & 0x00000000FFFFFFFF);
	ftCreate.dwHighDateTime = DWORD ((fName.readTime & 0xFFFFFFFF00000000) >> 32);
	FileTimeToSystemTime (&ftCreate, &stUTC);

	dEntry.aTime |= stUTC.wDay;
	dEntry.aTime |= stUTC.wMonth << 5;
	dEntry.aTime |= (stUTC.wYear - 1980) << 9;
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
	{
		dEntry.attributes |= 0x20;
		dEntry.reserved = 0x18;
	}
	if (fName.flags & 0x10000000)
	{
		dEntry.attributes |= 0x10;
		dEntry.reserved = 0x08;
	}
}

void FATWrite::writeEntry (const uint32_t& depth)
{
	setEntryPointer (depth);
	write (&dEntry, sizeof DirectoryEntry);
}

void FATWrite::writeData (char* buffer, const uint32_t& size, int64_t& leftSize, const uint64_t& fileSize)			// max size of buffer is bytesPerCluster
{
	int32_t clusterNumber;
	if (fileSize == leftSize)
		clusterNumber = calculateClusterNumber ();
	else
		clusterNumber = searchCluster ();

	partition.seekg (dataOffset + bytesPerCluster * (clusterNumber - 2));
	write (buffer, size);

	leftSize -= bytesPerCluster;
	if (leftSize <= 0)
		writeToFAT (0x0FFFFFFF, clusterNumber);
	else
		if (fileSize == leftSize + bytesPerCluster)
			writeToFAT (searchCluster (), clusterNumber);
		else
			writeToFAT (nextCluster (), clusterNumber);
}

void FATWrite::writeToFAT (const uint32_t& value, const int32_t& clusterNumber)
{
	uint32_t p = value;
	partition.seekg (fatOffset + clusterNumber * sizeof int32_t);
	write (&p, sizeof int32_t);

	partition.seekg (copyOffset + clusterNumber * sizeof int32_t);
	write (&p, sizeof int32_t);
}

void FATWrite::setEntryPointer (const uint32_t& parentNumber)
{
	if (offsetOfEntries[parentNumber] + sizeof DirectoryEntry >  bytesPerCluster)
	{
		offsetOfEntries[parentNumber] = 0;
		writeToFAT (searchCluster (), entryClusters[parentNumber]);
		entryClusters[parentNumber] = searchCluster ();
	}

	partition.seekg (dataOffset + offsetOfEntries[parentNumber] + bytesPerCluster * (entryClusters[parentNumber] - 2));
	offsetOfEntries[parentNumber] += sizeof DirectoryEntry;
}

void FATWrite::addToMap (const MFTHeader& mft)
{
	entryClusters[mft.MFTRecordNumber] = calculateClusterNumber ();
	offsetOfEntries[mft.MFTRecordNumber] = 0;
}

uint32_t FATWrite::calculateClusterNumber ()
{
	return (static_cast<uint32_t>(dEntry.firstClusterHi)) << 16 | (static_cast<uint32_t>(dEntry.firstClusterLow));
}

void FATWrite::clearCluster (const int32_t& cluusterNumber)
{
	char empty[512]{};

	partition.seekg (dataOffset + bytesPerCluster * (cluusterNumber - 2));
	for (int i{}; i < bytesPerCluster; i += 512)
		write (empty, 512);
}


int32_t FATWrite::nextCluster ()
{
	int32_t first = searchCluster();
	int32_t next;

	writeToFAT (0x0FFFFFFF, first);				// temporary write to FAT
	next = searchCluster ();
	writeToFAT (0x00000000, first);

	return next;
}