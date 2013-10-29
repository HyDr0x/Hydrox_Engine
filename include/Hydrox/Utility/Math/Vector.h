#ifndef VECTOR_H_
#define VECTOR_H_

#define VECTOR_NUM_ARGS 1
#define VECTOR_INIT m_x[0] = x0;
#define VECTOR_INIT_VEC m_x[0] = v.m_x[0];
#define VECTOR_PARAMS Type x0
#define VECTOR_ARGS m_x[0]
#define VECTOR_ASSIGNEMENT_OP m_x[0] = v.m_x[0];
#define VECTOR_PLUS_OP        m_x[0] + v.m_x[0]
#define VECTOR_MINUS_OP       m_x[0] - v.m_x[0]
#define VECTOR_MULTIPLY_OP    m_x[0] * v.m_x[0]
#define VECTOR_DIVIDE_OP      m_x[0] / v.m_x[0]
#define VECTOR_MULTIPLY_OP_SCALAR m_x[0] * s
#define VECTOR_DIVIDE_OP_SCALAR   m_x[0] / s
#define VECTOR_EQ_PLUS_OP     m_x[0] += v.m_x[0];
#define VECTOR_EQ_MINUS_OP    m_x[0] -= v.m_x[0];
#define VECTOR_EQ_MULTIPLY_OP m_x[0] *= v.m_x[0];
#define VECTOR_EQ_DIVIDE_OP   m_x[0] /= v.m_x[0];
#define VECTOR_EQ_MULTIPLY_OP_SCALAR m_x[0] *= s;
#define VECTOR_EQ_DIVIDE_OP_SCALAR   m_x[0] /= s;
#define VECTOR_COMP_EQ    m_x[0] == v.m_x[0];
#define VECTOR_COMP_UQ    m_x[0] != v.m_x[0];
#define VECTOR_COMP_GR    m_x[0] >  v.m_x[0];
#define VECTOR_COMP_LS    m_x[0] <  v.m_x[0];
#define VECTOR_COMP_EQ_GR m_x[0] >= v.m_x[0];
#define VECTOR_COMP_EQ_LS m_x[0] <= v.m_x[0];
#define VECTOR_CAST m_x[0] = static_cast<Type>(v[0]);

#include "Hydrox/Utility/Math/Vector_Template.hpp"

#undef VECTOR_COMP_EQ_LS
#undef VECTOR_COMP_EQ_GR
#undef VECTOR_COMP_LS
#undef VECTOR_COMP_GR
#undef VECTOR_COMP_UQ
#undef VECTOR_COMP_EQ
#undef VECTOR_EQ_DIVIDE_OP_SCALAR
#undef VECTOR_EQ_MULTIPLY_OP_SCALAR
#undef VECTOR_EQ_DIVIDE_OP
#undef VECTOR_EQ_MULTIPLY_OP
#undef VECTOR_EQ_MINUS_OP
#undef VECTOR_EQ_PLUS_OP
#undef VECTOR_DIVIDE_OP_SCALAR
#undef VECTOR_MULTIPLY_OP_SCALAR
#undef VECTOR_DIVIDE_OP
#undef VECTOR_MULTIPLY_OP
#undef VECTOR_MINUS_OP
#undef VECTOR_PLUS_OP
#undef VECTOR_ASSIGNEMENT_OP
#undef VECTOR_ARGS
#undef VECTOR_PARAMS
#undef VECTOR_INIT_VEC
#undef VECTOR_INIT
#undef VECTOR_NUM_ARGS
#undef VECTOR_CAST


