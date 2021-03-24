cmake_minimum_required(VERSION 3.12)

# set source directory name as the project name
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})


# add the sources
file(GLOB SPARKFUN "sparkfun/*.cc")

set(SOURCE_FILES ${SPARKFUN})

# include directories
include_directories(./)
include_directories(sparkfun/)
include_directories(arc_mli_package/include)

# add the executable
add_executable(${PROJECT_NAME}.elf main.cc print_all.c ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}.elf ${MY_LINKER_FLAGS})

add_custom_target(image DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET image USES_TERMINAL
    COMMAND cp ${PROJECT_NAME}.elf ../image_gen_linux_v3
    COMMAND cp ${PROJECT_NAME}.map ../image_gen_linux_v3
    COMMAND ${CMAKE_COMMAND} ARGS -E chdir ../image_gen_linux_v3 image_gen -e ${PROJECT_NAME}.elf -m ${PROJECT_NAME}.map -o out.img
    COMMAND ${CMAKE_COMMAND} ARGS -E chdir ../image_gen_linux_v3 rm ${PROJECT_NAME}.elf
    COMMAND ${CMAKE_COMMAND} ARGS -E chdir ../image_gen_linux_v3 rm ${PROJECT_NAME}.map)
