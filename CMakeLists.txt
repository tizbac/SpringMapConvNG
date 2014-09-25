project(SpringMapConvNG)
cmake_minimum_required(VERSION 2.6)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


if (NOT MINGW )
        include_directories(${CMAKE_CURRENT_BINARY_DIR} ${IL_INCLUDE_DIR})
else ( NOT MINGW )
        include_directories(${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/winlibs/Devil_${ARCH}/include )
        link_directories( ${PROJECT_SOURCE_DIR}/winlibs/Devil_${ARCH}/lib ${PROJECT_SOURCE_DIR}/winlibs/Devil_${ARCH} )
endif ( NOT MINGW )

#qt4_automoc(${springMapConvNG_SRCS})

find_package(ImageMagick COMPONENTS Magick++)
find_package(DevIL REQUIRED)
include_directories(${ImageMagick_INCLUDE_DIRS})


add_library(mapconv SHARED CRC.cpp Image.cpp SMFMap.cpp TileStorage.cpp )
add_library(mapconv_static STATIC CRC.cpp Image.cpp SMFMap.cpp TileStorage.cpp )
add_executable(springMapConvNG main.cpp)
add_executable(smfdecompiler decompiler.cpp)

set(MAPCONV_LIBS ${IL_LIBRARIES} ${ImageMagick_LIBRARIES})

if (NOT MINGW )
	target_link_libraries(springMapConvNG mapconv_static ${MAPCONV_LIBS})
	target_link_libraries(mapconv ${IL_LIBRARIES})
	target_link_libraries(smfdecompiler mapconv_static ${MAPCONV_LIBS})
else ( NOT MINGW )
	target_link_libraries(springMapConvNG mapconv ${MAPCONV_LIBS})
	target_link_libraries(mapconv ${IL_LIBRARIES})
	target_link_libraries(smfdecompiler mapconv ${MAPCONV_LIBS})
endif ( NOT MINGW )

install(TARGETS springMapConvNG smfdecompiler RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
install(TARGETS mapconv_static ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
install(TARGETS mapconv LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

