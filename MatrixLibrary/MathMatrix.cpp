#include "pch.h"
#include "MathMatrix.h"
#include <cmath>

MathMatrix::MathMatrix() {};
MathMatrix::MathMatrix(unsigned int numRows, unsigned int numCols)
{
	numRows_ = numRows;
	numCols_ = numCols;

	if (numRows == 0 || numCols == 0)
	{
		numRows_ = numCols_ = 0;
		// No need to allocate anything so just return without
		//     allocating any memory
		return;
	}
	
	preAlloc_ = 0x01;

	unsigned int tempNumCols = numCols;
	while (tempNumCols > 0)
	{
		preAlloc_ <<= 1;
		tempNumCols >>= 1;
	}

	// Now allocate all the memory
	vectorSpace_ = new MathVector*[preAlloc_];


	for (unsigned int i = 0; i < numCols; ++i)
	{
		vectorSpace_[i] = new MathVector(numRows);
	}
}

MathMatrix::MathMatrix(const MathVector& v, vector_space_t spaceOfVector)
{
	unsigned int vSize = v.getOperationSize();

	spaceToRepresentMatrixAs_ = spaceOfVector;

	if (vSize == 0)
	{
		numRows_ = numCols_ = 0;
		return;
	}
	
	numRows_ = 1;
	numCols_ = vSize;

	if (spaceOfVector == COLUMNSPACE)
	{
		numRows_ = vSize;
		numCols_ = 1;
	}
	
	// Make it two so we can add another vector easily
	preAlloc_ = 2;

	vectorSpace_ = new MathVector* [preAlloc_];

	// We actually need to construct this and not just use the reference
	vectorSpace_[0] = new MathVector(v);
}

/**
 * @brief Constructor that takes in two vectors and returns the matrix resulting from
 *     @ref colVector * @ref rowVector.  The resulting matrix is of size m x p where m is the
 *     size of @ref colVector and p is the size of @ref rowVector
 * @param colVector is the column vector to multiply with
 * @param rowVector is the row vector to multiply with
 */
MathMatrix::MathMatrix(const MathVector& colVector, const MathVector& rowVector)
{
	numRows_ = colVector.getOperationSize();
	numCols_ = rowVector.getOperationSize();

	if (numRows_ == 0 || numCols_ == 0)
	{
		numRows_ = numCols_ = 0;
		return;
	}

	// @todo Add a part of this function where a user can choose the space
	spaceToRepresentMatrixAs_ = COLUMNSPACE;
	
	preAlloc_ = pow2Above(numCols_);

	// Allocate the matrix
	vectorSpace_ = new MathVector * [preAlloc_];

	for (unsigned int i = 0; i < numCols_; ++i)
	{
		vectorSpace_[i] = new MathVector(numRows_);
	}

	// Now actually plug in the values
	for (unsigned int col = 0; col < numCols_; ++col)
	{
		for (unsigned int row = 0; row < numRows_; ++row)
		{
			(*vectorSpace_[col])[row] = colVector[row] * rowVector[col];
		}
	}
}

MathMatrix::MathMatrix(const MathMatrix& other)
{
	copy(other);
}
MathMatrix& MathMatrix::operator=(const MathMatrix& other)
{
	cleanUpDynamicallyAllocatedMemory();
	copy(other);
	return *this;
}

MathMatrix::MathMatrix(const std::initializer_list < std::initializer_list<double>> list2d)
{
	makeMatrixFromInitLists(list2d);
}
MathMatrix& MathMatrix::operator=(const std::initializer_list < std::initializer_list<double>> list2d)
{
	cleanUpDynamicallyAllocatedMemory();
	makeMatrixFromInitLists(list2d);
	return (*this);
}

/**
 * @brief Compares the matrix to 2d initializer lists.  Each inner initialzer list will be
 *     treated as a row in the matrix to provide understandable comparisons
 * @param other A 2d initializer list representing the matrix to compare with
 * @return true if the rows of the matrix are equal to the initalizer list
 */
