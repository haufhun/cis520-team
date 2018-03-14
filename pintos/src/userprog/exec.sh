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

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/exec-bound -a exec-bound -p tests/userprog/child-args -a child-args -- -q  -f run exec-bound
read -rp 'Exec-bound done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/exec-bound-2 -a exec-bound-2 -- -q  -f run exec-bound-2
read -rp 'Exec-bound-2 done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/exec-bound-3 -a exec-bound-3 -- -q  -f run exec-bound-3
read -rp 'Exec-bound-3 done. Press enter to continue...' secondyn </dev/tty
