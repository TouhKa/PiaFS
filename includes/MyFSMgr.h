

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
#include "logger.h"
#include "myfs-structs.h"

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
    int writeInode(Inode* node);
    void write(char* content, uint32_t startBlock);
    int writeRootPointer(uint32_t newPointer);
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
    int createNewInode(char* path, mode_t mode);
    uint32_t changeFileContent(char *path, char *buf, uint32_t size, uint32_t offset);
    void changeTime(Inode* node, bool atim, bool mtim, bool ctim);
    int moveBuffer(DataBuffer* db, int offset);
    int moveBuffer(DataBuffer* db);
    int copyDataToBuffer(char* buf, char read[512], int from, int to, int offset);
};

#endif /* MYFSMGR_H_ */
