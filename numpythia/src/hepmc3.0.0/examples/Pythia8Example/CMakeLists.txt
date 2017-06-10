
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/interfaces/pythia8/include
                    ${PYTHIA8_INCLUDE_DIR})
add_executable(pythia8_example.exe ${CMAKE_CURRENT_SOURCE_DIR}/pythia8_example.cc ${PROJECT_SOURCE_DIR}/interfaces/pythia8/src/Pythia8ToHepMC3.cc)
target_link_libraries(pythia8_example.exe ${PYTHIA8_LIBRARIES} HepMC)
set_target_properties(pythia8_example.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR})

# create environment scripts

set(bindir "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
get_filename_component(PYTHIA8_LIB_DIR ${PYTHIA8_LIBRARY} PATH)
set(libdir "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}:${PYTHIA8_LIB_DIR}")
set(pythia8data "${PYTHIA8_XMLDOC_DIR}")

configure_file(${CMAKE_SOURCE_DIR}/examples/Pythia8Example/pythia8_example_env.sh.in
	       ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR}/pythia8_example_env.sh
     	       @ONLY
      	       )
# installs 

install(TARGETS pythia8_example.exe DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES pythia8_ee_to_Z_to_tautau.conf DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR}/ DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILES_MATCHING PATTERN "*.sh")
