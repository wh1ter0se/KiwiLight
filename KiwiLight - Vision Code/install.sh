#
# UNIX INSTALLER FOR KIWILIGHT
# Written By: Brach Knutson
#

echo "Welcome to the KiwiLight installer!"
echo
echo "Running this program will install/uninstall the following packages/dependencies:"
echo " - OpenCV"
echo " - Video4Linux"
echo " - GTK 3"
echo " - Cmake"
echo " - VNC"
echo ""
read -p "Do you want to continue? [y/n]:" confirm

if [ $confirm != "y" ];
then
echo "Aborting."
return 0
fi

echo "What would you like to do?"
echo "1: Install"
echo "2: Uninstall"
read -p "Enter an action:" action

if [ $action = "1" ];
then
    echo "Installing."
    #install packages
    sudo apt-get update
    sudo apt-get --assume-yes install v4l-utils
    sudo apt-get --assume-yes install libgtk-3-dev
    sudo apt-get --assume-yes install cmake
    sudo apt-get --assume-yes install realvnc-vnc-server
    
    #install OpenCV
    currentDir=$PWD
    cd $HOME
    mkdir opencv
    cd opencv
    wget https://github.com/opencv/opencv/archive/3.4.5.zip
    unzip 3.4.5.zip
    mkdir build
    cmake -S opencv-3.4.5 -B build \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_TEST=FALSE \
    -DBUILD_JAVA=FALSE \
    -DWITH_GTK=TRUE
    
    cd build
    sudo make -j3 install
    sudo ldconfig
    cd "$currentDir"
    
    #prepare KiwiLight
    cd $HOME
    mkdir KiwiLightData
    mkdir KiwiLightData/confs
    mkdir KiwiLightData/logs
    mkdir KiwiLightData/tmp
    cp "$currentDir/generic.xml" KiwiLightData/confs
    
    cd "$currentDir"
    
    make setup
    make -j4 KiwiLight
    sudo ln KiwiLight /usr/bin/KiwiLight
    
elif [ $action = "2" ];
then
    echo "Uninstalling"
    #uninstall packages
    sudo apt-get --assume-yes remove v4l-utils
    sudo apt-get --assume-yes remove libgtk-3-dev
    sudo apt-get --assume-yes remove cmake
    sudo apt-get --assume-yes autoremove
    
    #uninstall OpenCV
    currentDir=$PWD
    cd $HOME
    cd opencv/build
    sudo make -j4 uninstall
    cd $HOME
    sudo rm -r opencv
    cd $currentDir
    
    #remove KiwiLight
    rm -r $HOME/KiwiLightData
    sudo rm /usr/bin/KiwiLight
    
else
echo "That is not a valid option."
fi

echo "Finished!"
