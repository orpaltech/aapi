/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2025 ORPAL Technology, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AAPI_MATH_UTILS_H
#define AAPI_MATH_UTILS_H

#include "aapi_complex.h"

/*
 * Bitwise operations
 */
#define LO16(ui32)  (uint16_t)(0xFFFF & (ui32))
#define HI16(ui32)  (uint16_t)(0xFFFF & ((ui32) >> 16))
#define LO8(ui16)   (uint8_t)(0xFF & (ui16))
#define HI8(ui16)   (uint8_t)(0xFF & ((ui16) >> 8))
#define MAKE16(lo, hi)  ((uint16_t)((lo) & 0xFF) | (((uint16_t)(hi)) << 8))
#define MAKE32(lo, hi)  ((uint32_t)((lo) & 0xFFFF) | (((uint32_t)(hi)) << 16))

#define AAPI_SMALL_REAL  1e-30

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class MathUtils
///////////////////////////////////////////////////////////////////////////////

class MathUtils
{
public:
    /* Ensure val to be non-zero (safe to use as denominator)*/
    template<typename T>
    static AAPiComplexT<T> _cnonz(const AAPiComplexT<T>& val);

    /* Ensure val to be non-zero (safe to use as denominator)*/
    template<typename T>
    static T _nonz(T val);

    /* x1, x2, x3 - frequencies
     * x - frequency between x2 and x3 where we want to interpolate
     * y1, y2, y3 - values at frequencies x1, x2, x3
     */
    template<typename T>
    static AAPiComplexT<T>
    parabolic_interpolate(const AAPiComplexT<T>& y1,
                          const AAPiComplexT<T>& y2,
                          const AAPiComplexT<T>& y3,
                          T x1, T x2, T x3,
                          T x)
    {
        AAPiComplexT<T> a = ( (y3 - y2)/(x3 - x2) - (y2 - y1)/(x2 - x1) )/( x3 - x1 );
        AAPiComplexT<T> b = ( (y3 - y2)/(x3 - x2)*(x2 - x1) + (y2 - y1)/(x2 - x1)*(x3 - x2) )/( x3 - x1 );
        AAPiComplexT<T> res = a * std::pow(x - x2, 2.f) + b * (x - x2) + y2;
        return res;
    }


    /* Calculate determinant of 3x3 matrix*/
    /* Input: matrix 3x3
     *          [[ a, b, c ],
     *           [ m, n, k ],
     *           [ u, v, w ]]
    */
    template<typename T>
    static AAPiComplexT<T>
    determinant_3(const AAPiComplexT<T>& a, const AAPiComplexT<T>& b,
                  const AAPiComplexT<T>& c,
                  const AAPiComplexT<T>& m, const AAPiComplexT<T>& n,
                  const AAPiComplexT<T>& k,
                  const AAPiComplexT<T>& u, const AAPiComplexT<T>& v,
                  const AAPiComplexT<T>& w) {
        return ( a * n * w ) + ( b * k * u ) + ( m * v * c ) - ( c * n * u ) -
                    ( b * m * w ) - ( a * k * v );
    }

    /* Cramer's rule implementation , 3 equations with 3 unknowns */
    template<typename T>
    static void
    cramers_rule(const AAPiComplexT<T>& a11, const AAPiComplexT<T>& a12,
                 const AAPiComplexT<T>& a13, const AAPiComplexT<T>& b1,
                 const AAPiComplexT<T>& a21, const AAPiComplexT<T>& a22,
                 const AAPiComplexT<T>& a23, const AAPiComplexT<T>& b2,
                 const AAPiComplexT<T>& a31, const AAPiComplexT<T>& a32,
                 const AAPiComplexT<T>& a33, const AAPiComplexT<T>& b3,
                 AAPiComplexT<T> result[3])
    {
        AAPiComplexT<T> div;

        div = determinant_3(a11, a12, a13, a21, a22, a23, a31, a32, a33);
        div = _cnonz(div);

        result[0] = determinant_3(b1, a12, a13, b2, a22, a23, b3, a32, a33) / div;
        result[1] = determinant_3(a11, b1, a13, a21, b2, a23, a31, b3, a33) / div;
        result[2] = determinant_3(a11, a12, b1, a21, a22, b2, a31, a32, b3) / div;
    }

};

template<typename T>
AAPiComplexT<T> MathUtils::_cnonz(const AAPiComplexT<T>& val)
{
    if (0. == std::abs( val ))
        return AAPiComplexT<T>( AAPI_SMALL_REAL, 0.); /* small real value */
    return val;
}

template<typename T>
T MathUtils::_nonz(T val)
{
    if (0. == val || -0. == val)
        return AAPI_SMALL_REAL; /* small value, but much larger than min */
    return val;
}

} //namespace aapi

#endif // AAPI_MATH_UTILS_H
