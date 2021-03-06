function (cc_plugin_demo)
    set (name "demo")
    
    set (meta_file "${CMAKE_CURRENT_SOURCE_DIR}/demo.json")
    set (stamp_file "${CMAKE_CURRENT_BINARY_DIR}/demo_refresh_stamp.txt")
    
    set (refresh_plugin_header TRUE)
    if ((NOT EXISTS ${stamp_file}) OR (${meta_file} IS_NEWER_THAN ${stamp_file}))
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/Plugin.h)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${stamp_file})
    endif ()
    
    
    set (src
        Plugin.cpp
        Protocol.cpp
        TransportMessage.cpp
        Message.cpp
        message/IntValues.cpp
        message/EnumValues.cpp
        message/BitmaskValues.cpp
        message/Bitfields.cpp
        message/Strings.cpp
        message/Lists.cpp
        message/Optionals.cpp
        message/FloatValues.cpp
        message/Variants.cpp
    )
    
    set (hdr
        Plugin.h
    )
    
    qt5_wrap_cpp(
        moc
        ${hdr}
    )
    
    add_library (${name} MODULE ${src} ${moc})
    target_link_libraries(${name} cc::${COMMS_CHAMPION_LIB_NAME})

    target_compile_options(${name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wno-old-style-cast>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-old-style-cast>
    )      
    
    install (
        TARGETS ${name}
        DESTINATION ${PLUGIN_INSTALL_DIR})
        
endfunction()

######################################################################

find_package(Qt5Core REQUIRED)

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/comms_champion/lib/include)

cc_plugin_demo ()

FILE(GLOB_RECURSE headers "*.h")
add_custom_target(demo.cc_plugin_headers SOURCES ${headers})
