#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <iostream>

#include "CPPFile.hpp"
#include "Fat16Entry.hpp"
#include "PartitionTable.hpp"
#include "BootSector.hpp"
#include "Fat16FileManager.hpp"
#include "IAllocator.hpp"

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

void printAllEntriesInDirectory (Fat16FileManager& fileManager)
{
	std::vector<Fat16Entry*>& directoryEntries = fileManager.getCurrentDirectoryEntries();
	std::vector<Fat16Entry*>::iterator entry = directoryEntries.begin();

	unsigned int entryNum = 0;
	while ( ! (*entry)->isUnusedEntry() && entry != directoryEntries.end() )
	{

		if ( ! (*entry)->isDeletedEntry()
				&& ! (*entry)->isHiddenEntry()
				&& ! (*entry)->isSystemFile()
				&& ! (*entry)->isDiskVolumeLabel() )
		{
			std::cout << "ENTRY NUMBER " << std::to_string( entryNum ) << " ------------------" << std::endl;
			printFat16Entry( **entry );
			std::cout << std::endl;

		}

		entry++;
		entryNum++;
	}
}

int main()
{
	CPPFile sdCardFile( "SDCard.img" );
	if ( sdCardFile.needsInitialization() )
	{
		std::cout << "Couldn't find file :(" << std::endl;
	}

	uint8_t blockOfMemory[524288]; // 512kB
	IAllocator allocator( blockOfMemory, 524288 );

	Fat16FileManager fileManager( sdCardFile, &allocator );
	if ( ! fileManager.isValidFatFileSystem() )
	{
		std::cout << "Couldn't find a valid FAT file system :(" << std::endl;
	}

	std::vector<PartitionTable>* partitions = fileManager.getPartitionTables();
	for ( unsigned int partitionTableNum = 0; partitionTableNum < partitions->size(); partitionTableNum++ )
	{
		std::cout << "PARTITION TABLE " << std::to_string(partitionTableNum) << " ---------------------------------" << std::endl;
		printPartitionTable( partitions->at(partitionTableNum) );
		std::cout << "END OF PARTITION TABLE " << std::to_string(partitionTableNum) << " ---------------------------------" << std::endl;
	}
	std::cout << std::endl << std::endl << std::endl;

	std::cout << "BOOT SECTOR --------------------------------------------------------------" << std::endl;
	printBootSector( *fileManager.getActiveBootSector() );
	std::cout << "END OF BOOT SECTOR -------------------------------------------------------" << std::endl << std::endl << std::endl;

	printAllEntriesInDirectory( fileManager );

	std::cout << "Navigate through FAT16 file system (type 'print' to print current directory, type 'delete' to delete an entry, "
		<< "type 'write' to write a file, or type 'quit' to exit)" << std::endl;

	while ( true )
	{
		std::string inputVal;

		std::cout << "Select entry by number: ";
		std::getline( std::cin, inputVal );

		if ( inputVal == "quit" )
		{
			break;
		}
		else if ( inputVal == "print" )
		{
			printAllEntriesInDirectory( fileManager );
		}
		else if ( inputVal == "delete" )
		{
			std::cout << "Delete entry by number: ";
			std::getline( std::cin, inputVal );

			try
			{
				unsigned int entryNum = static_cast<unsigned int>( std::stoi(inputVal) );
				std::cout << "Deleting: " << std::to_string( entryNum ) << std::endl;

				bool success = fileManager.deleteEntry( entryNum );
				if ( success )
				{
					std::cout << "Successful deletion" << std::endl;
				}
				else
				{
					std::cout << "Unable to delete" << std::endl;
				}
			}
			catch ( std::invalid_argument )
			{
				std::cout << "Couldn't convert that string to a valid number" << std::endl;
			}
			catch ( std::out_of_range )
			{
				std::cout << "This number is out of integer range" << std::endl;
			}
		}
		else if ( inputVal == "write" )
		{
			std::cout << "File name (extension will be .txt): ";
			std::getline( std::cin, inputVal );

			if ( inputVal.length() > 8 )
			{
				std::cout << "This filename is too large, must be 8 characters max" << std::endl;
			}
			else
			{
				bool needToFinalize = true;

				Fat16Entry newFile( inputVal, "txt" );
				if ( ! fileManager.createEntry(newFile) )
				{
					std::cout << "Unable to create file" << std::endl;
				}
				else
				{
					while ( true )
					{
						std::cout << "Write characters to repeat (or 'end' to finalize the file): ";
						std::getline( std::cin, inputVal );

						if ( inputVal == "end" ) break;

						std::string charsToRepeat = inputVal;

						std::cout << "How many times to repeat (or 'flush' to finalize the file with the current text): ";
						std::getline( std::cin, inputVal );

						if ( inputVal == "flush" )
						{
							const char* characters = charsToRepeat.c_str();
							unsigned int numCharsInString = strlen( characters );
							SharedData<uint8_t> data = SharedData<uint8_t>::MakeSharedData( numCharsInString );
							for ( unsigned int byte = 0; byte < numCharsInString; byte++ )
							{
								data[byte] = static_cast<uint8_t>( characters[byte % numCharsInString] );
							}

							bool success = fileManager.flushToEntry( newFile, data );
							if ( success )
							{
								std::cout << "Successful flush" << std::endl;
							}
							else
							{
								std::cout << "Unable to flush" << std::endl;
							}

							needToFinalize = false;

							break;
						}

						try
						{
							unsigned int numTimesToRepeat = static_cast<unsigned int>( std::stoi(inputVal) );
							const char* characters = charsToRepeat.c_str();
							unsigned int numCharsInString = strlen( characters );
							SharedData<uint8_t> data =
								SharedData<uint8_t>::MakeSharedData( numCharsInString * numTimesToRepeat );
							for ( unsigned int byte = 0; byte < numCharsInString * numTimesToRepeat; byte++ )
							{
								data[byte] = static_cast<uint8_t>( characters[byte % numCharsInString] );
							}

							bool success = fileManager.writeToEntry( newFile, data );
							if ( success )
							{
								std::cout << "Successful write" << std::endl;
							}
							else
							{
								std::cout << "Unable to write" << std::endl;
							}
						}
						catch ( std::invalid_argument )
						{
							std::cout << "Couldn't convert that string to a valid number" << std::endl;
						}
						catch ( std::out_of_range )
						{
							std::cout << "This number is out of integer range" << std::endl;
						}
					}
				}

				if ( needToFinalize )
				{
					if ( fileManager.finalizeEntry(newFile) )
					{
						std::cout << "File successfully finalized" << std::endl;
					}
					else
					{
						std::cout << "Unable to finalize file" << std::endl;
					}
				}
			}
		}
		else
		{
			try
			{
				unsigned int entryNum = static_cast<unsigned int>( std::stoi(inputVal) );
				std::cout << "Value chosen: " << std::to_string( entryNum ) << std::endl;

				Fat16Entry entry = fileManager.selectEntry( entryNum );
				printFat16Entry( entry );

				if ( fileManager.readEntry(entry) )
				{
					SharedData<uint8_t> fileData = fileManager.getSelectedFileNextSector( entry );
					unsigned int fileDataByteCounter = 0;
					for ( unsigned int character = 0; character < entry.getFileSizeInBytes(); character++ )
					{
						if ( character == 0 )
						{
							std::cout << std::endl;
							std::cout << "FILE OUTPUT ------------------------------------------------------" << std::endl;
						}

						// if we've read all the bytes in this sector, get a new one
						if ( fileDataByteCounter == fileData.getSizeInBytes() )
						{
							fileData = fileManager.getSelectedFileNextSector( entry );
							fileDataByteCounter = 0;
						}

						std::cout << fileData[fileDataByteCounter];

						fileDataByteCounter++;
					}
					std::cout << std::endl;
				}
			}
			catch ( std::invalid_argument )
			{
				std::cout << "Couldn't convert that string to a valid number" << std::endl;
			}
			catch ( std::out_of_range )
			{
				std::cout << "This number is out of integer range" << std::endl;
			}
		}
	}

	return 0;
}