bool MathMatrix::equals(const std::initializer_list<std::initializer_list<double>>& other) const
{
	if (getNumRowsInOperationSize() != other.size()) return false;

	unsigned int r = 0;
	unsigned int c = 0;

	unsigned int* firstAccess = &r;
	unsigned int* secondAccess = &c;
	if (spaceToRepresentMatrixAs_ == COLUMNSPACE)
	{
		firstAccess = &c;
		secondAccess = &r;
	}

	unsigned int numColsInOp = getNumColsInOperationSize();

	for (std::initializer_list<std::initializer_list<double>>::const_iterator rowItr = other.begin();
		rowItr != other.end(); ++rowItr)
	{
		if (rowItr->size() != numColsInOp)
		{
			return false;
		}

		c = 0;
		std::initializer_list<double>::const_iterator colItrEnd = rowItr->end();
		for (std::initializer_list<double>::const_iterator indItr = rowItr->begin(); indItr != colItrEnd; ++indItr)
		{
			if (*indItr != (*vectorSpace_[*firstAccess])[*secondAccess])
			{
				return false;
			}
			++c;
		}
		++r;
	}
	return true;
}

// Programming related helper functions

double MathMatrix::getVal(unsigned int row, unsigned int col) const
{
	if (row >= numRows_ || col >= numCols_) { return NAN;}

	unsigned int firstAccess = row;
	unsigned int secondAccess = col;

	if (spaceToRepresentMatrixAs_ == COLUMNSPACE)
	{
		firstAccess = col;
		secondAccess = row;
	}

	return (*vectorSpace_[firstAccess])[secondAccess];
}
bool MathMatrix::setVal(unsigned int row, unsigned int col, double valueToSetTo) const
{
	if (row >= numRows_ || col >= numCols_) { return false; }

	unsigned int firstAccess = row;
	unsigned int secondAccess = col;

	if (spaceToRepresentMatrixAs_ == COLUMNSPACE)
	{
		firstAccess = col;
		secondAccess = row;
	}

	(*vectorSpace_[firstAccess])[secondAccess] = valueToSetTo;

	return true;
}

unsigned int MathMatrix::getNumRowsInOperationSize() const
{
	if (useNonDefaultNumberOfRows_ == true)
	{
		return numRowsSeenInOperations_;
	}
	else
	{
		return numRows_;
	}
}

unsigned int MathMatrix::getNumColsInOperationSize() const
{
	if (useNonDefaultNumberOfCols_ == true)
	{
		return numColsSeenInOperations_;
	}
	else
	{
		return numCols_;
	}
}

bool MathMatrix::addRow(const MathVector& rowToAdd)
{
	bool vectorSuccessfullyAdded = false;

	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		vectorSuccessfullyAdded = addMathVectorToSameSpace(rowToAdd, numRows_, numCols_);
	}
	else if (spaceToRepresentMatrixAs_ == COLUMNSPACE)
	{
		vectorSuccessfullyAdded = addMathVectorToEndsOfEachVector(rowToAdd, numCols_, numRows_);
	}
	else
	{
		// AN UNKNOWN SPACE TYPE IS USED: Return false
		return vectorSuccessfullyAdded = false;
	}
	return vectorSuccessfullyAdded;
}

bool MathMatrix::addCol(const MathVector& colToAdd)
{
	bool vectorSuccessfullyAdded = false;

	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		vectorSuccessfullyAdded = addMathVectorToSameSpace(colToAdd, numRows_, numCols_);
	}
	else if (spaceToRepresentMatrixAs_ == COLUMNSPACE)
	{
		vectorSuccessfullyAdded = addMathVectorToEndsOfEachVector(colToAdd, numCols_, numRows_);
	}
	else
	{
		// AN UNKNOWN SPACE TYPE IS USED: Return false
		return vectorSuccessfullyAdded = false;
	}
	return vectorSuccessfullyAdded;
}

