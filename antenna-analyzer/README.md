ORPAL AA-PI version 1.0
==============

Antenna analyzer software for portable Linux-based devices

Based on the Yuri Kuchura's EU1KY_AA_V3 
https://bitbucket.org/kuchura/eu1ky_aa_v3/wiki/Home

Build process explained
==============

The build process depends on the platform you have chosen for prototyping. The only proven and tested platform so far is Raspberry Pi 3 (more platforms will be adopted soon).

*Raspberry Pi 3*


1) <b>Prepare your device </b>

    a) download Raspbian Jessie Light  https://www.raspberrypi.org/downloads/raspbian/
    
    b) flash the image to an SD card
    
    c) make sure your device can boot; setup network, ssh-server, rsync; use "raspi-config" for basic device configuration
    
    d) enable i2c, spi, i2s  overlays in /boot/config.txt or by using "raspi-config"
    
    e) install linux packages which QT5 framework depends on:
    
        apt-get install libfontconfig1-dev libdbus-1-dev libfreetype6-dev libudev-dev libpng12-dev libjpeg-dev libinput-dev libstdc++6  libstdc++7-dev

    f) install i2c & alsa development libraries
    
        apt-get install libi2c-dev i2c-tools libasound2-dev alsa-utils
        
    g) setup TFT-display and touchscreen (if you are planning to use it); in my prototype I either use my resistive Waveshare-compatible 3.5-inch display http://www.waveshare.com/wiki/3.5inch_RPi_LCD_(A) or Kedei HDMI 3.5-inch display. The last one does not require any DT-overlay and has a better image quality. You can also use a HDMI-monitor during prototyping stage as the AA software is capable of running on any display.

    h) configure your sound card; I use audioinjector-pi sound card in my prototype, the required DT-overlay is already included into Raspbian Jessie kernel (thanks to Flatmax); here is how it should look in /boot/config.txt:
        
        dtoverlay=audioinjector-wm8731-audio
        


2) <b>Prepare your build host </b>

    a) you need x64 multi-core platform with >= 8GB RAM
    
    b) install Ubuntu 16.04 (the only tested OS)
    
    c) follow instructions below to build local QT5 & QT-Creator; alternatively you can install QT5 & QT-Creator from Ubuntu repositories using "apt-get install" but I personally prefer building from sources; here is the list of required QT5 modules: *qtbase, qtxmlpatterns, qtscript, qttools, qtdeclarative, qtquickcontrols, qtquickcontrols2, qtcharts*
    
    http://wiki.qt.io/Building_Qt_5_from_Git
    
    https://wiki.qt.io/Building_Qt_Creator_from_Git
    
    d) make sure your QT-Creator runs without errors
    
    e) download Linaro ARM toolchain https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/arm-linux-gnueabihf/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz and extract the file into /usr/local/toolchains/ folder on the host machine using the command "tar xvf <file>". You can use a newer version of the toolchain if you prefer it.
   


