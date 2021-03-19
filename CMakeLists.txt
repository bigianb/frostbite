# travis has cmake 3.9
cmake_minimum_required(VERSION 3.9)

project(FROSTBITE)

enable_testing()

set(PROJECT_NAME "Frostbite")
set(PROJECT_Version 0.1)
set(PROJECT_LIBS)

# Use this to find Qt on windows
if(ADDITIONAL_FIND_PATH)
    list(APPEND CMAKE_PREFIX_PATH ${ADDITIONAL_FIND_PATH})
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(COVERAGE OFF CACHE BOOL "Coverage")


find_package(SDL2 REQUIRED)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})



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
  snowlib/AnimationProcessor.h
  snowlib/AnimData.cpp
  snowlib/AnimData.h
  snowlib/AnimDecoder.cpp
  snowlib/AnimDecoder.h
  snowlib/DataAccess.h
  snowlib/DataUtil.h
  snowlib/FntDecoder.cpp
  snowlib/FntDecoder.h
  snowlib/GameRegion.h
  snowlib/GameType.h
  snowlib/GIFTag.cpp
  snowlib/GIFTag.h
  snowlib/GobFile.cpp
  snowlib/GobFile.h
  snowlib/Helpers.cpp
  snowlib/Helpers.h
  snowlib/LmpFile.cpp
  snowlib/LmpFile.h
  snowlib/LmpRepository.cpp
  snowlib/LmpRepository.h
  snowlib/Logger.cpp
  snowlib/Logger.h
  snowlib/Mesh.cpp
  snowlib/Mesh.h
  snowlib/Model.h
  snowlib/Palette.h
  snowlib/Scene.h
  snowlib/TexDecoder.cpp
  snowlib/TexDecoder.h
  snowlib/Texture.h
  snowlib/VertexDefs.h
  snowlib/VertexWeight.h
  snowlib/VifDecoder.cpp
  snowlib/VifDecoder.h
  snowlib/World.cpp
  snowlib/World.h
  snowlib/WorldReader.cpp
  snowlib/WorldReader.h
  )
  
include_directories("${FROSTBITE_SOURCE_DIR}/snowlib")

set(TESTAPP_SRC_FILES
  TestApp/Main.cpp
  TestApp/ModelWindow.cpp
)

set(CUTTOWN_SRC_FILES
  CuttownTest/gameData.cpp
  CuttownTest/gameData.h
  CuttownTest/infoPanel.cpp
  CuttownTest/infoPanel.h
  CuttownTest/Main.cpp
  CuttownTest/mainWindow.cpp
  CuttownTest/mainWindow.h
)

add_library(snowlib STATIC ${SNOWLIB_SRC_FILES})

add_executable(testApp ${TESTAPP_SRC_FILES})
target_link_libraries(testApp snowlib ${Boost_LIBRARIES} ${GLEW_LIBRARIES} SDL2::SDL2 SDL2::SDL2main opengl32.lib)

add_executable(cuttownTest ${CUTTOWN_SRC_FILES})
target_link_libraries(cuttownTest snowlib ${Boost_LIBRARIES} ${GLEW_LIBRARIES} SDL2::SDL2 SDL2::SDL2main opengl32.lib Qt6::Widgets)
set_target_properties(cuttownTest PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${Qt6Core_DIR}\\..\\..\\..\\bin;${Qt6Core_DIR}\\..\\..\\..\\plugins\\platforms")

