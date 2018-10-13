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

#include "math_utils.h"

namespace aapi
{

/*
 * Ensure val to be non-zero (safe to use as denominator)
 */
std::complex<float> AAPIMathUtils::_cnonz(const std::complex<float>& val)
{
    if (std::abs(val) == 0.f)
        return std::complex<float>(1e-30f, 0.f); /* small real float value */
    return val;
}

/*
 * Ensure val to be non-zero (safe to use as denominator)
 */
float AAPIMathUtils::_nonz(float val)
{
    if (0.f == val || -0.f == val)
        return 1e-30f; /* small value, but much larger than min float */
    return val;
}

/*
 * x1, x2, x3 - frequencies
 * x - frequency between x2 and x3 where we want to interpolate
 * y1, y2, y3 - values at frequencies x1, x2, x3
 */
std::complex<float>
AAPIMathUtils::parabolic_interpolate(const std::complex<float>& y1, const std::complex<float>& y2,
                                     const std::complex<float>& y3,
                                     float x1, float x2, float x3, float x)
{
    std::complex<float> a = ( (y3 - y2)/(x3 - x2) - (y2 - y1)/(x2 - x1) )/( x3 - x1 );
    std::complex<float> b = ( (y3 - y2)/(x3 - x2)*(x2 - x1) + (y2 - y1)/(x2 - x1)*(x3 - x2) )/( x3 - x1 );
    std::complex<float> res = a * std::pow(x - x2, 2.f) + b * (x - x2) + y2;
    return res;
}

/*
 * Cramer's rule implementation , 3 equations with 3 unknowns
 */
void
AAPIMathUtils::cramers_rule(const std::complex<float>& a11, const std::complex<float>& a12, const std::complex<float>& a13,
                            const std::complex<float>& b1,
                            const std::complex<float>& a21, const std::complex<float>& a22, const std::complex<float>& a23,
                            const std::complex<float>& b2,
                            const std::complex<float>& a31, const std::complex<float>& a32, const std::complex<float>& a33,
                            const std::complex<float>& b3,
                            std::complex<float> result[3])
{
    std::complex<float> div;

    div = determinant_3(a11, a12, a13, a21, a22, a23, a31, a32, a33);
    div = _cnonz(div);

    result[0] = determinant_3(b1, a12, a13, b2, a22, a23, b3, a32, a33) / div;
    result[1] = determinant_3(a11, b1, a13, a21, b2, a23, a31, b3, a33) / div;
    result[2] = determinant_3(a11, a12, b1, a21, a22, b2, a31, a32, b3) / div;
}

} //namespace aapi
