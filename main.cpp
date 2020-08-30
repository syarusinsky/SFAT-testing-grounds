#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <iostream>

#include "Fat16Entry.hpp"

// boot sector offsets and sizes
#define BOOT_SEC_SIZE_IN_BYTES 				512
#define BOOT_SEC_JUMP_OFFSET 				0x000
#define BOOT_SEC_JUMP_SIZE 				3
#define BOOT_SEC_OEM_NAME_OFFSET 			0x003
#define BOOT_SEC_OEM_NAME_SIZE          		8
#define BOOT_SEC_SECTOR_SIZE_OFFSET 			0x00B
#define BOOT_SEC_SECTOR_SIZE_SIZE 			2
#define BOOT_SEC_NUM_SECS_PER_CLUSTER_OFFSET 		0x00D
#define BOOT_SEC_NUM_SECS_PER_CLUSTER_SIZE 		1
#define BOOT_SEC_RESERVED_SECS_OFFSET 			0x00E
#define BOOT_SEC_RESERVED_SECS_SIZE 			2
#define BOOT_SEC_NUM_FATS_OFFSET 			0x010
#define BOOT_SEC_NUM_FATS_SIZE 				1
#define BOOT_SEC_NUM_DIRS_IN_ROOT_OFFSET 		0x011
#define BOOT_SEC_NUM_DIRS_IN_ROOT_SIZE 			2
#define BOOT_SEC_NUM_SECS_ON_DISK_LT_32MB_OFFSET 	0x013
#define BOOT_SEC_NUM_SECS_ON_DISK_LT_32MB_SIZE 		2
#define BOOT_SEC_MEDIA_DESCRIPTOR_OFFSET 		0x015
#define BOOT_SEC_MEDIA_DESCRIPTOR_SIZE 			1
#define BOOT_SEC_NUM_SECS_PER_FAT_OFFSET 		0x016
#define BOOT_SEC_NUM_SECS_PER_FAT_SIZE 			2
#define BOOT_SEC_NUM_SECS_PER_TRACK_OFFSET 		0x018
#define BOOT_SEC_NUM_SECS_PER_TRACK_SIZE 		2
#define BOOT_SEC_NUM_HEADS_OFFSET 			0x01A
#define BOOT_SEC_NUM_HEADS_SIZE 			2
#define BOOT_SEC_NUM_HIDDEN_SECS_OFFSET 		0x01C
#define BOOT_SEC_NUM_HIDDEN_SECS_SIZE 			4
#define BOOT_SEC_NUM_SECS_ON_DISK_GT_32MB_OFFSET 	0x020
#define BOOT_SEC_NUM_SECS_ON_DISK_GT_32MB_SIZE 		4
#define BOOT_SEC_NUM_SECS_PER_FAT_FAT32_OFFSET 		0x024
#define BOOT_SEC_NUM_SECS_PER_FAT_FAT32_SIZE 		4
#define BOOT_SEC_FAT_MIRR_ACTIVE_FAT_FAT32_OFFSET 	0x028
#define BOOT_SEC_FAT_MIRR_ACTIVE_FAT_FAT32_SIZE 	2
#define BOOT_SEC_FILE_SYSTEM_VER_FAT32_OFFSET 		0x02A
#define BOOT_SEC_FILE_SYSTEM_VER_FAT32_SIZE 		2
#define BOOT_SEC_CLUSTER_NUM_FOR_ROOT_FAT32_OFFSET 	0x02C
#define BOOT_SEC_CLUSTER_NUM_FOR_ROOT_FAT32_SIZE 	4
#define BOOT_SEC_FSINFO_SEC_NUM_FAT32_OFFSET 		0x030
#define BOOT_SEC_FSINFO_SEC_NUM_FAT32_SIZE 		2
#define BOOT_SEC_BACKUP_SEC_NUM_FAT32_OFFSET 		0x032
#define BOOT_SEC_BACKUP_SEC_NUM_FAT32_SIZE 		2
#define BOOT_SEC_RESERVED_FAT32_OFFSET 			0x034
#define BOOT_SEC_RESERVED_FAT32_SIZE 			12
#define BOOT_SEC_DRIVE_NUMBER_FAT32_OFFSET 		0x040
#define BOOT_SEC_DRIVE_NUMBER_FAT16_OFFSET 		0x024
#define BOOT_SEC_DRIVE_NUMBER_SIZE 			1
#define BOOT_SEC_CURRENT_HEAD_FAT32_OFFSET 		0x041
#define BOOT_SEC_CURRENT_HEAD_FAT16_OFFSET 		0x025
#define BOOT_SEC_CURRENT_HEAD_SIZE 			1
#define BOOT_SEC_BOOT_SIGNATURE_FAT32_OFFSET 		0x042
#define BOOT_SEC_BOOT_SIGNATURE_FAT16_OFFSET 		0x026
#define BOOT_SEC_BOOT_SIGNATURE_SIZE 			1
#define BOOT_SEC_VOLUME_ID_FAT32_OFFSET 		0x043
#define BOOT_SEC_VOLUME_ID_FAT16_OFFSET 		0x027
#define BOOT_SEC_VOLUME_ID_SIZE 			4
#define BOOT_SEC_VOLUME_LABEL_FAT32_OFFSET 		0x047
#define BOOT_SEC_VOLUME_LABEL_FAT16_OFFSET 		0x02B
#define BOOT_SEC_VOLUME_LABEL_SIZE 			11
#define BOOT_SEC_FILE_SYS_TYPE_FAT32_OFFSET 		0x052
#define BOOT_SEC_FILE_SYS_TYPE_FAT16_OFFSET 		0x036
#define BOOT_SEC_FILE_SYS_TYPE_SIZE 			8
#define BOOT_SEC_BOOT_CODE_FAT32_OFFSET 		0x08A
#define BOOT_SEC_BOOT_CODE_FAT32_SIZE 			372
#define BOOT_SEC_BOOT_CODE_FAT16_OFFSET 		0x03E
#define BOOT_SEC_BOOT_CODE_FAT16_SIZE 			448
#define BOOT_SEC_BOOT_SECTOR_SIGNATURE_OFFSET 		0x1FE
#define BOOT_SEC_BOOT_SECTOR_SIGNATURE_SIZE 		2

