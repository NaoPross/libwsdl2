cmake_minimum_required(VERSION 3.10)
project(
    WSDL2 
    VERSION 0.1
    DESCRIPTION "C++ SDL2 wrapper"
    LANGUAGES CXX
)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

############################
# find dependencies
find_package(SDL2 REQUIRED)

# optional dependencies
find_package(MM)
if (NOT MM-NOTFOUND)
    message("Building with libmm support")
endif ()

find_package(Threads)
if (NOT Threads-NOTFOUND)
    message("Building with Threads support")
endif()

find_package(SDL2TTF)
if (NOT SDL2TTF-NOTFOUND)
    message("Building with ttf support")
endif()

find_package(SDL2_image)
if (NOT SDL2_image-NOTFOUND)
    message("Building with image support")
endif()

############################
# libwsdl2
add_library(wsdl2 STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/video.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapsdl2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ttf.cpp
)

add_library(WSDL2::wsdl2 ALIAS wsdl2)

set_property(TARGET wsdl2 PROPERTY POSITION_INDEPENDENT_CODE ON)

# set up headers
target_include_directories(wsdl2
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_DIR}
)

target_compile_options(wsdl2
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:
            -pedantic -Wall -Wextra -Wcast-qual -Wcast-align -Wpointer-arith
            -Winit-self -Wshadow -Wswitch-enum -Wredundant-decls -Wfloat-equal
            -Wundef -Wvla -Wconversion -Wstrict-aliasing
        >
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# configuration of SDL2 under windows
if (MSVC)
	# setting up SDL for windows
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")

    # Support both 32 and 64 bit builds
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib")
    else ()
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2main.lib")
    endif ()

    string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

endif()

target_compile_definitions(wsdl2
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

# set compiler features
# For a complete list see:
# https://cmake.org/cmake/help/latest/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html
target_compile_features(wsdl2
    PRIVATE
        cxx_std_17
)

# set up required libraries
target_link_libraries(wsdl2
    PUBLIC
        ${SDL2_LIBRARIES}
)

if (NOT MM-NOTFOUND)
    target_compile_definitions(wsdl2 PRIVATE WSDL2_USE_MM)
    target_link_libraries(wsdl2 PRIVATE MM::MM)
endif ()

if (NOT SDL2TTF-FOUND)
    target_compile_definitions(wsdl2 PRIVATE WSDL2_TTF)
    target_link_libraries(wsdl2 PRIVATE ${SDL2TTF_LIBRARY})
endif()

if (SDL2IMAGE_FOUND)
    target_compile_definitions(wsdl2 PRIVATE WSDL2_IMG)
    target_link_libraries(wsdl2 PRIVATE ${SDL2IMAGE_LIBRARY})
endif()

###########################
# test section
###########################

enable_testing()

# window_test                                                                     
add_executable(window_test test/window_test.cpp)                                    

target_link_libraries(window_test 
    PRIVATE  
        WSDL2::wsdl2
) 

target_compile_features(window_test
    PRIVATE
        cxx_std_17
)

#target_include_directories(window_test
#    PRIVATE
#        ${CMAKE_CURRENT_DIR}/include
#)

                                                                                
add_test(
    NAME window_test 
    CONFIGURATIONS Debug  
    WORKING_DIRECTORY test 
    COMMAND window_test 
)      


if (NOT Threads-NOTFOUND)
    # threaded_window_test                                                                     
    add_executable(threaded_window_test test/threaded_window_test.cpp)                                    
                                                                                    
    target_link_libraries(threaded_window_test 
        PRIVATE  
            WSDL2::wsdl2
            Threads::Threads
    ) 

    target_compile_features(threaded_window_test
        PRIVATE
            cxx_std_17
    )

    # target_include_directories(threaded_window_test
    #     PRIVATE
    #         ${CMAKE_CURRENT_DIR}/include
    # )
                                                                                   
    add_test(
        NAME threaded_window_test 
        CONFIGURATIONS Debug  
        WORKING_DIRECTORY test 
        COMMAND threaded_window_test 
    ) 
endif ()

############################
# installation
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/WSDL2)

install(TARGETS wsdl2
    EXPORT wsdl2-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set_target_properties(wsdl2 PROPERTIES EXPORT_NAME WSDL2)

# install public headers
install(
    FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/include/wsdl2/event.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/wsdl2/util.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/wsdl2/video.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/wsdl2/wsdl2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/wsdl2/debug.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/wsdl2/ttf.hpp
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/wsdl2
)


# export targets
install(EXPORT wsdl2-targets
    FILE
        WSDL2Targets.cmake
    NAMESPACE
        WSDL2::
    DESTINATION
        ${INSTALL_CONFIGDIR}
)

# create ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/WSDL2ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/WSDL2Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/WSDL2Config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# install config, configversion
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/WSDL2Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/WSDL2ConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

############################
# export from the build tree
export(EXPORT wsdl2-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/WSDL2Targets.cmake
    NAMESPACE WSDL2::
)

export(PACKAGE WSDL2)
