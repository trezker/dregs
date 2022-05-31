project(test)
add_executable(test src/main.cpp)
target_link_libraries(test allegro allegro_primitives allegro_dialog m GLU GL)