// parition table offsets
#define PARTITION_TABLE_1_OFFSET 0x1BE
#define PARTITION_TABLE_2_OFFSET PARTITION_TABLE_1_OFFSET + 16
#define PARTITION_TABLE_3_OFFSET PARTITION_TABLE_1_OFFSET + (16 * 2)
#define PARTITION_TABLE_4_OFFSET PARTITION_TABLE_1_OFFSET + (16 * 3)

enum class PartitionType : uint8_t
{
	EMPTY 			= 0,
	FAT12 			= 1,
	FAT16_LTOREQ_32MB 	= 4,
	EXTENDED_PARTITION 	= 5,
	FAT16_GT_32MB 		= 6,
	FAT32_LTOREQ_2GB 	= 11,
	FAT32_LBA 		= 12,
	FAT16_LBA 		= 14,
	EXTENDED_LBA 		= 15
};

void printFat16Entry (const Fat16Entry& entry)
{
	std::cout << "Filename : " << entry.getFilenameDisplay() << " : ";
	if ( entry.isUnusedEntry() )
	{
		std::cout << "(UNUSED ENTRY)";
	}
	else if ( entry.isDeletedEntry() )
	{
		std::cout << "(DELETED)";
	}
	else if ( entry.isRootDirectory() )
	{
		std::cout << "(ROOT DIRECTORY)";
	}
	else if ( entry.isDirectory() && ! entry.clusterNumIsParentDirectory() )
	{
		std::cout << "(DIRECTORY)";
	}
	else if ( entry.isDirectory() && entry.clusterNumIsParentDirectory() )
	{
		std::cout << "(DIRECTORY WITH PARENT DIRECTORY)";
	}
	std::cout << std::endl;

	std::cout << "Attributes : ";
	if ( entry.isReadOnly() )
	{
		std::cout << "(READ ONLY) ";
	}
	if ( entry.isHiddenEntry() )
	{
		std::cout << "(HIDDEN) ";
	}
	if ( entry.isSystemFile() )
	{
		std::cout << "(SYSTEM) ";
	}
	if ( entry.isDiskVolumeLabel() )
	{
		std::cout << "(DISK VOLUME LABEL) ";
	}
	if ( entry.isSubdirectory() )
	{
		std::cout << "(SUBDIRECTORY) ";
	}
	std::cout << std::endl;

	std::cout << "Time last updated : "
		<< entry.getTimeUpdated().hours << ":" 				// hours
		<< entry.getTimeUpdated().minutes << ":" 			// minutes
		<< entry.getTimeUpdated().twoSecondIntervals << std::endl; 	// two second periods

	std::cout << "Date last updated : "
		<< entry.getDateUpdated().year << ":" 				// year
		<< entry.getDateUpdated().month << ":" 				// month
		<< entry.getDateUpdated().day << std::endl; 			// day

	std::cout << "Starting cluster num : " << std::to_string( entry.getStartingClusterNum() ) << std::endl;
	std::cout << "Entry size in bytes : " << std::to_string( entry.getFileSizeInBytes() ) << std::endl;
}

