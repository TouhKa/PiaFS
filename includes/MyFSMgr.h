

#ifndef MYFSMGR_H_
#define MYFSMGR_H_

// Global
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <time.h>

//Abs file path
#include <unistd.h>
#include <cstring>

// Project specific
#include "blockdevice.h"
#include "Logger.h"
#include "myfs-structs.h"

// Definitions
//Strucure Superblock | FAT  | Inodes  | Root Block | Data Blocks

// static int const NAME_LENGTH      = 255; // Max. length of a filename
// static int const BLOCK_SIZE       = 512; // Logical Block Size
// static int const BLOCK_COUNT      = 60000; //TODO 

// static int const NUM_DIR_ENTRIES  =  64; // Max. directory entries

// static int const NUM_OPEN_FILES   =  64; // Max. open files per MyFS container file

// static uint32_t const MAX_UINT    =  -1;   //Default Value     
// static uint32_t const SIZE        =  MAX_UINT;  
// static uint32_t const SYSTEM_SIZE =  BLOCK_SIZE * BLOCK_COUNT; // 3000KiB

// static uint32_t const FAT_START   =  1; //2. block
// static uint32_t const FAT_SIZE    =  (SYSTEM_SIZE / BLOCK_SIZE) / (BLOCK_SIZE / 4); //As the FAT saves pointers with 4 bytes, we need to check howm many blocks we need
// static uint32_t const FAT_ENDE    =  FAT_START + FAT_SIZE;

// static uint32_t const NODE_START  =  FAT_ENDE + 1; //Start INodes
// static uint32_t const NODE_ENDE   =  NODE_START + NUM_DIR_ENTRIES - 1; //As the root block lies in specific area. 

// static uint32_t const ROOT_BLOCK  =  NODE_ENDE + 1; 

// static uint32_t const DATA_START  =  ROOT_BLOCK + 1;

// //The use of structs simplifys the access of the dat ato read or to write
// struct Superblock {
//     uint32_t size;
//     uint32_t pointerFat;
//     uint32_t pointerNode;
//     uint32_t pointerRoot;
//     uint32_t pointerData;
//     uint32_t fileCount;
// };

// struct FATBlock{
//     uint32_t pointer[BLOCK_SIZE/4]; //TODO
// };

// struct Inode {
//     char fileName[NAME_LENGTH]; // Max. 255 bytes 
//     uint32_t size;              // 4 byte
//     short gid;                  // 2 byte
//     short uid;                  // 2 byte
//     short mode;                 // 2 byte
//     uint32_t atim;              // 4 byte
//     uint32_t mtim;              // 4 byte
//     uint32_t ctim;              // 4 byte
//     uint32_t pointer;           // 4 byte
// };

// struct DataBuffer {
//     uint32_t blockNumber;
//     uint32_t dataPointer;
//     char data[BLOCK_SIZE];      
// };

// struct RootDirect {
//     uint32_t pointer[BLOCK_SIZE/4];
// };

/**
 * Provides methods to handle MyFS.
 */
class MyFSMgr {
private:

    static MyFSMgr* _instance;
    static BlockDevice* _blockDevice;
public:
    

    MyFSMgr();
    virtual ~MyFSMgr();


    static MyFSMgr* instance();
    static BlockDevice* BDInstance();

    void getAbsPath(char* arg, char* path);
         // Operations
    int rootPointerCount();         
    uint32_t findNextFreeBlock();
    Inode* readNode(uint32_t nodePointer);
    void createInode(char* path, uint32_t blockPointer);
    void writeInode(Inode* node);
    void write(char* content, uint32_t startBlock);
    void writeRootPointer(uint32_t newPointer);
    void setFATBlockPointer(uint32_t blockPointer, uint32_t nextPointer);
    void removeFatPointer(uint32_t delPointer);
    void removeRootPointer(uint32_t delPointer);
    void deleteFollowingBlocks(uint32_t dataPointer);


    // BlockDevice
    void fillBlocks(uint32_t startBlockIndex, uint32_t endBlockIndex);
    void writeSuperBlock();
    void changeSBFileCount(int difference);

    // Operations
    int importFile(char* path);
    bool findInode(char* fileName, Inode* node, uint32_t* nodePointer);
    bool fileExists(char* path);
    void removeFile(uint32_t nodePointer);
    uint32_t readNextRootPointer(uint32_t position);
    uint32_t readNextFATPointer(uint32_t blockPointer);
    void createNewInode(char* path, mode_t mode);
    uint32_t changeFileContent(char *path, char *buf, uint32_t size, uint32_t offset);
    void changeTime(Inode* node, bool atim, bool mtim, bool ctim);
    int moveBuffer(DataBuffer* db, int offset);
    int moveBuffer(DataBuffer* db);
    int copyDataToBuffer(char* buf, char read[512], int from, int to, int offset);
};

#endif /* MYFSMGR_H_ */
