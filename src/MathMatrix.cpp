#include "MathMatrix.h"
#include <cmath>

/* ================================================================================================
 * Constructor Definitions
 * ==============================================================================================*/

/**
 * @brief Initializes a 0x0 MathMatrix object.
 * @note This constructor does not allocate memory for any dynamically allocated
 *     pointers since a 0x0 matrix does not need to have memory allocated for it yet.
 */
MathMatrix::MathMatrix() = default;

/**
 * @brief Constructor for generating a (n x m) matrix where n = @ref numRows and
 *     m = @ref numCols.
 * @param numRows Is the number of rows to initialize the matrix with.  If this equals 0
 *     then a 0x0 matrix will be created no matter the value of @ref numCols.
 * @param numCols Is the number of columns to initialize the matrix with.  If this equals 0
 *    then a 0x0 matrix will be created and pointers will not be allocated memory.
 * @todo Add an optional parameter for a value of type @ref vector_space_t allowing
 *     for the user to specify whether the matrix is represented as a rowspace or columnspace
 *     in the underlying representation.
 */
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

/**
 *
 * @param v The vector used to create a 1xn matrix or nx1 matrix that consists of
 *     of only the vector.  For example, if @ref spaceOfVector is set to @ref ROWSPACE
 *     and v = {1,2,3} then a Matrix object will be created with only 1 row.
 * @param spaceOfVector Is whether the vector used to create the matrix is a row vector
 *     or column vector within the matrix. If @ref spaceOfVector = @ref ROWSPACE then it will
 *     be a row vecotr.  If @ref spaceOfVector = @ref COLUMNSPACE then it will be a column vector.
 */
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
	vectorSpace_ = new MathVector* [preAlloc_];

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

/**
 * @brief This allows users to hardcode a matrix using the following syntax.
 *    MathMatrix m( { {1,2,3}, {4,5,6}, {7,8,9}});
 * @param list2d Is the 2d list to make the matrix from.
 * @note The list is not passed by reference because the user might be hardcoding the
 *     initializer list in which case the list has no assigned address.
 */
MathMatrix::MathMatrix(const std::initializer_list < std::initializer_list<double>> list2d)
{
	makeMatrixFromInitLists(list2d);
}

/* ================================================================================================
 * Rule of 5 function definitions (6 are actually defined to allow for assignment to init lists)
 * ===============================================================================================*/

/**
 * @brief This function defines the copy constructor since the class uses dynamically
 *     allocated memory.
 * @param other The matrix to copy from
 */
MathMatrix::MathMatrix(const MathMatrix& other)
{
	copy(other);
}

/**
 * @brief This function defines the move operator to prevent needlessly copying dynamically
 *     allocated memory if not necessary.
 * @param other
 */
MathMatrix::MathMatrix(MathMatrix&& other) noexcept
{
	copyNonPointerMembers(other);
	movePointersFromOtherMatrix(other);

	// Ensure we default the copied matrix so the destructor does not try to delete a nullptr
	other.clear();
}

/**
 *
 * @param other The matrix to be copied by the assignment operator.
 * @return The matrix that @ref other was copied into.  Returning the matrix
 *    allows for triple assignment (ie. m = m1 = m2).
 */
MathMatrix& MathMatrix::operator=(const MathMatrix& other)
{
	if (&other == this)
	{
		return *this;
	}
	cleanUpDynamicallyAllocatedMemory();
	copy(other);
	return *this;
}

/**
 *
 * @param other The matrix to move into the matrix variable on thel left side of the equals sign.
 * @return The matrix object that the matrix in @ref other was moved into.
 */
MathMatrix& MathMatrix::operator=(MathMatrix&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}
	cleanUpDynamicallyAllocatedMemory();
	copyNonPointerMembers(other);
	movePointersFromOtherMatrix(other);

	other.clear();

	return *this;
}

/**
 * @brief A constructor defined to allow for the construction of a @ref MathMatrix object
 *     with the syntax m = { {1,2,3}, {4,5,6}, {7,8,9}};
 * @param list2d The list representing the matrix to generate.
 * @return The Matrix object represented by the matrix on the left side of the equals sign.
 */
MathMatrix& MathMatrix::operator=(const std::initializer_list < std::initializer_list<double>> list2d)
{
	cleanUpDynamicallyAllocatedMemory();
	makeMatrixFromInitLists(list2d);
	return (*this);
}



/**
 * @brief This function sets the matrix back to a 0x0 matrix keeping all configurations
 *     such as the space to represent the matrix as, to what they are currently set to.
 */
void MathMatrix::clear()
{
	cleanUpDynamicallyAllocatedMemory();
	numRows_ = numCols_ = numRowsSeenInOperations_ = numColsSeenInOperations_ = numRowsSeenInOperations_ = preAlloc_ = 0;
	vectorSpace_ = nullptr;
}

