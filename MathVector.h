#pragma once

#ifndef __MATRIX_LIBRARY_H
#define __MATRIX_LIBRARY_H
#include <initializer_list>

#include "pch.h"
#include "pch.cpp"



class MathVector
{

public:

	MathVector();

	MathVector(unsigned int size);

	MathVector(const MathVector& other) { this->copyVector(other); }
	MathVector& operator= (const MathVector& other)
	{
		this->deleteAllocatedMemory(); this->copyVector(other); return *this;
	}
	MathVector(const std::initializer_list<double> arr);
	MathVector& operator=(const std::initializer_list<double> arr);

	~MathVector() { this->deleteAllocatedMemory(); }

	// Inherantly an unsafe operator since it returns
	//     the reference to a variable but is useful for accessing
	//     elements
	double& operator[](int index) const {return data_[index];}

	bool isEqualTo(const MathVector& other) const;

	bool isZeroVector() const;
	double getMagnitude() const;

	unsigned int getSize() const { return size_; }
	unsigned int getOperationSize() const;

	bool operator+=(const MathVector& mv2);
	bool operator-=(const MathVector& mv2);
	
	bool operator*=(double alpha);

	double dotProduct(const MathVector& other) const;

	bool push_back(const double& element);


	
private:

	/*
	Private function for copying of a vector
	*/
	void copyVector(const MathVector& copyFrom);

	/*
	Private Member functions for the class
	*/
	void deleteAllocatedMemory();

	/*
	Private Member Variables
	*/

	// The size_ of the vector
	unsigned int size_;

	// The size that the vector is treated as when performing operations
	//    this should only be overrided if the user wants to keep the vector
	//    intact while decreasing the size that operations are performed with
	// However, items can still be added without changing this size 
	unsigned int sizeSeenInOperations_;

	bool useSizeFromOperations_ = false;

	// The variable for how much space is allocated to
	//     the vector 
	unsigned int preAlloc_;

	// Vectors for this class should use doubles otherwise these vectors would not 
	//    be closed under scalar multiplication
	double* data_;

};

// Outside of class functions that generate new vectors when used
bool approxEqual(const double& d1, const double& d2);

bool operator==(const MathVector& v1, const MathVector& v2);

MathVector add(const MathVector& v1, const MathVector& v2);
MathVector operator+(const MathVector& v1, const MathVector& v2);


MathVector scalarMult(const MathVector& vec, double alpha);
MathVector operator*(const MathVector& vec, double alpha);

double dotProduct(const MathVector& v1, const MathVector& v2);
double operator*(const MathVector& v1, const MathVector& v2);

MathVector findProjection(const MathVector& b, const MathVector& a);


#endif // __MATRIX_LIBARY_H
