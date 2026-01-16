set(METAL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/shader.metal)
set(METAL_LIB ${CMAKE_CURRENT_BINARY_DIR}/shader.metallib)
set(METAL_AIR ${CMAKE_CURRENT_BINARY_DIR}/shader.air)

add_custom_command(
    OUTPUT ${METAL_LIB}
    COMMAND xcrun -sdk macosx metal -c ${METAL_SRC} -o ${CMAKE_CURRENT_BINARY_DIR}/shader.air
    COMMAND xcrun -sdk macosx metallib ${CMAKE_CURRENT_BINARY_DIR}/shader.air -o ${METAL_LIB}
    DEPENDS ${METAL_SRC}
    COMMENT "Compiling Metal shader"
)

add_custom_target(idk ALL DEPENDS ${METAL_LIB})
add_dependencies(speed idk)
target_link_libraries(speed metal)
