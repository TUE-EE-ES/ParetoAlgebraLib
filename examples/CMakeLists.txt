cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 11)

include_directories(. ../cudd/cudd ../src/gbdd ../src)

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

find_library(GBDD_DIR GBDD HINT ../src/gbdd)
link_directories(${GBDD_DIR} ../src ../cudd/cudd/.libs)
target_link_libraries(Betsy
	"libGBDD.a"
	"libcudd.a"
	"libParetoCalculator.a"
)
target_link_libraries(ProdConsTest
	"../gbdd/libGBDD.a"
	"../cudd/cudd/.libs/libcudd.a"
	"../src/libParetoCalculator.a"
)
target_link_libraries(Test
	"../gbdd/libGBDD.a"
	"../cudd/cudd/.libs/libcudd.a"
	"../src/libParetoCalculator.a"
)

