
project(liboctarine)

include(../incLLVM.cmake)
set(SOURCE_DIR ../src)
include(../globfiles.cmake)

# definition needed in headers to manage the dll export/import declarations
add_definitions(-Doct_LINK_STATIC)
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_h} ${${PROJECT_NAME}_c})
