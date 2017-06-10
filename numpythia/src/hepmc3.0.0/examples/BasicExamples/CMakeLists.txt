# building tests
#------------------
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)
#

#---Add executables------------------------------------------------------------

add_executable(basic_tree.exe ${CMAKE_CURRENT_SOURCE_DIR}/basic_tree.cc)
target_link_libraries(basic_tree.exe HepMC ${ROOT_LIBRARIES})
set_target_properties(basic_tree.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR})

install(TARGETS basic_tree.exe DESTINATION ${CMAKE_INSTALL_BINDIR})

if(CMAKE_Fortran_COMPILER)
 add_executable(hepevt_wrapper_example.exe ${CMAKE_CURRENT_SOURCE_DIR}/hepevt_wrapper_example_main.cc ${CMAKE_CURRENT_SOURCE_DIR}/hepevt_wrapper_example_fortran.f)
 target_link_libraries(hepevt_wrapper_example.exe HepMC_static)
 set_target_properties(hepevt_wrapper_example.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR})

install(TARGETS hepevt_wrapper_example.exe DESTINATION ${CMAKE_INSTALL_BINDIR})

else()
 message(STATUS "No Fortran compiler - dissabling HepEvt example.")
endif()

add_executable(HepMC2_reader_example.exe ${CMAKE_CURRENT_SOURCE_DIR}/HepMC2_reader_example.cc)
target_link_libraries(HepMC2_reader_example.exe HepMC_static ${ROOT_LIBRARIES})
set_target_properties(HepMC2_reader_example.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR})

install(TARGETS HepMC2_reader_example.exe DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(HepMC3_fileIO_example.exe ${CMAKE_CURRENT_SOURCE_DIR}/HepMC3_fileIO_example.cc)
target_link_libraries(HepMC3_fileIO_example.exe HepMC_static ${ROOT_LIBRARIES})
set_target_properties(HepMC3_fileIO_example.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/${CMAKE_INSTALL_BINDIR})

install(TARGETS HepMC3_fileIO_example.exe DESTINATION ${CMAKE_INSTALL_BINDIR})
