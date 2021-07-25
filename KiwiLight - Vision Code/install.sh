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
echo " - VNC Server"
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
echo "3: Create Packager(KiwiLight must be installed for this to work)"
read -p "Enter an action:" action

if [ $action = "1" ];
then
    read -p "Would you like to create a packager? (Required for KiwiLightPackagingTool.sh to work) [y/n]:" packager
    if [ $packager = "y" ];
    then
        action="3"
    fi

    echo "Installing."
    #install packages
    sudo apt-get update
    sudo apt-get --assume-yes install v4l-utils
    sudo apt-get --assume-yes install libgtk-3-dev
    sudo apt-get --assume-yes install make
    sudo apt-get --assume-yes install cmake
    sudo apt-get --assume-yes install wget
    sudo apt-get --assume-yes install unzip
    sudo apt-get --assume-yes install zip
    sudo apt-get --assume-yes install crontab
    sudo apt-get --assume-yes install realvnc-vnc-server
    
    #install OpenCV
    currentDir=$PWD
    cd $HOME
    mkdir opencv
    cd opencv
    wget https://github.com/opencv/opencv/archive/refs/tags/3.4.14.zip
    unzip 3.4.14.zip
    mkdir build
    cmake -S opencv-3.4.14 -B build \
    -DCMAKE_BUILD_TYPE=RELEASE \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_TESTS=FALSE \
    -DBUILD_JAVA=FALSE \
    -DWITH_GTK=TRUE
    
    cd build
    sudo make -j4 install
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
    sudo apt-get --assume-yes remove realvnc-vnc-server
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
    
fi

if [ $action = "3" ];
then
    currentDir=$PWD
    make -j4 KiwiLight
    
    echo "Creating Packager"
    cd $HOME
    
    mkdir KiwiLightPackage
    cd KiwiLightPackage
    mkdir KiwiLight
    cd KiwiLight
    
    #assemble the /home/<user>/KiwiLightPackage/KiwiLight/directory
    mkdir KiwiLight
    mkdir opencv
    cp "$currentDir"/packager/install.sh $PWD
    
    #assemble the /home/<user>/KiwiLightPackage/KiwiLight/KiwiLight directory
    cd KiwiLight
    
    mkdir confs
    mkdir ui
    cp "$currentDir"/*.png $PWD #copy all images
    cp "$currentDir"/packager/icon.png $PWD
    cp "$currentDir"/KiwiLight $PWD
    cp "$currentDir"/packager/KiwiLight.desktop $PWD
    cp "$currentDir"/packager/KiwiLight.sh $PWD
    
    #assemble subdirectories of this directory
    cp "$currentDir"/generic.xml confs
    cp "$currentDir"/ui/Style.css ui
    
    #assemble the /home/<user>/KiwiLightPackage/KiwiLight/opencv directory
    cd ../opencv
    mkdir bin
    mkdir include
    mkdir lib
    mkdir share
    
    cp /usr/local/bin/*opencv*  bin
    cp -r /usr/local/include/*opencv*/ include
    cp /usr/local/lib/libopencv* lib
    cp -r /usr/local/share/OpenCV share
    
    #put the packaging tool in the home directory and rename it to KiwiLightPackager.sh
    
    cd "$currentDir"
    cp packager/packager.sh $HOME
    mv $HOME/packager.sh $HOME/KiwiLightPackager.sh
    
    echo "Packager Created"
    
fi

echo "Finished!"
