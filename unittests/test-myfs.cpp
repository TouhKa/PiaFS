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
    
    MyFSMgr* fsHelper = fsHelper->instance();       //See: Method "changeSBFileCount"
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

TEST_CASE("read/write", "[fat]"){
     remove(BD_PATH);
    BlockDevice bd;
    bd.create(BD_PATH);
    MyFS* fs = new MyFS();

    SECTION("writing superblock"){
        
    }
 

    bd.close();
    remove(BD_PATH);
}


TEST_CASE("read/write", "[dmap]"){
    remove(BD_PATH);
    BlockDevice bd;
    bd.create(BD_PATH);
    MyFS* fs = new MyFS();

    SECTION("writing superblock"){
        
    }
  
    bd.close();
    remove(BD_PATH);
}


TEST_CASE("read/write", "[blockdevice]"){
    remove(BD_PATH);
    BlockDevice bd;
    bd.create(BD_PATH);
    MyFS* fs = new MyFS();

    SECTION("writing superblock"){
        
    }
 

    bd.close();
    remove(BD_PATH);
}


TEST_CASE("read/write", "[]"){
    remove(BD_PATH);
    BlockDevice bd;
    bd.create(BD_PATH);
    MyFS* fs = new MyFS();

    SECTION("")

    bd.close();
    remove(BD_PATH);
}


