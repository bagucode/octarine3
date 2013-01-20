
project(octarine)

include(../incLLVM.cmake)
set(SOURCE_DIR ../src)
include(../globfiles.cmake)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_h} ${${PROJECT_NAME}_c})

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit native)

# TODO: factor out the link stuff to another file? It is now duplicated
if(WIN32)
  target_link_libraries(${PROJECT_NAME} liboctarine ${REQ_LLVM_LIBRARIES})
elseif(APPLE)
  find_library (CF_LIBRARY CoreFoundation)
  #find_library (CS_LIBRARY CoreServices)
  mark_as_advanced (CF_LIBRARY)
  set (EXTRA_LIBS ${CF_LIBRARY})
  target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS} liboctarine ${REQ_LLVM_LIBRARIES})
else()
  target_link_libraries(${PROJECT_NAME} liboctarine pthread ${REQ_LLVM_LIBRARIES})
endif()