#define VECTOR_NUM_ARGS 2
#define VECTOR_INIT m_x[0] = x0; m_x[1] = x1;
#define VECTOR_INIT_VEC m_x[0] = v.m_x[0]; m_x[1] = v.m_x[1];
#define VECTOR_PARAMS Type x0, Type x1
#define VECTOR_ARGS m_x[0], m_x[1]
#define VECTOR_ASSIGNEMENT_OP m_x[0] = v.m_x[0];  m_x[1] = v.m_x[1];
#define VECTOR_PLUS_OP        m_x[0] + v.m_x[0],  m_x[1] + v.m_x[1]
#define VECTOR_MINUS_OP       m_x[0] - v.m_x[0],  m_x[1] - v.m_x[1]
#define VECTOR_MULTIPLY_OP    m_x[0] * v.m_x[0],  m_x[1] * v.m_x[1]
#define VECTOR_DIVIDE_OP      m_x[0] / v.m_x[0],  m_x[1] / v.m_x[1]
#define VECTOR_MULTIPLY_OP_SCALAR m_x[0] * s, m_x[1] * s
#define VECTOR_DIVIDE_OP_SCALAR   m_x[0] / s, m_x[1] / s
#define VECTOR_EQ_PLUS_OP     m_x[0] += v.m_x[0]; m_x[1] += v.m_x[1];
#define VECTOR_EQ_MINUS_OP    m_x[0] -= v.m_x[0]; m_x[1] -= v.m_x[1];
#define VECTOR_EQ_MULTIPLY_OP m_x[0] *= v.m_x[0]; m_x[1] *= v.m_x[1];
#define VECTOR_EQ_DIVIDE_OP   m_x[0] /= v.m_x[0]; m_x[1] /= v.m_x[1];
#define VECTOR_EQ_MULTIPLY_OP_SCALAR m_x[0] *= s; m_x[1] *= s;
#define VECTOR_EQ_DIVIDE_OP_SCALAR   m_x[0] /= s; m_x[1] /= s;
#define VECTOR_COMP_EQ    m_x[0] == v.m_x[0] && m_x[1] == v.m_x[1];
#define VECTOR_COMP_UQ    m_x[0] != v.m_x[0] || m_x[1] ||  v.m_x[1];
#define VECTOR_COMP_GR    m_x[0] >  v.m_x[0] && m_x[1] >   v.m_x[1];
#define VECTOR_COMP_LS    m_x[0] <  v.m_x[0] && m_x[1] <   v.m_x[1];
#define VECTOR_COMP_EQ_GR m_x[0] >= v.m_x[0] && m_x[1] >=  v.m_x[1];
#define VECTOR_COMP_EQ_LS m_x[0] <= v.m_x[0] && m_x[1] <=  v.m_x[1];
#define VECTOR_CAST m_x[0] = static_cast<Type>(v[0]); m_x[1] = static_cast<Type>(v[1]);

#include "Hydrox/Utility/Math/Vector_Template.hpp"

#undef VECTOR_COMP_EQ_LS
#undef VECTOR_COMP_EQ_GR
#undef VECTOR_COMP_LS
#undef VECTOR_COMP_GR
#undef VECTOR_COMP_UQ
#undef VECTOR_COMP_EQ
#undef VECTOR_EQ_DIVIDE_OP_SCALAR
#undef VECTOR_EQ_MULTIPLY_OP_SCALAR
#undef VECTOR_EQ_DIVIDE_OP
#undef VECTOR_EQ_MULTIPLY_OP
#undef VECTOR_EQ_MINUS_OP
#undef VECTOR_EQ_PLUS_OP
#undef VECTOR_DIVIDE_OP_SCALAR
#undef VECTOR_MULTIPLY_OP_SCALAR
#undef VECTOR_DIVIDE_OP
#undef VECTOR_MULTIPLY_OP
#undef VECTOR_MINUS_OP
#undef VECTOR_PLUS_OP
#undef VECTOR_ASSIGNEMENT_OP
#undef VECTOR_ARGS
#undef VECTOR_PARAMS
#undef VECTOR_INIT_VEC
#undef VECTOR_INIT
#undef VECTOR_NUM_ARGS
#undef VECTOR_CAST


