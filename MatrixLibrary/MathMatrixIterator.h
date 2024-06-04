#pragma once
#include "MathVector.h"

// Predefintion of Mathmatrix
class MathMatrix;

#ifndef VECTOR_SPACE_ENUM_DEFINED_
	typedef enum { ROWSPACE, COLUMNSPACE } vector_space_t;
#define VECTOR_SPACE_ENUM_DEFINED_
#endif

class MathMatrixIterator
{
public:
	MathMatrixIterator(MathVector * const * const matrixSpace, unsigned int const pos, 
		vector_space_t const matrixSpaceType, vector_space_t const toIterate);

	MathMatrixIterator operator++();
	MathMatrixIterator operator++(int);
	double& operator*();

	bool operator==(const MathMatrixIterator& other) const;

	unsigned int operator-(const MathMatrixIterator& other) const;

private:

	void increment(int amount);

	typedef enum { FIRST_ACCESS, SECOND_ACCESS } access_order_t;
	
	MathVector* const *  ptr_;
	unsigned int posInVec_;

	access_order_t access_to_increment;
};

double dotProduct(MathMatrixIterator beginItr1, const MathMatrixIterator& endItr1,
	MathMatrixIterator beginItr2, const MathMatrixIterator& endItr2);

