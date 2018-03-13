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
pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/exec-arg -a exec-arg -p tests/userprog/child-args -a child-args -- -q  -f run exec-arg

#read -rp 'close-normal done. Press enter to continue...' secondyn </dev/tty