class BootSector
{
	public:
		BootSector (uint8_t* offset, const PartitionType& partitionType = PartitionType::EMPTY) :
			m_PartitionType( partitionType ),
			m_JumpInstruction( (offset[2] << 16) | (offset[1] << 8) | (offset[0]) ),
			m_OemName{ 0 }, // we'll do this in a loop in the constructor
			m_SectorSizeInBytes( (offset[BOOT_SEC_SECTOR_SIZE_OFFSET + 1] << 8) | (offset[BOOT_SEC_SECTOR_SIZE_OFFSET]) ),
			m_NumSectorsPerCluster( offset[BOOT_SEC_NUM_SECS_PER_CLUSTER_OFFSET] ),
			m_NumReservedSectors( (offset[BOOT_SEC_RESERVED_SECS_OFFSET + 1] << 8) | offset[BOOT_SEC_RESERVED_SECS_OFFSET] ),
			m_NumFats( offset[BOOT_SEC_NUM_FATS_OFFSET] ),
			m_NumDirEntriesInRoot( (offset[BOOT_SEC_NUM_DIRS_IN_ROOT_OFFSET + 1] << 8) | offset[BOOT_SEC_NUM_DIRS_IN_ROOT_OFFSET] ),
			m_NumSectorsOnDisk( (offset[BOOT_SEC_NUM_SECS_ON_DISK_LT_32MB_OFFSET + 1] << 8) // this may be zero, so we use logic in the
						| offset[BOOT_SEC_NUM_SECS_ON_DISK_LT_32MB_OFFSET] ), 	// constructor to find true size
			m_MediaDescriptor( offset[BOOT_SEC_MEDIA_DESCRIPTOR_OFFSET] ),
			m_NumSectorsPerFat( (offset[BOOT_SEC_NUM_SECS_PER_FAT_OFFSET + 1] << 8) | offset[BOOT_SEC_NUM_SECS_PER_FAT_OFFSET] ),
			m_NumSectorsPerTrack( (offset[BOOT_SEC_NUM_SECS_PER_TRACK_OFFSET + 1] << 8) | offset[BOOT_SEC_NUM_SECS_PER_TRACK_OFFSET] ),
			m_NumHeads( (offset[BOOT_SEC_NUM_HEADS_OFFSET + 1] << 8) | offset[BOOT_SEC_NUM_HEADS_OFFSET] ),
			m_NumHiddenSectors( (offset[BOOT_SEC_NUM_HIDDEN_SECS_OFFSET + 3] << 24)
						| (offset[BOOT_SEC_NUM_HIDDEN_SECS_OFFSET + 2] << 16)
						| (offset[BOOT_SEC_NUM_HIDDEN_SECS_OFFSET + 1] << 8)
						| offset[BOOT_SEC_NUM_HIDDEN_SECS_OFFSET] ),
			m_DriveNum( offset[BOOT_SEC_DRIVE_NUMBER_FAT16_OFFSET] ),
			m_CurrentHead( offset[BOOT_SEC_CURRENT_HEAD_FAT16_OFFSET] ),
			m_BootSignature( offset[BOOT_SEC_BOOT_SIGNATURE_FAT16_OFFSET] ),
			m_VolumeID( (offset[BOOT_SEC_VOLUME_ID_FAT16_OFFSET + 3] << 24)
						| (offset[BOOT_SEC_VOLUME_ID_FAT16_OFFSET + 2] << 16)
						| (offset[BOOT_SEC_VOLUME_ID_FAT16_OFFSET + 1] << 8)
						| offset[BOOT_SEC_VOLUME_ID_FAT16_OFFSET] ),
			m_VolumeLabel{ 0 },
			m_FileSystemType{ 0 },
			// ignoring boot code
			m_BootSectorSignature( (offset[BOOT_SEC_BOOT_SECTOR_SIGNATURE_OFFSET + 1] << 8)
						| offset[BOOT_SEC_BOOT_SECTOR_SIGNATURE_OFFSET] ),
			m_NumSectorsForFat( (offset[BOOT_SEC_NUM_SECS_PER_FAT_FAT32_OFFSET + 3] << 24)
						| (offset[BOOT_SEC_NUM_SECS_PER_FAT_FAT32_OFFSET + 2] << 16)
						| (offset[BOOT_SEC_NUM_SECS_PER_FAT_FAT32_OFFSET + 1] << 8)
						| offset[BOOT_SEC_NUM_SECS_PER_FAT_FAT32_OFFSET] ),
			m_FlagsForFatMirroringAndActiveFat( (offset[BOOT_SEC_FAT_MIRR_ACTIVE_FAT_FAT32_OFFSET + 1] << 8)
								| offset[BOOT_SEC_FAT_MIRR_ACTIVE_FAT_FAT32_OFFSET] ),
			m_FileSystemVersionNum( (offset[BOOT_SEC_FILE_SYSTEM_VER_FAT32_OFFSET + 1] << 8)
						| offset[BOOT_SEC_FILE_SYSTEM_VER_FAT32_OFFSET] ),
			m_ClusterNumForRoot( (offset[BOOT_SEC_CLUSTER_NUM_FOR_ROOT_FAT32_OFFSET + 3] << 24)
						| (offset[BOOT_SEC_CLUSTER_NUM_FOR_ROOT_FAT32_OFFSET + 2] << 16)
						| (offset[BOOT_SEC_CLUSTER_NUM_FOR_ROOT_FAT32_OFFSET + 1] << 8)
						| offset[BOOT_SEC_CLUSTER_NUM_FOR_ROOT_FAT32_OFFSET] ),
			m_FSInfoSectorNum( (offset[BOOT_SEC_FSINFO_SEC_NUM_FAT32_OFFSET + 1] << 8)
						| offset[BOOT_SEC_FSINFO_SEC_NUM_FAT32_OFFSET] ),
			m_BackupSectorNum( (offset[BOOT_SEC_BACKUP_SEC_NUM_FAT32_OFFSET + 1] << 8)
						| offset[BOOT_SEC_BACKUP_SEC_NUM_FAT32_OFFSET] )
		{
			// get the OEM name
			for ( unsigned int character = 0; character < BOOT_SEC_OEM_NAME_SIZE; character++ )
			{
				m_OemName[character] = offset[BOOT_SEC_OEM_NAME_OFFSET + character];
			}

			// if num sectors is 0, the partition is likely > 32MB and we need to check other offset
			if ( m_NumSectorsOnDisk == 0 )
			{
				m_NumSectorsOnDisk = (offset[BOOT_SEC_NUM_SECS_ON_DISK_GT_32MB_OFFSET + 3] << 24)
							| (offset[BOOT_SEC_NUM_SECS_ON_DISK_GT_32MB_OFFSET + 2] << 16)
							| (offset[BOOT_SEC_NUM_SECS_ON_DISK_GT_32MB_OFFSET + 1] << 8)
							| offset[BOOT_SEC_NUM_SECS_ON_DISK_GT_32MB_OFFSET];
			}

			// if the file system type isn't FAT16 or FAT12, we need to use FAT32 offsets
			if ( partitionType == PartitionType::FAT32_LTOREQ_2GB || partitionType == PartitionType::FAT32_LBA )
			{
				m_DriveNum = offset[BOOT_SEC_DRIVE_NUMBER_FAT32_OFFSET];
				m_CurrentHead = offset[BOOT_SEC_CURRENT_HEAD_FAT32_OFFSET];
				m_BootSignature = offset[BOOT_SEC_BOOT_SIGNATURE_FAT32_OFFSET];
				m_VolumeID = (offset[BOOT_SEC_VOLUME_ID_FAT32_OFFSET + 3] << 24)
						| (offset[BOOT_SEC_VOLUME_ID_FAT32_OFFSET + 2] << 16)
						| (offset[BOOT_SEC_VOLUME_ID_FAT32_OFFSET + 1] << 8)
						| offset[BOOT_SEC_VOLUME_ID_FAT32_OFFSET];

				// get the file system type name from the FAT32 offset
				for ( unsigned int character = 0; character < BOOT_SEC_FILE_SYS_TYPE_SIZE; character++ )
				{
					m_FileSystemType[character] = offset[BOOT_SEC_FILE_SYS_TYPE_FAT32_OFFSET + character];
				}

				// get the volume label from the FAT32 offset
				for ( unsigned int character = 0; character < BOOT_SEC_VOLUME_LABEL_SIZE; character++ )
				{
					m_VolumeLabel[character] = offset[BOOT_SEC_VOLUME_LABEL_FAT32_OFFSET + character];
				}
			}
			else // a FAT16 or FAT12 file system
			{

				// get the file system type name from the FAT16/FAT12 offset
				for ( unsigned int character = 0; character < BOOT_SEC_FILE_SYS_TYPE_SIZE; character++ )
				{
					m_FileSystemType[character] = offset[BOOT_SEC_FILE_SYS_TYPE_FAT16_OFFSET + character];
				}

				// get the volume label from the FAT16/FAT12 offset
				for ( unsigned int character = 0; character < BOOT_SEC_VOLUME_LABEL_SIZE; character++ )
				{
					m_VolumeLabel[character] = offset[BOOT_SEC_VOLUME_LABEL_FAT16_OFFSET + character];
				}
			}
		}

