cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 11)

include_directories(. ../cudd/cudd ../gbdd)

add_compile_options(-Wno-deprecated-declarations)

add_library(ParetoCalculator STATIC
	bddconfset.cc
	conf.cc
	explicit.cc
	paretobase.cc
	quantity.cc
	symbolic.cc
)

#link_directories(../gbdd/)
#target_link_libraries(ParetoCalculator "/home/mgeilen/temp/gbdd/libGBDD.a" "/home/mgeilen/temp/cudd/cudd/.libs/libcudd.a")

