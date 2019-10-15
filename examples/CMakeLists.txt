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

target_link_libraries(Betsy
	GBDD
	ParetoCalculator
	${CMAKE_SOURCE_DIR}/cudd/cudd/.libs/libcudd.a
)
target_link_libraries(ProdConsTest
	GBDD
	ParetoCalculator
	${CMAKE_SOURCE_DIR}/cudd/cudd/.libs/libcudd.a
)
target_link_libraries(Test
	GBDD
	ParetoCalculator
	${CMAKE_SOURCE_DIR}/cudd/cudd/.libs/libcudd.a
)