		~BootSector() {}

		PartitionType getPartitionType() const { return m_PartitionType; }

		bool volumeIdLabelAndFSTypeAreValid() const
		{
			if ( m_BootSignature == 0x29 )
			{
				return true;
			}

			return false;
		}

		bool bootSectorSignatureIsValid() const
		{
			if ( m_BootSectorSignature == 0xAA55 )
			{
				return true;
			}

			return false;
		}

		uint32_t getJumpInstruction() const { return m_JumpInstruction; }

		const char* getOemName() const { return m_OemName; }

		uint16_t getSectorSizeInBytes() const { return m_SectorSizeInBytes; }

		uint8_t getNumSectorsPerCluster() const { return m_NumSectorsPerCluster; }

		uint16_t getNumReservedSectors() const { return m_NumReservedSectors; }

		uint8_t getNumFats() const { return m_NumFats; }

		uint16_t getNumDirectoryEntriesInRoot() const { return m_NumDirEntriesInRoot; }

		uint32_t getNumSectorsOnDisk() const { return m_NumSectorsOnDisk; }

		uint8_t getMediaDescriptor() const { return m_MediaDescriptor; }

		uint16_t getNumSectorsPerFat() const
		{
			if ( m_PartitionType == PartitionType::FAT32_LTOREQ_2GB
					|| m_PartitionType == PartitionType::FAT32_LBA )
			{
				return m_NumSectorsForFat;
			}

			return m_NumSectorsPerFat;
		}

