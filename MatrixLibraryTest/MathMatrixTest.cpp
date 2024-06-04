#include "pch.h"

#include "../MatrixLibrary/MathMatrix.h"
#include <ostream>

namespace MATRIX_LIBRARY_TESTS {
	;
	TEST(BasicConstructorTests, DEFAULT_CONSTRUCTOR_RETURNS_0BY0_MATRIX)
	{
		MathMatrix m;
		EXPECT_EQ(m.getNumRows(), 0);
		EXPECT_EQ(m.getNumCols(), 0);
	}

	TEST(MathMatrixIteratorTests, SUBTRACTION_ON_ITERATORS_WORKS_CORRECTLY)
	{
		MathMatrix m(4, 4);

		MathMatrix::rowIterator startItr = m.rowBegin(1);
		MathMatrix::rowIterator endItr = m.rowEnd(1);

		EXPECT_EQ(endItr - startItr, 4);
	}

	TEST(DotProductWithIterators, ITERATORS_ON_A_2x2_MATRIX_WORKS)
	{
		MathMatrix m(2, 2);
		// [ 2 4 ]
		// [ 8 16 ]
		m.setVal(0, 0, 2); m.setVal(0, 1, 4); m.setVal(1, 0, 8); m.setVal(1, 1, 16);
		
		EXPECT_EQ(68, dotProduct(m.colBegin(0), m.colEnd(0), m.colBegin(0), m.colEnd(0) ));
		EXPECT_EQ(20, dotProduct(m.rowBegin(0), m.rowEnd(0), m.rowBegin(0), m.rowEnd(0)));

		// EXPECT_EQ(136, dotProduct(m.colBegin(0), m.colEnd(0), m.colBegin(1), m.colEnd(1)));
		// EXPECT_EQ(330, dotProduct(m.colBegin(1), m.colEnd(1), m.colBegin(1), m.colEnd(1)));
	}

	TEST(MatrixMultiplicationTest, MATRIX_MULTIPLICATION_WITH_2BY2_MATRICES_WORK)
	{
		MathMatrix m1(2, 2);
		MathMatrix m2(2, 2);

		m1.setVal(0, 0, 1); m1.setVal(1, 1, 1);

		m2.setVal(0, 1, 5); m2.setVal(1, 0, 3);

		EXPECT_EQ(m1.rowEnd(1) - m1.rowBegin(1), 2);
		EXPECT_EQ(m1.colEnd(0) - m1.colBegin(0), 2);

		MathMatrix m3 = m1 * m2;
		EXPECT_EQ(m3.getVal(0, 0), 0);
		EXPECT_EQ(m3.getVal(0, 1), 5);
		EXPECT_EQ(m3.getVal(1, 0), 3);
		EXPECT_EQ(m3.getVal(1, 1), 0);
	}

}