/*
*NodeJS Test using the mocha framework
*It can be either started with "npm fusetests" or with "mocha fusetest.js"
*
*Please note: node js as well as npm and the following API need to be installed with "npm install xy -g". 
*It will be installed on the local js library of your user
*
*DESCRIBE is used for grouping
*TO define a CALLBACK "it("nameofcallback", function(done){...})": Here it a
*/

const expect = require("chai").expect;
const chai = require("chai");
const fs = require("fs");
const path = require("path");
const shell = require("shelljs");
const chalk = require("chalk");
const crypto = require("crypto");


let basePath = "/home/pia/Documents/FS";
let testPath = "../";
let input = ["data1.txt", "data2.txt"];

describe("Myfs", async function () {
    it("test if File System is mounted", function (done) {
        if (fs.readdirSync(basePath).length != 0) {
            expect(true).to.be.true;
            done();
        } else {
            try{
              
                console.log(chalk.blue("Filling an mounting the container.bin"));
                var c = shell.exec('bash runMountFuseTest.sh', {
                    silent: true
                });
           
                done();
            }catch(error){
                console.log(error.status);
                console.log(error.message);
            }
          
        }
    }).timeout(25000);                                  //default is too short


    it("check if container.bin has files", function () {
        let files = fs.readdirSync(basePath);
        expect(files).to.be.not.empty;
        console.log(files);
       
    });

    it("bulk read files", async function () {
        let testContents = input;
        let containerFiles = fs.readdirSync(basePath);
        
        for (let j = 0; j < 100; j++) {                 //read first 100 chars and  compare
            for (const i in containerFiles) {
                let content = fs.readFileSync(path.join(basePath, containerFiles[i]), "utf8");
                let test = fs.readFileSync(path.join(testPath, containerFiles[i]), "utf8");
                expect(content).to.be.equal(test);
                }
            }
    });
        describe("Read:", async () => {
            let txtFile = path.join(basePath, "data1.txt");
            let txtTest = path.join(testPath, "data1.txt");
            console.log("FS: " + txtFile + " Test: " + txtTest);
            describe('without offset:', () => {
                it("Exact", async () => {
                    compareTestToContainer(txtFile, txtTest, 0, 512);
                });
                it("Half", async () => {
                    compareTestToContainer(txtFile, txtTest, 0, 256);
                });
                it("Long", async () => {
                    compareTestToContainer(txtFile, txtTest, 0, 20 * 512);
                });
                it("short", async () => {
                    compareTestToContainer(txtFile, txtTest, 0, 20 * 600);
                });
                it("nothing", async () => {
                    compareTestToContainer(txtFile, txtTest, 0, 0);
                });
            });
            describe('with offset:', () => {
                it("Exact", async () => {
                    compareTestToContainer(txtFile, txtTest, 512, 512);
                });
                it("Half", async () => {
                    compareTestToContainer(txtFile, txtTest, 256, 256);
                });
                it("One", async () => {
                    compareTestToContainer(txtFile, txtTest, 500, 12);
                });
                it("Long", async () => {
                    compareTestToContainer(txtFile, txtTest, 512 * 2, 20 * 512);
                });
                it("short", async () => {
                    compareTestToContainer(txtFile, txtTest, 20, 20);
                });
            });
            describe('with oneBlock offset:', () => {
                it("Exact", async () => {
                    compareTestToContainer(txtFile, txtTest, 512, 512);
                });
                it("Half", async () => {
                    compareTestToContainer(txtFile, txtTest, 512, 256);
                });
                it("Long", async () => {
                    compareTestToContainer(txtFile, txtTest, 512, 20 * 512);
                });
                it("short", async () => {
                    compareTestToContainer(txtFile, txtTest, 512, 20);
                });
                it("nothing", async () => {
                    compareTestToContainer(txtFile, txtTest, 512, 0);
                });
            });
        });

    });

    describe('Write:', () => {
        let smallString = "test";
        let oneBlockString = new Array(512 + 1).join("#");
        let largeString = crypto.randomBytes(20).toString('hex');
        console.log(largeString);
        describe('append:', () => {
            it('append: short', () => {
                let files = fs.readdirSync(basePath);
                console.log(files);
                files.forEach(file => {
                    append(path.join(basePath, file), smallString);
                    append(path.join(testPath, file), smallString);
                    expect(fs.readFileSync(path.join(basePath, file), "utf8") ).to.be.equal(fs.readFileSync(path.join(testPath, file), "utf8"));
                });
                
            });
            it('append: long', () => {
                let files = fs.readdirSync(basePath);
                files.forEach(file => {
                    append(path.join(basePath, file), largeString);
                    append(path.join(testPath, file), largeString);
                    expect(fs.readFileSync(path.join(basePath, file), "utf8") ).to.be.equal(fs.readFileSync(path.join(testPath, file), "utf8"));
                    });  
            });
        });

        describe('write:', () => {
            it('short', () => {
                let files = fs.readdirSync(basePath);
                files.forEach(file => {
                    writeByByte(path.join(basePath, file), 0, smallString, 0);
                    writeByByte(path.join(testPath, file), 0, smallString, 0);
                    expect(fs.readFileSync(path.join(basePath, file), "utf8") ).to.be.equal(fs.readFileSync(path.join(testPath, file), "utf8"));
                });
            });
            it('block', () => {
                let files = fs.readdirSync(basePath);
                files.forEach(file => {
                    writeByByte(path.join(basePath, file), 0, oneBlockString, 0);
                    writeByByte(path.join(testPath, file), 0, oneBlockString, 0);
                    expect(fs.readFileSync(path.join(basePath, file), "utf8") ).to.be.equal(fs.readFileSync(path.join(testPath, file), "utf8"));
                });
               
            });
            it('long', () => {
                let files = fs.readdirSync(basePath);
                files.forEach(file => {
                    writeByByte(path.join(basePath, file), 0, largeString, 0);
                    writeByByte(path.join(testPath, file), 0, largeString, 0);
                    expect(fs.readFileSync(path.join(basePath, file), "utf8") ).to.be.equal(fs.readFileSync(path.join(testPath, file), "utf8"));
                });
              
            });
        });
        // describe('Write one block offset', () => {
        //     it('short', () => {
        //         let files = fs.readdirSync(basePath);
        //         files.forEach(file => {
        //             writeByByte(path.join(basePath, file), 512, smallString, 512);
        //             writeByByte(path.join(testPath, file), 512, smallString, 512);
        //             expect(fs.readFileSync(path.join(basePath, file), "utf8") ).to.be.equal(fs.readFileSync(path.join(testPath, file), "utf8"));
        //         });
        //     });
            // it('block', () => {
            //     let files = fs.readdirSync(basePath);
            //     files.forEach(file => {
            //         writeByByte(path.join(basePath, file), 512, oneBlockString);
            //         writeByByte(path.join(testPath, file), 512, oneBlockString);
            //         expect(fs.readFileSync(path.join(basePath, file), "utf8") ).to.be.equal(fs.readFileSync(path.join(testPath, file), "utf8"));
            //     });
                
            // });
        //     it('long', () => {
        //         let files = fs.readdirSync(basePath);
        //         files.forEach(file => {
        //             writeByByte(path.join(basePath, file), 512, largeString);
        //             writeByByte(path.join(testPath, file), 512, largeString);
        //         });
        //         compareDirectorys();
        //     });
        // });
        // describe('Write offset', () => {
        //     it('short', () => {
        //         //let files = fs.readdirSync(basePath);
        //         let files = ['file1.txt', 'file2.txt', 'file3.txt'];
        //         files.forEach(file => {
        //             writeByByte(path.join(basePath, file), 2000, smallString);
        //             writeByByte(path.join(testPath, file), 2000, smallString);
        //         });
        //         compareDirectorys();
        //     });
        //     it('block', () => {
        //         let files = fs.readdirSync(basePath);
        //         files.forEach(file => {
        //             writeByByte(path.join(basePath, file), 2000, oneBlockString);
        //             writeByByte(path.join(testPath, file), 2000, oneBlockString);
        //         });
        //         compareDirectorys();
        //     });
        //     it('long', () => {
        //         let files = fs.readdirSync(basePath);

        //         files.forEach(file => {
        //             writeByByte(path.join(basePath, file), 2000, largeString);
        //             writeByByte(path.join(testPath, file), 2000, largeString);
        //         });
        //         compareDirectorys();
        //     });
        //});
   // });

    describe('delete files:', () => {
        it('delete all files in directory', () => {
            let files = fs.readdirSync(basePath);
            files.forEach(file => {
                fs.unlinkSync(path.join(basePath, file));
            });
        });
    });
    describe("create new files:", () => {
        it('generate 64 small test files', () => {
            for (let i = 1; i < 2; i++) {
                token = crypto.randomBytes(513).toString('hex');
                fs.writeFileSync(path.join(testPath, "file" + i + ".txt"), token, {
                    flag: 'w'
                });
                shell.chmod(7777, path.join(testPath, "file" + i + ".txt") );

                fs.writeFileSync(path.join(basePath, "file" + i + ".txt"), token, {
                    flag: 'w'
                });
           
                shell.chmod(7777, path.join(basePath, "file" + i + ".txt"));
            }
            let file1 = fs.readdirSync(basePath);
            let file2 = fs.readdirSync(testPath);
            let tempfile2 = [];
            file2.forEach(element => {
                
                if ((element.indexOf("file") > -1) && (element.indexOf("txt") > -1))  {         //remove all other files
                   tempfile2.push(element);
                }
            });
            file2 = tempfile2;
            expect(file1).to.have.same.members(file2);
        });
    });
    describe('test time', () => {
        describe('atime:', () => {
            it('read:', () => {
                let file = path.join(basePath, "file1.txt");
                let atime = fs.statSync(file).atime;
                var time = readByByte(file, 0, 1024);
                expect(atime).to.be.not.equal(fs.statSync(file).atimeMs);
            });
        });

        //schreiben geht!! 
        //Problem: The test is so fast, so there are no time differences between reading a-, c- mtime the first time , change the content and reading  a-, c- mtime again.
        describe('mtime:', () => {
            it('append:', () => {
                let file = path.join(basePath, "file1.txt");
                let mtime = fs.statSync(file).mtimeMs;
                append(file, "#kmd");
                expect(mtime).to.be.equal(fs.statSync(file).mtimeMs);
            });
            it('write:', () => {
                let file = path.join(basePath, "file1.txt");
                let mtime = fs.statSync(file).mtimeMs;
                writeByByte(file, 0, "test", 0);
                expect(mtime).to.be.equal(fs.statSync(file).mtimeMs);
            });
        });
        describe('ctime', () => {
            it('append:', () => {
                let file = path.join(basePath, "file1.txt");
                let ctimeMs = fs.statSync(file).ctimeMs;
                append(file, "#kmd");
                expect(ctimeMs).to.be.equal(fs.statSync(file).ctimeMs);
            });
            it('write:', () => {
                let file = path.join(basePath, "file1.txt");
                let ctime = fs.statSync(file).ctimeMs;
                writeByByte(file, 0, "test", 0)
                expect(ctime).to.be.equal(fs.statSync(file).ctimeMs);
            });
        });
     });
    describe('cleanup', () => {
        it('unmount FS', ()=>{
            console.log(chalk.blue("\tfill container and mounting container.bin..."));
            shell.exec("bash runUnmount.sh", {
                silent: true
            })
            
        }).timeout(250000);
    });
});


function compareTestToContainer(file, testFile, pos, length) {
    let chunk1 = readByByte(file, pos, length);
    let chunk2 = readByByte(testFile, pos, length);
    expect(chunk1.toString('hex')).to.be.equal(chunk2.toString('hex'));

}

function readByByte(file, pos, length) {
    let fd = fs.openSync(file, "rs");
    let buffer = Buffer.alloc(length);
    fs.readSync(fd, buffer, 0, length, pos);
    fs.fsyncSync(fd);
    fs.closeSync(fd);
    return buffer
}

function writeByByte(file, pos, string, offset) {
    let fd = fs.openSync(file, "w");
    let buffer = Buffer.from(string, 'utf8');
    fs.writeSync(fd, buffer, offset, string.length, pos);
    fs.fsyncSync(fd);
    fs.closeSync(fd);
}

function append(file, string) {
    let fd = fs.openSync(file, "a");
    let buffer = Buffer.from(string, 'utf8');
    fs.writeSync(fd, buffer, 0, buffer.length, null);
    fs.fsyncSync(fd);
    fs.closeSync(fd);
}