		uint16_t getNumSectorsPerTrack() const { return m_NumSectorsPerTrack; }

		uint16_t getNumHeads() const { return m_NumHeads; }

		uint32_t getNumHiddenSectors() const { return m_NumHiddenSectors; }

		uint8_t getDriveNum() const { return m_DriveNum; }

		uint8_t getCurrentHead() const { return m_CurrentHead; }

		uint8_t getBootSignature() const { return m_BootSignature; }

		uint32_t getVolumeID() const { return m_VolumeID; }

		const char* getVolumeLabel() const { return m_VolumeLabel; }

		const char* getFileSystemType() const { return m_FileSystemType; }

		uint16_t getBootSectorSignature() const { return m_BootSectorSignature; }

		uint16_t getFlagsForFatMirroringAndActiveFat() const { return m_FlagsForFatMirroringAndActiveFat; }

		uint16_t getFileSystemVersionNum() const { return m_FileSystemVersionNum; }

		uint32_t getClusterNumForRoot() const { return m_ClusterNumForRoot; }

		uint16_t getFSInfoSectorNum() const { return m_FSInfoSectorNum; }

		uint16_t getBackupSectorNum() const { return m_BackupSectorNum; }

		void print() const
		{
			std::cout << "Jump instruction : " << std::to_string( m_JumpInstruction ) << std::endl;
			std::cout << "OEM name : " << m_OemName << std::endl;
			std::cout << "Sector size in bytes : " << std::to_string( m_SectorSizeInBytes ) << std::endl;
			std::cout << "Number of sectors per cluster : " << std::to_string( m_NumSectorsPerCluster ) << std::endl;
			std::cout << "Number of reserved sectors : " << std::to_string( m_NumReservedSectors ) << std::endl;
			std::cout << "Number of FATs : " << std::to_string( m_NumFats ) << std::endl;
			std::cout << "Number of directory entries in root (if FAT16) : " << std::to_string( m_NumDirEntriesInRoot ) << std::endl;
			std::cout << "Number of sectors on disk : " << std::to_string( m_NumSectorsOnDisk ) << std::endl;
			std::cout << "Media descriptor : " << std::to_string( m_MediaDescriptor ) << std::endl;
			std::cout << "Number of sectors per FAT (if FAT16) : " << std::to_string( m_NumSectorsPerFat ) << std::endl;
			std::cout << "Number of sectors per track : " << std::to_string( m_NumSectorsPerTrack ) << std::endl;
			std::cout << "Number of heads : " << std::to_string( m_NumHeads ) << std::endl;
			std::cout << "Number of hidden sectors : " << std::to_string( m_NumHiddenSectors ) << std::endl;
			std::cout << "Drive number : " << std::to_string( m_DriveNum ) << std::endl;
			std::cout << "Current head : " << std::to_string( m_CurrentHead ) << std::endl;
			std::cout << "Boot signature : " << std::to_string( m_BootSignature ) << std::endl;
			std::cout << "Volume ID : " << std::to_string( m_VolumeID ) << std::endl;
			std::cout << "Volume label : " << m_VolumeLabel << std::endl;
			std::cout << "File system name : " << m_FileSystemType << std::endl;
			std::cout << "Boot sector signature : " << std::to_string( m_BootSectorSignature ) << std::endl;
			std::cout << "(if FAT32) Number of sectors per FAT : " << std::to_string( m_NumSectorsForFat ) << std::endl;
			std::cout << "(if FAT32) Flags for mirroring and active : " << std::to_string( m_FlagsForFatMirroringAndActiveFat ) << std::endl;
			std::cout << "(if FAT32) File system version number : " << std::to_string( m_FileSystemVersionNum ) << std::endl;
			std::cout << "(if FAT32) Cluster number for root : " << std::to_string( m_ClusterNumForRoot ) << std::endl;
			std::cout << "(if FAT32) FSInfo sector number : " << std::to_string( m_FSInfoSectorNum ) << std::endl;
			std::cout << "(if FAT32) Backup boot sector sector number : " << std::to_string( m_BackupSectorNum ) << std::endl;
		}

