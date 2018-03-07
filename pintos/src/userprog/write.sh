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

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/write-normal -a write-normal -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run write-normal;
read -rp 'WRITE-NORMAL done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/write-bad-ptr -a write-bed-ptr -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run write-bad-ptr;
read -rp 'WRITE-BAD-PTR done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/write-boundary -a write-boundary -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run write-boundary;
read -rp 'WRITE-BOUNDARY done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/write-zero -a write-zero -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run write-zero;
read -rp 'WRITE-ZERO done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/write-stdin -a write-stdin -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run write-stdin;
read -rp 'WRITE-STDIN done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/write-bad-fd -a write-bad-fd -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run write-bad-fd;
read -rp 'WRITE-BAD-FD done. Press enter to continue...' secondyn </dev/tty

