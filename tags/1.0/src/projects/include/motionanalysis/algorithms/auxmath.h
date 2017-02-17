#pragma once
#include <cstdint>
#include <cmath>

//#ifdef _MSC_VER
//#include <float.h>  // for _isnan() on VC++
//#define isnan(x) _isnan(x)  // VC++ uses _isnan() instead of isnan()
//#define isfinite(x) _finite(x)

//#else
//#include <cmath>  // for isnan() everywhere else
//#endif


int popcount(uint32_t v);

int popcount(uint64_t v);

template <typename T>
T sqr( T a )
{
	return a * a;
}

