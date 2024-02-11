link_directories(
    ${GTKMM_LIBRARY_DIRS}  )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )


add_executable(mdr
    main.cc
    midi/communication.cc
    midi/controller.cc
    midi/animations.cc )

#    examplewindow.cc
#    labeledoptionmenu.cc

target_link_libraries(mdr 
    ${GTKMM_LIBRARIES} ${PORTMIDI_LIBRARIES} )
    
target_include_directories(mdr PRIVATE ${PORTMIDI_INCLUDE_DIRS})

set_target_properties( mdr
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)