#include <math.h>
#include "MathVector.h"

MathVector::MathVector()
{
	size_ = 0;
	useSizeFromOperations_ = 0;
	preAlloc_ = 0;
}

MathVector::MathVector(int size)
{
	if (size == 0)
	{
		size_ = 0;
		useSizeFromOperations_ = 0;
		preAlloc_ = 0;
	}
	else
	{
		size_ = size;
		useSizeFromOperations_ = size;

		preAlloc_ = 2;
		// Make sure whats allocated is a power of 2
		while (preAlloc_ < size)
		{
			// Multiply it by 2
			preAlloc_ <<= 1;
		}
		data_ = new double[preAlloc_];
	}
}


/**
 * @brief A private member function assist with copying the contents
 *     of one vector to another
 * @param copyFrom is the vector that will be copied
 * @param copyTo is the vector where the contents of @ref copyFrom will be
 *     placed
 * @note copyVector does not use the operationSize assigned to the vector
 */
void MathVector::copyVector(const MathVector& copyFrom)
{
	// Copy all of the non-dynamically allocated memory over
	this->size_ = copyFrom.size_;
	this->sizeSeenInOperations_ = copyFrom.sizeSeenInOperations_;
	this->useSizeFromOperations_ = copyFrom.useSizeFromOperations_;
	this->preAlloc_ = copyFrom.preAlloc_;

	// Now make a copy of the dynamically allocated memory
	this->data_ = new double[preAlloc_];
	for (unsigned int i = 0; i < preAlloc_; ++i)
	{
		this->data_[i] = copyFrom.data_[i];
	}
}



unsigned MathVector::getOperationSize() const
{
	if (useSizeFromOperations_ == true)
	{
		return sizeSeenInOperations_;
	}
	else
	{
		return size_;
	}
}

/*
Operations we can perform on the vectors
*/


/*
* Overload the addition operator to add another vector to this vector.
*     This vector then becomes the sum of both vectors
@Returns whether or not the second vector was successfully
   added to this vector
*/
bool MathVector::operator+=(const MathVector& mv2)
{
	if (mv2.getOperationSize() != this->getOperationSize()
|| mv2.getOperationSize() == 0)
	{
		// Vector addition is not defined
		return false;
	}
	for (unsigned int i = 0; i < mv2.getOperationSize(); ++i)
	{
		data_[i] += mv2[i];
	}
	return true;
}

/*
* 
* Operator overload for subtraction of two vectors,

@Returns whether or not the second vector was successfully 
*     added to this vector
*/
bool MathVector::operator-=(const MathVector& mv2)
{
	if (mv2.getOperationSize() != this->getOperationSize()
		|| mv2.getOperationSize() == 0)
	{
		// Vector addition is not defined
		return false;
	}
	for (unsigned int i = 0; i < mv2.getOperationSize(); ++i)
	{
		data_[i] += mv2[i];
	}
	return true;
}

/*
* Scalar Multiplication for vectors
*/
bool MathVector::operator*=(double alpha)
{
	if (getOperationSize() == 0)
	{
		return false;
	}
	for (unsigned int i = 0; i < getOperationSize(); ++i)
	{
		data_[i] *= alpha;
	}
}


/**
 * @brief Returns the dot product of this vector with another vector
 * @param other is the other vector with which to take the dot product
 * @return NAN if the dot product is not defined between the two vectors
 *    and the dot product if it is defined
 */
double MathVector::dotProduct(const MathVector& other) const
{
	if (this->getOperationSize() == 0 || this->getOperationSize() != other.getOperationSize())
	{
		return NAN;
	}

	double result = 0.0;
	for (unsigned int i = 0; i < getOperationSize(); ++i)
	{
		data_[i] *= other.data_[i];
	}
}


/*
* Adding an element to the back
* @note: This adds to the last element meaning that even if 
*     @ref size_ > sizeSeenInOperations_ it will still be placed
*     after the last element in the vector
*/
bool MathVector::push_back(double element)
{
	if (size_ == preAlloc_)
	{
		// Make a new array to store all the data
		double* newData = new double[preAlloc_ << 1];
		for (unsigned int i = 0; i < size_; ++i)
		{
			newData[i] = data_[i];
		}
		delete data_;
		data_ = newData;

		preAlloc_ <<= 1;
	}
	// Post-imcrement here increases the size while inserting the new
	//    element at the correct location
	data_[size_++] = element;
}


//////////////////////////////////////////////////////////////////////////
// Outside of class functions that generate new vectors when used
/////////////////////////////////////////////////////////////////////////

MathVector add(const MathVector& v1, const MathVector& v2)
{
	MathVector newVec(v1);
	newVec += v2;
}
MathVector operator+(const MathVector& v1, const MathVector& v2)
{
	MathVector newVec(v1);
	newVec += v2;
}

MathVector scalarMult(const MathVector& vec, double alpha)
{
	MathVector newVec(vec);
	newVec *= alpha;
}
MathVector operator*(const MathVector& vec, double alpha)
{
	MathVector newVec(vec);
	newVec *= alpha;
}

/**
 * @brief Returns the dot product of @ref v1 and @ref v2
 * @param v1 
 * @param v2 
 * @return NAN if the dot product is not defined and the dot product result
 *    of @ref v1 and @ref v2.
 */
double dotProduct(const MathVector& v1, const MathVector& v2)
{
	return v1.dotProduct(v2);
}
double operator*(const MathVector& v1, const MathVector& v2)
{
	return v1.dotProduct(v2);
}

/**
 * @brief Returns the projection of vector @ref b onto vector @ref a
 *     if the projection is defined and returns the vector of the
 *     projection.
 * @param b is the vector to which we project onto a
 * @param a is the vector which is projected onto
 * @return A blank @ref MathVector if the projection does not exist and otherwise
 *     returns a MathVector which is the projection of vector @ref b onto
 *     vector @ref a.  The projection does not exist if either @ref a and @ref b
 *     are not the same size and/or if the a is the 0 vector.
 * @note A vector of length 0 is still considered to be a 0 vector
 */
MathVector findProjection(const MathVector& b, const MathVector& a)
{
	// The projection of b onto a is defined as 
	//     (b.a)/(a.a)*a
	double scalar = (b.dotProduct(a)) / (a.dotProduct(a));
	return a * scalar;
}

