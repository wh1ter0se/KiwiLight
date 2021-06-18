#!/bin/sh
echo "Enter the name for the output zip (it should have a .zip at the end!)"
read outputFile

currentDir=$PWD

#clear and recompile the project
echo "Compiling"
cd "KiwiLightProject/KiwiLight - Vision Code"
make clean
make -j2 KiwiLight

echo "Placing"
rm $HOME/KiwiLight/KiwiLight/KiwiLight
rm $HOME/KiwiLight/KiwiLight.desktop
cp KiwiLight $HOME/KiwiLight/KiwiLight

echo "Zipping"
cd
zip -r $outputFile KiwiLight

echo "Reinstall? [y/n]"
read reinstall

if [ $reinstall = 'y' ]; then

echo "Removing old components"
rm Desktop/KiwiLight.desktop
rm -r KiwiLightData
sudo rm /usr/bin/KiwiLight
sudo rm -r /usr/lib/KiwiLight

echo "Running Installer"
cd KiwiLight
sh install.sh

fi

cd $currentDir
echo "The utility has finished."