	private:
		PartitionType 	m_PartitionType;

		uint32_t 	m_JumpInstruction; // never going to use this, but whatever
		char 	 	m_OemName[BOOT_SEC_OEM_NAME_SIZE];
		uint16_t 	m_SectorSizeInBytes;
		uint8_t 	m_NumSectorsPerCluster;
		uint16_t 	m_NumReservedSectors; // including boot sector
		uint8_t 	m_NumFats;
		uint16_t 	m_NumDirEntriesInRoot; // NA for FAT32
		uint32_t 	m_NumSectorsOnDisk;
		uint8_t 	m_MediaDescriptor;
		uint16_t 	m_NumSectorsPerFat; // NA for FAT32
		uint16_t 	m_NumSectorsPerTrack; // CHS addressing
		uint16_t 	m_NumHeads; // CHS addressing
		uint32_t 	m_NumHiddenSectors; // sectors before the boot sector
		uint8_t 	m_DriveNum;
		uint8_t 	m_CurrentHead;
		uint8_t 	m_BootSignature; // must be 0x29 for next 3 fields to be valid
		uint32_t 	m_VolumeID;
		char 		m_VolumeLabel[BOOT_SEC_VOLUME_LABEL_SIZE];
		char 		m_FileSystemType[BOOT_SEC_FILE_SYS_TYPE_SIZE];
		// uint8_t 	m_BootCode[448]; // not currently using this, as we don't actually plan to boot from SD card
		uint16_t 	m_BootSectorSignature; // must be 0xAA55

