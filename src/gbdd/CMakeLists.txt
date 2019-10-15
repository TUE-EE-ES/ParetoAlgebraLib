cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 11)

include_directories(. ../../cudd/cudd)

ADD_LIBRARY(GBDD STATIC
	bdd.cc
	bdd-equivalence-relation.cc
	bdd-relation.cc
	bool-constraint.cc
	cudd-space.cc
	domain.cc
	mutex-space.cc
	space.cc
	structure-constraint.cc
	structure-relation.cc
)

