#!/bin/bash
PROJDIR=$1
CONFIG=$2
BOARD=$3
QT5_DIR=$4
PROJFILE=${PROJDIR}/sources/antenna-analyzer.pro
DEVSPEC="none"

case $BOARD in
"bananapim2")
    DEVSPEC="linux-sun6i-g++"
    ;;
"orangepipc")
    DEVSPEC="linux-sun8i-g++"
    ;;
"rpi3-aplus-arm")
    DEVSPEC="linux-rasp-pi3-vc4-g++"
    ;;
"rpi3-aplus-arm64")
    DEVSPEC="linux-rpi3-arm64-vc4-g++"
    ;;
esac

if [ "$DEVSPEC" = "none" ]; then
    echo "Unknown device specification."
    exit 1
fi

QMAKE=${QT5_DIR}/build/${DEVSPEC}/qt5host/bin/qmake

if [ "$CONFIG" = "Debug" ]; then
    ${QMAKE} -makefile -spec ${QT5_DIR}/qtbase/mkspecs/devices/${DEVSPEC} CONFIG+=debug CONFIG+=qml_debug ${PROJFILE}
else
    ${QMAKE} -makefile -spec ${QT5_DIR}/qtbase/mkspecs/devices/${DEVSPEC} ${PROJFILE}
fi

/usr/bin/make qmake_all

/usr/bin/make