		// FAT32 only!
		uint32_t 	m_NumSectorsForFat;
		uint16_t 	m_FlagsForFatMirroringAndActiveFat;
		uint16_t 	m_FileSystemVersionNum;
		uint32_t 	m_ClusterNumForRoot;
		uint16_t 	m_FSInfoSectorNum;
		uint16_t 	m_BackupSectorNum;
};

class PartitionTable
{
	public:
		PartitionTable (uint32_t* offset) :
			m_Bootable( (*offset & 0x000000FF) ),
			m_StartAddrCHS( (*offset & 0xFFFFFF00) >> 8 ),
			m_PartitionType( (*(offset + 1) & 0x000000FF) ),
			m_EndAddrCHS( (*(offset + 1) & 0xFFFFFF00) >> 8 ),
			m_OffsetLBA( *(offset + 2) ),
			m_PartitionSize( *(offset + 3) )
		{
		}

		~PartitionTable() {}

		bool isBootable() const
		{
			if ( m_Bootable == 0x80 )
			{
				return true;
			}

			return false;
		}

		uint32_t getStartAddressCHS() const
		{
			return m_StartAddrCHS;
		}

		uint32_t getEndAddressCHS() const
		{
			return m_EndAddrCHS;
		}

		PartitionType getPartitionType() const
		{
			return static_cast<PartitionType>( m_PartitionType );
		}

		uint32_t getOffsetLBA() const
		{
			return m_OffsetLBA;
		}

		uint32_t getPartitionSize() const
		{
			return m_PartitionSize;
		}