/**
 * @brief Matrix comparison.  The underlying space representations do not need to match
 *     in order for this to return true.  However, the abstracted matrix needs to the same
 *     for the comparison to return true.
 * @param other The object of type @ref MathMatrix to compare this matrix to.
 * @return True if the abstracted matrices are equal (ie. get(i,j) = other.get(i,j)) for all i,j.
 *     False if the matrices are not equal.
 * @todo The function does not actually do the correct comparison now.
 */
bool MathMatrix::equals(const MathMatrix& other) const
{
	unsigned int m1NumberOfRows = this->getNumRowsInOperationSize();
	unsigned int m2NumberOfRows = other.getNumRowsInOperationSize();
	unsigned int m1NumberOfCols = this->getNumColsInOperationSize();
	unsigned int m2NumberOfCols = other.getNumColsInOperationSize();

	if ((m1NumberOfRows != m2NumberOfRows) || (m1NumberOfCols != m2NumberOfCols))
	{
		return false; // <--- RETURN FALSE SINCE SIZES DONT MATCH
	}

	return true;
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
			if (!approxEqual(*indItr , (*vectorSpace_[*firstAccess])[*secondAccess]))
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

/**
 * @brief Gets the value at row @ref row and column @ref col in the matrix object if (@ref row, @ref col) is
 *     within the bounds of the matrix.  Returns NAN if (@ref row, @ref col) is not within the bounds
 *     of the matrix.
 * @param row The row of the abstracted matrix to get the value of.
 * @param col The column of the abstracted matrix to get the value of.
 * @return The value at (row, col) of the matrix.  If the value is out of bounds for the size
 *     of the matrix then the value NAN is returned.
 * @note The space in which the matrix is represented as does not effect the results of this
 *     function.  Whether the matrix is represented as a row or column space does not affect
 *     how the user uses it for operations.
 */
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

/**
 * @brief Setter for a specific index of the matrix.
 * @param row is the row of the matrix to set.
 * @param col is the column of the matrix to set.
 * @param valueToSetTo is the value that (@ref row, @ref column) is set to.
 * @return true if the index of the matrix was successfully set to @ref valueToSetTo and
 *     false if the value was not successully set.  The value will not be successfully
 *     set to @ref valueToSetTo if (@ref row, @ref col) is not within the bounds of the matrix.
 *     In this case this function will return false.
 */
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

/**
 * @brief Returns the number of rows that the matrix that is seen when performing operations.
 *    This is intended to be used for erasing rows of the matrix without actually removing them
 *    incase that row needs to be used.
 * @return The number of rows that are seen by operations in the matrix.
 * @todo Functionality for not using the number of rows that are actually stored needs
 *    to have specifications written around the feature of the library.
 */
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

/**
 * @brief Returns the number of columns that the matrix that is seen when performing operations.
 *    This is intended to be used for erasing columns of the matrix without actually removing them
 *    incase that column needs to be used.
 * @return The number of columns that are seen by operations in the matrix.
 * @todo Functionality for not using the number of columns that are actually stored needs
 *    to have specifications written around the feature of the library.
 */
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

/**
 * @brief A function to add a row to the back of the matrix.
 * @param rowToAdd is a variable of type @ref MathVector to be added after the last row of the matrix.
 *     The size of this @ref MathVector must by the same size as every other row in the matrix
 *     or else the row will not be added and the function will return false.
 * @return true is the row could successfully be added to the end of the matrix and false if the
 *     row could not be successfully added due to the row not having the same size as every other row
 *     in the matrix.
 */
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

/**
 * @brief A function to add a column to the back of the matrix.
 * @param colToAdd is a variable of type @ref MathVector to be added after the last column of the matrix.
 *     The size of this @ref MathVector must by the same size as every other column in the matrix
 *     or else the column will not be added and the function will return false.
 * @return true is the column could successfully be added to the end of the matrix and false if the
 *     column could not be successfully added due to the column not having the same size as every
 *     other column in the matrix.
 */
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

/**
 * @brief A function to swap rows in the matrix.  Swaps the index of @ref rowNum1 and @ref rowNum2
 *     if both rows are defined within the matrix.
 * @param rowNum1 The first row to swap positions with.
 * @param rowNum2 The second row to swap positions with.
 * @return true if the function could successfully swap the rows in the matrix.  This function
 *     returns false if the function could not successfully swap rows due to one of the two
 *     row indexes being outside the bounds of the matrix or due to some other issue.
 * @note This function is far more efficient if the underlying matrix is represented as a row
 *     vector space of row vectors.  This still works if the matrix is represented as a column
 *     space, though it will have O(c) runtime where c is the number of columns within the matrix.
 */
bool MathMatrix::swapRows(unsigned int rowNum1, unsigned int rowNum2)
{
	// Check the bounds first
	unsigned int rowNumUpperBound = getNumRowsInOperationSize();
	if ((rowNum1 >= rowNumUpperBound) || (rowNum2 >= rowNumUpperBound))
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

/**
 * @brief A function to swap columns in the matrix.  Swaps the index of @ref colNum1 and @ref colNum2
 *     if both columns are defined within the matrix.
 * @param colNum1 The first column to swap positions with.
 * @param colNum2 The second column to swap positions with.
 * @return true if the function could successfully swap the columns in the matrix.  This function
 *     returns false if the function could not successfully swap columns due to one of the two
 *     column indexes being outside the bounds of the matrix or due to some other issue.
 * @note This function is far more efficient if the underlying matrix is represented as a column
 *     vector space of column vectors.  This still works if the matrix is represented as a row
 *     space, though it will have O(r) runtime where r is the number of rows within the matrix.
 */
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

/**
 * @brief A function which adds a multiple of one row to another row within the matrix.
 *    This operation is useful for larger operations such as for finding the rref of a matrix object.
 *    The mathematical function represented by this operation is Rk1 <- Rk1 + C*Rk2 where
 *    Rk1 = @ref rowNumToAddTo, Rk2 = @ref rowNumToAdd, C = multiple
 * @param rowNumToAddTo The index of the row which will be added to
 * @param rowNumToAdd The index of the row which is being added to @ref rowNumToAddTo.
 * @param multiple is the constant to multiply @ref rowNumToAdd by.
 * @return A boolean indicating whether the operation was successful.  If the function
 *     returns true then the operation was completed successfully.  If the function returns
 *     false then the matrix remained unchanged due to an invalid input.
 * @todo Add a check for when @ref multiple = NAN and return false.
 */
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

/**
 * @brief multiplies every value in a row by a constant.
 * @param row is the index of the row to multiply by @ref constant.
 * @param constant Is the constant to multiply every element in the row by.
 * @return true if every value in the row was successfully multiplied by the constant and returns
 *     false if the operation was not successfully.  If the operation is unsuccessful, the row
 *     is unchanged.  The function would return false if the row index given is out of bounds
 *     or if @ref constant is equal to NAN.
 * @todo Handle the case where constant is equal to NAN.
 */
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
 * @brief A function that performs the transpose operation on the matrix object.
 *     This function is extremely efficient due to it just changing the space in which the matrix
 *     is represented as instead of swapping any actual data.
 * @return true if the matrix could be transposed and false if the matrix
 *     does not have a transpose
 * @note This function will transpose the entire matrix and WILL NOT use
 *     @ref numRowsSeenInOperations_ and @ref numColsSeenInOperations_ when
 *     transposing.
 */
void MathMatrix::transpose()
{
	unsigned int unsigned_temp;

	// Swap the number of rows with the number of columns
	unsigned_temp = numRows_; numRows_ = numCols_; numCols_ = unsigned_temp;

	unsigned_temp = numRowsSeenInOperations_; numRowsSeenInOperations_ = numColsSeenInOperations_;
	numColsSeenInOperations_ = unsigned_temp;

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

/**
 * @brief A function which returns an iterator that with iterate over a row within the matrix.
 *    The iterator starts at the beginning of row @ref row.
 * @param row is the row for the iterator to iterate over.
 * @return An iterator that when incremented will iterate over the matrix.
 */
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

/**
 * @brief A function which returns an iterator that points to one past the last index of the row
 *     indicated by @ref row.  This is useful for iterating over an entire row and knowing
 *     when to stop.
 * @param row is the row to iterate over.
 * @return An iterator that points to one past the last index in the row.
 */
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
	unsigned int endRow = getNumRowsInOperationSize();
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

/**
 * @brief A function to perform matrix multiplication on two matrices and return the result.
 *    The operation represented by this function is m1*m2.
 * @param m1 Is the left matrix to multiply.
 * @param m2 Is the right matrix to multiply.
 * @return The result of matrix m1*m2 if the matices are of the corrent size.  If either matrix
 *     is the empty matrix or the sizes are incompatible for multiplication then the function
 *     returns an empty matrix.
 * @note Matrices can only be multipled if the number of columns in @ref m1 is equal to the number
 *     of rows in @ref m2.
 */
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
		double dotProdRes = 0.0;
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

/**
 * @brief This function finds the projection matrix of column vector @ref vectorToFindProjectionMatrixOf.
 *     A projection matrix is a matrix that when multiplied by another vector results in a column vector
 *     representing the point closest to that vector that is on the line represented by the vector that
 *     created the projection matrix.
 * @param vectorToFindProjectionMatrixOf Is the vector that we find the projection matrix of
 * @return The projection matrix generated from the given vector
 */
MathMatrix createProjectionMatrix(const MathVector& vectorToFindProjectionMatrixOf)
{
	// Finding the dot product of the vector with itself
	double dotProduct = vectorToFindProjectionMatrixOf.dotProduct(vectorToFindProjectionMatrixOf);

	MathVector vectorDividedByScalar = (1 / dotProduct) * vectorToFindProjectionMatrixOf;

	MathMatrix m(vectorToFindProjectionMatrixOf, vectorDividedByScalar);

	return m;
}



/* ================================================================================================
 * Private helper functions for the class
 * ================================================================================================


/* ================================================================================================
 * Rule of 5 function helper function definitions
 * ===============================================================================================*/

/**
 * @brief A function that cleans up all dynamically allocated memory within the function
 *    and resets the pointers to nullptr.
 */
void MathMatrix::cleanUpDynamicallyAllocatedMemory() {
	unsigned int numToDelete = numCols_;
	if (spaceToRepresentMatrixAs_ == ROWSPACE)
	{
		numToDelete = numRows_;
	}

	if (vectorSpace_ != nullptr)
	{
		for (unsigned int i = 0; i < numToDelete; ++i)
		{
			delete vectorSpace_[i];
		}
		delete vectorSpace_;

		// Make sure to set the memory pointer to null so there isnt a dangling pointer.
		vectorSpace_ = nullptr;
	}
}

/**
 * @brief A function that copies all non-pointer members from @ref other to
 *     this matrix object.
 * @param other Copies all the non pointer members of the function to their respective
 *     class variables of this objects instance.
 */
void MathMatrix::copyNonPointerMembers(const MathMatrix& other)
{
	this->spaceToRepresentMatrixAs_ = other.spaceToRepresentMatrixAs_;
	this->numRows_ = other.numRows_;
	this->numCols_ = other.numCols_;

	this->preAlloc_ = other.preAlloc_;

	this->numRowsSeenInOperations_ = other.numRowsSeenInOperations_;
	this->numColsSeenInOperations_ = other.numColsSeenInOperations_;
	this->useNonDefaultNumberOfRows_ = other.useNonDefaultNumberOfRows_;
	this->useNonDefaultNumberOfCols_ = other.useNonDefaultNumberOfCols_;
}

/**
 * @brief A function that moves pointers from another matrix to the current matrix.
 *     This is really useful for the move constructor to copy pointers.
 * @param other Is the matrix with which to move the pointers from.
 */
void MathMatrix::movePointersFromOtherMatrix(MathMatrix& other)
{
	vectorSpace_ = other.vectorSpace_;
	other.vectorSpace_ = nullptr;
}

/**
 * @brief A function that copies the matrix in object @ref other.
 * @param other The matrix to copy from.
 */
void MathMatrix::copy(const MathMatrix& other)
{
	copyNonPointerMembers(other);

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

/**
 * @brief A function that returns whether the index of a specific row is within
 *     the "operation bounds" of the matrix.
 * @param rowNum Is the row number that is being checked for whether it is in the operation
 *     bounds of the matrix.
 * @return true if the row is within the operation bounds of the matrix and false if it is not.
 */
bool MathMatrix::isRowNumInOperationBounds(unsigned int rowNum) const
{
	return (rowNum > 0) && (rowNum < getNumRowsInOperationSize());
}

/**
 * @brief A function that returns whether the index of a specific column is within
 *     the "operation bounds" of the matrix.
 * @param colNum Is the row number that is being checked for whether it is in the operation
 *     bounds of the matrix.
 * @return true if the cilumn is within the operation bounds of the matrix and false if it is not.
 */
bool MathMatrix::isColNumInOperationBounds(unsigned int colNum) const
{
	return (colNum > 0) && (colNum < getNumColsInOperationSize());
}

/**
 * @brief Adds each element in v to the end of each vector made up in the space of the matrix
 * @param v is the vector to add.
 * @param innerSpaceSize[in, out] is a reference to the size of 
 * @note This private member function provides no bounds check as it does not care about whether
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
 * @param vectorSpaceSize[in, out] is the size of the main space to which the vector is added.  This variable
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
		// Don't delete the previous vectors though since only the pointer is copied
		delete[] vectorSpace_;
		vectorSpace_ = newSpace;
	}

	vectorSpace_[vectorSpaceSize++] = new MathVector(v);

	return true;
}

/**
 * @brief A function that is used for the constructors of making matrices from initializer lists
 *     allowing users to more easily create matrices without needing to set each individual
 *     row and column.
 * @param list2d The 2D initializer list with which to generate the matrix from.
 */
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




