#include <iostream>

#include "gtest/gtest.h"

extern void example1();

int main(int argc, char** argv) {
	
	::testing::InitGoogleTest(&argc, argv);
	
	RUN_ALL_TESTS();

	example1();

	return 0;
}