3) <b>Cross-compile libraries</b>

    a) cross-compile FFTW library and deploy it to your device; you can use the following sequence:
    
        cd /home/pi/raspberrypi/rpi3
        wget http://www.fftw.org/fftw-3.3.6-pl2.tar.gz
        tar xvf fftw-3.3.6-pl2.tar.gz
        cp fftw-3.3.6-pl2
        mkdir build
        cd ./build
        mkdir dist
        export DIST_PATH="$(pwd)/dist"
        ../configure --with-slow-timer --host=arm-linux-gnueabihf --enable-float --enable-shared --enable-neon CC="/usr/local/toolchains/gcc-linaro-6.3.1-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc" CFLAGS="-march=armv8-a -mfpu=neon-fp-armv8 -mtune=cortex-a53 -mfloat-abi=hard" --prefix=$DIST_PATH --disable-fortran --enable-threads
        make
        make install
        rsync --rsync-path="sudo rsync" -avz dist/* pi@<DEVICE-IP-ADDR>:/usr

    
    b) you can optionally cross-compile DirectFB (if you wish to use it); I have not tried it personally and I would appreciate someone documenting their experience with that.



4) <b>Cross-compile QT5</b>

    a) download sysroot from the device; you can use rsync:
    
        cd /home/pi/raspberrypi/rpi3
        mkdir sysroot
        export HOST=<DEVICE-IP-ADDR>
        
        # clean sysroot directories
        sudo rm -rf sysroot/lib
        sudo rm -rf sysroot/usr/*
        sudo rm -rf sysroot/opt/*

        # download sysroot directories
        rsync -avz pi@${HOST}:/lib sysroot
        rsync -avz pi@${HOST}:/usr/include sysroot/usr
        rsync -avz pi@${HOST}:/usr/lib sysroot/usr
        rsync -avz pi@${HOST}:/usr/local/include sysroot/usr/local
        rsync -avz pi@${HOST}:/usr/local/lib sysroot/usr/local
        rsync -avz pi@${HOST}:/opt/vc sysroot/opt
        
        # qt5pi directory is not required at this step 
        # but MUST be downloaded after QT5 is built & deployed 
        rsync -avz pi@${HOST}:/usr/local/qt5pi sysroot/usr/local 
        
   
    b) download the following script and run it on sysroot directory:
    
        wget https://raw.githubusercontent.com/riscv/riscv-poky/master/scripts/sysroot-relativelinks.py
        ./sysroot-relativelinks.py sysroot
    
    
    c) make sure you repeat steps above after updating the kernel(firmware) or installing(removing) software parts on your device
    
    
    d) configure QT5 Embedded (recommended version >= 5.9) with no X11 support; here is the list of required QT5 modules: *qtbase, qtimageformats, qtxmlpatterns, qtscript, qtdeclarative, qtquickcontrols, qtquickcontrols2, qtcharts*; you can use the following sequence:
    
        cd /home/pi/raspberrypi/rpi3
        mkdir qt59
        cd qt59
        git clone https://code.qt.io/qt/qtbase.git -b 5.9
        git clone https://code.qt.io/qt/qtimageformats.git -b 5.9
        git clone https://code.qt.io/qt/qtxmlpatterns.git -b 5.9
        git clone https://code.qt.io/qt/qtscript.git -b 5.9
        git clone https://code.qt.io/qt/qtdeclarative.git -b 5.9
        git clone https://code.qt.io/qt/qtquickcontrols.git -b 5.9
        git clone https://code.qt.io/qt/qtquickcontrols2.git -b 5.9
        git clone https://code.qt.io/qt/qtcharts.git -b 5.9
        cd qtbase
        mkdir build
        cd build
        export RPI_TOOLCHAIN=/usr/local/toolchains/gcc-linaro-6.3.1-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
        export RPI_SYSROOT=/home/pi/raspberrypi/rpi3/sysroot
        ../configure -prefix /usr/local/qt5pi \
            -extprefix  /home/pi/raspberrypi/rpi3/qt5pi \
            -hostprefix /home/pi/raspberrypi/rpi3/qt5host \
            -v \
            -opensource -confirm-license \
            -release \
            -no-pch \
            -make libs \
            -device linux-rasp-pi3-g++ \
            -device-option CROSS_COMPILE=$RPI_TOOLCHAIN \
            -sysroot $RPI_SYSROOT \
            -opengl es2 \
            -no-directfb \
            -no-use-gold-linker \
            -no-openssl \
            -system-zlib \
            -system-libpng \
            -system-libjpeg \
            -no-xcb \
            -nomake examples -nomake tools -nomake tests \
            -no-sql-db2 -no-sql-ibase -no-sql-mysql -no-sql-oci -no-sql-odbc -no-sql-psql \
            -no-sql-tds  -no-sql-sqlite -no-sql-sqlite2

    
    e) when configure ends, please check configuration parameters listed in console; make sure QPA Backends of types EGLFS Raspberry Pi and LinuxFB have been configured (marked "yes")
   
   
    f) compile qtbase; resolve compilation issues (if you get any)
    
            make
            make install
   
   
    g) after qtbase compilation, please step into each module and compile it:
    
            cd ../../qtimageformats
            /home/pi/raspberrypi/rpi3/qt5host/bin/qmake -r
            make 
            make install
            
            cd ../../qtxmlpatterns
            /home/pi/raspberrypi/rpi3/qt5host/bin/qmake -r
            make 
            make install
            
            # repeat steps for qtscript, qtdeclarative, qtquickcontrols, qtquickcontrols2 
            # and qtcharts
            
            cd ../../qtcharts
            /home/pi/raspberrypi/rpi3/qt5host/bin/qmake -r
            make 
            make install
   
   
    h) deploy QT5 to your device; you can use rsync:
    
            rsync --rsync-path="sudo rsync" -avz /home/pi/raspberrypi/rpi3/qt5pi pi@<DEVICE-IP-ADDR>:/usr/local
    
 
 
5) <b>Setup QT-Creator Raspberry Pi3 kit</b>

    a) run QT-Creator on the host machine, for example:
    
            /opt/qt-creator/bin/qtcreator&
            
            
    b) open the project (use *antenna-analyzer.pro* file)
            
            
    c) you will be prompted to create a new development kit, please follow the steps and fill the required fields in
    
    
            - specify Kit name
            - device type: Generic Linux Device
            - create a new device
            - specify path to device's system root /home/pi/raspberrypi/rpi3/sysroot
            - specify paths to Linaro compilers
            - select Linaro GDB (or Multiarch GDB, if you have installed it)
            - Qt version should be set to /home/pi/raspberrypi/rpi3/qt5host
            
            
            
            
6) <b>Cross-compile & debug </b>

    a) build the application
    
    b) power up your device and debug application
    


    
