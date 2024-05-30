#include "pch.h"

#include "../MatrixLibrary/MathVector.h"
#include <ostream>


TEST(BasicConstructorTests, DEFAULT_CONSTRUCTOR_MAKES_VECTOR_OF_SIZE_0)
{
	MathVector v;

	EXPECT_EQ(v.getSize(), 0);
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}
TEST(BasicConstructorTests, CONSTRUCTOR_WITH_PREDETERMINED_SIZE_CREATES_VECTOR_OF_THAT_SIZE )
{

	MathVector v0(0);
	EXPECT_EQ(v0.getSize(), 0);


	MathVector v1(1);
	EXPECT_EQ(v1.getSize(), 1);

	MathVector v5(5);
	EXPECT_EQ(v5.getSize(), 5);

	MathVector vHigh(93320);
	EXPECT_EQ(vHigh.getSize(), 93320);
}
TEST(BasicConstructorTests, DEFAULT_CONSTRUCTOR_WITH_SPECIFIED_SIZE_CREATED_HAS_DEFAULT_VALUES)
{
	MathVector v(5);
	EXPECT_EQ(v[0], 0);
	EXPECT_EQ(v[4], 0);
}


TEST(BasicOperationTests, BRACKET_OPERATOR_SETS_ELEMENT_CORRECTLY)
{
	MathVector v(5);
	v[0] = 2;
	v[1] = 5;

	ASSERT_EQ(v[0], 2);
	ASSERT_EQ(v[1], 5);
}
TEST(BasicOperationTests, PUSH_BACK_FUNCTION_ADDS_ELEMENT)
{
	MathVector v;

	v.push_back(2.42);
	v.push_back(5);
	v.push_back(7.89);

	ASSERT_EQ(v[0], 2.42);
	ASSERT_EQ(v[2], 7.89);
	ASSERT_EQ(v[1], 5);

	v.push_back(8.53);
	ASSERT_EQ(v[0], 2.42);
	ASSERT_EQ(v[3], 8.53);

	MathVector vPre(6);

	vPre[5] = 5.43;
	vPre.push_back(70.98);
	vPre.push_back(57.43);
	vPre.push_back(98.32);

	vPre[7] = 57.43;
	vPre[6] = 70.98;
	vPre[8] = 98.32;
}
TEST(BasicOperationTests, IF_VECTORS_ARE_EQUIV_THEN_ISEQUAL_WITH_2VECTORS_RETURNS_TRUE)
{
	MathVector v1 = { 0, 1.24, -2.47 };
	MathVector v2 = { 0, 1.24, -2.47 };

	MathVector v0;
	MathVector v0_2;

	EXPECT_TRUE(v1.isEqualTo(v1));
	EXPECT_TRUE(v2.isEqualTo(v2));
	EXPECT_TRUE(v1.isEqualTo(v2));
	EXPECT_TRUE(v2.isEqualTo(v1));

	EXPECT_TRUE(v0.isEqualTo(v0));
	EXPECT_TRUE(v0.isEqualTo(v0_2));
	EXPECT_TRUE(v0_2.isEqualTo(v0));
	EXPECT_TRUE(v0_2.isEqualTo(v0_2));
}

TEST(BasicOperationTests, IF_VECTORS_ARE_EQUIV_THEN_ISEQUAL_WITH_AN_INITIALIZERLIST_RETURNS_TRUE)
{
	MathVector v1({ 5.73, -1 });
	MathVector v2 = { 5.73, -1 };

	MathVector v0;
	MathVector v0_2;

	EXPECT_TRUE(v1.isEqualTo(v1));
	EXPECT_TRUE(v2.isEqualTo(v2));
	EXPECT_TRUE(v1.isEqualTo(v2));
	EXPECT_TRUE(v2.isEqualTo(v1));

	EXPECT_TRUE(v0.isEqualTo(v0));
	EXPECT_TRUE(v0.isEqualTo(v0_2));
	EXPECT_TRUE(v0_2.isEqualTo(v0));
	EXPECT_TRUE(v0_2.isEqualTo(v0_2));
}
TEST(BasicOperationTests, IF_VECTORS_ARE_NOT_EQUIV_THEN_ISEQUAL_WITH_2VECTORS_RETURNS_FALSE)
{
	MathVector v1 = { 0.56, 9.23, 8.67 };
	MathVector v2 = { 0.56, 9.23, 8.56 };
	MathVector v3 = { 0.56, 9.23, 8.56 , 2 };
	MathVector v4;

	EXPECT_FALSE(v1.isEqualTo(v2));
	EXPECT_FALSE(v1.isEqualTo(v3));
	EXPECT_FALSE(v1.isEqualTo(v4));
	EXPECT_FALSE(v2.isEqualTo(v1));
	EXPECT_FALSE(v2.isEqualTo(v3));
	EXPECT_FALSE(v2.isEqualTo(v4));
	EXPECT_FALSE(v3.isEqualTo(v1));
	EXPECT_FALSE(v3.isEqualTo(v2));
	EXPECT_FALSE(v3.isEqualTo(v4));
	EXPECT_FALSE(v4.isEqualTo(v1));
	EXPECT_FALSE(v4.isEqualTo(v2));
	EXPECT_FALSE(v4.isEqualTo(v3));
}

