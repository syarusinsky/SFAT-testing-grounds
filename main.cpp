#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <iostream>

#include "Fat16Entry.hpp"
#include "PartitionTable.hpp"
#include "BootSector.hpp"

// parition table offsets
#define PARTITION_TABLE_1_OFFSET 0x1BE
#define PARTITION_TABLE_2_OFFSET PARTITION_TABLE_1_OFFSET + 16
#define PARTITION_TABLE_3_OFFSET PARTITION_TABLE_1_OFFSET + (16 * 2)
#define PARTITION_TABLE_4_OFFSET PARTITION_TABLE_1_OFFSET + (16 * 3)

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

void printBootSector (const BootSector& bootSector)
{
	std::cout << "Jump instruction : " << std::to_string( bootSector.getJumpInstruction() ) << std::endl;
	std::cout << "OEM name : " << bootSector.getOemName() << std::endl;
	std::cout << "Sector size in bytes : " << std::to_string( bootSector.getSectorSizeInBytes() ) << std::endl;
	std::cout << "Number of sectors per cluster : " << std::to_string( bootSector.getNumSectorsPerCluster() ) << std::endl;
	std::cout << "Number of reserved sectors : " << std::to_string( bootSector.getNumReservedSectors() ) << std::endl;
	std::cout << "Number of FATs : " << std::to_string( bootSector.getNumFats() ) << std::endl;
	std::cout << "Number of directory entries in root (if FAT16) : " << std::to_string( bootSector.getNumDirectoryEntriesInRoot() ) << std::endl;
	std::cout << "Number of sectors on disk : " << std::to_string( bootSector.getNumSectorsOnDisk() ) << std::endl;
	std::cout << "Media descriptor : " << std::to_string( bootSector.getMediaDescriptor() ) << std::endl;
	std::cout << "Number of sectors per FAT (if FAT16) : " << std::to_string( bootSector.getNumSectorsPerFat() ) << std::endl;
	std::cout << "Number of sectors per track : " << std::to_string( bootSector.getNumSectorsPerTrack() ) << std::endl;
	std::cout << "Number of heads : " << std::to_string( bootSector.getNumHeads() ) << std::endl;
	std::cout << "Number of hidden sectors : " << std::to_string( bootSector.getNumHiddenSectors() ) << std::endl;
	std::cout << "Drive number : " << std::to_string( bootSector.getDriveNum() ) << std::endl;
	std::cout << "Current head : " << std::to_string( bootSector.getCurrentHead() ) << std::endl;
	std::cout << "Boot signature : " << std::to_string( bootSector.getBootSignature() ) << std::endl;
	std::cout << "Volume ID : " << std::to_string( bootSector.getVolumeID() ) << std::endl;
	std::cout << "Volume label : " << bootSector.getVolumeLabel() << std::endl;
	std::cout << "File system name : " << bootSector.getFileSystemType() << std::endl;
	std::cout << "Boot sector signature : " << std::to_string( bootSector.getBootSectorSignature() ) << std::endl;
	std::cout << "(if FAT32) Number of sectors per FAT : " << std::to_string( bootSector.getNumSectorsPerFat() ) << std::endl;
	std::cout << "(if FAT32) Flags for mirroring and active : " << std::to_string( bootSector.getFlagsForFatMirroringAndActiveFat() ) << std::endl;
	std::cout << "(if FAT32) File system version number : " << std::to_string( bootSector.getFileSystemVersionNum() ) << std::endl;
	std::cout << "(if FAT32) Cluster number for root : " << std::to_string( bootSector.getClusterNumForRoot() ) << std::endl;
	std::cout << "(if FAT32) FSInfo sector number : " << std::to_string( bootSector.getFSInfoSectorNum() ) << std::endl;
	std::cout << "(if FAT32) Backup boot sector sector number : " << std::to_string( bootSector.getBackupSectorNum() ) << std::endl;
}

void printPartitionTable (const PartitionTable& partitionTable)
{
	std::cout << "Is bootable : " << partitionTable.isBootable() << std::endl;
	std::cout << "Start address chs : " << partitionTable.getStartAddressCHS() << std::endl;
	std::cout << "Partion type : " << std::to_string( static_cast<uint8_t>(partitionTable.getPartitionType()) ) << std::endl;
	std::cout << "End address chs : " << std::to_string( partitionTable.getEndAddressCHS() ) << std::endl;
	std::cout << "Offset lba : " << std::to_string( partitionTable.getOffsetLBA() ) << std::endl;
	std::cout << "Partition size : " << std::to_string( partitionTable.getPartitionSize() ) << std::endl;
}

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
		printPartitionTable( partitionTables[partition] );

		// read boot sector and print info
		std::cout << "PARTITION #" << std::to_string( partition ) << " BOOT SECTOR ----------------------------" << std::endl;
		uint8_t bsBuffer[BOOT_SEC_SIZE_IN_BYTES];
		fseek( sdCardImage, partitionTables[partition].getOffsetLBA() * 512, SEEK_SET );
		fread( bsBuffer, sizeof(uint8_t), BOOT_SEC_SIZE_IN_BYTES, sdCardImage );

		BootSector bootSector( bsBuffer );
		printBootSector( bootSector );

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
