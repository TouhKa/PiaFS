#include "MyFSMgr.h"
using namespace std;

MyFSMgr* MyFSMgr::_instance = NULL;     //static instance of this class
BlockDevice* MyFSMgr::_blockDevice = NULL;  //static instance of a blockdevice

/**
 * Aufgabe 1
 * Access the MyFS manager instance.
 *
 * @return The current MyFS manager instance.
 */
MyFSMgr* MyFSMgr::instance() {
    if(_instance == NULL) {
        _instance = new MyFSMgr();
    }
    return _instance;
}

/**
 * Aufgabe 1
 * Access the BlockDevice instance.
 *
 * @return The current BlockDevice instance.
 */
BlockDevice* MyFSMgr::BDInstance() {
    if(_blockDevice == NULL) {
        _blockDevice = new BlockDevice(BLOCK_SIZE);
    }
    return _blockDevice;
}


MyFSMgr::MyFSMgr() {
}

MyFSMgr::~MyFSMgr() {
    // delete[] _instance;
    // delete[] _blockDevice;
}


/**
 * Aufgabe 1
 * Determine the full path of a given file which must be inside the current working directory.
 * 
 * @param arg The name of a given file inside the current working directory.
 * @param path out The absolute path of the file.
 */
void MyFSMgr::getAbsPath(char* arg, char* path) {
    char currentWorkingDirectory[1024];

    // Get working directory and check if it couldn't be determined or SIZE was too small
    if (getcwd(currentWorkingDirectory, sizeof(currentWorkingDirectory)) == NULL)
       return;

    string absolutePath = (string)currentWorkingDirectory + "/" + (string)arg;
    strncpy(path, absolutePath.c_str(), 1024);
}

/**
 * Aufgabe 1
 * Fill the specified blocks of the BlockDevice with zeros.
 *
 * @param startBlockIndex Index of the first block to fill.
 * @param endBlockIndex Index of the last block to fill.
 */
void MyFSMgr::fillBlocks(uint32_t startBlockIndex, uint32_t endBlockIndex) {
    char rawBlock[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++)        //get a block full o zeros
        rawBlock[i] = 0; 

    for (uint32_t i = startBlockIndex; i < endBlockIndex; i++){     //write for every part of the given  interall the empty block
       // LOGF("Filled Block %i\n",i);
        _blockDevice->write(i, rawBlock);
    }
}

/**
 * Aufgabe 1
 * Write the SuperBlock into the FileSystem. The SuperBlock is always
 * the first block in a FileSystem and contains information about where which part of the FS is to be found.
 */
void MyFSMgr::writeSuperBlock() {
    char copy[BLOCK_SIZE];
    Superblock* sb = (Superblock*) copy;
    LOGF("\n| SUPER BLOCK | FAT %u - %u | INODES %u - %u | ROOT BLOCK %u  | DATA %u - %u |\n\n",
         FAT_START, FAT_ENDE, NODE_START, NODE_ENDE, ROOT_BLOCK, DATA_START, SYSTEM_SIZE);

    sb->size = SYSTEM_SIZE;
    sb->pointerData = DATA_START;
    sb->pointerFat = FAT_START;
    sb->pointerRoot = ROOT_BLOCK;
    sb->pointerNode = NODE_START;
    sb->fileCount = 0;              //when init the superblock ther are no files yet

    _blockDevice->write(0, (char*) sb);
}

/**
 * Aufgabe 1
 * Change the stored file count inside the superblock
 *
 * @param difference The signed amount to inc-/decrement.
 */
void MyFSMgr::changeSBFileCount(int difference) {
    char read[BLOCK_SIZE]; //buffer
    int isSuccess =_blockDevice->read(0, read); //read 1. block which is the superblocks into the buffer
    LOGF("Supberblock is read with status : %u \n", isSuccess);

    Superblock* sb = (Superblock*) read;   //cast the char buffer into the structure of the superblock

    if (difference > 0) {   
                        //add a file
        sb->fileCount += difference;
        LOGF("Total files : %u \n", sb->fileCount);

    } else if (difference != 0 && sb->fileCount >= (uint32_t) abs(difference)) { //delete a file
        sb->fileCount += difference;
        LOGF("Total files : %u \n", sb->fileCount);

    } else {
        LOGF("No files to import %d!", difference);
    }
    _blockDevice->write(0, (char*) sb);     //update superblock
}

