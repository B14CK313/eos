enable_testing()
add_executable(EOS_TEST testSimple.cpp testSimple.cpp)
target_link_libraries(EOS_TEST PRIVATE eos doctest)

add_test(NAME EOS_TEST COMMAND EOS_TEST)