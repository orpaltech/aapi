/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2026 ORPAL Technology, Inc.
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

#ifndef AAPI_DSP_DEFS_H
#define AAPI_DSP_DEFS_H

#include <complex>
#include <fftw3.h>


namespace aapi
{
#if AAPI_DSP_DOUBLE_PRECISION
    using AAPiReal = double;

    #define AAPI_PI M_PI
#else
    using AAPiReal = float;

    #define AAPI_PI M_PIf
#endif

    namespace math {
        constexpr AAPiReal pi = AAPI_PI;
    }

template<typename T>
using AAPiComplexT = std::complex<T>;

using AAPiComplex = AAPiComplexT<AAPiReal>;
using AAPiComplexType = AAPiComplex::_ComplexT;

// FFTW
#if AAPI_DSP_DOUBLE_PRECISION
    #define FFTW_TYPE(name) fftw_ ## name
#else
    #define FFTW_TYPE(name) fftwf_ ## name
#endif

using fftw_real = aapi::AAPiReal;

// This maps custom name to the actual dynamic FFTW type/function
#define fftw_plan               FFTW_TYPE(plan)
#define fftw_complex            FFTW_TYPE(complex)
#define fftw_alloc_real(sz)     (fftw_real*) FFTW_TYPE(malloc)(sizeof(fftw_real) * (sz))
#define fftw_alloc_complex(sz)  (fftw_complex*) FFTW_TYPE(malloc)(sizeof(fftw_complex) * (sz))
#define fftw_plan_dft_r2c_1d    FFTW_TYPE(plan_dft_r2c_1d)
#define fftw_execute_dft_r2c    FFTW_TYPE(execute_dft_r2c)
#define fftw_free               FFTW_TYPE(free)
#define fftw_destroy_plan       FFTW_TYPE(destroy_plan)

};

#endif // AAPI_DSP_DEFS_H