#define VECTOR_NUM_ARGS 3
#define VECTOR_INIT m_x[0] = x0; m_x[1] = x1; m_x[2] = x2;
#define VECTOR_INIT_VEC m_x[0] = v.m_x[0]; m_x[1] = v.m_x[1]; m_x[2] = v.m_x[2];
#define VECTOR_PARAMS Type x0, Type x1, Type x2
#define VECTOR_ARGS m_x[0], m_x[1], m_x[2]
#define VECTOR_ASSIGNEMENT_OP m_x[0] = v.m_x[0];  m_x[1] = v.m_x[1];  m_x[2] = v.m_x[2];
#define VECTOR_PLUS_OP        m_x[0] + v.m_x[0],  m_x[1] + v.m_x[1],  m_x[2] + v.m_x[2]
#define VECTOR_MINUS_OP       m_x[0] - v.m_x[0],  m_x[1] - v.m_x[1],  m_x[2] - v.m_x[2]
#define VECTOR_MULTIPLY_OP    m_x[0] * v.m_x[0],  m_x[1] * v.m_x[1],  m_x[2] * v.m_x[2]
#define VECTOR_DIVIDE_OP      m_x[0] / v.m_x[0],  m_x[1] / v.m_x[1],  m_x[2] / v.m_x[2]
#define VECTOR_MULTIPLY_OP_SCALAR m_x[0] * s, m_x[1] * s, m_x[2] * s
#define VECTOR_DIVIDE_OP_SCALAR   m_x[0] / s, m_x[1] / s, m_x[2] / s
#define VECTOR_EQ_PLUS_OP     m_x[0] += v.m_x[0]; m_x[1] += v.m_x[1]; m_x[2] += v.m_x[2];
#define VECTOR_EQ_MINUS_OP    m_x[0] -= v.m_x[0]; m_x[1] -= v.m_x[1]; m_x[2] -= v.m_x[2];
#define VECTOR_EQ_MULTIPLY_OP m_x[0] *= v.m_x[0]; m_x[1] *= v.m_x[1]; m_x[2] *= v.m_x[2];
#define VECTOR_EQ_DIVIDE_OP   m_x[0] /= v.m_x[0]; m_x[1] /= v.m_x[1]; m_x[2] /= v.m_x[2];
#define VECTOR_EQ_MULTIPLY_OP_SCALAR m_x[0] *= s; m_x[1] *= s; m_x[2] *= s;
#define VECTOR_EQ_DIVIDE_OP_SCALAR   m_x[0] /= s; m_x[1] /= s; m_x[2] /= s;
#define VECTOR_COMP_EQ    m_x[0] == v.m_x[0] && m_x[1] == v.m_x[1] && m_x[2] == v.m_x[2];
#define VECTOR_COMP_UQ    m_x[0] != v.m_x[0] || m_x[1] != v.m_x[1] || m_x[2] != v.m_x[2];
#define VECTOR_COMP_GR    m_x[0] >  v.m_x[0] && m_x[1] >  v.m_x[1] && m_x[2] >  v.m_x[2];
#define VECTOR_COMP_LS    m_x[0] <  v.m_x[0] && m_x[1] <  v.m_x[1] && m_x[2] <  v.m_x[2];
#define VECTOR_COMP_EQ_GR m_x[0] >= v.m_x[0] && m_x[1] >= v.m_x[1] && m_x[2] >= v.m_x[2];
#define VECTOR_COMP_EQ_LS m_x[0] <= v.m_x[0] && m_x[1] <= v.m_x[1] && m_x[2] <= v.m_x[2];
#define VECTOR_CAST m_x[0] = static_cast<Type>(v[0]); m_x[1] = static_cast<Type>(v[1]); m_x[2] = static_cast<Type>(v[2]);

#include "Hydrox/Utility/Math/Vector_Template.hpp"

#undef VECTOR_COMP_EQ_LS
#undef VECTOR_COMP_EQ_GR
#undef VECTOR_COMP_LS
#undef VECTOR_COMP_GR
#undef VECTOR_COMP_UQ
#undef VECTOR_COMP_EQ
#undef VECTOR_EQ_DIVIDE_OP_SCALAR
#undef VECTOR_EQ_MULTIPLY_OP_SCALAR
#undef VECTOR_EQ_DIVIDE_OP
#undef VECTOR_EQ_MULTIPLY_OP
#undef VECTOR_EQ_MINUS_OP
#undef VECTOR_EQ_PLUS_OP
#undef VECTOR_DIVIDE_OP_SCALAR
#undef VECTOR_MULTIPLY_OP_SCALAR
#undef VECTOR_DIVIDE_OP
#undef VECTOR_MULTIPLY_OP
#undef VECTOR_MINUS_OP
#undef VECTOR_PLUS_OP
#undef VECTOR_ASSIGNEMENT_OP
#undef VECTOR_ARGS
#undef VECTOR_PARAMS
#undef VECTOR_INIT_VEC
#undef VECTOR_INIT
#undef VECTOR_NUM_ARGS
#undef VECTOR_CAST


