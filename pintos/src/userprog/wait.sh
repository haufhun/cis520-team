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


pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/wait-simple -a wait-simple -p tests/userprog/child-simple -a child-simple -- -q  -f run wait-simple

#read -rp 'create-normal done. Press enter to continue...' secondyn </dev/tty