TEST(BasicOperationTests, IF_VECTORS_ARE_NOT_EQUIV_THEN_ISEQUAL_WITH_INITIALIZER_LIST_RETURNS_FALSE)
{
	MathVector v1 = { -0.56, 9.23, 8.67 };
	MathVector v2 = { 0.56, 9.23, 8.56 , 2 };
	MathVector v3;

	EXPECT_FALSE(v1.isEqualTo({0.56, 9.23, -8.67}));
	EXPECT_FALSE(v1.isEqualTo({ 0.89, 9.23, 8.67 }));
	EXPECT_FALSE(v1.isEqualTo({ 0.56, -7.53, 8.67, 83 }));
	EXPECT_FALSE(v1.isEqualTo({ 0.50, 7.53, 8.67 }));

	EXPECT_FALSE(v2.isEqualTo({ 0.56, 7.53, 8.67 }));
	EXPECT_FALSE(v2.isEqualTo({ 0.56, 9.23, 8.36 , 2 }));
	EXPECT_FALSE(v2.isEqualTo({ 0.56, 9.23, 8.56 , -2 }));
	EXPECT_FALSE(v2.isEqualTo({ 0.50, 7.53 }));

	EXPECT_FALSE(v3.isEqualTo({ 0.56, 7.53 }));
	EXPECT_FALSE(v3.isEqualTo({ 0.56 , 2, -53.2 }));
	EXPECT_FALSE(v3.isEqualTo({ 0.56, 9.13, 8.56 , 2.22 }));
	EXPECT_FALSE(v3.isEqualTo({ 0.5 }));
}


//=======================================================================================
// TESTS FOR BASIC MATH DEFINITIONS
//     INCLUDES TESTS FOR: Magnitude, 2D vectors
//=======================================================================================

TEST(BasicMathDefinitionTests, MAGNITUDE_OF_2D_VECTOR_IS_CORRECT)
{
	MathVector v;

	v.push_back(3.0F);
	v.push_back(4.0F);

	
	EXPECT_DOUBLE_EQ(v.getMagnitude(), 5.0F);
	
	v[0] = -9.25F;
	v[1] = 6.34F;

	EXPECT_NEAR(v.getMagnitude(), 11.214, 0.001);

}
TEST(BasicMathDefinitionTests, ZERO_VECTOR_IS_FOUND_CORRECTLY)
{
	MathVector v(3);
	
	EXPECT_TRUE(v.isZeroVector());
	
	v[2] = 4.0F;

	EXPECT_FALSE(v.isZeroVector());

	MathVector v2(1);

	EXPECT_TRUE(v2.isZeroVector());

	v2.push_back(0.0F);

	EXPECT_TRUE(v2.isZeroVector());

	v2.push_back(4);

	EXPECT_FALSE(v2.isZeroVector());
	
}


TEST(VectorAdditionTests, PLUS_EQUALS_OPERATOR_WORKS_CORRECTLY_ON_SAME_SIZE_VECTORS)
{
	MathVector v1(3);
	v1[0] = 2; v1[1] = 4; v1[2] = 0.356;

	MathVector v2(3);
	v2[0] = 5.37; v2[1] = 8.73; v2[2] = 729.00;

	v1 += v2;

	EXPECT_NEAR(v1[0], 7.37, 0.0001);
	EXPECT_NEAR(v1[1], 12.73, 0.0001);
	EXPECT_NEAR(v1[2], 729.356, 0.0001);

	v2 += v1;

	EXPECT_NEAR(v1[0], 7.37, 0.0001);
	EXPECT_NEAR(v1[1], 12.73, 0.0001);
	EXPECT_NEAR(v1[2], 729.356, 0.0001);

	EXPECT_NEAR(v2[0], 12.74, 0.0001);
	EXPECT_NEAR(v2[1], 21.46, 0.0001);
	EXPECT_NEAR(v2[2], 1458.356, 0.0001);
}
TEST(VectorAdditionTests, PLUS_EQUALS_OPERATOR_DOES_NOT_CHANGE_ANYTHING_IF_NOT_THE_SAME_SIZE)
{

	MathVector v1( { 1.0,2.2,3.0 } );
	MathVector v2({ 1.5, 33, 6.5, 55 });

	MathVector v1Before = v1;
	MathVector v2Before = v2;

	v1 += v2;

	EXPECT_TRUE(v1 == v1Before && v1Before == v1);
	EXPECT_TRUE(v2 == v2Before && v2Before == v2);
}