//======================================================================
// Math related operations
//======================================================================

bool MathMatrix::swapRows(unsigned int rowNum1, unsigned int rowNum2)
{
	// Check the bounds first
	unsigned int rowNumUpperBound = getNumRowsInOperationSize();
	if ((rowNum1 < 0) || (rowNum2) < 0 || (rowNum1 >= rowNumUpperBound) || 
		(rowNum2 >= rowNumUpperBound))
	{
		return false;
	}

	// If we are at this point then we know we are in bounds

	unsigned int col = 0;

	unsigned int* first_access_for_row1 = &rowNum1;
	unsigned int* second_access_for_row1 = &col;

	unsigned int* first_access_for_row2 = &rowNum1;
	unsigned int* second_access_for_row2 = &col;

	unsigned int* tempPtr;
	if (spaceToRepresentMatrixAs_ == COLUMNSPACE)
	{
		// Swap the accesses for the first row
		tempPtr = first_access_for_row1;
		first_access_for_row1 = second_access_for_row1;
		second_access_for_row2 = tempPtr;

		// Swap the accesses for the second row
		tempPtr = first_access_for_row1;
		first_access_for_row1 = second_access_for_row1;
		second_access_for_row2 = tempPtr;
	}

	// Now swap each of the elements
	unsigned int numCols = getNumColsInOperationSize();
	double temp;
	for (unsigned int c = 0; c < numCols; ++c)
	{
		temp = (*vectorSpace_[*first_access_for_row1])[*second_access_for_row1];
		
		vectorSpace_[*first_access_for_row1][*second_access_for_row1] = 
			vectorSpace_[*first_access_for_row2][*second_access_for_row2];
		
		(*vectorSpace_[*first_access_for_row2])[*second_access_for_row2] = temp;
	}

	// Finally we can return true since the swaps have all been successful
	return true;
}
bool MathMatrix::swapCols(unsigned int colNum1, unsigned int colNum2)
{
	// Check the bounds first
	unsigned int colNumUpperBound = getNumColsInOperationSize();
	if ((colNum1 < 0) || (colNum2) < 0 || (colNum1 >= colNumUpperBound) ||
		(colNum2 >= colNumUpperBound))
	{
		return false;
	}

	// If we are at this point then we know we are in bounds

	unsigned int col = 0;

	unsigned int* first_access_for_col1 = &colNum1;
	unsigned int* second_access_for_col1 = &col;

	unsigned int* first_access_for_col2 = &colNum2;
	unsigned int* second_access_for_col2 = &col;

	unsigned int* tempPtr;
	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		// Swap the accesses for the first row
		tempPtr = first_access_for_col1;
		first_access_for_col1 = second_access_for_col1;
		second_access_for_col2 = tempPtr;

		// Swap the accesses for the second row
		tempPtr = first_access_for_col1;
		first_access_for_col1 = second_access_for_col1;
		second_access_for_col2 = tempPtr;
	}

	// Now swap each of the elements
	unsigned int numRows = getNumRowsInOperationSize();
	double temp;
	for (unsigned int c = 0; c < numRows; ++c)
	{
		temp = (*vectorSpace_[*first_access_for_col1])[*second_access_for_col1];

		vectorSpace_[*first_access_for_col1][*second_access_for_col1] =
			vectorSpace_[*first_access_for_col2][*second_access_for_col2];

		(*vectorSpace_[*first_access_for_col2])[*second_access_for_col2] = temp;
	}

	// Finally we can return true since the swaps have all been successful
	return true;
}

