#!/bin/sh
echo "Enter the name for the output zip (it should have a .zip at the end!)"
read outputFile

currentDir=$PWD
cd

#clear and recompile the project
echo "Compiling"
cd "KiwiLightProject/KiwiLight - Vision Code"
read -p "Recompile? [y/n]:" recomp

if [ $recomp = "y" ];
then
	make clean
	make -j4 KiwiLight
fi

echo "Placing"
rm $HOME/KiwiLightPackage/KiwiLight/KiwiLight/KiwiLight
cp KiwiLight $HOME/KiwiLightPackage/KiwiLight/KiwiLight

echo "Zipping"
cd
cd KiwiLightPackage
zip -r $HOME/$outputFile KiwiLight

cd $currentDir
echo "The utility has finished."
