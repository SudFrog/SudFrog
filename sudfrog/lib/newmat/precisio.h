/// \ingroup newmat
///@{

/// \file precisio.h
/// Floating point precision constants.

#ifndef PRECISION_LIB
#define PRECISION_LIB 0

#define WANT_MATH
#include "include.h"                                        // in case being used as stand alone

#ifdef _STANDARD_                                           // standard library available
#include <limits>
#endif

#ifdef use_namespace
namespace NEWMAT
{
#endif

#ifdef _STANDARD_                                       // standard library available

#ifdef OPT_COMPATIBLE
#include <cfloat>                                       // for FLT_MAX
#endif

    using namespace std;

/// Floating point precision.
    class FloatingPointPrecision
    {
        public:
            static int Dig()                                // number of decimal digits or precision
                { return numeric_limits<Real>::digits10 ; }

            static Real Epsilon()                           // smallest number such that 1+Eps!=Eps
                { return numeric_limits<Real>::epsilon(); }

            static int Mantissa()                           // bits in mantisa
                { return numeric_limits<Real>::digits; }

            static Real Maximum()                           // maximum value
                { return numeric_limits<Real>::max(); }

            static int MaximumDecimalExponent()             // maximum decimal exponent
                { return numeric_limits<Real>::max_exponent10; }

            static int MaximumExponent()                    // maximum binary exponent
                { return numeric_limits<Real>::max_exponent; }

            static Real LnMaximum()                         // natural log of maximum
                { return (Real)log(Maximum()); }

            static Real Minimum()                           // minimum positive value
                { return numeric_limits<Real>::min(); }

            static int MinimumDecimalExponent()             // minimum decimal exponent
                { return numeric_limits<Real>::min_exponent10; }

            static int MinimumExponent()                    // minimum binary exponent
                { return numeric_limits<Real>::min_exponent; }

            static Real LnMinimum()                         // natural log of minimum
                { return (Real)log(Minimum()); }

            static int Radix()                              // exponent radix
                { return numeric_limits<Real>::radix; }

            static int Rounds()                             // addition rounding (1 = does round)
            {
                return numeric_limits<Real>::round_style ==
                    round_to_nearest ? 1 : 0;
            }

    };

#else                                                   // _STANDARD_ not defined

#ifndef SystemV                                         // if there is float.h

#ifdef USING_FLOAT

/// Floating point precision (type float).
    class FloatingPointPrecision
    {
        public:
            static int Dig()
            {                                               // number of decimal digits or precision
                return FLT_DIG;
            }

            static Real Epsilon()
            {                                               // smallest number such that 1+Eps!=Eps
                return FLT_EPSILON;
            }

            static int Mantissa()
            {                                               // bits in mantisa
                return FLT_MANT_DIG;
            }

            static Real Maximum()
            {                                               // maximum value
                return FLT_MAX;
            }

            static int MaximumDecimalExponent()
            {                                               // maximum decimal exponent
                return FLT_MAX_10_EXP;
            }

            static int MaximumExponent()
            {                                               // maximum binary exponent
                return FLT_MAX_EXP;
            }

            static Real LnMaximum()
            {                                               // natural log of maximum
                return (Real)log(Maximum());
            }

            static Real Minimum()
            {                                               // minimum positive value
                return FLT_MIN;
            }

            static int MinimumDecimalExponent()
            {                                               // minimum decimal exponent
                return FLT_MIN_10_EXP;
            }

            static int MinimumExponent()
            {                                               // minimum binary exponent
                return FLT_MIN_EXP;
            }

            static Real LnMinimum()
            {                                               // natural log of minimum
                return (Real)log(Minimum());
            }

            static int Radix()
            {                                               // exponent radix
                return FLT_RADIX;
            }

            static int Rounds()
            {                                               // addition rounding (1 = does round)
                return FLT_ROUNDS;
            }

    };
#endif                                                  // USING_FLOAT

#ifdef USING_DOUBLE

/// Floating point precision (type double).
    class FloatingPointPrecision
    {
        public:

            static int Dig()
            {                                               // number of decimal digits or precision
                return DBL_DIG;
            }

            static Real Epsilon()
            {                                               // smallest number such that 1+Eps!=Eps
                return DBL_EPSILON;
            }

            static int Mantissa()
            {                                               // bits in mantisa
                return DBL_MANT_DIG;
            }

            static Real Maximum()
            {                                               // maximum value
                return DBL_MAX;
            }

            static int MaximumDecimalExponent()
            {                                               // maximum decimal exponent
                return DBL_MAX_10_EXP;
            }

            static int MaximumExponent()
            {                                               // maximum binary exponent
                return DBL_MAX_EXP;
            }

            static Real LnMaximum()
            {                                               // natural log of maximum
                return (Real)log(Maximum());
            }

            static Real Minimum()
            {
//#ifdef __BCPLUSPLUS__
//       return 2.225074e-308;     // minimum positive value
//#else
                return DBL_MIN;
//#endif
            }

            static int MinimumDecimalExponent()
            {                                               // minimum decimal exponent
                return DBL_MIN_10_EXP;
            }

            static int MinimumExponent()
            {                                               // minimum binary exponent
                return DBL_MIN_EXP;
            }

            static Real LnMinimum()
            {                                               // natural log of minimum
                return (Real)log(Minimum());
            }

            static int Radix()
            {                                               // exponent radix
                return FLT_RADIX;
            }

            static int Rounds()
            {                                               // addition rounding (1 = does round)
                return FLT_ROUNDS;
            }

    };
#endif                                                  // USING_DOUBLE

#else                                                   // if there is no float.h

#ifdef OPT_COMPATIBLE
#define FLT_MAX MAXFLOAT
#endif

#ifdef USING_FLOAT

/// Floating point precision (type float).
    class FloatingPointPrecision
    {
        public:

            static Real Epsilon()
                { return pow(2.0,(int)(1-FSIGNIF)); }
// smallest number such that 1+Eps!=Eps

            static Real Maximum()
            {                                               // maximum value
                return MAXFLOAT;
            }

            static Real LnMaximum()
            {                                               // natural log of maximum
                return (Real)log(Maximum());
            }

            static Real Minimum()
            {                                               // minimum positive value
                return MINFLOAT;
            }

            static Real LnMinimum()
            {                                               // natural log of minimum
                return (Real)log(Minimum());
            }

    };
#endif                                                  // USING_FLOAT

#ifdef USING_DOUBLE

/// Floating point precision (type double).
    class FloatingPointPrecision
    {
        public:

            static Real Epsilon()
                { return pow(2.0,(int)(1-DSIGNIF)); }
// smallest number such that 1+Eps!=Eps

            static Real Maximum()
            {                                               // maximum value
                return MAXDOUBLE;
            }

            static Real LnMaximum()
            {                                               // natural log of maximum
                return LN_MAXDOUBLE;
            }

            static Real Minimum()
                { return MINDOUBLE; }

            static Real LnMinimum()
            {                                               // natural log of minimum
                return LN_MINDOUBLE;
            }
    };
#endif                                                  // USING_DOUBLE
#endif                                                  // SystemV
#endif                                                  // _STANDARD_

#ifdef use_namespace
}
#endif                                                      // use_namespace
#endif                                                      // PRECISION_LIB

///@}