		void print() const
		{
			std::cout << "Is bootable : " << this->isBootable() << std::endl;
			std::cout << "Start address chs : " << this->getStartAddressCHS() << std::endl;
			std::cout << "Partion type : " << std::to_string( static_cast<uint8_t>(this->getPartitionType()) ) << std::endl;
			std::cout << "End address chs : " << std::to_string( this->getEndAddressCHS() ) << std::endl;
			std::cout << "Offset lba : " << std::to_string( this->getOffsetLBA() ) << std::endl;
			std::cout << "Partition size : " << std::to_string( this->getPartitionSize() ) << std::endl;
		}

	private:
		uint8_t  m_Bootable;
		uint32_t m_StartAddrCHS;
		uint8_t  m_PartitionType;
		uint32_t m_EndAddrCHS;
		uint32_t m_OffsetLBA;
		uint32_t m_PartitionSize;
};

int main()
{
	// TODO we need a way of figuring out if the drive is partition (boot sector in each partition) or not partition (boot sector at beginning)
	// TODO we need to check partition type for compatibility (are all FAT16 types okay?)

	FILE* sdCardImage = fopen( "SDCard.img", "rb" );

	// read each partition table entry (we're doing it this way just to test if the defines work)
	uint32_t ptBuffer[4 * 4]; // since there are 4 partition tables
	fseek( sdCardImage, PARTITION_TABLE_1_OFFSET, SEEK_SET );
	fread( &ptBuffer[0], sizeof(uint32_t), 4, sdCardImage );
	fseek( sdCardImage, PARTITION_TABLE_2_OFFSET, SEEK_SET );
	fread( &ptBuffer[4], sizeof(uint32_t), 4, sdCardImage );
	fseek( sdCardImage, PARTITION_TABLE_3_OFFSET, SEEK_SET );
	fread( &ptBuffer[8], sizeof(uint32_t), 4, sdCardImage );
	fseek( sdCardImage, PARTITION_TABLE_4_OFFSET, SEEK_SET );
	fread( &ptBuffer[12], sizeof(uint32_t), 4, sdCardImage );

	PartitionTable partitionTables[4] = { PartitionTable( ptBuffer ), PartitionTable( ptBuffer + 4 ), PartitionTable( ptBuffer + 8 ),
						PartitionTable( ptBuffer + 12 ) };

	// printing out partition table info
	for ( unsigned int partition = 0; partition < 4; partition++ )
	{
		// print partition table info
		std::cout << "PARTITION #" << std::to_string( partition ) << " ----------------------------------------" << std::endl;
		partitionTables[partition].print();

		// read boot sector and print info
		std::cout << "PARTITION #" << std::to_string( partition ) << " BOOT SECTOR ----------------------------" << std::endl;
		uint8_t bsBuffer[BOOT_SEC_SIZE_IN_BYTES];
		fseek( sdCardImage, partitionTables[partition].getOffsetLBA() * 512, SEEK_SET );
		fread( bsBuffer, sizeof(uint8_t), BOOT_SEC_SIZE_IN_BYTES, sdCardImage );

		BootSector bootSector( bsBuffer );
		bootSector.print();

		// navigate to root directory and print entries
		uint8_t entryBuffer[FAT16_ENTRY_SIZE * bootSector.getNumDirectoryEntriesInRoot()];
		fseek( sdCardImage, (bootSector.getNumReservedSectors() + (bootSector.getNumFats() * bootSector.getNumSectorsPerFat())) *
				bootSector.getSectorSizeInBytes(), SEEK_SET );
		fread( entryBuffer, FAT16_ENTRY_SIZE, bootSector.getNumDirectoryEntriesInRoot(), sdCardImage );

		for ( unsigned int entry = 0; entry < bootSector.getNumDirectoryEntriesInRoot(); entry++ )
		{
			Fat16Entry fat16Entry( &entryBuffer[entry * FAT16_ENTRY_SIZE] );

			std::cout << "FAT16 ENTRY #" << std::to_string( entry ) << "-----------------------------------" << std::endl;
			printFat16Entry( fat16Entry );
		}
	}

	fclose( sdCardImage );
	return 0;
}
