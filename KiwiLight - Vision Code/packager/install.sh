# ----------------------------------------- #
#                                           #
#   UNIX INSTALLER FOR KIWILIGHT BINARIES   #
#                                           #
# ----------------------------------------- #

Install() {
    echo "Installing KiwiLight"
    echo "  Installing some dependencies"
    sudo apt-get --assume-yes --quiet install pkg-config
    sudo apt-get --assume-yes --quiet install gnuplot
    sudo apt-get --assume-yes --quiet install libv4l-dev
    sudo apt-get --assume-yes --quiet install realvnc-vnc-server
    sudo apt-get --assume-yes --quiet install crontab
    sudo apt-get --assume-yes --quiet install v4l-utils
    echo "  Installing OpenCV"
    #copy opencv binaries into their places
    sudo cp -r opencv/bin/* /usr/local/bin/
    sudo cp -r opencv/include/* /usr/local/include/
    sudo cp -r opencv/lib/* /usr/local/lib/
    sudo cp -r opencv/share/* /usr/local/share/
    sudo ldconfig

    echo "  Positioning KiwiLight"
    #copy KiwiLight into its place
    sudo cp -r KiwiLight /usr/lib/

    #place KiwiLightData under HOME
    currentDir1=$PWD
    cd
    mkdir KiwiLightData
    cd KiwiLightData
    mkdir confs
    mkdir logs
    mkdir tmp
    cd tmp
    echo "" >> shell_out.log
    cd $currentDir1
    cp KiwiLight/confs/generic.xml $HOME/KiwiLightData/confs/

    echo "  Linking KiwiLight" 
    #create hard link with ln
    sudo ln /usr/lib/KiwiLight/KiwiLight.sh /usr/bin/KiwiLight
    sudo cp KiwiLight/KiwiLight.desktop /usr/share/applications/

    echo "  Changing some permissions"
    sudo chmod a+x /usr/lib/KiwiLight/KiwiLight.sh
    sudo chmod a+x /usr/bin/KiwiLight

    echo "  Copying Executable"
    #copy the kiwilight executable to the current directory.
    cp KiwiLight/KiwiLight.desktop $PWD

    echo "Done installing KiwiLight."
    echo "Put shortcut on desktop? [y/n]"

    read shortcut

    if [ ${shortcut} = 'y' ]; then
        #save current dir so we can go to desktop
        cd
        cp $currentDir1/KiwiLight/KiwiLight.desktop Desktop
        sudo chmod +x Desktop/KiwiLight.desktop
        echo "Shortcut placed."
    fi
}

echo "Install KiwiLight? [y/n]"
read install

if [ ${install} = 'y' ]; then
    Install
    else
    echo "Not installing. Goodbye!"
fi

echo "The installer has finished!"
