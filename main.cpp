#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <iostream>

#include "CPPFile.hpp"
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
		<< std::to_string( entry.getTimeUpdated().hours ) << ":" 				// hours
		<< std::to_string( entry.getTimeUpdated().minutes ) << ":" 			// minutes
		<< std::to_string( entry.getTimeUpdated().twoSecondIntervals ) << std::endl; 	// two second periods

	std::cout << "Date last updated : "
		<< std::to_string( entry.getDateUpdated().year ) << ":" 				// year
		<< std::to_string( entry.getDateUpdated().month ) << ":" 				// month
		<< std::to_string( entry.getDateUpdated().day ) << std::endl; 			// day

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

	// FILE* sdCardImage = fopen( "SDCard.img", "rb" );
	CPPFile sdCardFile( "SDCard.img" );
	if ( sdCardFile.needsInitialization() )
	{
		std::cout << "Couldn't find file :(" << std::endl;
	}

	// load all four partition tables
	uint32_t* ptBuffer = sdCardFile.readFromMedia( sizeof(uint32_t) * 4, PARTITION_TABLE_1_OFFSET ).getPtr<uint32_t>();
	PartitionTable partitionTables[4] = { PartitionTable( ptBuffer ), PartitionTable( ptBuffer + 4 ), PartitionTable( ptBuffer + 8 ),
						PartitionTable( ptBuffer + 12 ) };
	delete ptBuffer;

	for ( unsigned int partition = 0; partition < 4; partition++ )
	{
		// print partition table info
		// std::cout << "PARTITION #" << std::to_string( partition ) << " ----------------------------------------" << std::endl;
		// printPartitionTable( partitionTables[partition] );

		// read boot sector and print info
		// std::cout << "PARTITION #" << std::to_string( partition ) << " BOOT SECTOR ----------------------------" << std::endl;
		uint8_t* bsBuffer = sdCardFile.readFromMedia( BOOT_SEC_SIZE_IN_BYTES,
						partitionTables[partition].getOffsetLBA() * 512 ).getPtr<uint8_t>();
		BootSector bootSector( bsBuffer );
		delete bsBuffer;
		// printBootSector( bootSector );

		// navigate to root directory and print entries
		unsigned int firstEntryOffset = (partitionTables[partition].getOffsetLBA() + bootSector.getNumReservedSectors() +
						bootSector.getNumFats() * bootSector.getNumSectorsPerFat()) * bootSector.getSectorSizeInBytes();
		uint8_t* entryBuffer = sdCardFile.readFromMedia(FAT16_ENTRY_SIZE * bootSector.getNumDirectoryEntriesInRoot(),
								firstEntryOffset).getPtr<uint8_t>();

		for ( unsigned int entry = 0; entry < bootSector.getNumDirectoryEntriesInRoot(); entry++ )
		{
			Fat16Entry fat16Entry( &entryBuffer[entry * FAT16_ENTRY_SIZE] );

			// if we find an unused entry is found, that's the end of the directory entries
			if ( fat16Entry.isUnusedEntry() )
			{
				break;
			}

			if ( ! fat16Entry.isUnusedEntry() && ! fat16Entry.isDiskVolumeLabel() && ! fat16Entry.isHiddenEntry()
					&& ! fat16Entry.isSystemFile() && ! fat16Entry.isDeletedEntry() )
			{
				std::cout << "FAT16 ENTRY #" << std::to_string( entry ) << "-----------------------------------" << std::endl;
				printFat16Entry( fat16Entry );

				// if txt file, print file to console
				const char* extension = fat16Entry.getExtensionRaw();
				unsigned int dataOffset = firstEntryOffset + ( bootSector.getNumDirectoryEntriesInRoot() * FAT16_ENTRY_SIZE );
				if ( extension[0] == 'T' && extension[1] == 'X' && extension[2] == 'T' )
				{
					// create a buffer for the file to live in, then load it with the file contents
					unsigned int fileOffset = dataOffset + ( (fat16Entry.getStartingClusterNum() - 2) *
									bootSector.getNumSectorsPerCluster() * bootSector.getSectorSizeInBytes() );
					uint8_t* fileBuffer = sdCardFile.readFromMedia(fat16Entry.getFileSizeInBytes(),
											fileOffset).getPtr<uint8_t>();

					for ( unsigned int character = 0; character < fat16Entry.getFileSizeInBytes(); character++ )
					{
						std::cout << fileBuffer[character];
					}
					std::cout << std::endl;

					delete fileBuffer;
				}
				else if ( fat16Entry.isSubdirectory() )
				{
					unsigned int subdirectoryOffset = dataOffset + ( (fat16Entry.getStartingClusterNum() - 2) *
									bootSector.getNumSectorsPerCluster() * bootSector.getSectorSizeInBytes() );
					uint8_t* subdirectoryBuffer = sdCardFile.readFromMedia(FAT16_ENTRY_SIZE * 7,
												subdirectoryOffset).getPtr<uint8_t>();

					// TODO how do we know how many directory entries are in a cluster???
					Fat16Entry subdirectoryEntry( subdirectoryBuffer );
					printFat16Entry( subdirectoryEntry );
					Fat16Entry subdirectoryEntry2( &subdirectoryBuffer[FAT16_ENTRY_SIZE] );
					printFat16Entry( subdirectoryEntry2 );
					Fat16Entry subdirectoryEntry3( &subdirectoryBuffer[FAT16_ENTRY_SIZE * 2] );
					printFat16Entry( subdirectoryEntry3 );
					Fat16Entry subdirectoryEntry4( &subdirectoryBuffer[FAT16_ENTRY_SIZE * 3] );
					printFat16Entry( subdirectoryEntry4 );
					Fat16Entry subdirectoryEntry5( &subdirectoryBuffer[FAT16_ENTRY_SIZE * 4] );
					printFat16Entry( subdirectoryEntry5 );
					Fat16Entry subdirectoryEntry6( &subdirectoryBuffer[FAT16_ENTRY_SIZE * 5] );
					printFat16Entry( subdirectoryEntry6 );
					Fat16Entry subdirectoryEntry7( &subdirectoryBuffer[FAT16_ENTRY_SIZE * 6] );
					printFat16Entry( subdirectoryEntry7 );

					delete subdirectoryBuffer;
				}
			}
		}

		delete entryBuffer;
	}

	return 0;
}
