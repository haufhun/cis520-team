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


pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/args-none -a args-none -- -q  -f run args-none 
read -rp 'args-none done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/args-single -a args-single -- -q  -f run 'args-single onearg' 