bool MathMatrix::addMultipleOfRow(unsigned int rowNumToAddTo, unsigned int rowNumToAdd,
	double multiple)
{
	// Check the bounds first
	if (!isRowNumInOperationBounds(rowNumToAddTo) ||
		!isRowNumInOperationBounds(rowNumToAdd))
	{
		return false;
	}

	// If we are here then we can add each row to each other
	unsigned int col = 0;

	unsigned int* first_access_for_row1 = &rowNumToAddTo;
	unsigned int* second_access_for_row1 = &col;

	unsigned int* first_access_for_row2 = &rowNumToAdd;
	unsigned int* second_access_for_row2 = &col;

	unsigned int* tempPtr;
	if (spaceToRepresentMatrixAs_ == COLUMNSPACE)
	{
		// Swap the accesses for the first row
		tempPtr = first_access_for_row1;
		first_access_for_row1 = second_access_for_row1;
		second_access_for_row2 = tempPtr;

		// Swap the accesses for the second row
		tempPtr = first_access_for_row1;
		first_access_for_row1 = second_access_for_row1;
		second_access_for_row2 = tempPtr;
	}

	// Now add the multiple of the row
	unsigned int numCols = getNumColsInOperationSize();
	
	for (unsigned int i = 0; i < numCols; ++i)
	{
		(*vectorSpace_[*first_access_for_row1])[*second_access_for_row1] =
			multiple * (*vectorSpace_[*first_access_for_row2])[*second_access_for_row2];
	}

	return true;
}

bool MathMatrix::multiplyRowByConstant(unsigned int row, double constant)
{
	if (!isRowNumInOperationBounds(row))
	{
		return false;
	}

	unsigned int col = 0;
	
	unsigned int* first_access = &row;
	unsigned int* second_access = &col;

	if (spaceToRepresentMatrixAs_ == COLUMNSPACE)
	{
		first_access = &col;
		second_access = &row;
	}

	unsigned int numCols = getNumColsInOperationSize();

	for (unsigned int i = 0; i < numCols; ++i)
	{
		vectorSpace_[*first_access][*second_access] *= constant;
	}

	return true;
}

/**
 * @brief Transposes the matrix.
 * @return true if the matrix could be transposed and false if the matrix
 *     does not have a transpose
 * @note This function will transpose the entire matrix and WILL NOT use
 *     @ref numRowsSeenInOperations_ and @ref numColsSeenInOperations_ when
 *     transposing.
 */
void MathMatrix::transpose()
{
	unsigned int unsgined_temp;

	// Swap the number of rows with the number of columns
	unsgined_temp = numRows_; numRows_ = numCols_; numCols_ = unsgined_temp;

	unsgined_temp = numRowsSeenInOperations_; numRowsSeenInOperations_ = numColsSeenInOperations_;
	numColsSeenInOperations_ = unsgined_temp;

	bool booltemp;

	booltemp = useNonDefaultNumberOfRows_;
	useNonDefaultNumberOfRows_ = useNonDefaultNumberOfCols_;
	useNonDefaultNumberOfCols_ = booltemp;

	// Now swap how the matrix is represented
	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		spaceToRepresentMatrixAs_ = COLUMNSPACE;
	}
	else if (spaceToRepresentMatrixAs_ = COLUMNSPACE)
	{
		spaceToRepresentMatrixAs_ = ROWSPACE;
	}
	else
	{
		// The space should be either a row or columnspace
	}
}

