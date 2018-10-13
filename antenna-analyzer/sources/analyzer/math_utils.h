/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-Pi software.
 *
 * 	ORPAL-AA-Pi is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-Pi is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-Pi. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AAPI_MATH_HELPER_H
#define AAPI_MATH_HELPER_H

#include <complex>

/*
 * Bitwise operations
 */
#define LO16(ui32)  (uint16_t)(0xFFFF & (ui32))
#define HI16(ui32)  (uint16_t)(0xFFFF & ((ui32) >> 16))
#define LO8(ui16)   (uint8_t)(0xFF & (ui16))
#define HI8(ui16)   (uint8_t)(0xFF & ((ui16) >> 8))
#define MAKE16(lo, hi)  ((uint16_t)((lo) & 0xFF) | (((uint16_t)(hi)) << 8))
#define MAKE32(lo, hi)  ((uint32_t)((lo) & 0xFFFF) | (((uint32_t)(hi)) << 16))


namespace aapi
{

class AAPIMathUtils
{
public:
    /* Ensure val to be non-zero (safe to use as denominator)*/
    static std::complex<float> _cnonz(const std::complex<float>& val);

    /* Ensure val to be non-zero (safe to use as denominator)*/
    static float _nonz(float val);

    /* x1, x2, x3 - frequencies
     * x - frequency between x2 and x3 where we want to interpolate
     * y1, y2, y3 - values at frequencies x1, x2, x3
     */
    static std::complex<float>
    parabolic_interpolate(const std::complex<float>& y1,
                          const std::complex<float>& y2,
                          const std::complex<float>& y3,
                          float x1, float x2, float x3,
                          float x);


    /* Calculate determinant of 3x3 matrix*/
    /* Input: matrix 3x3
     *          [[ a, b, c ],
     *           [ m, n, k ],
     *           [ u, v, w ]]
    */
    template<typename T>
    static inline std::complex<T>
    determinant_3(const std::complex<T>& a, const std::complex<T>& b,
                  const std::complex<T>& c,
                  const std::complex<T>& m, const std::complex<T>& n,
                  const std::complex<T>& k,
                  const std::complex<T>& u, const std::complex<T>& v,
                  const std::complex<T>& w) {
        return ( a * n * w ) + ( b * k * u ) + ( m * v * c ) - ( c * n * u ) -
                    ( b * m * w ) - ( a * k * v );
    }

    /* Cramer's rule implementation , 3 equations with 3 unknowns */
    static void
    cramers_rule(const std::complex<float>& a11, const std::complex<float>& a12,
                 const std::complex<float>& a13, const std::complex<float>& b1,
                 const std::complex<float>& a21, const std::complex<float>& a22,
                 const std::complex<float>& a23, const std::complex<float>& b2,
                 const std::complex<float>& a31, const std::complex<float>& a32,
                 const std::complex<float>& a33, const std::complex<float>& b3,
                 std::complex<float> result[3]);

};

} //namespace aapi

#endif // AAPI_MATH_HELPER_H