/**
 * Aufgabe 1
 * Imports a file to the FileSystem. Checks if the file already exists,
 * if not creates an Inode and write the data into the DataBlocks. If
 * needed links multiple blocks together in the FAT.
 *
 * @param path The path to the file that should be imported.
 * @return  0 On success.
 *          -1 If the file already exists.
 */
int MyFSMgr::importFile(char* path) {
    if (fileExists(path))
        return -1;
    
    FILE* f = fopen(path, "rb"); //Mode = Read binary file
    fseek(f, 0, SEEK_END);      //sets the file position indicator of the file, relative to the end of the file.
    long fsize = ftell(f);      // obtain the current value  of  the  file  position-> et file size in bytes
    fseek(f, 0, SEEK_SET);      //reset the file postiion indicator to the start of the file

    char* fileContent = (char*) malloc(fsize + 1);   
    fread(fileContent, fsize, 1, f);                //read 1 element with fsize from the input file into the buffer
    fclose(f);
   
   
    uint32_t blockPointer = findNextFreeBlock();  
    
    if (blockPointer == 0) {
        printf("Couldnt import the new file: no free blocks left!"); 
        return -1;
    }
        
   
    createInode(path, blockPointer);

    printf("Importing file: %s\n", path);

    write(fileContent, blockPointer);

    changeSBFileCount(1); //increase the nr of files in the root directory 
    return 0;
}

/**
 * Generalized method for all kinds of writing
 * @param content: the content to write
 * @param startBlock : the Block from where to start writing
 * */
void MyFSMgr::write(char* content, uint32_t startBlock){
//    größer?? --> such neuen Block, setze Fatpointer, Fülle inhalt, wiederhole
    LOGM();
    char write[BLOCK_SIZE];  //buffer for 1 block

    uint32_t oldBlock =0;
    uint32_t writtenBlock = startBlock;
    uint32_t blocksUsed = 0;
    uint32_t sizeToCopy;
    

    while (blocksUsed * BLOCK_SIZE < strlen(content)){ // write a block till all content is persistent 

        if(writtenBlock != startBlock){             //needed for the 2. +++ blocks: set reference in FAT table
            setFATBlockPointer(oldBlock, writtenBlock);
        }

        sizeToCopy = strlen(content) - blocksUsed*BLOCK_SIZE;   //

        if (sizeToCopy > BLOCK_SIZE){
            sizeToCopy = BLOCK_SIZE;
        }
        memset(&write[0], 0, BLOCK_SIZE);           //fill 255 byte with zeros into the buffer
        memcpy(write, content + (blocksUsed * BLOCK_SIZE), sizeToCopy);  //copy content into buffer

        _blockDevice->write(writtenBlock, write);       //write the current block
        oldBlock = writtenBlock;                        
        if(readNextFATPointer(writtenBlock) == MAX_UINT || readNextFATPointer(writtenBlock) == 0){
            setFATBlockPointer(writtenBlock, MAX_UINT);       //set end of block by -1
            writtenBlock = findNextFreeBlock();                //get next block in FAT that contains the default value
        } else {
            writtenBlock = readNextFATPointer(writtenBlock);    
        }

        blocksUsed++;
    }

    if(readNextFATPointer(oldBlock) != 0)    {

         //delete any left reference from a deleted file
        deleteFollowingBlocks(oldBlock);
        setFATBlockPointer(oldBlock, MAX_UINT);         //set the end of the linked data blocks

    }    

}
/**
 * Aufgabe 2: is called from FUSE WRITE
 * 
 * 
 * 
*/
uint32_t MyFSMgr::changeFileContent(char *path, char *buf, uint32_t size, uint32_t offset){
    char* fileName = basename(path);
    uint32_t inodePointer;
    char nodechar[BLOCK_SIZE];
    char copy[BLOCK_SIZE];
    char* bufcopy;
    uint32_t fileSize = size + offset;
    Inode* node = (Inode*) nodechar;
    if(!findInode(fileName, node, &inodePointer)){
        LOGF("Inode zum schreiben nicht gefunden '%s'.\n", fileName);
        return EBADF;  //file existiert nicht
    }

    uint32_t pointer = node->pointer;
    uint32_t oldPointer = 0;

    while (offset>= BLOCK_SIZE){
        oldPointer = pointer;
        pointer = readNextFATPointer(pointer);
        offset -= BLOCK_SIZE;
    }

    if(pointer == MAX_UINT){
        pointer = findNextFreeBlock();
        setFATBlockPointer(oldPointer, pointer);
    }

    if(offset != 0){
        // Inhalt des aktuellen Blocks (bis offseet) vorne an buff dranhängen;
        LOGF("offset ist nicht durch 512 teilbar (%i) -> innerhalb eines Blockes\n", offset);
        _blockDevice->read(pointer, copy);
        bufcopy = new char[offset + size + 1];

        memcpy(bufcopy, copy, offset);
        memcpy(bufcopy + offset, buf, size);
        bufcopy[offset+size] = 0;


    }else{
        bufcopy = new char[size + 1];
        memcpy(bufcopy, buf, size);
        bufcopy[size] = 0;
    }

    LOGF("Target-Pointer: %d, buffer:\n%s, (Alte size: %i)\n", pointer,bufcopy, node->size);
    write(bufcopy,pointer);

    changeTime(node, false, true, true);
    node->size = fileSize;
    LOGF("New Inode size: %i\n", node->size);
    _blockDevice->write(inodePointer, (char*) node);
    delete[] bufcopy;
    return size;

}



