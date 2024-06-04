#include "pch.h"
#include "MathMatrixIterator.h"
#include <cmath>

MathMatrixIterator::MathMatrixIterator(MathVector * const * const matrixSpace, unsigned int const pos,
	vector_space_t const matrixSpaceType, vector_space_t const toIterate)
{
	ptr_ = matrixSpace;
	posInVec_ = pos;

	if (matrixSpaceType == toIterate)
	{
		access_to_increment = SECOND_ACCESS;
	}
	else
	{
		access_to_increment = FIRST_ACCESS;
	}
}

MathMatrixIterator MathMatrixIterator::operator++()
{
	increment(1);
	return *this;
}

MathMatrixIterator MathMatrixIterator::operator++(int)
{
	MathMatrixIterator tmp(*this);
	increment(1);
	return tmp;
}

double& MathMatrixIterator::operator*()
{
	return (**ptr_)[posInVec_];
}

bool MathMatrixIterator::operator==(const MathMatrixIterator& other) const
{
	return (this->ptr_ == other.ptr_ && this->posInVec_ == other.posInVec_ &&
		this->access_to_increment == other.access_to_increment);
}

unsigned int MathMatrixIterator::operator-(const MathMatrixIterator& other) const
{
	// We can add them since only one should be different and the other should be
	//     0
	return (unsigned int)((this->ptr_ - other.ptr_) + (this->posInVec_ - other.posInVec_));
}

// =============================================================================================
// OUTSIDE OF CLASS ITERATOR
// =============================================================================================

double dotProduct(MathMatrixIterator beginItr1, const MathMatrixIterator& endItr1,
	MathMatrixIterator beginItr2, const MathMatrixIterator& endItr2)
{
	unsigned int len = (endItr1 - beginItr1);

	if (endItr2 - beginItr2 != len)
	{
		return NAN;
	}

	double result = 0.0F;
	for (unsigned int i = 0; i < len; ++i)
	{
		result += (*beginItr1)*(*beginItr2);
		++beginItr1; ++beginItr2;
	}
	return result;
}


// ==============================================================================
// Private Member functions
// ==============================================================================

void MathMatrixIterator::increment(int amount)
{
	if (access_to_increment == FIRST_ACCESS)
	{
		ptr_ += amount;
	}
	else
	{
		posInVec_ += amount;
	}
}