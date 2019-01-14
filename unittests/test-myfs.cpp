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

TEST_CASE("FAT", "[fat]"){
     remove(BD_PATH);

    SECTION("FAT"){
        MyFSMgr::instance()->BDInstance()->create(BD_PATH); //create container instance 
        static BlockDevice * bd =  MyFSMgr::instance()->BDInstance();
        MyFSMgr::instance()->fillBlocks(0, BLOCK_COUNT);
        MyFSMgr::instance()->writeSuperBlock();

        //read random emty entry
        REQUIRE(MyFSMgr::instance()->readNextFATPointer(511) == 0);
        REQUIRE(MyFSMgr::instance()->readNextFATPointer(514) == 0);
    
        //modify
        MyFSMgr::instance()->setFATBlockPointer(511, 514);
        REQUIRE(MyFSMgr::instance()->readNextFATPointer(511) == 514);
  
        //delete ENTRY
        MyFSMgr::instance()->removeFatPointer(511);
        REQUIRE(MyFSMgr::instance()->readNextFATPointer(511) == 0);

        MyFSMgr::instance()->BDInstance()->close();
      
    }
 
    remove(BD_PATH);
}


TEST_CASE("dmap", "[dmap]"){ //Please note: As we read through the FAT Map for any empty entries, there is no real dmap.
    remove(BD_PATH);
  

    SECTION("dmap"){
        MyFSMgr::instance()->BDInstance()->create(BD_PATH); //create container instance 
        static BlockDevice * bd =  MyFSMgr::instance()->BDInstance();
        MyFSMgr::instance()->fillBlocks(0, BLOCK_COUNT);
        MyFSMgr::instance()->writeSuperBlock();

        u_int32_t free1 = MyFSMgr::instance()->findNextFreeBlock();
        MyFSMgr::instance()->setFATBlockPointer(free1, 1);  
        
        u_int32_t free2 = MyFSMgr::instance()->findNextFreeBlock();  
         MyFSMgr::instance()->setFATBlockPointer(free1, free2);                     //set the blocks as used
        REQUIRE(MyFSMgr::instance()->readNextFATPointer(free1) == free2);
        REQUIRE(MyFSMgr::instance()->readNextFATPointer(free2) == 0);
        MyFSMgr::instance()->removeFatPointer(free1);                              //free block
        MyFSMgr::instance()->removeFatPointer(free2);
      
        REQUIRE(MyFSMgr::instance()->findNextFreeBlock() == free1);

        MyFSMgr::instance()->BDInstance()->close();
    }

    remove(BD_PATH);
}


TEST_CASE("imap", "[imap]"){
    remove(BD_PATH);
    SECTION("imap"){

        MyFSMgr::instance()->BDInstance()->create(BD_PATH); //create container instance 
        static BlockDevice * bd =  MyFSMgr::instance()->BDInstance();
        MyFSMgr::instance()->fillBlocks(0, BLOCK_COUNT);
        MyFSMgr::instance()->writeSuperBlock();
        char file [11];
        char file2 [11];
        strcpy(file, "data3.txt");
        strcpy(file2, "data4.txt");

        uint32_t inodePointer; 
        uint32_t fatPointer;   
        uint32_t blockPointer;                                  //inode
        char nodechar[BLOCK_SIZE];
        Inode* node = (Inode*) nodechar;
        blockPointer = MyFSMgr::instance()->findNextFreeBlock();
        MyFSMgr::instance()->createInode(file, blockPointer);                   //createInode

        REQUIRE(MyFSMgr::instance()->findInode(file, node, &inodePointer) == true);    //check if inode exists
        fatPointer = node->pointer ;
        REQUIRE(MyFSMgr::instance()->rootPointerCount() == 1);                         //check if there is exact one entry
        REQUIRE(MyFSMgr::instance()->createNewInode(file2, 777) == 0);                    //check if writing is correct
        REQUIRE(MyFSMgr::instance()->rootPointerCount() == 2);         //check if there are exact two entries    

        MyFSMgr::instance()->removeFile(inodePointer);                                 //remove first inode
        REQUIRE(MyFSMgr::instance()->rootPointerCount() == 1);
        REQUIRE(MyFSMgr::instance()->findInode(file, node, &inodePointer) == false);   
        REQUIRE(MyFSMgr::instance()->readNextFATPointer(fatPointer) == 0);      //no FAT entry left

        MyFSMgr::instance()->BDInstance()->close();
    }
 
    remove(BD_PATH);
}

TEST_CASE("rootblock", "[rootblock]"){
    remove(BD_PATH);

    SECTION("rootblock"){     
        MyFSMgr::instance()->BDInstance()->create(BD_PATH); //create container instance 
        static BlockDevice * bd =  MyFSMgr::instance()->BDInstance();
        MyFSMgr::instance()->fillBlocks(0, BLOCK_COUNT);
        MyFSMgr::instance()->writeSuperBlock();             
        char file[11];
        strcpy(file, "data3.txt");

        uint32_t inodePointer; 
        uint32_t fatPointer;   
        uint32_t blockPointer;                                  //inode
        char nodechar[BLOCK_SIZE];
        Inode* node = (Inode*) nodechar;
    
        blockPointer = MyFSMgr::instance()->findNextFreeBlock();
        MyFSMgr::instance()->createInode(file, blockPointer);                   //createInode

        REQUIRE(MyFSMgr::instance()->findInode(file, node, &inodePointer) == true);    //check if inode exists
        int entries = MyFSMgr::instance()->rootPointerCount();
        REQUIRE(MyFSMgr::instance()->rootPointerCount() == entries);
    }
   
    remove(BD_PATH);
}


