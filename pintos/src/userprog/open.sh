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

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/open-normal -a open-normal -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run open-normal
read -rp 'OPEN-NORMAL done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/open-missing -a open-missing -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run open-missing
read -rp 'OPEN-MISSING done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/open-boundary -a open-boundary -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run open-boundary
read -rp 'OPEN-boundary done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/open-null -a open-null -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run open-null
read -rp 'OPEN-NULL done. Press enter to continue...' secondyn </dev/tty

pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/open-bad-ptr -a open-bad-ptr -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run open-bad-ptr
read -rp 'OPEN-BAD-PTR done. Press enter to continue...' secondyn </dev/tty
