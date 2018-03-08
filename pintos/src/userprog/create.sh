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


pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/create-normal -a create-normal -- -q  -f run create-normal
read -rp 'create-normal done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/create-empty -a create-empty -- -q  -f run create-empty
read -rp 'create-empty done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/create-null -a create-null -- -q  -f run create-null
read -rp 'create-null done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/create-bad-ptr -a create-bad-ptr -- -q  -f run create-bad-ptr
read -rp 'create-bad-ptr done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/create-long -a create-long -- -q  -f run create-long
read -rp 'create-long done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/create-exists -a create-exists -- -q  -f run create-exists
read -rp 'create-exists done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --bochs  --filesys-size=2 -p tests/userprog/create-bound -a create-bound -- -q  -f run create-bound
read -rp 'create-bound done. Press enter to continue...' secondyn </dev/tty
