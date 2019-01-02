#/bin/sh
pwd
cd ..
rm container.bin
make -f Makefile
./mkfs.myfs container.bin data1.txt 

./mount.myfs /home/pia/Documents/PiaFS/container.bin /home/pia/Documents/PiaFS/log.txt /home/pia/Documents/FS
