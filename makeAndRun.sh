g++ -o main main.cpp SFAT/src/Fat16Entry.cpp SFAT/src/PartitionTable.cpp SFAT/src/BootSector.cpp SFAT/src/Fat16FileManager.cpp DevLib/src/CPPFile.cpp -ISFAT/include -IDevLib/include
./main
