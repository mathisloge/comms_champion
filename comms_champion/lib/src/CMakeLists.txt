function (lib_comms_champion)
    set (name "${COMMS_CHAMPION_LIB_NAME}")
    
    if (NOT Qt5Core_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Core library")
        return()
    endif ()
    
    if (NOT Qt5Widgets_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Widgets library")
        return()
    endif ()
    
    set (src
        ErrorStatus.cpp
        Message.cpp
        Protocol.cpp
        Filter.cpp
        Socket.cpp
        MessageHandler.cpp
        Plugin.cpp
        DataInfo.cpp
        PluginProperties.cpp
        ConfigMgr.cpp
        PluginMgr.cpp
        PluginMgrImpl.cpp
        MsgFileMgr.cpp
        MsgSendMgr.cpp
        MsgSendMgrImpl.cpp
        MsgMgr.cpp
        MsgMgrImpl.cpp
        field_wrapper/FieldWrapper.cpp
        field_wrapper/IntValueWrapper.cpp
        field_wrapper/UnsignedLongValueWrapper.cpp
        field_wrapper/BitmaskValueWrapper.cpp
        field_wrapper/EnumValueWrapper.cpp
        field_wrapper/StringWrapper.cpp
        field_wrapper/BitfieldWrapper.cpp
        field_wrapper/OptionalWrapper.cpp
        field_wrapper/BundleWrapper.cpp
        field_wrapper/ArrayListRawDataWrapper.cpp
        field_wrapper/ArrayListWrapper.cpp
        field_wrapper/FloatValueWrapper.cpp
        field_wrapper/VariantWrapper.cpp
        field_wrapper/UnknownValueWrapper.cpp
        property/message.cpp
        property/field.cpp
    )
    
    qt5_wrap_cpp(
        moc
        MsgSendMgrImpl.h
    )
    
    add_library(${name} SHARED ${src} ${moc})
    add_library (cc::${COMMS_CHAMPION_LIB_NAME} ALIAS ${name})

    target_link_libraries(${name} cc::comms Qt5::Widgets Qt5::Core ${CC_PLATFORM_SPECIFIC})

    target_include_directories(${name} 
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        INTERFACE
            $<INSTALL_INTERFACE:include>
    )

    target_compile_definitions(${name} PRIVATE -DCC_LIB_EXPORT)    
    
    set_target_properties(${name} PROPERTIES VERSION ${COMMS_VERSION})
    
    install (
        TARGETS ${name} EXPORT ${COMMS_CHAMPION_LIB_NAME}Export
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
    )

    install(EXPORT ${COMMS_CHAMPION_LIB_NAME}Export NAMESPACE cc::
        DESTINATION ${LIB_INSTALL_DIR}/${INSTALL_NAME}/cmake
    )
    
endfunction ()

###########################################################
find_package(Qt5 COMPONENTS Widgets REQUIRED)

lib_comms_champion()
