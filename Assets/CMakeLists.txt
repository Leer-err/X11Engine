file(GLOB_RECURSE PIPELINES CONFIGURE_DEPENDS "Shaders/Pipelines/**.slang")
file(GLOB_RECURSE SHADER_MODULES CONFIGURE_DEPENDS "Shaders/Modules/**.slang")

if(UNIX AND NOT APPLE)
    set(SLANG_COMPILER slangc)
elseif(WIN32)
    set(SLANG_COMPILER slangc.exe)
endif()

foreach(src_file ${SHADER_MODULES})
    get_filename_component(parent_folder ${src_file} DIRECTORY)

    list(APPEND SHADER_INCLUDES -I${parent_folder})
endforeach()

foreach(src_file ${PIPELINES})
    get_filename_component(filename ${src_file} NAME_WE)
    get_filename_component(parent_folder ${src_file} DIRECTORY)
    get_filename_component(parent_folder ${parent_folder} NAME)
    set(out_file "${CMAKE_BINARY_DIR}/bin/Assets/Shaders/${parent_folder}/${filename}.spv")

    add_custom_command(
        OUTPUT ${out_file}
        COMMAND ${SLANG_COMPILER} ${src_file} -o ${out_file} ${SHADER_INCLUDES}
        DEPENDS ${src_file}
        COMMENT "Generating ${filename}.spv"
        VERBATIM
    )
    
    list(APPEND SHADER_OUTPUTS ${out_file})
endforeach()

add_custom_target(compile_shaders ALL DEPENDS ${SHADER_OUTPUTS})