/**
 * Aufgabe 1
 * Search for a free DataBlocks. Loops through the FAT and checks every
 * block if its empty (=0)
 *
 * @return  A pointer to a free DataBlock
 *          0 If the FileSystem is full
 */
uint32_t MyFSMgr::findNextFreeBlock() {
    uint32_t result;

    char read[BLOCK_SIZE];                          //buffer
    for(uint32_t i = FAT_START; i <= FAT_ENDE; i++) {
        _blockDevice->read(i, read);                //loop throug FAT blocks. 
    
        for (int x = 0; x < 128; x++) { // Max. entries within a block
            result = 0;
            for(int m = (4 * x); m < (4 * (x + 1)); m++) { //get valueof Entry Size = 4 Bytes
                result <<= 8;
                result |= read[m];          //read 1 bytes
            }
           
            if (result == 0) //free block
                return ((i - FAT_START) * 128) + x + DATA_START; // retrun position within the data blocks: offset within FAT * 128, 
        }
    }

    LOG("FindNextFreeBlock: No empty block left");
    return 0; 
}

/**Aufgabe 1
 * Create a Inode of a given file. A Inode contains meta
 * data about the file.
 *
 * @param path The path to the file.
 * @param blockPointer The pointer to the first DataBlock.
 */
void MyFSMgr::createInode(char* path, uint32_t blockPointer) {
    char copy[BLOCK_SIZE]; // 1 Block for 1 Inode
    Inode* node = (Inode*) copy;
    char* pathSegments = strtok(path, "/"); //spilt by "/" into string tokens. At the end PathSegments contains just the file name

    struct stat meta;   //get metdata from a specific file
    stat(path, &meta);  //get metadata from the given file

    strcpy(node->fileName, pathSegments); //set file name
    node->size = meta.st_size;             //Attribut aus den meta informationen
    node->gid = getgid();
    node->uid = getuid();
    node->mode = meta.st_mode;

    node->atim = meta.st_atim.tv_sec;
    node->mtim = meta.st_mtim.tv_sec;
    node->ctim = meta.st_ctim.tv_sec;

    node->pointer = blockPointer;

    writeInode(node);
}

/**
 * Aufgabe 1
 * Called From FUSE GETATTR -> Updates a Inode
 *
 * @param path The name of the file.
 */
int MyFSMgr::createNewInode(char* path, mode_t mode){	
    char copy[BLOCK_SIZE];      
    Inode* node = (Inode*) copy;

    char* fileName = basename(path);
    LOGF("Creating file: %s\n", fileName);

    struct stat meta;
    stat(path, &meta);

    strcpy(node->fileName, fileName);
    node->size = 0;
    node->gid = getgid();
    node->uid = getuid();
    node->mode = mode;              

    changeTime(node, true, true, true);

    node->pointer = findNextFreeBlock();

    setFATBlockPointer(node->pointer, MAX_UINT);

    LOGF("Write Inode of file: %s\n", node->fileName);

    return writeInode(node);
}

/**
 * Aufgabe1
 * Write the provided Inode to the first empty block.
 *
 * @param node The Inode to be written.
 */
int MyFSMgr::writeInode(Inode* node) {
    char read[BLOCK_SIZE];
    printf("inode written");
    for (uint32_t pointer = NODE_START; pointer <= NODE_ENDE; pointer++) {
        _blockDevice->read(pointer, read);                          //write current INODES into the buffer

        if (read[0] == 0) {                                         // Block is empty
            _blockDevice->write(pointer, (char*) node);             //update block with new inode
            return writeRootPointer(pointer);                              //add inode to root directory
         
        }
       
    }
     return -1;
}

