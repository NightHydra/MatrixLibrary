#C++ Matrix Library

This repository contains homeade classes implementing a vector and matrix from mathematical linear algebra.  Operations included with the library are dot-product and scalar multiplication for vectors as well as matrix multiplication and common row operations.
Due to the various operations that can be performed with this class currently only doubles are supported for the numbers contained within them.  This is due to the principle in linear algebra that a matrix is a linear transformation between two vector spaces and using fixed point numbers would make these transformations non-linear.

#Unique Features of this library
1. The matrix can either be selected to be a space of column vectors or row vectors.  This allows for users to optimize their code towards the application.  For example, if you are adding a lot of column vectors to your matrix then you can select the matrix to be represented as a space of column vectors.  This also allows for fast transposes by just swapping how the matrix is stored.  Users can elect to not worry about this feature as well and still have O(1) transpose.
1. The operation size of vectors and matrices can be set without changing the contents of the vector/matrix.  Simply set the operation size to a value less than the actual size and most operations will use this adjusted size.
2. 
