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

	TEST(BasicConstructorTests, CAN_CONSTRUCT_MATRIX_FROM_INITIALIZER_LIST)
	{
		MathMatrix m = { {4, 5}, {6, 7} };

		EXPECT_EQ(m.getVal(0, 0), 4);
		EXPECT_EQ(m.getVal(0, 1), 5);
		EXPECT_EQ(m.getVal(1, 0), 6);
		EXPECT_EQ(m.getVal(1, 1), 7);
	}

	//=================================================================================================
	// ADVANCED CONSTRUCTOR TESTS
	//=================================================================================================

	TEST(AdvancedConstructorTests, COPY_CONSTRUCTOR_WORKS_CORRECTLY)
	{
		MathMatrix m1({ {2, 3}, {4.4, 5.2}, {-6, -9.8} });
		MathMatrix mCopy(m1);

		EXPECT_TRUE(m1.equals({ {2, 3}, {4.4, 5.2}, {-6, -9.8} }));
	}
	TEST(AdvancedConstructorTests, CAN_CONSTRUCT_MATRIX_FROM_V_TIMES_V_TRANSPOSE)
	{
		MathVector v = { 3.0, 4.0 };

		MathMatrix m(v, v);

		EXPECT_TRUE(m.equals({ {9, 12}, {12, 16} }));

		MathVector v2 = { 3.0 / 25, 4.0 / 25 };

		MathMatrix differentVectorsMatrix(v, v2);
		
		EXPECT_EQ(differentVectorsMatrix.getVal(1, 1), 16.0 / 25);
		EXPECT_EQ(differentVectorsMatrix.getNumRowsInOperationSize(), 2);
		EXPECT_EQ(differentVectorsMatrix.getNumColsInOperationSize(), 2);
		//EXPECT_TRUE(m.equals({ { 9.0 / 25, 12.0 / 25 }, {12.0 / 25, 16.0 / 25} }));
	}

	//=================================================================================================
	// PROGRAMMING RELATED FUNCTIONS
	//=================================================================================================
	TEST(AddingRowColumnsTest, SUCCESSFULLY_ADDS_ROW_TO_MATRIX)
	{
		MathMatrix m = { {7.8, 4.5}, {3.6, -2} };

		MathVector rowToAdd = { -6.5, 12 };

		m.addRow(rowToAdd);
		EXPECT_TRUE(m.equals({ { 7.8, 4.5 }, { 3.6, -2 }, { -6.5, 12 } }));

	}

	TEST(MathMatrixIteratorTests, SUBTRACTION_ON_ITERATORS_WORKS_CORRECTLY)
	{
		MathMatrix m(4, 4);

		MathMatrix::rowIterator startItr = m.rowBegin(1);
		MathMatrix::rowIterator endItr = m.rowEnd(1);

		EXPECT_EQ(endItr - startItr, 4);
	}

	TEST(ComparisonOperatorTests, OPERATOR_COMPARISON_WORKS_AS_INTENDED)
	{
		MathMatrix m({ {4, 5, 7}, { 3, 7, 2 }, { 9, 3, 1 }});

		EXPECT_TRUE( (m.equals( { {4, 5, 7}, { 3, 7, 2 }, { 9, 3, 1 }} ) ) );
	}

	//=======================================================================================
	// STANDARD MATH FUNCTION TESTS
	//=======================================================================================
	TEST(DotProductWithIterators, ITERATORS_ON_A_2x2_MATRIX_WORKS)
	{
		MathMatrix m(2, 2);
		// [ 2 4 ]
		// [ 8 16 ]
		m.setVal(0, 0, 2); m.setVal(0, 1, 4); m.setVal(1, 0, 8); m.setVal(1, 1, 16);

		EXPECT_EQ(68, dotProduct(m.colBegin(0), m.colEnd(0), m.colBegin(0), m.colEnd(0)));
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

	//=======================================================================================
	// MATH MATRIX ITERATOR TESTS
	//=======================================================================================

	TEST(MatrixIteratorTests, ITERATOR_SUBTRACTION_WORKS)
	{
		MathMatrix m1({ {4.3}, {7.9} });

		EXPECT_EQ(m1.rowEnd(0) - m1.rowBegin(0), 1);
		EXPECT_EQ(m1.rowEnd(1) - m1.rowBegin(1), 1);
		EXPECT_EQ(m1.colEnd(0) - m1.colBegin(0), 2);
	}
	TEST(MatrixIteratorTests, ITERATOR_INCREMENT_WORKS)
	{
		MathMatrix m({ {1,2}, {3,4} });

		MathMatrix::rowIterator itr1 = m.rowBegin(1);

		EXPECT_EQ(*itr1, 3);
		++itr1;
		EXPECT_EQ(*itr1, 4);
		++itr1;

		EXPECT_EQ(itr1, m.rowEnd(1));
	}

	TEST(MatrixIteratorTests, BASIC_DOT_PRODUCT_WORKS_AS_INTENDED)
	{
		MathMatrix m1({ {3, 4}, {0.5, 0.3} });
		MathMatrix m2({ {3}, {2} });

		MathMatrix::rowIterator startItr1 = m1.rowBegin(0);
		MathMatrix::rowIterator endItr1 = m1.rowEnd(0);

		MathMatrix::colIterator startItr2 = m2.colBegin(0);
		MathMatrix::colIterator endItr2 = m2.colEnd(0);

		EXPECT_EQ(endItr1 - startItr1, endItr2 - startItr2);
		float res = dotProduct(startItr1, endItr1, startItr2, endItr2);

		EXPECT_EQ(res, 17.0);
	}

	//=======================================================================================
	// ADVANCED MATH FUNCTION TESTS
	//=======================================================================================

	TEST(AdvancedOperationTests, PROJECTION_MATRIX_IS_GENERATED_FOR_SIMPLE_VECTOR)
	{
		MathVector v1 = { 3, 4 };

		MathMatrix m( createProjectionMatrix(v1) );

		//ASSERT_EQ(m.getVal(0, 0), 9.0F / 25);

		EXPECT_TRUE(createProjectionMatrix(v1).equals({ { 9.0F /25, 12.0F /25 }, { 12.0F /25, 16.0F /25 } }));
	}

	TEST(AdvancedOperationTests, PROJECTION_MATRIX_TIMES_AN_ORTHOGONAL_VECTOR_PRODUCES_A_ZERO_VECTOR)
	{
		MathVector vectorToProjectOnto = { 1, 2 };
		MathVector orthogonalVector = { -2, 1 };

		MathMatrix projMatrix(createProjectionMatrix(vectorToProjectOnto));

		MathMatrix orthoMatrix = MathMatrix(orthogonalVector, COLUMNSPACE);

		MathMatrix multResult(projMatrix * orthoMatrix);

		EXPECT_TRUE(multResult.equals({ {0}, {0} }));
		
	}

}