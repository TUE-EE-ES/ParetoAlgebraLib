cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 11)

include_directories(. ../cudd/cudd ../gbdd ../calculator)

add_compile_options(-Wno-deprecated-declarations)

add_executable(Betsy
	betsy.cc
)
add_executable(ProdConsTest
	prodconstest.cc
)
add_executable(Test
	test.cc
)

#link_directories(../gbdd/)
target_link_libraries(Betsy
	"/home/mgeilen/temp/gbdd/libGBDD.a"
	"/home/mgeilen/temp/cudd/cudd/.libs/libcudd.a"
	"/home/mgeilen/temp/calculator/libParetoCalculator.a"
)
target_link_libraries(ProdConsTest
	"/home/mgeilen/temp/gbdd/libGBDD.a"
	"/home/mgeilen/temp/cudd/cudd/.libs/libcudd.a"
	"/home/mgeilen/temp/calculator/libParetoCalculator.a"
)
target_link_libraries(Test
	"/home/mgeilen/temp/gbdd/libGBDD.a"
	"/home/mgeilen/temp/cudd/cudd/.libs/libcudd.a"
	"/home/mgeilen/temp/calculator/libParetoCalculator.a"
)

