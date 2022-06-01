project(test)
file(GLOB test_SRC "src/*.h" "src/*.c")
add_executable(test ${test_SRC})
target_link_libraries(test allegro allegro_primitives allegro_dialog allegro_image m GLU GL)