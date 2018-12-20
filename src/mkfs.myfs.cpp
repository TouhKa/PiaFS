//
//  mk.myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#include <errno.h>
#include <stdlib.h>
#include <iostream>

#include "myfs.h"
#include "blockdevice.h"
#include "macros.h"
#include "MyFSMgr.h"


/**
 * This method creates an empty container, and writes the given text files in it.
 * Please note: To mount the FS run the script "runMount" or "runUnmount"
 *
 * @param argc The argument count as integer.
 * @param argv All passed arguments as string array.
 * @return 0 on success, nonzero on failure.
 */
int main(int argc, char* argv[]) {

    // Validate arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s containerfile.bin inputfile1 [inputfile2 ...]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    char containerFilePath[1024];

    MyFSMgr::instance()->getAbsPath(argv[1], containerFilePath); //MyFSMgr is a helper class 

    LOG("Initialize block device...\n");
    MyFSMgr::instance()->BDInstance()->create(containerFilePath); //create container instance 
    MyFSMgr::instance()->fillBlocks(0, BLOCK_COUNT);        //fill whole container with zeros starting from block 0 to the last available block 

    LOG("Initialize superblock...\n");
    MyFSMgr::instance()->writeSuperBlock();                 //write the superblock

    LOG("Copying files into container file...\n");
    for (int i = 2; i < argc; i++) {
        if (MyFSMgr::instance()->importFile(argv[i]) == 0)
            printf("Files are imported successfully \n"); 
    }

    MyFSMgr::instance()->BDInstance()->close(); 
    printf("BlockDevice is closed \n");
    return (EXIT_SUCCESS);
}
