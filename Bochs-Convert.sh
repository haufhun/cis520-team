#!/bin/sh
clear

echo '--------------------Starting Bochs pintos conversion--------------------'
echo ''
echo 'Please make sure you are runing this bash file in the same directory of the "pintos" folder you want to convert.'
read -rp 'Press enter to continue...' secondyn </dev/tty
echo ''



#Adding Bochs folders that are needed for bochs.
echo 'Copying bochs-2.4.5 folder to cis520-team folder'
cp -a ~/Pintos-Conversion/Bochs_files/bochs-2.4.5/ .

echo 'Copying usr folder to cis520-team folder'
cp -a ~/Pintos-Conversion/Bochs_files/usr/ .

echo 'Copying tools folder to cis520-team folder'
cp -a ~/Pintos-Conversion/Bochs_files/tools/ .



#Replacing Pintos and Pintos.pm files
echo 'Replacing pintos file to cis520-team/pintos/src/utils'
cp -rf ~/Pintos-Conversion/Bochs_files/pintos ./pintos/src/utils/

echo 'Replacing Pintos.pm file to cis520-team/pintos/src/utils'
cp -rf ~/Pintos-Conversion/Bochs_files/Pintos.pm ./pintos/src/utils/



#Starts replacing all the Make.vars files
echo 'Replacing Make.vars file to cis520-team/pintos/src/userprog'
cp -rf ~/Pintos-Conversion/Bochs_files/Make-userprog.vars ./pintos/src/userprog/Make.vars

echo 'Replacing Make.vars file to cis520-team/pintos/src/threads'
cp -rf ~/Pintos-Conversion/Bochs_files/Make-threads.vars ./pintos/src/threads/Make.vars

echo 'Replacing Make.vars file to cis520-team/pintos/src/vm'
cp -rf ~/Pintos-Conversion/Bochs_files/Make-vm.vars ./pintos/src/vm/Make.vars

echo 'Replacing Make.vars file to cis520-team/pintos/src/filesys'
cp -rf ~/Pintos-Conversion/Bochs_files/Make-filesys.vars ./pintos/src/filesys/Make.vars



#End of bash script
echo ''
read -rp 'Finished bochs conversion, press enter to continue...' secondyn </dev/tty