MathMatrixIterator MathMatrix::rowBegin(unsigned int const row) const
{
	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		return MathMatrixIterator(vectorSpace_ + row, 0, spaceToRepresentMatrixAs_, ROWSPACE);
	}
	else
	{
		return MathMatrixIterator(vectorSpace_ + 0, row , spaceToRepresentMatrixAs_, ROWSPACE);
	}
}
MathMatrixIterator MathMatrix::rowEnd(unsigned int const row) const
{
	unsigned int endCol = getNumColsInOperationSize();
	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		return MathMatrixIterator(vectorSpace_ + row, endCol, spaceToRepresentMatrixAs_, ROWSPACE);
	}
	else
	{
		return MathMatrixIterator(vectorSpace_ + endCol, row, spaceToRepresentMatrixAs_, ROWSPACE);
	}
}
MathMatrixIterator MathMatrix::colBegin(unsigned int const col) const
{
	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		return MathMatrixIterator(vectorSpace_ + 0, col, spaceToRepresentMatrixAs_, COLUMNSPACE);
	}
	else
	{
		return MathMatrixIterator(vectorSpace_ + col, 0, spaceToRepresentMatrixAs_, COLUMNSPACE);
	}
}
MathMatrixIterator MathMatrix::colEnd(unsigned int const col) const
{
	unsigned int endRow = getNumColsInOperationSize();
	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		return MathMatrixIterator(vectorSpace_ + endRow, col, spaceToRepresentMatrixAs_, COLUMNSPACE);
	}
	else
	{
		return MathMatrixIterator(vectorSpace_ + col, endRow, spaceToRepresentMatrixAs_, COLUMNSPACE);
	}
}


//==================================================================================================
// Outside of class functions that are still related to the class
//=================================================================================================

MathMatrix operator*(const MathMatrix& m1, const MathMatrix& m2)
{
	// Make a new matrix with the correct size

	// However if the m1 is m x n the matrix m2 must have dimension n x p to be
	//     a legal matrix multiplication
	if (m1.getNumColsInOperationSize() != m2.getNumRowsInOperationSize())
	{
		return MathMatrix();
	}
	else
	{
		unsigned int numRowsInNewMatrix = m1.getNumRowsInOperationSize();
		unsigned int numColsInNewMatrix = m2.getNumColsInOperationSize();
		MathMatrix result(numRowsInNewMatrix, numColsInNewMatrix);

		// The result of the dot product
		double dotProdRes;
		for (unsigned int i = 0; i < numRowsInNewMatrix; ++i)
		{
			for (unsigned int j = 0; j < numColsInNewMatrix; ++j)
			{
				// M_i,j = row i of m1 * col j of m2
				dotProdRes = dotProduct(m1.rowBegin(i), m1.rowEnd(i), m2.colBegin(j), m2.colEnd(j));
				result.setVal(i, j, dotProdRes);
			}
		}
		return result;
	}
}


// Private helper functions for the class

void MathMatrix::cleanUpDynamicallyAllocatedMemory()
{
	unsigned int numToDelete = numCols_;
	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		numToDelete = numRows_;
	}

	for (unsigned int i = 0; i < numToDelete; ++i)
	{
		delete vectorSpace_[i];
	}
	delete vectorSpace_;
}

void MathMatrix::copy(const MathMatrix& other)
{
	this->spaceToRepresentMatrixAs_ = other.spaceToRepresentMatrixAs_;
	this->numRows_ = other.numRows_;
	this->numCols_ = other.numCols_;

	this->preAlloc_ = other.preAlloc_;

	this->numRowsSeenInOperations_ = other.numRowsSeenInOperations_;
	this->numColsSeenInOperations_ = other.numColsSeenInOperations_;
	this->useNonDefaultNumberOfRows_ = other.useNonDefaultNumberOfRows_;
	this->useNonDefaultNumberOfCols_ = other.useNonDefaultNumberOfCols_;

	// Now copy the dynamically allocated memory portion
	vectorSpace_ = new MathVector * [preAlloc_];

	unsigned int numVectorsToCopy = numCols_;
	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		numVectorsToCopy = numRows_;
	}

	for (unsigned int i = 0; i < numVectorsToCopy; ++i)
	{
		vectorSpace_[i] = new MathVector(*other.vectorSpace_[i]);
	}
	// Set the others to nullptrs
	for (unsigned int i = numVectorsToCopy; i < preAlloc_; ++i)
	{
		vectorSpace_[i] = nullptr;
	}
}

bool MathMatrix::isRowNumInOperationBounds(unsigned int rowNum)
{
	return (rowNum > 0) && (rowNum < getNumRowsInOperationSize());
}
bool MathMatrix::isColNumInOperationBounds(unsigned int colNum)
{
	return (colNum > 0) && (colNum < getNumColsInOperationSize());
}