/**
 * Aufgabe1 
 * Set the inodes metadata to the current time
 * @param node:   the inode to be updated
 * @param atim:  should the nodes atime be updated?
 * @param mtim:  ""      ""  ""     mtime "" ""?
 * @param ctim:  ""      ""  ""     ctime "" ""?
 * 
 * */
void MyFSMgr::changeTime(Inode* node, bool atim, bool mtim, bool ctim){
    timespec currentTime;

    clock_gettime(CLOCK_REALTIME, &currentTime); //get current time

    if(atim)
    node->atim = currentTime.tv_sec;               // unix timestap 
    if(mtim)
    node->mtim = currentTime.tv_sec;
    if(ctim)
    node->ctim = currentTime.tv_sec;
}
/**
 * Aufgabe1
 * Write a pointer to the given place in the FAT. If one block isn't enough
 * we need more blocks to address all the data for a single file. Each block
 * has a pointer to the next DataBlock. This way we have a link list pointing to  paired data blocks.
 *
 * 0x00000000 = FREI | 0xFFFFFFFF = BELEGT
 *
 * @param blockPointer  The current position which points to the next position.
 * @param nextPointer   The next position that points to more data.
 */
void MyFSMgr::setFATBlockPointer(uint32_t blockPointer, uint32_t nextPointer) {
    blockPointer -= DATA_START;    //chlean generall block nr. we need the offset position in the datblock part of the fs. 

    int offsetBlockNR = blockPointer / 128;     //512/4 = 128 get the nr of block
    int offsetBlockPos = blockPointer % 128;    //within the block find the position

    char read[BLOCK_SIZE];                      //again: the buffer

    _blockDevice->read(FAT_START + offsetBlockNR, read);    //read the right block oth the FAT into the buffer

    FATBlock* fat = (FATBlock*) read;                       //cast the buffer into a  FAT struct

    LOGF("FAT UPATE: SET: %i WITH %i\n", blockPointer+DATA_START,nextPointer);

    fat->pointer[offsetBlockPos]= nextPointer;          //set reference to next block

    _blockDevice->write(FAT_START + offsetBlockNR, (char*) fat); //update FAT
}

/**
 * Aufgabe1 
 * The root block has references of the inodes.
 *
 * @param newPointer    The pointer that will be filled in the Rootblock.
 */
int MyFSMgr::writeRootPointer(uint32_t newPointer) {
    char read[BLOCK_SIZE];                                      //buffer
    _blockDevice->read(ROOT_BLOCK, read);                       //read 1. root block
    RootDirect* root = (RootDirect*)read;                       //cast the buffer in the desired struct
    uint32_t i= 0;

    while(root->pointer[i] != 0 && i < NUM_DIR_ENTRIES){       //loop through entries till there is one found which is empty 
        i++;
    }
    if(i >= NUM_DIR_ENTRIES){                                  //check if there are already max. amount of entries.
       LOG("ROOTDIRECTORY: Erlaubte Anzahl Eintraege ueberschritten.");
       return -ENOSPC;
    }

    root->pointer[i] = newPointer;
    return _blockDevice->write(ROOT_BLOCK, (char*)root);

    }
/**
 * Aufgabe1 
 * Find Inode in root dir
 * @fileName: name of the file 
 * @node: empty inode
 * @nodePointer:  empty inode pointer
 *
*/
bool MyFSMgr::findInode(char* fileName, Inode* node, uint32_t* nodePointer){
    uint32_t position = MAX_UINT;

    while((position = readNextRootPointer(position)) != 0){ //loop through root directory as long as there are entries
       LOGF("Inodepointer: %u\n", position);
       _blockDevice->read(position, (char*)node);
       if (strcmp(fileName, node->fileName) == 0) {         //check if thereś arleady a inode for a file with the same name

           *nodePointer = position;                
           return true;
       }
    }
    return false;

}




/**
 * Aufgabe1
 * Check if the provided file exists inside the BlockDevice.
 *
 * @param path The path of the file to check.
 * @return True if the file does exist, otherwise false.
 */
bool MyFSMgr::fileExists(char* path) {
    char copy[BLOCK_SIZE];
    uint32_t inodePointer;
    Inode* node = (Inode*)copy;
    char* pathSegments = strtok(path, "/");
    char fileName[NAME_LENGTH];

    strcpy(fileName, pathSegments);

    if (rootPointerCount() == 0) // The first file can't be a duplicate
        return false;

    return findInode(fileName, node, &inodePointer);
}

