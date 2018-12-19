//
//  myfs-structs.h
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#ifndef myfs_structs_h
#define myfs_structs_h

#define NAME_LENGTH 255
#define BLOCK_SIZE 512
#define NUM_DIR_ENTRIES 64
#define NUM_OPEN_FILES 64

static int const BLOCK_COUNT      = 60000; //TODO 
static uint32_t const MAX_UINT    =  -1;   //Default Value     
static uint32_t const SIZE        =  MAX_UINT;  
static uint32_t const SYSTEM_SIZE =  BLOCK_SIZE * BLOCK_COUNT; // 3000KiB

static uint32_t const FAT_START   =  1; //2. block
static uint32_t const FAT_SIZE    =  (SYSTEM_SIZE / BLOCK_SIZE) / (BLOCK_SIZE / 4); //As the FAT saves pointers with 4 bytes, we need to check howm many blocks we need
static uint32_t const FAT_ENDE    =  FAT_START + FAT_SIZE;

static uint32_t const NODE_START  =  FAT_ENDE + 1; //Start INodes
static uint32_t const NODE_ENDE   =  NODE_START + NUM_DIR_ENTRIES - 1; //As the root block lies in specific area. 

static uint32_t const ROOT_BLOCK  =  NODE_ENDE + 1; 

static uint32_t const DATA_START  =  ROOT_BLOCK + 1;

//The use of structs simplifys the access of the dat ato read or to write
 struct Superblock {
    uint32_t size;
    uint32_t pointerFat;
    uint32_t pointerNode;
    uint32_t pointerRoot;
    uint32_t pointerData;
    uint32_t fileCount;
};

struct FATBlock{
    uint32_t pointer[BLOCK_SIZE/4]; //TODO
};

struct Inode {
    char fileName[NAME_LENGTH]; // Max. 255 bytes 
    uint32_t size;              // 4 byte
    short gid;                  // 2 byte
    short uid;                  // 2 byte
    short mode;                 // 2 byte
    uint32_t atim;              // 4 byte
    uint32_t mtim;              // 4 byte
    uint32_t ctim;              // 4 byte
    uint32_t pointer;           // 4 byte
};

struct DataBuffer {
    uint32_t blockNumber;
    uint32_t dataPointer;
    char data[BLOCK_SIZE];      
};

struct RootDirect {
    uint32_t pointer[BLOCK_SIZE/4];
};


#endif /* myfs_structs_h */