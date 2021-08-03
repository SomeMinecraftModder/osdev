cmake_minimum_required(VERSION 3.1)

project(PawsOS VERSION 0.0.3 LANGUAGES C ASM_NASM)

file(GLOB_RECURSE SOURCES *.c *.asm)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)

set(CMAKE_C_COMPILER_TARGET i386-elf)
set(CMAKE_C_COMPILER i386-elf-gcc)
set(CMAKE_SYSTEM_PROCESSOR i386)
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

add_compile_options(-Wall)

option(WERROR "Convert warnings into errors" OFF)

if(WERROR)
    add_compile_options(-Werror)
endif()

include_directories(include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -Wextra \
-fno-exceptions -fno-pic -fstack-protector-all")

set(CMAKE_C_FLAGS_RELEASE "-Os")

set(CMAKE_ASM_NASM_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_DEBUG "-g")

set(CMAKE_ASM_NASM_FLAGS_RELWITHDEBINFO "-g")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -Og")

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> -T \
${CMAKE_SOURCE_DIR}/linker.ld <OBJECTS> -o <TARGET> <LINK_LIBRARIES> \
<FLAGS> -nostdlib -lgcc -fno-pie")

set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS>\
 -o <OBJECT> <SOURCE> -Wno-gnu-elf-extensions")

add_compile_options("$<$<COMPILE_LANGUAGE:ASM_NASM>:-f \
$<IF:$<BOOL:$<TARGET_PROPERTY:NASM_OBJ_FORMAT>>, \
$<TARGET_PROPERTY:NASM_OBJ_FORMAT>, ${CMAKE_ASM_NASM_OBJECT_FORMAT}>>")

set(CMAKE_OBJCOPY i386-elf-objcopy)

set(PROJECT_EMBED_OBJ_FILES "")
set(PROJECT_EMBED_FILES "drivers/font.sfn")

foreach(FILENAME ${PROJECT_EMBED_FILES})
    if(${CMAKE_VERSION} VERSION_LESS "3.20.0")
        get_filename_component(FILENAME_ONLY ${FILENAME} NAME)
        get_filename_component(FILEPATH_ONLY ${FILENAME} PATH)
    else() # Use cmake_path for >= 3.20.0 versions
        cmake_path(GET FILENAME FILENAME FILENAME_ONLY)
        cmake_path(GET FILENAME PARENT_PATH FILEPATH_ONLY)
    endif()
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME_ONLY}.o
                       WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/${FILEPATH_ONLY}
                       COMMAND ${CMAKE_OBJCOPY}
                       ARGS -I binary -O elf32-i386 ${FILENAME_ONLY}
                       ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME_ONLY}.o)
    list(APPEND PROJECT_EMBED_OBJ_FILES
         ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME_ONLY}.o)
endforeach(FILENAME)

add_executable(os-image.iso ${SOURCES} ${PROJECT_EMBED_OBJ_FILES})

find_program(GRUB_MKRESCUE grub-mkrescue REQUIRED)

add_custom_command(TARGET os-image.iso
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND}
                   ARGS -E make_directory ${CMAKE_BINARY_DIR}/iso/boot/grub/
                   COMMAND ${CMAKE_COMMAND}
                   ARGS -E copy $<TARGET_FILE:os-image.iso>
                   ${CMAKE_BINARY_DIR}/iso/boot/
                   COMMAND ${CMAKE_COMMAND}
                   ARGS -E copy ${CMAKE_SOURCE_DIR}/grub.cfg
                   ${CMAKE_BINARY_DIR}/iso/boot/grub/
                   COMMAND ${GRUB_MKRESCUE}
                   ARGS -o ${CMAKE_BINARY_DIR}/os-image.iso iso)

set_target_properties(os-image.iso PROPERTIES NASM_OBJ_FORMAT elf32
                      ADDITIONAL_CLEAN_FILES "os-image.iso;iso"
                      OUTPUT_NAME "os-image.elf")
