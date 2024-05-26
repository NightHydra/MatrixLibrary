#pragma once
#include "pch.h"
#include "MathVector.h"

/**
 * @brief An enum to define the "dominant space" of the matrix.  This enum being
 *     set to @ref ROWSPACE means that the matrix will be represented as rows
 *     of @ref MathVector.  Setting it to COLUMNSPACE means that the matrix will
 *     be represented as columns of @ref MathVector.
 * @note This is useful since it allows the user to quickly take the transpose of
 *     the matrix since all we need to do is swap the variables rather than swap the
 *     entire matrix.
 * @note The user should not generally need to care about how the matrix is represented
 *     but if the user wants slight performance enchancement, ie. (to adding a extra column)
 *     or being able to extract and entire column the user may want to specify this value.
 */
typedef enum { ROWSPACE, COLUMNSPACE } vector_space_t;

/**
 * @brief A class used to represent mathematical matrices
 */
class MathMatrix
{
public:

	MathMatrix();
	MathMatrix(unsigned int numRows, unsigned int numCols);
	~MathMatrix() { cleanUpDynamicallyAllocatedMemory(); }
	MathMatrix(const MathMatrix& other);
	MathMatrix& operator=(const MathMatrix& other);

	// Programming related helper functions

	double getVal(unsigned int row, unsigned int col) const;
	bool setVal(unsigned int row, unsigned int col, double valueToSetTo) const;

	unsigned int getNumRowsInOperationSize();
	unsigned int getNumColsInOperationSize();

	// Math related operations
	
	bool swapRows(unsigned int rowNum1, unsigned int rowNum2);
	bool swapCols(unsigned int colNum1, unsigned int colNum2);

	bool addMultipleOfRow(unsigned int rowNumToAddTo, unsigned int rowNumToAdd,
		double multiple);

	bool multiplyRowByConstant(unsigned int row, double constant);

	void transpose();


private:
	// Dynamically Allocated Memory Helper Functions
	void cleanUpDynamicallyAllocatedMemory();
	void copy(const MathMatrix& other);

	bool isRowNumInOperationBounds(unsigned int rowNum);
	bool isColNumInOperationBounds(unsigned int colNum);

	/**
	 * @brief The dominant space of the matrix.  By defult the matrix
	 *     is represented as column vectors.
	 */
	vector_space_t spaceToRepresentMatrixAs_ = COLUMNSPACE;

	/**
	 * @brief Variables indicating the number of rows and number
	 *     of columns of the matrix
	 */
	unsigned int numRows_ = 0;
	unsigned int numCols_ = 0;

	/**
	 * @brief vectorSpace is the space of either row or column
	 *     vectors that form the matrix.
	 */
	MathVector** vectorSpace_ = nullptr;

	unsigned int preAlloc_ = 0;

	unsigned int numRowsSeenInOperations_ = 0;
	unsigned int numColsSeenInOperations_ = 0;

	bool useNonDefaultNumberOfRows_ = false;
	bool useNonDefaultNumberOfCols_ = false;

};