TEST(VectorAdditionTests, PLUS_OPERATOR_WORKS_CORRECTLY_ON_SAME_SIZE_VECTORS)
{
	MathVector v1 = { -4.2, 5.6, 9.6 };
	MathVector v2 = { 7.2, -10, 0.4 };


	MathVector v3 = v1 + v2;

	EXPECT_NEAR(v1[0], -4.2,  0.001);

	EXPECT_TRUE(v3.isEqualTo({ 3.0, -4.4, 10.0 }));

	v1 = v1 + v1;
	EXPECT_TRUE(v1.isEqualTo({ -8.4, 11.2, 19.2 }));
}
TEST(VectorAdditionTests, PLUS_OPERATOR_WITH_DIFFERENT_SIZE_VECTORS_RETURNS_NULLPTR)
{
	MathVector v1 = { 0.2, 0.4 };
	MathVector v2 = { 0.1 };

	MathVector v3 = v1 + v2;

	EXPECT_EQ(v3.getSize(), 0);
}
TEST(VectorAdditionTests, PLUS_OPERATOR_DOES_NOT_CHANGE_EITHER_VECTOR)
{
	MathVector v1 = { 0.45, 0.98, -2.35 };
	MathVector v2 = { 9.2, 6.5, 8.2 };
	MathVector v3 = { 0.3, 0.9 };

	MathVector temp = v1 + v2;
	
	EXPECT_TRUE(v1.isEqualTo({ 0.45, 0.98, -2.35 }));
	EXPECT_TRUE(v2.isEqualTo({ 9.2, 6.5, 8.2 }));

	temp = v3 + v2;
	EXPECT_TRUE(v3.isEqualTo({ 0.3, 0.9 }));
	EXPECT_TRUE(v2.isEqualTo({ 9.2, 6.5, 8.2 }));
}


TEST(VectorScalarMultTests, TIMES_EQUALS_OPERATOR_WORKS_AS_INTENDED)
{
	MathVector v1({ 4.0, 5.0, -3 });
	MathVector v2 = { 6, -0.01 };

	v1 *= 1;
	EXPECT_TRUE(v1.isEqualTo({ 4.0, 5.0, -3 }));

	v1 *= 2.5;
	EXPECT_TRUE(v1.isEqualTo({ 10.0, 12.5, -7.5 }));

	v1 *= -6.2;
	EXPECT_TRUE(v1.isEqualTo({ -62, -77.5, 46.5 }));

	v2 *= -2.5;
	EXPECT_TRUE(v2.isEqualTo({ -15, 0.025 }));
}
TEST(VectorScalarMultTests, TIMES_EQUALS_ZERO_RESULTS_IN_ZERO_VECTOR)
{
	MathVector v1 = { 4, 5, 8, 1, 0, 0.293, -4, -5.7, 7.2 };
	v1 *= 0;
	EXPECT_TRUE(v1.isZeroVector());
}

TEST(VectorScalarMultTests, TIMES_OPERATOR_WORKS_AS_INTENDED)
{
	MathVector v = { 0.2, 6, -7 };
	MathVector v2 = v * 2;
	MathVector v3 =  -0.5 * v;

	EXPECT_TRUE(v2.isEqualTo({ 0.4, 12, -14 }));
	EXPECT_TRUE(v3.isEqualTo({ -0.1 ,-3, 3.5 }));
}
TEST(VectorScalarMultTests, TIMES_OPERATOR_DOES_NOT_CHANGE_ORIGINAL_VECTOR)
{
	MathVector v1 = { 0, .2, 6 };

	MathVector temp = 3 * v1;

	EXPECT_TRUE(v1.isEqualTo({ 0, 0.2, 6 }));

	temp = v1 * -0;

	EXPECT_TRUE(v1.isEqualTo({ 0, 0.2, 6 }));
}

TEST(VectorScalarMultTests, ORDER_OF_OPERATIONS_WORKS_AS_INTENDED)
{
	MathVector v1 = { 4, 5.5 , 7 };
	MathVector v2 = { -2, 5, -.1 };

	MathVector v3 = 2 * v1 + v2 * -0.1;

	EXPECT_TRUE(v3.isEqualTo({ 8.2, 10.5, 14.01 }));

}
