#!/bin/sh
clear
clear
echo Entering directory /examples ...
cd ../examples/
make

echo Entering directory /userprog ...
cd ../userprog/
make
#Stop for the user to press the enter key
read -rp 'Press enter to continue...' secondyn </dev/tty

echo Entering directory /userprog/build ...
cd build
pintos-mkdisk filesys.dsk --filesys-size=2;

pintos -v -k -T 300 --bochs  --filesys-size=2 -p tests/filesys/base/syn-read -a syn-read -p tests/filesys/base/child-syn-read -a child-syn-read -- -q  -f run syn-read 
read -rp 'syn-read finished, Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/filesys/base/syn-remove -a syn-remove -- -q  -f run syn-remove 
read -rp 'syn-removed finished, Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/filesys/base/syn-write -a syn-write -p tests/filesys/base/child-syn-wrt -a child-syn-wrt -- -q  -f run syn-write
read -rp 'syn-write finished, Press enter to continue...' secondyn </dev/tty
