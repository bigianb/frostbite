# travis has cmake 3.9
cmake_minimum_required(VERSION 3.9)

project(FROSTBITE)

enable_testing()

set(PROJECT_NAME "Frostbite")
set(PROJECT_Version 0.1)
set(PROJECT_LIBS)

# CMAKE_PREFIX_PATH evn var or cach var should be set to find Qt
set(CMAKE_PREFIX_PATH "C:\\Qt\\6.0.2\\msvc2019_64")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(COVERAGE OFF CACHE BOOL "Coverage")

set(Boost_USE_STATIC_LIBS        ON)
find_package(Boost REQUIRED COMPONENTS program_options filesystem)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

find_package(SDL2 REQUIRED)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

# flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP")
endif()
if (COVERAGE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: None Debug Release"
		FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	add_definitions(-D_UNICODE -DUNICODE)
endif()

set(SNOWLIB_SRC_FILES
  snowlib/AnimationProcessor.cpp
  snowlib/AnimData.cpp
  snowlib/AnimDecoder.cpp
  snowlib/FntDecoder.cpp
  snowlib/GameType.h
  snowlib/GIFTag.cpp
  snowlib/GobFile.cpp
  snowlib/Helpers.cpp
  snowlib/LmpFile.cpp
  snowlib/LmpRepository.cpp
  snowlib/Logger.cpp
  snowlib/Mesh.cpp
  snowlib/TexDecoder.cpp
  snowlib/VifDecoder.cpp
  snowlib/World.cpp
  snowlib/WorldReader.cpp  
  )
  
include_directories("${FROSTBITE_SOURCE_DIR}/snowlib")

set(TESTAPP_SRC_FILES
  TestApp/Main.cpp
  TestApp/ModelWindow.cpp
)

set(CUTTOWN_SRC_FILES
  CuttownTest/Main.cpp
  CuttownTest/mainWindow.cpp
  CuttownTest/mainWindow.h
)

add_library(snowlib STATIC ${SNOWLIB_SRC_FILES})

add_executable(testApp ${TESTAPP_SRC_FILES})
target_link_libraries(testApp snowlib ${Boost_LIBRARIES} ${GLEW_LIBRARIES} SDL2::SDL2 SDL2::SDL2main opengl32.lib)

add_executable(cuttownTest ${CUTTOWN_SRC_FILES})
target_link_libraries(cuttownTest snowlib ${Boost_LIBRARIES} ${GLEW_LIBRARIES} SDL2::SDL2 SDL2::SDL2main opengl32.lib Qt6::Widgets)

