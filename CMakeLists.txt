cmake_minimum_required(VERSION 3.16)

project(ModbusBMS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Network SerialPort Qml)

qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/core/AppEngine.cpp src/core/AppEngine.h
    src/modbus/ModbusClient.cpp src/modbus/ModbusClient.h
    src/modbus/ModbusTcpClient.cpp src/modbus/ModbusTcpClient.h
    src/modbus/ModbusRtuClient.cpp src/modbus/ModbusRtuClient.h
    src/modbus/ModbusRequest.cpp src/modbus/ModbusRequest.h
    src/modbus/ModbusResponse.cpp src/modbus/ModbusResponse.h
    src/devices/Device.cpp src/devices/Device.h
    src/devices/DeviceManager.cpp src/devices/DeviceManager.h
    src/devices/SwitchDevice.cpp src/devices/SwitchDevice.h
    src/devices/RelayDevice.cpp src/devices/RelayDevice.h
    src/devices/SensorDevice.cpp src/devices/SensorDevice.h
    src/devices/DeviceModel.cpp src/devices/DeviceModel.h
    src/communication/CommandQueue.cpp src/communication/CommandQueue.h
    src/communication/PollingEngine.cpp src/communication/PollingEngine.h
    src/storage/JsonStorage.cpp src/storage/JsonStorage.h
    src/storage/SettingsManager.cpp src/storage/SettingsManager.h
    src/logging/AppLogger.cpp src/logging/AppLogger.h
    qml.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Network
    Qt6::SerialPort
    Qt6::Qml
)

# Copy JSON config files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources/devices.json DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resources/settings.json DESTINATION ${CMAKE_BINARY_DIR})

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_import_qml_plugins(${PROJECT_NAME})
qt_import_plugins(${PROJECT_NAME}
    INCLUDE Qt6::QmlFolderListModelPlugin
)