#define VECTOR_NUM_ARGS 4
#define VECTOR_INIT m_x[0] = x0; m_x[1] = x1; m_x[2] = x2; m_x[3] = x3;
#define VECTOR_INIT_VEC m_x[0] = v.m_x[0]; m_x[1] = v.m_x[1]; m_x[2] = v.m_x[2]; m_x[3] = v.m_x[3];
#define VECTOR_PARAMS Type x0, Type x1, Type x2, Type x3
#define VECTOR_ARGS m_x[0], m_x[1], m_x[2], m_x[3]
#define VECTOR_ASSIGNEMENT_OP m_x[0] = v.m_x[0];  m_x[1] = v.m_x[1];  m_x[2] = v.m_x[2];  m_x[3] = v.m_x[3];
#define VECTOR_PLUS_OP        m_x[0] + v.m_x[0],  m_x[1] + v.m_x[1],  m_x[2] + v.m_x[2],  m_x[3] + v.m_x[3]
#define VECTOR_MINUS_OP       m_x[0] - v.m_x[0],  m_x[1] - v.m_x[1],  m_x[2] - v.m_x[2],  m_x[3] - v.m_x[3]
#define VECTOR_MULTIPLY_OP    m_x[0] * v.m_x[0],  m_x[1] * v.m_x[1],  m_x[2] * v.m_x[2],  m_x[3] * v.m_x[3]
#define VECTOR_DIVIDE_OP      m_x[0] / v.m_x[0],  m_x[1] / v.m_x[1],  m_x[2] / v.m_x[2],  m_x[3] / v.m_x[3]
#define VECTOR_MULTIPLY_OP_SCALAR m_x[0] * s, m_x[1] * s, m_x[2] * s, m_x[3] * s
#define VECTOR_DIVIDE_OP_SCALAR   m_x[0] / s, m_x[1] / s, m_x[2] / s, m_x[3] / s
#define VECTOR_EQ_PLUS_OP     m_x[0] += v.m_x[0]; m_x[1] += v.m_x[1]; m_x[2] += v.m_x[2]; m_x[3] += v.m_x[3];
#define VECTOR_EQ_MINUS_OP    m_x[0] -= v.m_x[0]; m_x[1] -= v.m_x[1]; m_x[2] -= v.m_x[2]; m_x[3] -= v.m_x[3];
#define VECTOR_EQ_MULTIPLY_OP m_x[0] *= v.m_x[0]; m_x[1] *= v.m_x[1]; m_x[2] *= v.m_x[2]; m_x[3] *= v.m_x[3];
#define VECTOR_EQ_DIVIDE_OP   m_x[0] /= v.m_x[0]; m_x[1] /= v.m_x[1]; m_x[2] /= v.m_x[2]; m_x[3] /= v.m_x[3];
#define VECTOR_EQ_MULTIPLY_OP_SCALAR m_x[0] *= s; m_x[1] *= s; m_x[2] *= s; m_x[3] *= s;
#define VECTOR_EQ_DIVIDE_OP_SCALAR   m_x[0] /= s; m_x[1] /= s; m_x[2] /= s; m_x[3] /= s;
#define VECTOR_COMP_EQ    m_x[0] == v.m_x[0] && m_x[1] == v.m_x[1] && m_x[2] == v.m_x[2] && m_x[3] == v.m_x[3];
#define VECTOR_COMP_UQ    m_x[0] != v.m_x[0] || m_x[1] != v.m_x[1] || m_x[2] != v.m_x[2] || m_x[3] != v.m_x[3];
#define VECTOR_COMP_GR    m_x[0] >  v.m_x[0] && m_x[1] >  v.m_x[1] && m_x[2] >  v.m_x[2] && m_x[3] >  v.m_x[3];
#define VECTOR_COMP_LS    m_x[0] <  v.m_x[0] && m_x[1] <  v.m_x[1] && m_x[2] <  v.m_x[2] && m_x[3] <  v.m_x[3];
#define VECTOR_COMP_EQ_GR m_x[0] >= v.m_x[0] && m_x[1] >= v.m_x[1] && m_x[2] >= v.m_x[2] && m_x[3] >= v.m_x[3];
#define VECTOR_COMP_EQ_LS m_x[0] <= v.m_x[0] && m_x[1] <= v.m_x[1] && m_x[2] <= v.m_x[2] && m_x[3] <= v.m_x[3];
#define VECTOR_CAST m_x[0] = static_cast<Type>(v[0]); m_x[1] = static_cast<Type>(v[1]); m_x[2] = static_cast<Type>(v[2]); m_x[3] = static_cast<Type>(v[3]);

#include "Hydrox/Utility/Math/Vector_Template.hpp"

#undef VECTOR_COMP_EQ_LS
#undef VECTOR_COMP_EQ_GR
#undef VECTOR_COMP_LS
#undef VECTOR_COMP_GR
#undef VECTOR_COMP_UQ
#undef VECTOR_COMP_EQ
#undef VECTOR_EQ_DIVIDE_OP_SCALAR
#undef VECTOR_EQ_MULTIPLY_OP_SCALAR
#undef VECTOR_EQ_DIVIDE_OP
#undef VECTOR_EQ_MULTIPLY_OP
#undef VECTOR_EQ_MINUS_OP
#undef VECTOR_EQ_PLUS_OP
#undef VECTOR_DIVIDE_OP_SCALAR
#undef VECTOR_MULTIPLY_OP_SCALAR
#undef VECTOR_DIVIDE_OP
#undef VECTOR_MULTIPLY_OP
#undef VECTOR_MINUS_OP
#undef VECTOR_PLUS_OP
#undef VECTOR_ASSIGNEMENT_OP
#undef VECTOR_ARGS
#undef VECTOR_PARAMS
#undef VECTOR_INIT_VEC
#undef VECTOR_INIT
#undef VECTOR_NUM_ARGS
#undef VECTOR_CAST

#endif