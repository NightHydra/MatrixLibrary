#include "pch.h"
#include "pch.cpp"
#include <math.h>
#include "MathVector.h"

static constexpr double ALPHA = 0.001;

MathVector::MathVector()
{
	size_ = 0;
	useSizeFromOperations_ = 0;
	preAlloc_ = 0;

	data_ = nullptr;
}

MathVector::MathVector(unsigned int size)
{
	if (size == 0)
	{
		size_ = 0;
		preAlloc_ = 0;
	}
	else
	{
		size_ = size;

		preAlloc_ = 2;
		// Make sure whats allocated is a power of 2
		while (preAlloc_ < size)
		{
			// Multiply it by 2
			preAlloc_ <<= 1;
		}
		data_ = new double[preAlloc_];

		for (unsigned int i = 0; i < size; ++i)
		{
			data_[i] = 0.0F;
		}
	}
}

MathVector::MathVector(const std::initializer_list<double> arr)
{
	size_ = arr.size();
	preAlloc_ = 2;

	while (preAlloc_ < size_)
	{
		preAlloc_ <<= 1;
	}
	data_ = new double[preAlloc_];

	std::initializer_list<double>::iterator itr = arr.begin();

	for (unsigned int i = 0; i < size_; ++i)
	{
		data_[i] = *itr;
		++itr;
	}
}

MathVector& MathVector::operator=(const std::initializer_list<double> arr)
{
	deleteAllocatedMemory();

	size_ = arr.size();
	preAlloc_ = 2;

	while (preAlloc_ < size_)
	{
		preAlloc_ <<= 1;
	}
	data_ = new double[preAlloc_];

	std::initializer_list<double>::iterator itr = arr.begin();

	for (unsigned int i = 0; i < size_; ++i)
	{
		data_[i] = *itr;
		++itr;
	}

	return *this;
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

void MathVector::deleteAllocatedMemory()
{
	if (size_ != 0)
	{
		delete[] data_;
	}
}

bool MathVector::isZeroVector() const
{
	unsigned int operationSize = getOperationSize();

	for (unsigned int i = 0; i < operationSize; ++i)
	{
		if (!approxEqual(data_[i], 0.0F)) return false;
	}
	return true;
}

/**
 * @brief Returns the magnitude of the vector as defined by the mathematical
 *     definition as the root of the sum of the squares of each component.
 * @return 0.0 if the vectors operation size is of size 0 and the magnitude
 *     otherwise.
 * @note To get the square of the magnitude just call "v1.dotProduct(v1)".
 */
double MathVector::getMagnitude() const
{
	double mag = 0.0;
	unsigned int opsize = getOperationSize();

	if (opsize == 0) return NAN;

	for (unsigned int i = 0; i < opsize; ++i)
	{
		mag += data_[i] * data_[i];
	}
	return sqrt(mag);
}

bool MathVector::isEqualTo(const MathVector& other) const
{
	if (other.size_ != this->size_) return false;

	for (unsigned int i = 0; i < size_; ++i)
	{
		if (!approxEqual(this->data_[i], other.data_[i]))
		{
			return false;
		}
	}
	return true;
}
bool MathVector::isEqualTo(const std::initializer_list<double> arr) const
{
	unsigned int opSize = this->getOperationSize();
	if (opSize != arr.size()) return false;

	std::initializer_list<double>::iterator itr = arr.begin();
	for (unsigned int i = 0; i < opSize; ++i)
	{
		if (data_[i] != *itr) return false;
		++itr;
	}
	return true;
}

bool MathVector::isNotEqualTo(const MathVector& other) const
{

	return !isEqualTo(other);
}
bool MathVector::isNotEqualTo(const std::initializer_list<double> arr) const
{
	unsigned int opSize = this->getOperationSize();
	if (opSize != arr.size()) return true;

	std::initializer_list<double>::iterator itr = arr.begin();
	for (unsigned int i = 0; i < opSize; ++i)
	{
		if (data_[i] != *itr) return true;
		++itr;
	}
	return false;
}

unsigned int MathVector::getOperationSize() const
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
* 
@Returns whether or not the second vector was successfully
   added to this vector
*/

/**
 * @brief Overload the addition operator to add another vector to this vector.
 *     This vector then becomes the sum of both vectors
 * @param mv2 The @ref mathvector which is added to this vector
 * @return true if the vectors could be successfully added and false if the vectors
 *     do not have the same size or the operation size of either vector is equal to 0
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
	return true;
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
		result = data_[i] * other.data_[i];
	}
	return result;

}


/*
* Adding an element to the back
* @note: This adds to the last element meaning that even if 
*     @ref size_ > sizeSeenInOperations_ it will still be placed
*     after the last element in the vector
*/
bool MathVector::push_back(const double& element)
{

	// Case where we havent allocated anything yet
	if (size_ == 0)
	{
		size_ = 1;
		preAlloc_ = 2;
		data_ = new double[preAlloc_];
		data_[0] = element;
		return true; // <----- Return
	}

	// Cannot double size if preAlloc would overflow
	// This fancy math is so that we can tell if we overflow
	//     no matter the size of preAlloc.
	// We should never return false here though since this would mean our vector
	//     is very, very big and would also mean that the vector would use
	//     ~ 2 GB of data.
	if (preAlloc_ >> (sizeof(preAlloc_)*8 - 1) > 0) return false;

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

	return true;
}


//////////////////////////////////////////////////////////////////////////
// Outside of class functions that generate new vectors when used
/////////////////////////////////////////////////////////////////////////

bool approxEqual(const double& d1, const double& d2)
{
	return fabs(d1 - d2) < ALPHA;
}

bool operator==(const MathVector& v1, const MathVector& v2)
{
	return v1.isEqualTo(v2);
}

bool operator==(const MathVector& v1, std::initializer_list<double> arr)
{
	return v1.isEqualTo(arr);
}

bool operator!=(const MathVector& v1, const MathVector& v2)
{
	return v1.isNotEqualTo(v2);
}

bool operator!=(const MathVector& v1, std::initializer_list<double> arr)
{
	return v1.isNotEqualTo(arr);
}

MathVector add(const MathVector& v1, const MathVector& v2)
{
	MathVector newVec(v1);
	newVec += v2;

	return newVec;
}
MathVector operator+(const MathVector& v1, const MathVector& v2)
{
	if (v1.getSize() != v2.getSize()) return MathVector();

	MathVector newVec(v1);
	newVec += v2;

	return newVec;
}

MathVector scalarMult(const MathVector& vec, double alpha)
{
	MathVector newVec(vec);
	newVec *= alpha;

	return newVec;
}
MathVector operator*(const MathVector& vec, double alpha)
{
	MathVector newVec(vec);
	newVec *= alpha;

	return newVec;
}
MathVector operator*(double alpha, const MathVector& vec)
{
	MathVector newVec(vec);
	newVec *= alpha;

	return newVec;
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