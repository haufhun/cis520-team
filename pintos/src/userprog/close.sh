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
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/close-normal -a close-normal -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run close-normal
read -rp 'close-normal done. Press enter to continue...' secondyn </dev/tty
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/close-twice -a close-twice -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run close-twice
read -rp 'close-twice done.Press enter to continue...' secondyn </dev/tty
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/close-stdin -a close-stdin -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run close-stdin
read -rp 'close-stdin done. Press enter to continue...' secondyn </dev/tty
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/close-stdout -a close-stdout -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run close-stdout
read -rp 'close-stdout done. Press enter to continue...' secondyn </dev/tty
pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/close-bad-fd -a close-bad-fd -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run close-bad-fd
