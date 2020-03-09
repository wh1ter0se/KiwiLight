# ---------------------------------- #
#                                    #
#    UNIX INSTALLER FOR KIWILIGHT    #
#                                    #
# ---------------------------------- #

Install() {
    echo "Downloading Packages"

    sudo apt-get --assume-yes install pkg-config
    sudo apt-get --assume-yes install cmake
    sudo apt-get --assume-yes install libgtk-3-dev
    sudo apt-get --assume-yes install libv4l-dev
    sudo apt-get --assume-yes install v4l-utils
    
    echo "Preparing to download OPENCV"
    mkdir opencv
    cd opencv

    echo "downloading OPENCV"
    wget https://github.com/opencv/opencv/archive/3.4.5.zip
    mkdir src

    echo "unzipping OPENCV"
    unzip 3.4.5.zip -d src
    mkdir build
    cd build

    echo "Preparing to compile OPENCV"
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
          -D CMAKE_INSTALL_PREFIX=/usr/local \
          -D WITH_GTK_2_X=FALSE \
          -D WITH_GTK=TRUE \
          -D BUILD_TESTS=FALSE \
          ../src/opencv-3.4.5

    echo "Compiling OPENCV (this will take a while!)"
    sudo make -j2 install
    sudo ldconfig

    #return back to normal directory
    cd ../..

    echo "Preparing to compile KIWILIGHT"
    make setup

    echo "Compiling KIWILIGHT"
    make -j2 KiwiLight

    echo "Preparing KIWILIGHT"
    #install the confs folder
    currentDir = $PWD
    cd
    mkdir KiwiLightData
    mkdir KiwiLightData/confs
    mkdir KiwiLightData/tmp
    cp $PWD/generic.xml KiwiLightData/confs/
    echo "" >> KiwiLightData/tmp/shell_out.log

    cd $currentDir

    echo "KIWILIGHT has been successfully installed."
}


echo "Welcome to the KiwiLight installer!"
echo "Installing KiwiLight from the sources will download many packages and may take a long time."
echo "Would you like to continue? [y/n]: "

read -N1 confirm
echo

if [ ${confirm} = 'y' ]; then
    Install
    else
    echo "Aborting."
fi
