#-------------------------------------------------
#
# Project created by QtCreator 2017-03-08T18:46:21
#
#-------------------------------------------------

QT      += core quick quickcontrols2 charts

CONFIG  += c++11
# Virtual keyboard desktop integration will be disabled for the embedded system
CONFIG  += disable-desktop

TEMPLATE = app

# Additional import path used to resolve QML modules in Qt Creator's code model
# QML_IMPORT_PATH += $$[QT_SYSROOT]/usr/local/qt5pi/qml
# QML2_IMPORT_PATH += $$[QT_SYSROOT]/usr/local/qt5pi/qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    serial/uart_device.h \
    serial/serial_modem.h \
    analyzer/callback_holder.h \
    analyzer/math_utils.h \
    analyzer/antscope/antscope_device.h \
    audio/impl/alsa_reader.h \
    audio/audio_reader.h \
    analyzer_object.h \
    analyzer_app.h \
    ui/qml_about_view.h \
    ui/qml_view_backend.h \
    ui/qml_dsp_view.h \
    ui/qml_config_view.h \
    ui/qml_hwcal_view.h \
    ui/qml_oslcal_view.h \
    ui/qml_measure_view.h \
    ui/qml_panvswr_view.h \
    analyzer/analyzer_dsp.h \
    analyzer/analyzer_dsp.h \
    analyzer/analyzer_generator.h \
    analyzer/analyzer_config.h \
    analyzer/analyzer_measure.h \
    error_codes.h \
    analyzer/analyzer_calibrator.h

SOURCES += main.cpp \
    serial/uart_device.cpp \
    serial/serial_modem.cpp \
    audio/audio_reader.cpp \
    audio/impl/alsa_reader.cpp \
    analyzer/math_utils.cpp \
    analyzer/antscope/antscope_device.cpp \
    analyzer_app.cpp \
    analyzer/analyzer_config.cpp \
    analyzer/analyzer_dsp.cpp \
    analyzer/analyzer_generator.cpp \
    analyzer/analyzer_measure.cpp \
    ui/qml_about_view.cpp \
    ui/qml_config_view.cpp \
    ui/qml_dsp_view.cpp \
    ui/qml_hwcal_view.cpp \
    ui/qml_measure_view.cpp \
    ui/qml_oslcal_view.cpp \
    ui/qml_panvswr_view.cpp \
    ui/qml_view_backend.cpp \
    analyzer/analyzer_calibrator.cpp


RESOURCES += qml.qrc

INCLUDEPATH +=

LIBS    += -lasound -lfftw3f -lm

# Default rules for deployment.
target.path = /opt/$${TARGET}/bin
INSTALLS += target

DISTFILES +=
