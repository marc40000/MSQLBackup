#pragma once

#include <math.h>

#define MDivRound(a, b)					\
	(									\
		((a) >= 0) ? (					\
			((a) + ((b) / 2)) / (b)		\
			) : (						\
			((a) - ((b) / 2)) / (b)		\
		)								\
	)

//#define MDivRound(a, b) ((a) / (b))


#define MRound(a)						\
	(									\
		((a) >= 0) ? (					\
			((a) + 0.5)					\
			) : (						\
			((a) - 0.5)					\
		)								\
	)

#define MRoundf(a)						\
	(									\
		((a) >= 0) ? (					\
			((a) + 0.5f)					\
			) : (						\
			((a) - 0.5f)					\
		)								\
	)


inline double sqr(const double a)
{
	return a * a;
}
inline float sqr(const float a)
{
	return a * a;
}
inline int sqr(const int a)
{
	return a * a;
}
inline unsigned int sqr(const unsigned int a)
{
	return a * a;
}
inline long long sqr(const long long a)
{
	return a * a;
}
inline unsigned long long sqr(const unsigned long long a)
{
	return a * a;
}

inline double Msqr(const double a)
{
	return a * a;
}
inline float Msqr(const float a)
{
	return a * a;
}
inline int Msqr(const int a)
{
	return a * a;
}
inline unsigned int Msqr(const unsigned int a)
{
	return a * a;
}
inline long long Msqr(const long long a)
{
	return a * a;
}
inline unsigned long long Msqr(const unsigned long long a)
{
	return a * a;
}

inline float MMin(const float a, const float b)
{
	return a < b ? a : b;
}
inline float MMax(const float a, const float b)
{
	return a >= b ? a : b;
}
inline double MMin(const double a, const double b)
{
	return a < b ? a : b;
}
inline double MMax(const double a, const double b)
{
	return a >= b ? a : b;
}
inline int MMin(const int a, const int b)
{
	return a < b ? a : b;
}
inline int MMax(const int a, const int b)
{
	return a >= b ? a : b;
}
inline unsigned int MMin(const unsigned int a, const unsigned int b)
{
	return a < b ? a : b;
}
inline unsigned int MMax(const unsigned int a, const unsigned int b)
{
	return a >= b ? a : b;
}
inline unsigned long long MMin(const unsigned long long a, const unsigned long long b)
{
	return a < b ? a : b;
}
inline unsigned long long MMax(const unsigned long long a, const unsigned long long b)
{
	return a >= b ? a : b;
}

const double doublemax = 1.7976931348623157E+308;
const double doublemin = -1.7976931348623157E+308;

const double MPId = 3.1415926535897932384626433832795;
const float MPIf = 3.1415926535897932384626433832795f;
#define MPI 3.1415926535897932384626433832795

const double MPI2d = 6.283185307179586476925286766559;
const float MPI2f = 6.283185307179586476925286766559f;
#define MPI2 6.283185307179586476925286766559

const double MPIhalfd = 1.5707963267948966192313216916398;
const float MPIhalff = 1.5707963267948966192313216916398f;
#define MPIhalf 1.5707963267948966192313216916398

const double MSqrt2d = 1.4142135623730950488016887242097;
const float MSqrt2f = 1.4142135623730950488016887242097f;
#define MSqrt2 1.4142135623730950488016887242097


inline int MTrunc(const float v)
{
	int i = (int)v;
	if ((v >= 0) || (((float)i) == v))
		return i;
	else
		return (int)(v - 1.0f);
}
#define MDiv MTrunc

inline float MMod(const float v)
{
	int i = (int)v;
	if ((v >= 0) || (((float)i) == v))
		return v - ((float)i);
	else
		return ((int)(v - 1.0f)) - v;
}


inline bool MEqualTolerance(const float a, const float b, const float tolerance)
{
	if (fabs(b - a) <= tolerance)
		return true;
	else
		return false;
}

inline bool MEqualTolerance(const double a, const double b, const double tolerance)
{
	if (fabs(b - a) <= tolerance)
		return true;
	else
		return false;
}


inline float Macos(const float v)
{
	if (-1.0f < v)
	{
		if (v < 1.0f)
			return acosf(v);
		else
			return 0.0f;
	}
	else
	{
		return MPIf;
	}
}

inline double Macos(const double v)
{
	if (-1.0 < v)
	{
		if (v < 1.0)
			return acos(v);
		else
			return 0.0;
	}
	else
	{
		return MPId;
	}
}

inline float Masin(const float v)
{
	if (-1.0f < v)
	{
		if (v < 1.0f)
			return asinf(v);
		else
			return MPIhalff;
	}
	else
	{
		return -MPIhalff;
	}
}

inline double Masin(const double v)
{
	if (-1.0 < v)
	{
		if (v < 1.0)
			return asin(v);
		else
			return MPIhalfd;
	}
	else
	{
		return -MPIhalfd;
	}
}


// according to IEEE standard => http://stackoverflow.com/questions/570669/checking-if-a-double-or-float-is-nan-in-c
inline const bool IsNaN(const double v)
{
	return v != v;
}
inline const bool IsNaN(const float v)
{
	return v != v;
}


inline const float floorprecision(const float v, const float precision)
{
	return floorf(v / precision) * precision;
}
inline const float ceilprecision(const float v, const float precision)
{
	return ceilf(v / precision) * precision;
}
inline const float roundprecision(const float v, const float precision)
{
	return floorf(MRoundf(v / precision)) * precision;
}

inline const double floorprecision(const double v, const double precision)
{
	return floor(v / precision) * precision;
}
inline const double ceilprecision(const double v, const double precision)
{
	return ceil(v / precision) * precision;
}
inline const double roundprecision(const double v, const double precision)
{
	return floor(MRound(v / precision)) * precision;
}

