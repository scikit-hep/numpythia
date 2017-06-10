

if(ROOT_FOUND)
include_directories(${PROJECT_SOURCE_DIR}/rootIO/include ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}  ${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Generate dictionaries
#


add_executable(convert_example.exe
        ${CMAKE_CURRENT_SOURCE_DIR}/convert_example.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WriterRootTreeOPAL.cc 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WriterHEPEVTZEUS.cc )
target_link_libraries(convert_example.exe ${ROOT_LIBRARIES} HepMC HepMCrootIO)
set_target_properties(convert_example.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR})
add_dependencies( convert_example.exe dictionary )


# create environment scripts

set(bindir "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set(libdir "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}:${ROOT_LIBRARY_DIR}")
else()
include_directories(${PROJECT_SOURCE_DIR}/rootIO/include ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
#----------------------------------------------------------------------------
# Generate dictionaries
#
add_executable(convert_example.exe
        ${CMAKE_CURRENT_SOURCE_DIR}/convert_example.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WriterHEPEVTZEUS.cc )
target_link_libraries(convert_example.exe HepMC )
set_target_properties(convert_example.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR})
add_dependencies( convert_example.exe dictionary )
# create environment scripts
set(bindir "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set(libdir "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()
 
configure_file(
${CMAKE_SOURCE_DIR}/examples/ConvertExample/convert_example_env.sh.in
	    ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR}/convert_example_env.sh
     	     @ONLY
      	     )

# installs 
install(TARGETS convert_example.exe DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${PROJECT_BINARY_DIR}/examples/ConvertExample/ DESTINATION ${CMAKE_INSTALL_BINDIR} FILES_MATCHING PATTERN "*.pcm" PATTERN "CMakeFiles" EXCLUDE )
install(DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR}/ DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILES_MATCHING PATTERN "*.sh")
