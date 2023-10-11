//============================================================================
// Name        : DesignByContract.h
// Author      : Serge Demeyer
// Version     : ?
// Copyright   : Project Software Engineering - BA1 Informatica - Serge Demeyer - University of Antwerp
// Description : Declarations for design by contract in C++
//============================================================================
#ifndef PROJECT_V1_DESIGNBYCONTRACT_H
#define PROJECT_V1_DESIGNBYCONTRACT_H

#include <assert.h>

#ifdef __unix__
#define REQUIRE(assertion, what) \
	if (!(assertion)) __assert (what, __FILE__, __LINE__)

#define ENSURE(assertion, what) \
	if (!(assertion)) __assert (what, __FILE__, __LINE__)
#endif

#ifdef _WIN32
#define REQUIRE(assertion, what) \
	if (!(assertion)) _assert (what, __FILE__, __LINE__)

#define ENSURE(assertion, what) \
	if (!(assertion)) _assert (what, __FILE__, __LINE__)
#endif

#endif //PROJECT_V1_DESIGNBYCONTRACT_H
