//
//  test-myfs.cpp
//  testing
//
//  Created by Oliver Waldhorst on 15.12.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#include "catch.hpp"
#include "helper.hpp"
#include "myfs.h"
#include "MyFSMgr.h"
#include "blockdevice.h"
#include "myfs-structs.h"
#define BD_PATH "/dummy.bin"
#define NUM_TESTBYTES  1500    
#define NUM_BLOCKS 1024 

TEST_CASE("read/write the superblock", "[superblock]"){
    remove(BD_PATH);

    //See: Method "changeSBFileCount".
    SECTION("writing superblock"){                  
        MyFSMgr::instance()->BDInstance()->create(BD_PATH); //create container instance 
        static BlockDevice * bd =  MyFSMgr::instance()->BDInstance();
        MyFSMgr::instance()->fillBlocks(0, BLOCK_COUNT);
        MyFSMgr::instance()->writeSuperBlock();

        char r [BD_BLOCK_SIZE];                    //read first block
        memset(r, 0, BD_BLOCK_SIZE);                //fill buffer with zeros
        bd->read(0, r);                            //read first block                         
        Superblock* sb = (Superblock*) r;           //cast buffer into the right struct
    
        MyFSMgr::instance()->changeSBFileCount(-1);            //change some values
        MyFSMgr::instance()->changeSBFileCount(1);
                                                    
        REQUIRE(sb->size == SYSTEM_SIZE);
        REQUIRE(sb->pointerData == DATA_START);
        REQUIRE(sb->pointerFat == FAT_START);
        REQUIRE(sb->pointerRoot == ROOT_BLOCK);
        REQUIRE(sb->pointerNode == NODE_START);
        REQUIRE(sb->fileCount == 0); 

        MyFSMgr::instance()->BDInstance()->close(); 
    }

    
    remove(BD_PATH);
}

// TEST_CASE("FAT", "[fat]"){
//      remove(BD_PATH);
//     BlockDevice bd;
//     bd.create(BD_PATH);
//     MyFSMgr* fsHelper = fsHelper->instance();

//     SECTION("FAT"){

//         //read empty entry
//         REQUIRE(fsHelper->readNextFATPointer(3) == MAX_UINT);
//         REQUIRE(fsHelper->readNextFATPointer(511) == MAX_UINT);
//         REQUIRE(fsHelper->readNextFATPointer(514) == MAX_UINT);
    
//         //modify
//         fsHelper->setFATBlockPointer(2, 511);
//         fsHelper->setFATBlockPointer(511, 514);
//         REQUIRE(fsHelper->readNextFATPointer(3) == 511);
//         REQUIRE(fsHelper->readNextFATPointer(511) == 514);
//         REQUIRE(fsHelper->readNextFATPointer(511) == MAX_UINT);


//         //delete ENTRY
//         fsHelper->removeFatPointer(3);
//         fsHelper->removeFatPointer(511);
//         REQUIRE(fsHelper->readNextFATPointer(3) == MAX_UINT);
//         REQUIRE(fsHelper->readNextFATPointer(511) == MAX_UINT);
//     }
 

//     bd.close();
//     remove(BD_PATH);
// }


// TEST_CASE("dmap", "[dmap]"){ //Please note: As we read through the FAT Map for any empty entries, there is no real dmap.
//     remove(BD_PATH);
//     BlockDevice bd;
//     bd.create(BD_PATH);
//     MyFSMgr* fsHelper = fsHelper->instance();

//     SECTION("dmap"){
        
//         REQUIRE(fsHelper->findNextFreeBlock() == 0);                //finde next free block

//         fsHelper->setFATBlockPointer(0, 55);                       //set the blocks as used
//         REQUIRE(fsHelper->findNextFreeBlock() == 1);

//         fsHelper->setFATBlockPointer(1, 14);
//         fsHelper->setFATBlockPointer(2, 6);
//         REQUIRE(fsHelper->findNextFreeBlock() == 3);

//         fsHelper->removeFatPointer(0);                              //free block
//         fsHelper->removeFatPointer(1);
//         fsHelper->removeFatPointer(2);
   
//         REQUIRE(fsHelper->findNextFreeBlock() == 0);
//     }
  
//     bd.close();
//     remove(BD_PATH);
// }


// TEST_CASE("imap", "[imap]"){
//     remove(BD_PATH);
//     BlockDevice bd;
//     bd.create(BD_PATH);
//     MyFSMgr* fsHelper = fsHelper->instance();
//     char* file = new char[11];
//      char* file2 = new char[11];
//     strcpy(file, "data3.txt");
//     strcpy(file2, "data4.txt");
   
//     SECTION("imap"){
        
//         uint32_t inodePointer; 
//         uint32_t fatPointer;   
//         uint32_t blockPointer;                                  //inode
//         char nodechar[BLOCK_SIZE];
//         Inode* node = (Inode*) nodechar;
//         fsHelper->fillBlocks(0, BLOCK_COUNT);                      
//         fsHelper->writeSuperBlock();
//         blockPointer = fsHelper->findNextFreeBlock();
//         fsHelper->createInode(file, blockPointer);                   //createInode

//         REQUIRE(fsHelper->findInode(file, node, &inodePointer) == true);    //check if inode exists
//         fatPointer = node->pointer ;
//         REQUIRE(fsHelper->rootPointerCount() == 1);                         //check if there is exact one entry
//         REQUIRE(fsHelper->createNewInode(file2, 777) == 0);                    //check if writing is correct
//         REQUIRE(fsHelper->rootPointerCount() == 2);         //check if there are exact two entries    

//         fsHelper->removeFile(inodePointer);                                 //remove first inode
//         REQUIRE(fsHelper->rootPointerCount() == 1);
//         REQUIRE(fsHelper->findInode(file, node, &inodePointer) == false);   
//         REQUIRE(fsHelper->readNextFATPointer(fatPointer) == MAX_UINT);      //no FAT entry left

//         delete[] node;

//     }
 
//     delete[] file;
//     delete[] file2;
//     delete[] fsHelper;

//     bd.close();
//     remove(BD_PATH);
// }

// TEST_CASE("rootblock", "[rootblock]"){
//     remove(BD_PATH);
//     BlockDevice bd;
//     bd.create(BD_PATH);
//     char* file = new char[11];
    
//     strcpy(file, "data3.txt");
 
   
//     MyFSMgr* fsHelper = fsHelper->instance();     
   
//     SECTION("rootblock"){                  
    
//         uint32_t inodePointer; 
//         uint32_t fatPointer;   
//         uint32_t blockPointer;                                  //inode
//         char nodechar[BLOCK_SIZE];
//         Inode* node = (Inode*) nodechar;
//         fsHelper->fillBlocks(0, BLOCK_COUNT);                      
//         fsHelper->writeSuperBlock();

//         blockPointer = fsHelper->findNextFreeBlock();
//         fsHelper->createInode(file, blockPointer);                   //createInode

//         REQUIRE(fsHelper->findInode(file, node, &inodePointer) == true);    //check if inode exists
//         int entries = fsHelper->readNextRootPointer(node->pointer);
//         REQUIRE(fsHelper->rootPointerCount() == entries);
//     }
//     delete[] file;
//     delete[] fsHelper;  
//     bd.close();
//     remove(BD_PATH);
// }