/**
 * Aufgabe 1                                                          
 * Return the sum of all the pointers to an Inode inside the Rootblock.
 *
 * @return The sum of all pointers. 0 if none.
 */
int MyFSMgr::rootPointerCount() {
    char read[BLOCK_SIZE];
    int sum = 0;

    _blockDevice->read(ROOT_BLOCK, read);

    for (int i = 0; i < BLOCK_SIZE; i += 4) { //loop through entries (4 bytes pro pointer)
        uint32_t pointer = read[i] << 24 | read[i + 1] << 16 | read[i + 2] << 8 | read[i + 3]; //get bytes of an entry and do an OR
        if (pointer != 0) 
            sum++;
    }
    return sum;
}

/**Aufgabe1
 * Return the following pointer of an Inode from the Rootblock.
 *
 * @param   oldPointer The pointer of the old Inode. If set to -1 return the first Inode pointer.
 *
 * @return  0 if it was the last Pointer.
 *          else the pointer to the next Inode.
 */
uint32_t MyFSMgr::readNextRootPointer(uint32_t oldPointer) {
    char read[BLOCK_SIZE];
    _blockDevice->read(ROOT_BLOCK, read);
    RootDirect* root = (RootDirect*)read;
    uint32_t i= 0;
    if (oldPointer == MAX_UINT){
        return root->pointer[0];
    }
    while(root->pointer[i] != oldPointer && i < NUM_DIR_ENTRIES){          //loop through root directory till there are no entries left
        i++;
    }

    if(i >= NUM_DIR_ENTRIES){  //more than 64 entries
        return -1;
    }

    return root->pointer[i+1];

}

/**Aufgabe
 * Please compare structure with setNextFATPointer
 * Read FAT entry of given pointer.
 *
 * @param blockPointer Pointer which points to a entry inside the FAT.
 * @return The linked value inside the FAT of the provided blockPointer.
 */
uint32_t MyFSMgr::readNextFATPointer(uint32_t blockPointer){
    blockPointer -= DATA_START;                          //offset
    int offsetBlockNR = blockPointer / 128;             //128 pointer within each fat block 
    int offsetBlockPos = blockPointer % 128;            //Position witin the block

    char read[BLOCK_SIZE];                              //buffer
    _blockDevice->read(FAT_START + offsetBlockNR, read); // FAT Start + block Offset 
    FATBlock* fat = (FATBlock*) read;                   //cast the buffer into the FATBock structure

    return fat->pointer[offsetBlockPos];                
}

/**
 * Read the given node of the Pointer.
 * UNUSED.
 *
 * @param nodePointer The Pointer of the node to read.
 * @return The node of the given Pointer.
 */
Inode* MyFSMgr::readNode(uint32_t nodePointer){
    char copy[BLOCK_SIZE];
    Inode* node = (Inode*)copy;
    uint32_t currentPointer = 0;

    for (unsigned int i = NODE_START; i < NODE_ENDE; i++){
        if (nodePointer == currentPointer) { // Richtige Node
            _blockDevice->read(i, (char*) &node);
            return node;
        }
        currentPointer++;
    }

    return node;
}

/**Aufgabe 2
 * Remove the file from MyFS and all dependencies.
 *
 * @param nodePointer The Pointer of the file to remove.
 */
void MyFSMgr::removeFile(uint32_t nodePointer) {
    char copy[BLOCK_SIZE];
    Inode* node = (Inode*)copy;
    _blockDevice->read(nodePointer, (char*)node); // Node die gelöscht werden soll

    uint32_t pointer = node->pointer;
    uint32_t oldpointer = pointer;
    LOGF("removed File: %i\n", pointer);

    fillBlocks(pointer, pointer + 1);   //Der erste Datenblock wird gelöscht.


    while ((pointer = readNextFATPointer(pointer)) != MAX_UINT) { // Die Datei war größer als 1 Block, daher einträge in der FAT die gelöscht werden müssen

        fillBlocks(pointer, pointer + 1); //Der Folgeblock wird gelöscht.
        setFATBlockPointer(oldpointer, 0); // Der alte FAT Pointer wird gelöscht.
        oldpointer = pointer;
    }
    LOGF("Max_unit?: %d\n", pointer);
    LOGF("Removed FATPOINTER: %i\n", oldpointer);
    setFATBlockPointer(oldpointer,0); // Der End Fatpointer wird geslöscht (der mit Max_uint)
    removeRootPointer(nodePointer);           // löschen im RootBlock
    fillBlocks(nodePointer, nodePointer + 1); // löschen der Node
}

