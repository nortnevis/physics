function(copy_cl file)
	if(EXISTS ${PROJECT_SOURCE_DIR}/snippets/${file})
	    message("Copying ${PROJECT_SOURCE_DIR}/snippets/${file} to ${PROJECT_BINARY_DIR}/snippets")
	    file(COPY ${PROJECT_SOURCE_DIR}/snippets/${file} DESTINATION ${PROJECT_BINARY_DIR}/snippets)
	else()
	    message(WARN "File ${PROJECT_SOURCE_DIR}/snippets/${file} not found. Maybe this file didn't created yet, so reconfigure CMake project.")
	endif()
endfunction()

include_directories(./)

# Hello World printing
add_executable(opencl_hello_world hello_world/main.cpp)
target_precompile_headers(opencl_hello_world REUSE_FROM pch)
target_link_libraries(opencl_hello_world PRIVATE pch) 
copy_cl(hello_world/hello_world.cl)

# Mandelbrot set
add_executable(mandelbrot_set mandelbrot_set/main.cpp mandelbrot_set/render.cpp)
target_precompile_headers(mandelbrot_set REUSE_FROM pch)
target_link_libraries(mandelbrot_set PRIVATE pch)
copy_cl(mandelbrot_set/mandelbrot_set.cl)

# Check fp64 support
add_executable(check_fp64_support check_fp64_support.cpp)
target_precompile_headers(check_fp64_support REUSE_FROM pch)
target_link_libraries(check_fp64_support PRIVATE pch)

# Check max NDRange 
add_executable(get_max_ndrange get_max_ndrange.cpp)
target_precompile_headers(get_max_ndrange REUSE_FROM pch)
target_link_libraries(get_max_ndrange PRIVATE pch)

# Retrieve device and platform info using cpp wrapper
add_executable(get_info_cpp get_info_cpp.cpp)
target_precompile_headers(get_info_cpp REUSE_FROM pch)
target_link_libraries(get_info_cpp PRIVATE pch)

# Matrix multiplication
add_executable(matmul matmul/main.cpp)
target_precompile_headers(matmul REUSE_FROM pch)
target_link_libraries(matmul PRIVATE pch)
copy_cl(matmul/matmul.cl)
