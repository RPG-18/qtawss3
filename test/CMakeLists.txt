macro(add_target_to_test targetName testName testFile)
    set(current_target ${targetName}.${testName})
    add_executable(${current_target} ${testFile})
    target_link_libraries(${current_target} gtest test_main ${ARGN})
    add_test(NAME ${current_target}
             COMMAND ${current_target})
endmacro(add_target_to_test)

add_library(test_main STATIC test_main.cpp)
target_link_libraries(test_main Qt5::Core)

add_target_to_test(Core Bucket TestBucket.cpp core)
add_target_to_test(Core BucketRespons TestBucketResponse.cpp core)
add_target_to_test(Gui ObjectTree TestObjectTree.cpp core gui)