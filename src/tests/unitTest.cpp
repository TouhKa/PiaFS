#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "myfs.h"
#include "MyFSMgr.h"
#include "string.h"

char* pathFs = "../TEST";
char* testFile1 = strcat (pathFs, "/data1.txt");
char* testFile2 = strcat (pathFs, "/data2.txt");
char* testFile3 = strcat (pathFs, "/data3.txt");

//A1
//test Updat inode: atime,ctime, mtime
TEST_CASE( "Testing Inode time updates", "[time]"){
    //setup
    

    SECTION("Initial import"){          //runs the whole testcase
       // fs.useGetattr(testFile1, );
    }

     SECTION("read file"){              //runs the whole testcase

    }

       SECTION("write file"){          //runs the whole testcase

    }


}
//test current user  and group


TEST_CASE("Testing updating user and group", "[user]"){
    SECTION("Imporing files from other fs"){

    }

}

//test permissions
TEST_CASE("Testing updating user and group", "[user]"){
    SECTION("Imporing files from other fs"){

    }

}



//test buffer
TEST_CASE("Testing updating user and group", "[user]"){
    //scenario 1 block
    SECTION("Read just one block"){

    }

    //scneario several blocks
    SECTION("Read a file with several blocks"){

    }

    //scenario jump for- and backwards
    SECTION("Jump vor and backward"){

    }
}

//test mehrere geöffnete dateien
TEST_CASE( "Import Files", "[import]"){
    //setup
    

    SECTION("Each file only once"){          //runs the whole testcase

    }

     SECTION("One doubled filename"){          //runs the whole testcase

    }

    SECTION("Are there any files"){

    }
}




//A2
//Datei anlegen - fuse mknode
TEST_CASE( "Create a new file", "[createFile]"){
    //setup
    SECTION("todo"){          //runs the whole testcase

    }
}

//datei schreiben und ändern - fuse write 
TEST_CASE( "Change content", "[changeFile]"){
    //setup
    SECTION("todo"){          //runs the whole testcase

    }
}
//löschen - fuse unlink
TEST_CASE( "delete a file", "[deleteFile]"){
    //setup
    SECTION("todo"){          //runs the whole testcase

    }
}