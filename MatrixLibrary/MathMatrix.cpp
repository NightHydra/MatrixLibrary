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
	vectorSpace_ = new MathVector*[preAlloc_];

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

// Math related operations

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
bool MathMatrix::isRowNumInOperationBounds(unsigned int rowNum)
{
	return (rowNum > 0) && (rowNum < getNumRowsInOperationSize());
}
bool MathMatrix::isColNumInOperationBounds(unsigned int colNum)
{
	return (colNum > 0) && (colNum < getNumColsInOperationSize());
}