/**
 * @brief Adds each element in v to the end of each vector made up in the space of the matrix
 * @param v is the vector to add.
 * @param innerSpaceSize[in, out] is a reference to the size of 
 * @note This private member function provides no bounds check as it does not care about whether or not
 *     the matrix is represented as a row space or a column space
 */
bool MathMatrix::addMathVectorToEndsOfEachVector(const MathVector& v, unsigned int const vectorSpaceSize,
	unsigned int & numElementsInVectorOfSpace)
{
	unsigned int sizeOfOtherMathVector = v.getOperationSize();
	
	// We are trying to add a row/column that is an invalid size
	if (sizeOfOtherMathVector != vectorSpaceSize)
	{
		return false;
	}

	for (unsigned int i = 0; i < sizeOfOtherMathVector; ++i)
	{
		vectorSpace_[i]->push_back(v[i]);
	}
	++numElementsInVectorOfSpace;
	return true;
}

/**
 * @brief Adds a @ref MathVector to the space of the matrix.  This function
 *     DOES NOT UPDATE @ref numRows or @ref numCols
 * @param v is the vector to add to the main space
 * @param mainSpaceSize[in, out] is the size of the main space to which the vector is added.  This variable
 *     will be updated if the size of that space changes
 */
bool MathMatrix::addMathVectorToSameSpace(const MathVector& v, unsigned int & vectorSpaceSize,
	unsigned int numElementsInVectorOfSpace)
{
	if (vectorSpaceSize >= preAlloc_)
	{
		// A fancy way of checking for overflow
		if (vectorSpaceSize == (0x01 << (sizeof(vectorSpaceSize)-1)) )
		{
			return false;
		}
		// Now we need to copy everything over
		preAlloc_ <<= 1;

		MathVector** newSpace = new MathVector * [preAlloc_];
		
		for (unsigned int i = 0; i < vectorSpaceSize; ++i)
		{
			newSpace[i] = vectorSpace_[i];
		}
		// Dont delete the previous vectors though since only the pointer is copied
		delete[] vectorSpace_;
		vectorSpace_ = newSpace;
	}

	vectorSpace_[vectorSpaceSize++] = new MathVector(v);

	return true;
}

void MathMatrix::makeMatrixFromInitLists(const std::initializer_list<std::initializer_list<double>>& list2d)
{
	if (list2d.size() == 0)
	{
		return;
	}

	std::initializer_list<std::initializer_list<double>>::const_iterator itr = list2d.begin();

	unsigned int numColsInMatrix = (unsigned int) itr->size();
	++itr;

	for (itr; itr != list2d.end(); ++itr)
	{
		if (itr->size() != numColsInMatrix)
		{
			return;
		}
	}

	numRows_ = (unsigned int) list2d.size();
	numCols_ = numColsInMatrix;

	preAlloc_ = 0x01;

	unsigned int tempNumCols = numCols_;
	while (tempNumCols > 0)
	{
		preAlloc_ <<= 1;
		tempNumCols >>= 1;
	}

	// Now allocate all the memory
	vectorSpace_ = new MathVector * [preAlloc_];


	for (unsigned int i = 0; i < numCols_; ++i)
	{
		vectorSpace_[i] = new MathVector(numRows_);
	}

	// Fill in the matrix

	unsigned int r = 0;
	unsigned int c = 0;


	for (std::initializer_list<std::initializer_list<double>>::iterator rowItr = list2d.begin();
		rowItr != list2d.end(); ++rowItr)
	{
		std::initializer_list<double>::iterator colEndItr = rowItr->end();
		c = 0;
		for (std::initializer_list<double>::iterator indexItr = rowItr->begin(); indexItr != colEndItr; ++indexItr)
		{
			(*vectorSpace_[c])[r] = *indexItr;
			++c;
		}
		++r;
	}
}