/**Aufgabe 1
 * Remove the FAT pointer to a provided file-> set to default value "0"
 *
 * @param delPointer The pointer that should be deleted.
 */
void MyFSMgr::removeFatPointer(uint32_t delPointer) {
    setFATBlockPointer(delPointer, 0);
}

/**
 * Aufgabe1
 * If a file was deleted and there are any references left, which are no longer valid ->delete
 * 
 * @param : the block from which on the following block are to be deleted (set to 0)
 * */
void MyFSMgr::deleteFollowingBlocks(uint32_t dataPointer){
    uint32_t oldPointer = dataPointer;
    uint32_t currentPointer = readNextFATPointer(dataPointer);

    while(currentPointer != MAX_UINT){
        fillBlocks(currentPointer, currentPointer + 1);
        oldPointer = currentPointer;
        currentPointer = readNextFATPointer(currentPointer);
        setFATBlockPointer(oldPointer, 0);
        LOGF("currentPointer: %u\n", currentPointer);
    }


}

/**
 * Aufgabe 2
 * Remove the Root pointer to a provided file.
 *
 * @param delPointer The pointer that should be deleted.
 */
void MyFSMgr::removeRootPointer(uint32_t delPointer) {
    char read[BLOCK_SIZE];
    _blockDevice->read(ROOT_BLOCK, read);
    RootDirect* root = (RootDirect*)read;
    uint32_t i= 0;

    while(root->pointer[i] != delPointer && i < NUM_DIR_ENTRIES){ //loop throug root dir till there the pointer found, which should be deleted
        i++;
    }
    if(i >= NUM_DIR_ENTRIES){                               
       LOG("Über maxEntries hinaus");
    }

    root->pointer[i] = 0;
    _blockDevice->write(ROOT_BLOCK, (char*)root);           //update root dir
}

/**Aufgabe 1
 * Moves the buffer that the first Bytes are in the offset block range.
 *
 * @param db The DataBuffer with the buffer that should be moved.
 * @param off The offset for the Bytes that will be readed.
 */
int MyFSMgr::moveBuffer(DataBuffer* db, int off) {
    char read[BLOCK_SIZE];
    uint32_t blockOffset = off / BLOCK_SIZE; //The Block that should the Buffer contains
    if (blockOffset == db->blockNumber)
        return 0;

    while (blockOffset != db->blockNumber){
        db->dataPointer = MyFSMgr::instance()->readNextFATPointer(db->dataPointer);
        if (db->dataPointer == MAX_UINT)
            return -1;
        db->blockNumber++;
    }
    MyFSMgr::BDInstance()->read(db->dataPointer, read);
    memcpy(&db->data, read, BLOCK_SIZE);
    return 0;
}

/**
 * Aufgabe 1
 * Moves the buffer one block further.
 *
 * @param db    The DataBuffer that should be moved.
 * @return  0   on success.
 *          -1  if it's the last block.
 */
int MyFSMgr::moveBuffer(DataBuffer* db) {
    char read[BLOCK_SIZE];

    db->dataPointer = MyFSMgr::instance()->readNextFATPointer(db->dataPointer); //get next block of the file
    if (db->dataPointer == MAX_UINT)
        return -1;
    db->blockNumber++;          

    MyFSMgr::BDInstance()->read(db->dataPointer, read); //write into temp buffer "read"
    memcpy(&db->data, read, BLOCK_SIZE);                //copy data from temp buffer into the real buffer
    return 0;
}

/**Aufgabe 1
 * Write data in buf, beginns at 'from' till 'to' with an offset for the buffer.
 * This way you can write some Bytes out of a Array into a buffer. With the
 * offset it is possible to combine multiple Blockdata in one buffer.
 *
 * @param buf       The buffer, most of the time from fuse.
 * @param data      The data that will be written into the buffer.
 * @param from      The start index of data.
 * @param to        The end Index of data.
 * @param offset    An offset for the buffer.
 * @return          The position of the new offset, useful for combining multiple blocks.
 */
int MyFSMgr::copyDataToBuffer(char* buf, char data[512], int from, int to, int offset) {
    for (int i = from; i < to; i++) {
        memcpy(buf + offset, &data[i], 1);
        offset++;                           //new offset after reading the block
    }
    return offset;
}
