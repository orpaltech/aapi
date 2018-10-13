#!/bin/bash
PROJDIR=$1
CONFIG=$2
BOARD=$3
QT5_DIR=$4
QMAKE=${QT5_DIR}/build/${BOARD}/qt5host/bin/qmake
PROJFILE=${PROJDIR}/sources/antenna-analyzer.pro
DEVSPEC="none"

case $BOARD in
"bananapim2")
    DEVSPEC="linux-sun6i-g++"
    ;;
"orangepipc")
    DEVSPEC="linux-sun8i-g++"
    ;;
"rpi3-arm")
    DEVSPEC="linux-rasp-pi3-g++"
    ;;
esac

if [ "$DEVSPEC" = "none" ]; then
    echo "Unknown device specification."
    exit 1
fi

if [ "$CONFIG" = "Debug" ]; then
    ${QMAKE} -makefile -spec ${QT5_DIR}/qtbase/mkspecs/devices/${DEVSPEC} CONFIG+=debug CONFIG+=qml_debug ${PROJFILE}
else
    ${QMAKE} -makefile -spec ${QT5_DIR}/qtbase/mkspecs/devices/${DEVSPEC} ${PROJFILE}
fi

/usr/bin/make qmake_all

/usr/bin/make
