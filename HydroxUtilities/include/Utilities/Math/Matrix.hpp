#ifndef MATRIX_H_
#define MATRIX_H_

/*
All matrices are stored in column major order.
*/

#include "Vector.h"

namespace he
{
  namespace util
  {
    template<typename Type, unsigned int Dim> class Matrix;
  }
}

#include "Matrix2.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"

#endif
