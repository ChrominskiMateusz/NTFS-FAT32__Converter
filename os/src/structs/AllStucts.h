#pragma once

#include "mbr/MasterBootRecord.h"					// MBR

#include "fat32/BiosParameterBlock.h"				// FAT
#include "fat32/DirectoryEntry.h"

#include "ntfs/PartitionBootSector.h"				// NTFS
#include "ntfs/attributes/headers/CommonHeaderPart.h"
#include "ntfs/attributes/headers/ResidentHeader.h"
#include "ntfs/attributes/headers/NonResidentHeader.h"
#include "ntfs/IndexHeader.h"
#include "ntfs/IndexEntry.h"
#include "ntfs/MFTHeader.h"

#include "ntfs/attributes/StandardInformation.h"	// NTFS Attributes
#include "ntfs/attributes/FileName.h"
#include "ntfs/attributes/ObjectID.h"
#include "ntfs/attributes/IndexRoot.h"