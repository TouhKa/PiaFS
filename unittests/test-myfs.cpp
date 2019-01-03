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
    BlockDevice bd;
    bd.create(BD_PATH);
    
    MyFSMgr* fsHelper = fsHelper->instance();     
    
    //See: Method "changeSBFileCount".
    SECTION("writing superblock"){                  
        
        fsHelper->fillBlocks(0, BLOCK_COUNT);
        fsHelper->writeSuperBlock();

        char* r= new char[BD_BLOCK_SIZE];           //read first block
        memset(r, 0, BD_BLOCK_SIZE);                //fill buffer with zeros
        bd.read(0, r);                              //read first block
        Superblock* sb = (Superblock*) r;           //cast buffer into the right struct
        

        fsHelper->changeSBFileCount(-1);            //change some values
        fsHelper->changeSBFileCount(1);
                                                    //check if the written data is written right
        REQUIRE(sb->size == SYSTEM_SIZE);
        REQUIRE(sb->pointerData == DATA_START);
        REQUIRE(sb->pointerFat == FAT_START);
        REQUIRE(sb->pointerRoot == ROOT_BLOCK);
        REQUIRE(sb->pointerNode == NODE_START);
        REQUIRE(sb->fileCount == 0); 

        delete[] r;
    }

    delete[] fsHelper;  
    bd.close();
    remove(BD_PATH);
}

TEST_CASE("read/write FAT", "[fat]"){
     remove(BD_PATH);
    BlockDevice bd;
    bd.create(BD_PATH);
    MyFSMgr* fsHelper = fsHelper->instance();

    SECTION("FAT"){

        //read empty entry
        REQUIRE(fsHelper->readNextFATPointer(3) == MAX_UINT);
        REQUIRE(fsHelper->readNextFATPointer(511) == MAX_UINT);
        REQUIRE(fsHelper->readNextFATPointer(514) == MAX_UINT);
    
        //modify
        fsHelper->setFATBlockPointer(2, 511);
        fsHelper->setFATBlockPointer(511, 514);
        REQUIRE(fsHelper->readNextFATPointer(3) == 511);
        REQUIRE(fsHelper->readNextFATPointer(511) == 514);
        REQUIRE(fsHelper->readNextFATPointer(511) == MAX_UINT);


        //delete ENTRY
        fsHelper->removeFatPointer(3);
        fsHelper->removeFatPointer(511);
        REQUIRE(fsHelper->readNextFATPointer(3) == MAX_UINT);
        REQUIRE(fsHelper->readNextFATPointer(511) == MAX_UINT);
    }
 

    bd.close();
    remove(BD_PATH);
}


TEST_CASE("read/write", "[dmap]"){ //Please note: As we read through the FAT Map for any empty entries, there is no real dmap.
    remove(BD_PATH);
    BlockDevice bd;
    bd.create(BD_PATH);
    MyFS* fs = new MyFS();

    SECTION("dmap"){
        //finde next free block

        //set this block as used


        //free block

    }
  
    bd.close();
    remove(BD_PATH);
}


TEST_CASE("read/write", "[imap]"){
    remove(BD_PATH);
    BlockDevice bd;
    bd.create(BD_PATH);
    MyFS* fs = new MyFS();

    SECTION("imap"){
        //find free inode


        //write inode

        //read imap entry  and number of entrie in superblock 


    }
 

    bd.close();
    remove(BD_PATH);
}

TEST_CASE("read/write the rootblock", "[rootblock]"){
    remove(BD_PATH);
    BlockDevice bd;
    bd.create(BD_PATH);
    
    MyFSMgr* fsHelper = fsHelper->instance();     
    
    //See: Method "changeSBFileCount".
    SECTION("rootblock"){                  
      //set inode

      //update inode


      //exists file?

      //get filename
    }

    delete[] fsHelper;  
    bd.close();
    remove(BD_PATH);
}


