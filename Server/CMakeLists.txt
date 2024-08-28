cmake_minimum_required(VERSION 3.14)

project(Server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
find_path(WIRINGPI_INCLUDE_DIRS NAMES wiringPi.h)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core WiringPi Threads)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Multimedia)

add_executable(Server
  main.cpp

  doorphone.h
  doorphone.cpp

  threadclass1.h
  threadclass1.cpp

  tcpHandler.h
  tcpHandler.cpp

  server.h
  server.cpp

  phoneHandler.h
  phoneHandler.cpp

  callplayer.h
  callplayer.cpp

  udpphone.h
  udpphone.cpp

  pins.h
  pins.cpp

  res.qrc
)
target_link_libraries(Server Qt${QT_VERSION_MAJOR}::Core
                             Qt${QT_VERSION_MAJOR}::Multimedia
                             ${CMAKE_THREAD_LIBS_INIT}
                             ${WIRINGPI_LIBRARIES})

install(TARGETS Server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
