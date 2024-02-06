link_directories(
    ${GTKMM_LIBRARY_DIRS}  )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )


add_executable(mdr
    main.cc )

#    examplewindow.cc
#    labeledoptionmenu.cc

target_link_libraries(mdr 
    ${GTKMM_LIBRARIES} ${PORTMIDI_LIBRARIES} )
# target_link_libraries(mdr 
#     ${PORTMIDI_LIBRARIES} )

target_include_directories(mdr PRIVATE ${PORTMIDI_INCLUDE_DIRS})