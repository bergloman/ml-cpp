/*
 * Copyright Elasticsearch B.V. and/or licensed to Elasticsearch B.V. under one
 * or more contributor license agreements. Licensed under the Elastic License;
 * you may not use this file except in compliance with the Elastic License.
 */

#ifndef INCLUDED_ml_maths_CLinearAlgebraFwd_h
#define INCLUDED_ml_maths_CLinearAlgebraFwd_h

#include <maths/ImportExport.h>

#include <Eigen/src/Core/util/Constants.h>

#include <cstddef>

// Unfortunately, Eigen headers seem to be super fragile to
// include directly so we just forward declare here ourselves.
namespace Eigen {
template<typename SCALAR, int FLAGS, typename STORAGE_INDEX>
class SparseMatrix;
template<typename SCALAR, int FLAGS, typename STORAGE_INDEX>
class SparseVector;
template<typename SCALAR, int ROWS, int COLS, int OPTIONS, int MAX_ROWS, int MAX_COLS>
class Matrix;
}

namespace ml {
namespace maths {

//! Types of symmetric matrices constructed with a vector.
enum ESymmetricMatrixType { E_OuterProduct, E_Diagonal };

//! \brief Common types used by the vector and matrix classes.
class MATHS_EXPORT CLinearAlgebra {
public:
    static const char DELIMITER = ',';
};

//! \brief Base class to get a zero of type \p TYPE.
//!
//! Each of our vector and matrix types provides a specialization
//! of this class and define a static get method which takes the
//! dimension(s).
template<typename TYPE>
struct SZero {};

template<typename T, std::size_t>
class CVectorNx1;
template<typename T, std::size_t N>
class CSymmetricMatrixNxN;
template<typename T>
class CVector;
template<typename T>
class CSymmetricMatrix;
template<typename VECTOR, typename ANNOTATION>
class CAnnotatedVector;
template<typename MATRIX>
class CDenseMatrixInitializer;
template<typename VECTOR>
class CDenseVectorInitializer;
}
}

#endif // INCLUDED_ml_maths_CLinearAlgebraFwd_h
