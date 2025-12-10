set(CMAKE_AUTORCC ON)

qt_add_executable(aapi_app
    main.cpp
    aapi_error.h
    aapi_object.h
    analyzer/antscope/antscope_device.cpp analyzer/antscope/antscope_device.h
    analyzer/aapi_calibrator.cpp analyzer/aapi_calibrator.h
    analyzer/aapi_config.cpp analyzer/aapi_config.h
    analyzer/aapi_generator.cpp analyzer/aapi_generator.h
    analyzer/aapi_measure_task.cpp analyzer/aapi_measure_task.h
    analyzer/aapi_signal_process.cpp analyzer/aapi_signal_process.h
    analyzer/aapi_device.cpp analyzer/aapi_device.h
    audio/audio_reader.cpp audio/audio_reader.h
    audio/impl/alsa_reader.cpp audio/impl/alsa_reader.h
    serial/aapi_serial_modem.cpp serial/aapi_serial_modem.h
    serial/aapi_uart_device.cpp serial/aapi_uart_device.h
    utils/aapi_math_utils.h
    utils/simple_array.h
    utils/simple_string.cpp utils/simple_string.h
    utils/simple_variant.h utils/simple_variant.cpp
    utils/aapi_complex.h
)

set_source_files_properties(AapiTheme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(aapi_app
    URI aapi
    VERSION 1.0
    QML_FILES
        Main.qml
        BatteryIndicator.qml
        TabOscilloscope.qml
        TabSmithChart.qml TabSpectrum.qml TabVswrChart.qml
        SwipePage.qml
        TabRxChart.qml SmithChart.qml StatusIndicatorCtrl.qml TwoStateButtonCtrl.qml
        ProgressCtrl.qml
        TabPanVswr.qml
        MeasureViewPage.qml
        ViewParamEnum.qml ViewParamText.qml
        ViewHwCalibration.qml
        ViewOslCalibration.qml
        ViewConfiguration.qml
        ViewSignalProcess.qml
        ViewMeasurement.qml
        ViewPanoramicScan.qml
        ViewAboutApp.qml
        ViewGenerator.qml
        AapiTheme.qml
        MessageBoxItem.qml
        AapiMessageBox.qml
    SOURCES
        aapi_application.cpp aapi_application.h
        ui/aapi_about_app_view.cpp ui/aapi_about_app_view.h
        ui/aapi_configuration_view.cpp ui/aapi_configuration_view.h
        ui/aapi_hw_calibration_view.cpp ui/aapi_hw_calibration_view.h
        ui/aapi_measure_view.cpp ui/aapi_measure_view.h
        ui/aapi_osl_calibration_view.cpp ui/aapi_osl_calibration_view.h
        ui/aapi_panoramic_scan_view.cpp ui/aapi_panoramic_scan_view.h
        ui/aapi_signal_process_view.cpp ui/aapi_signal_process_view.h
        ui/aapi_status_backend.cpp ui/aapi_status_backend.h
        ui/aapi_view_backend.cpp ui/aapi_view_backend.h
        ui/aapi_base_style.cpp ui/aapi_base_style.h
        ui/aapi_style_smith.h ui/aapi_style_smith.cpp
        ui/aapi_aquish_style.cpp ui/aapi_aquish_style.h
        ui/aapi_style_chart.h ui/aapi_style_chart.cpp
        ui/aapi_messages.h ui/aapi_messages.cpp
        SOURCES utils/simple_map.h
)

set_source_files_properties(resources/battery-empty.svg PROPERTIES QT_RESOURCE_ALIAS empty)
set_source_files_properties(resources/battery-full.svg PROPERTIES QT_RESOURCE_ALIAS full)
set_source_files_properties(resources/battery-half-power.svg PROPERTIES QT_RESOURCE_ALIAS half-power)
set_source_files_properties(resources/battery-one-quarter.svg PROPERTIES QT_RESOURCE_ALIAS one-quarter)
set_source_files_properties(resources/battery-three-quarters.svg PROPERTIES QT_RESOURCE_ALIAS three-quarters)
set_source_files_properties(resources/error.png PROPERTIES QT_RESOURCE_ALIAS error)
set_source_files_properties(resources/warning.png PROPERTIES QT_RESOURCE_ALIAS warning)
set_source_files_properties(resources/exclamation.png PROPERTIES QT_RESOURCE_ALIAS exclamation)
set_source_files_properties(resources/errors-en_US.ini PROPERTIES QT_RESOURCE_ALIAS errors-en_US)
set_source_files_properties(resources/license.txt PROPERTIES QT_RESOURCE_ALIAS license)

qt_add_resources(aapi_app "images"
    PREFIX "/images"
    FILES
        resources/error.png
        resources/warning.png
        resources/exclamation.png
)

qt_add_resources(aapi_app "battery"
    PREFIX "/battery"
    FILES
        resources/battery-empty.svg
        resources/battery-full.svg
        resources/battery-half-power.svg
        resources/battery-one-quarter.svg
        resources/battery-three-quarters.svg
)

qt_add_resources(aapi_app "conf"
    PREFIX "/"
    FILES
        qtquickcontrols2.conf
)

qt_add_resources(aapi_app "files"
    PREFIX "/files"
    FILES
        resources/license.txt
)

qt_add_resources(aapi_app "errors"
    PREFIX "/errors"
    FILES
        resources/errors-en_US.ini
)

#images/battery-charged-plugin-dark.svg
#images/battery-charged-plugin.svg
#images/battery-charging-dark.svg
#images/battery-charging.svg
#images/battery-critical.svg


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(aapi_app PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appaapi
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(aapi_app PRIVATE
    AAPI_QML_UNCREATABLE_REASON="Create not allowed"
    AAPI_APP_NAME="orpal-aapi"
    AAPI_VERSION="4.2")


target_link_libraries(aapi_app PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::GuiPrivate
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Charts
)

include(GNUInstallDirs)
install(TARGETS aapi_app
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

