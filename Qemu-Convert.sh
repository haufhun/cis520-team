#!/bin/sh
clear

echo '--------------------Starting Qemu pintos conversion--------------------'
echo ''
echo 'Please make sure you are runing this bash file in the same directory of the "pintos" folder you want to convert.'
read -rp 'Press enter to continue...' secondyn </dev/tty
echo ''



#Removing Bochs folders that arent needed for Qemu.
echo 'Removing bochs-2.4.5 folder from cis520-team folder if it exists.'
rm -rf ./bochs-2.4.5

echo 'Removing usr folder from cis520-team folder if it exists.'
rm -rf ./usr

echo 'Removing tools folder from cis520-team folder if it exists.'
rm -rf ./tools



#Replacing Pintos and Pintos.pm files
echo 'Replacing pintos file in cis520-team/pintos/src/utils'
cp -rf ~/Pintos-Conversion/Qemu_files/pintos ./pintos/src/utils/

echo 'Replacing Pintos.pm file in cis520-team/pintos/src/utils'
cp -rf ~/Pintos-Conversion/Qemu_files/Pintos.pm ./pintos/src/utils/



#Starts replacing all the Make.vars files
echo 'Replacing Make.vars file in cis520-team/pintos/src/userprog'
cp -rf ~/Pintos-Conversion/Qemu_files/Make-userprog.vars ./pintos/src/userprog/Make.vars

echo 'Replacing Make.vars file in cis520-team/pintos/src/threads'
cp -rf ~/Pintos-Conversion/Qemu_files/Make-threads.vars ./pintos/src/threads/Make.vars

echo 'Replacing Make.vars file in cis520-team/pintos/src/vm'
cp -rf ~/Pintos-Conversion/Qemu_files/Make-vm.vars ./pintos/src/vm/Make.vars

echo 'Replacing Make.vars file in cis520-team/pintos/src/filesys'
cp -rf ~/Pintos-Conversion/Qemu_files/Make-filesys.vars ./pintos/src/filesys/Make.vars



#End of bash script
echo ''
read -rp 'Finished qemu conversion, press enter to continue...' secondyn </dev/tty


