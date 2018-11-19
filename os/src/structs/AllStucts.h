#pragma once

#include "mbr/MasterBootRecord.h"					// MBR

#include "fat32/BiosParameterBlock.h"				// FAT
#include "fat32/DirectoryEntry.h"

#include "ntfs/PartitionBootSector.h"				// NTFS
#include "ntfs/attributes/header/CommonHeaderPart.h"
#include "ntfs/attributes/header/ResidentHeader.h"
#include "ntfs/attributes/header/NonResidentHeader.h"
#include "ntfs/MFTHeader.h"

#include "ntfs/attributes/StandardInformation.h"	// NTFS Attributes
#include "ntfs/attributes/FileName.h"
