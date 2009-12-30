/* Inline math functions for i387.
   Copyright (C) 1995, 1996, 1997 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by John C. Bowman <bowman@math.ualberta.ca>, 1995.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <math.h>
#ifndef __GLIBC__
#ifndef _BITS_MATHINLINE_H
#define _BITS_MATHINLINE_H	1

#ifndef _Mldbl
#if __STDC__ - 0 || __GNUC__ - 0
#define _Mldbl(x) x##L
#else   /* Traditional C.  */
#define _Mldbl(x) x
#endif  /* Standard or GNU C.  */
#endif

#if defined __USE_ISOC9X && defined __GNUC__ && __GNUC__ >= 2
/* ISO C 9X defines some macros to perform unordered comparisons.  The
   ix87 FPU supports this with special opcodes and we should use them.
   These must not be inline functions since we have to be able to handle
   all floating-point types.  */
# define isgreater(x, y) \
     ({ int __result;							      \
	__asm__ ("fucompp; fnstsw; andb $0x45, %%ah; setz %%al;"	      \
		 "andl $0x01, %0"					      \
		 : "=a" (__result) : "u" (y), "t" (x) : "cc", "st", "st(1)"); \
	__result; })

# define isgreaterequal(x, y) \
     ({ int __result;							      \
	__asm__ ("fucompp; fnstsw; testb $0x05, %%ah; setz %%al;"	      \
		 "andl $0x01, %0"					      \
		 : "=a" (__result) : "u" (y), "t" (x) : "cc", "st", "st(1)"); \
	__result; })

# define isless(x, y) \
     ({ int __result;							      \
	__asm__ ("fucompp; fnstsw; xorb $0x01, %%ah; testb $0x45, %%ah;"      \
		 "setz %%al; andl $0x01, %0"				      \
		 : "=a" (__result) : "u" (y), "t" (x) : "cc", "st", "st(1)"); \
	__result; })

# define islessequal(x, y) \
     ({ int __result;							      \
	__asm__ ("fucompp; fnstsw; xorb $0x01, %%ah; testb $0x05, %%ah;"      \
		 "setz %%al; andl $0x01, %0"				      \
		 : "=a" (__result) : "u" (y), "t" (x) : "cc", "st", "st(1)"); \
	__result; })

# define islessgreater(x, y) \
     ({ int __result;							      \
	__asm__ ("fucompp; fnstsw; testb $0x44, %%ah; setz %%al;"	      \
		 "andl $0x01, %0"					      \
		 : "=a" (__result) : "u" (y), "t" (x) : "cc", "st", "st(1)"); \
	__result; })

# define isunordered(x, y) \
     ({ int __result;							      \
	__asm__ ("fucompp; fnstsw; sahf; setp %%al; andl $0x01, %0"	      \
		 : "=a" (__result) : "u" (y), "t" (x) : "cc", "st", "st(1)"); \
	__result; })
#endif

#ifdef	__GNUC__
	
#ifdef __cplusplus
# define __MATH_INLINE __inline
#else
# ifdef __GNUC_STDC_INLINE__ /* we want GNU extern inline behaviour */
#  define __MATH_INLINE extern __inline __attribute__((__gnu_inline__))
# else
#  define __MATH_INLINE extern __inline
# endif
#endif

/* A macro to define float, double, and long double versions of various
   math functions for the ix87 FPU.  FUNC is the function name (which will
   be suffixed with f and l for the float and long double version,
   respectively).  OP is the name of the FPU operation.  */

#if defined __USE_MISC || defined __USE_ISOC9X
# define __inline_mathop(func, op) \
  __inline_mathop_ (double, func, op)					      \
  __inline_mathop_ (float, __CONCAT(func,f), op)			      \
  __inline_mathop_ (long double, __CONCAT(func,l), op)
#else
# define __inline_mathop(func, op) \
  __inline_mathop_ (double, func, op)
#endif

#define __inline_mathop_(float_type, func, op) \
  __inline_mathop_decl_ (float_type, func, op, "0" (__x))


#if defined __USE_MISC || defined __USE_ISOC9X
# define __inline_mathop_decl(func, op, params...) \
  __inline_mathop_decl_ (double, func, op, params)			      \
  __inline_mathop_decl_ (float, __CONCAT(func,f), op, params)		      \
  __inline_mathop_decl_ (long double, __CONCAT(func,l), op, params)
#else
# define __inline_mathop_decl(func, op, params...) \
  __inline_mathop_decl_ (double, func, op, params)
#endif

#define __inline_mathop_decl_(float_type, func, op, params...) \
  __MATH_INLINE float_type func (float_type);				      \
  __MATH_INLINE float_type func (float_type __x)				      \
  {									      \
    register float_type __result;					      \
    __asm __volatile__ (op : "=t" (__result) : params);		      \
    return __result;							      \
  }


#if defined __USE_MISC || defined __USE_ISOC9X
# define __inline_mathcode(func, arg, code) \
  __inline_mathcode_ (double, func, arg, code)				      \
  __inline_mathcode_ (float, __CONCAT(func,f), arg, code)		      \
  __inline_mathcode_ (long double, __CONCAT(func,l), arg, code)
#else
# define __inline_mathcode(func, arg, code) \
  __inline_mathcode_ (double, func, arg, code)
#endif

#define __inline_mathcode_(float_type, func, arg, code) \
  __MATH_INLINE float_type func (float_type);				      \
  __MATH_INLINE float_type func (float_type arg)			      \
  {									      \
    code;								      \
  }


#if defined __USE_MISC || defined __USE_ISOC9X
# define __inline_mathcode2(func, arg1, arg2, code)			      \
  __inline_mathcode2_ (double, func, arg1, arg2, code)			      \
  __inline_mathcode2_ (float, __CONCAT(func,f), arg1, arg2, code)	      \
  __inline_mathcode2_ (long double, __CONCAT(func,l), arg1, arg2, code)
#else
# define __inline_mathcode2(func, arg1, arg2, code) \
  __inline_mathcode2_ (double, func, arg1, arg2, code)
#endif

#define __inline_mathcode2_(float_type, func, arg1, arg2, code) \
  __MATH_INLINE float_type func (float_type, float_type);		      \
  __MATH_INLINE float_type func (float_type arg1, float_type arg2)	      \
  {									      \
    code;								      \
  }

/* Miscellaneous functions */
	
__inline_mathcode (__sgn, __x,
  return __x == 0.0 ? 0.0 : (__x > 0.0 ? 1.0 : -1.0))

__inline_mathcode (__pow2, __x,
  register long double __value;						      
  register long double __exponent;
  long int __p = (long int) __x;
  if (__x == (long double) __p)
    {
    __asm __volatile__
      ("fscale" 
       : "=t" (__value) : "0" (1.0), "u" (__x));
      return __value;
    }
  __asm __volatile__
    ("fldl	%%st(0)\n\t"
     "frndint			# int(x)\n\t"
     "fxch\n\t"
     "fsub	%%st(1)		# fract(x)\n\t"
     "f2xm1			# 2^(fract(x)) - 1\n\t"
     : "=t" (__value), "=u" (__exponent) : "0" (__x));
  __value += 1.0;
  __asm __volatile__
    ("fscale"
     : "=t" (__value) : "0" (__value), "u" (__exponent));
  return __value)

#define __sincos_code \
  register long double __cosr;						      \
  register long double __sinr;						      \
  __asm __volatile__							      \
    ("fsincos\n\t"							      \
     "fnstsw	%%ax\n\t"						      \
     "testl	$0x400, %%eax\n\t"					      \
     "jz	1f\n\t"							      \
     "fldpi\n\t"							      \
     "fadd	%%st(0)\n\t"						      \
     "fxch	%%st(1)\n\t"						      \
     "2: fprem1\n\t"							      \
     "fnstsw	%%ax\n\t"						      \
     "testl	$0x400, %%eax\n\t"					      \
     "jnz	2b\n\t"							      \
     "fstp	%%st(1)\n\t"						      \
     "fsincos\n\t"							      \
     "1:"								      \
     : "=t" (__cosr), "=u" (__sinr) : "0" (__x));			      \
  *__sinx = __sinr;							      \
  *__cosx = __cosr

__MATH_INLINE void __sincos (double __x, double *__sinx, double *__cosx);
__MATH_INLINE void
__sincos (double __x, double *__sinx, double *__cosx)
{
  __sincos_code;
}

__MATH_INLINE void __sincosf (float __x, float *__sinx, float *__cosx);
__MATH_INLINE void
__sincosf (float __x, float *__sinx, float *__cosx)
{
  __sincos_code;
}

__MATH_INLINE void __sincosl (long double __x, long double *__sinx,
			      long double *__cosx);
__MATH_INLINE void
__sincosl (long double __x, long double *__sinx, long double *__cosx)
{
  __sincos_code;
}


#if defined __GNUC__ && \
    (__GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ <= 7))
/* gcc 2.7.2 and 2.7.2.1 have problems with inlining `long double'
   functions so we disable this now.  */
# undef __NO_MATH_INLINES
//# define __NO_MATH_INLINES
#endif

#if !defined __NO_MATH_INLINES && defined __OPTIMIZE__


#define __expm1_code \
  register long double __value;						      \
  register long double __exponent;					      \
  register long double __temp;						      \
  __asm __volatile__							      \
    ("fldl2e			# e^x - 1 = 2^(x * log2(e)) - 1\n\t"	      \
     "fmul	%%st(1)		# x * log2(e)\n\t"			      \
     "fstl	%%st(1)\n\t"						      \
     "frndint			# int(x * log2(e))\n\t"			      \
     "fxch\n\t"								      \
     "fsub	%%st(1)		# fract(x * log2(e))\n\t"		      \
     "f2xm1			# 2^(fract(x * log2(e))) - 1\n\t"	      \
     "fscale			# 2^(x * log2(e)) - 2^(int(x * log2(e)))\n\t" \
     : "=t" (__value), "=u" (__exponent) : "0" (__x));			      \
  __asm __volatile__							      \
    ("fscale			# 2^int(x * log2(e))\n\t"		      \
     : "=t" (__temp) : "0" (1.0), "u" (__exponent));			      \
  __temp -= 1.0;							      \
  return __temp + __value
__inline_mathcode_ (long double, __expm1l, __x, __expm1_code)


#define __exp_code \
  register long double __value;						      \
  register long double __exponent;					      \
  __asm __volatile__							      \
    ("fldl2e			# e^x = 2^(x * log2(e))\n\t"		      \
     "fmul	%%st(1)		# x * log2(e)\n\t"			      \
     "fstl	%%st(1)\n\t"						      \
     "frndint			# int(x * log2(e))\n\t"			      \
     "fxch\n\t"								      \
     "fsub	%%st(1)		# fract(x * log2(e))\n\t"		      \
     "f2xm1			# 2^(fract(x * log2(e))) - 1\n\t"	      \
     : "=t" (__value), "=u" (__exponent) : "0" (__x));			      \
  __value += 1.0;							      \
  __asm __volatile__							      \
    ("fscale"								      \
     : "=t" (__value) : "0" (__value), "u" (__exponent));		      \
  return __value
__inline_mathcode (exp, __x, __exp_code)
__inline_mathcode_ (long double, __expl, __x, __exp_code)
	
__inline_mathcode (tan, __x,
  register long double __value;
  register long double __value2 __attribute__ ((unused));
  __asm __volatile__
    ("fptan"
     : "=t" (__value2), "=u" (__value) : "0" (__x));
  return __value)


#define __atan2_code \
  register long double __value;						      \
  __asm __volatile__							      \
    ("fpatan\n\t"							      \
     : "=t" (__value) : "0" (__x), "u" (__y) : "st(1)");				      \
  return __value
__inline_mathcode2 (atan2, __y, __x, __atan2_code)
__inline_mathcode2_ (long double, __atan2l, __y, __x, __atan2_code)


__inline_mathcode2 (fmod, __x, __y,
  register long double __value;
  __asm __volatile__
    ("1:	fprem\n\t"
     "fnstsw	%%ax\n\t"
     "sahf\n\t"
     "jp	1b"
     : "=t" (__value) : "0" (__x), "u" (__y) : "ax", "cc");
  return __value)


__inline_mathcode2 (pow, __x, __y,
  register long double __value;
  register long double __exponent;
  long int __p = (long int) __y;
  if (__x == 0.0 && __y > 0.0)
    return 0.0;
  if (__y == (double) __p)
    {
      long double __r = 1.0;
      if (__p == 0)
	return 1.0;
      if (__p < 0)
	{
	  __p = -__p;
	  __x = 1.0 / __x;
	}
      while (1)
	{
	  if (__p & 1)
	    __r *= __x;
	  __p >>= 1;
	  if (__p == 0)
	    return __r;
	  __x *= __x;
	}
      /* NOTREACHED */
    }
  __asm __volatile__
    ("fyl2x" : "=t" (__value) : "0" (__x), "u" (1.0) : "st(1)");
  __asm __volatile__
    ("fmul	%%st(1)		# y * log2(x)\n\t"
     "fst	%%st(1)\n\t"
     "frndint			# int(y * log2(x))\n\t"
     "fxch\n\t"
     "fsub	%%st(1)		# fract(y * log2(x))\n\t"
     "f2xm1			# 2^(fract(y * log2(x))) - 1\n\t"
     : "=t" (__value), "=u" (__exponent) : "0" (__y), "1" (__value));
  __value += 1.0;
  __asm __volatile__
    ("fscale"
     : "=t" (__value) : "0" (__value), "u" (__exponent));
  return __value)


__inline_mathop (sqrt, "fsqrt")
__inline_mathop_ (long double, __sqrtl, "fsqrt")

#if defined __GNUC__ && (__GNUC__ > 2 || __GNUC__ == 2 && __GNUC_MINOR__ >= 8)
__inline_mathcode_ (double, fabs, __x, return __builtin_fabs (__x))
__inline_mathcode_ (float, fabsf, __x, return __builtin_fabsf (__x))
__inline_mathcode_ (long double, fabsl, __x, return __builtin_fabsl (__x))
__inline_mathcode_ (long double, __fabsl, __x, return __builtin_fabsl (__x))
#else
__inline_mathop (fabs, "fabs")
__inline_mathop_ (long double, __fabsl, "fabs")
#endif

/* The argument range of this inline version is reduced.  */
__inline_mathop (sin, "fsin")
/* The argument range of this inline version is reduced.  */
__inline_mathop (cos, "fcos")

__inline_mathop (atan, "fld1; fpatan")
__inline_mathop (log, "fldln2; fxch; fyl2x")
__inline_mathop (log10, "fldlg2; fxch; fyl2x")

__inline_mathcode (asin, __x, return __atan2l (__x, __sqrtl (1.0 - __x * __x)))
__inline_mathcode (acos, __x, return __atan2l (__sqrtl (1.0 - __x * __x), __x))

__inline_mathcode_ (long double, __sgn1l, __x, return __x >= 0.0 ? 1.0 : -1.0)
	
/* The argument range of the inline version of sinhl is slightly reduced.  */
__inline_mathcode (sinh, __x,
  register long double __exm1 = __expm1l (__fabsl (__x));
  return 0.5 * (__exm1 / (__exm1 + 1.0) + __exm1) * __sgn1l (__x))

__inline_mathcode (cosh, __x,
  register long double __ex = __expl (__x);
  return 0.5 * (__ex + 1.0 / __ex))

__inline_mathcode (tanh, __x,
  register long double __exm1 = __expm1l (-__fabsl (__x + __x));
  return __exm1 / (__exm1 + 2.0) * __sgn1l (-__x))


__inline_mathcode (floor, __x,
  register long double __value;
  __volatile unsigned short int __cw;
  __volatile unsigned short int __cwtmp;
  __asm __volatile ("fnstcw %0" : "=m" (__cw));
  __cwtmp = (__cw & 0xf3ff) | 0x0400; /* rounding down */
  __asm __volatile ("fldcw %0" : : "m" (__cwtmp));
  __asm __volatile ("frndint" : "=t" (__value) : "0" (__x));
  __asm __volatile ("fldcw %0" : : "m" (__cw));
  return __value)

__inline_mathcode (ceil, __x,
  register long double __value;
  __volatile unsigned short int __cw;
  __volatile unsigned short int __cwtmp;
  __asm __volatile ("fnstcw %0" : "=m" (__cw));
  __cwtmp = (__cw & 0xf3ff) | 0x0800; /* rounding up */
  __asm __volatile ("fldcw %0" : : "m" (__cwtmp));
  __asm __volatile ("frndint" : "=t" (__value) : "0" (__x));
  __asm __volatile ("fldcw %0" : : "m" (__cw));
  return __value)

#define __ldexp_code \
  register long double __value; \
  __asm __volatile__ \
    ("fscale" \
     : "=t" (__value) : "0" (__x), "u" ((long double) __y)); \
  return __value
	 
__MATH_INLINE double ldexp (double __x, int __y);
__MATH_INLINE double
ldexp (double __x, int __y)
{
	__ldexp_code;
}

/* Optimized versions for some non-standardized functions.  */
#if defined __USE_ISOC9X || defined __USE_MISC

__inline_mathop(log2, "fld1; fxch; fyl2x")

__inline_mathcode (expm1, __x, __expm1_code)

/* We cannot rely on M_SQRT being defined.  So we do it for ourself
   here.  */
# define __M_SQRT2	_Mldbl(1.41421356237309504880)	/* sqrt(2) */
	
__inline_mathcode (log1p, __x,
  register long double __value;
  if (__fabsl (__x) >= 1.0 - 0.5 * __M_SQRT2)
    __value = logl (1.0 + __x);
  else
    __asm __volatile__
      ("fldln2\n\t"
       "fxch\n\t"
       "fyl2xp1"
       : "=t" (__value) : "0" (__x));
  return __value)


/* The argument range of the inline version of asinhl is slightly reduced.  */
__inline_mathcode (asinh, __x,
  register long double  __y = __fabsl (__x);
  return log1pl (__y * __y / (__sqrtl (__y * __y + 1.0) + 1.0) + __y)
	  * __sgn1l (__x))

__inline_mathcode (acosh, __x,
  return logl (__x + __sqrtl (__x - 1.0) * __sqrtl (__x + 1.0)))

__inline_mathcode (atanh, __x,
  register long double __y = __fabsl (__x);
  return (-0.5 * log1pl (-(__y + __y) / (1.0 + __y)) *
	  __sgn1l (__x)))


/* The argument range of the inline version of hypotl is slightly reduced.  */
__inline_mathcode2 (hypot, __x, __y, return __sqrtl (__x * __x + __y * __y))

__inline_mathcode(logb, __x,
  register long double __value;
  register long double __junk;
  __asm __volatile__
    ("fxtract\n\t"
     : "=t" (__junk), "=u" (__value) : "0" (__x));
  return __value)

__MATH_INLINE float ldexpf (float __x, int __y);
__MATH_INLINE float
ldexpf (float __x, int __y)
{
	__ldexp_code;
}

__MATH_INLINE long double ldexpl (long double __x, int __y);
__MATH_INLINE long double
ldexpl (long double __x, int __y)
{
	__ldexp_code;
}

#endif

#ifdef __USE_MISC

__inline_mathcode2(drem, __x, __y,
				   register long double __value;
				   __asm __volatile__
				   ("1:	fprem1\n\t"
					"fstsw	%%ax\n\t"
					"sahf\n\t"
					"jp	1b"
					: "=t" (__value) : "0" (__x), "u" (__y) : "ax", "cc");
				   return __value)

/* This function is used in the `isfinite' macro.  */
#undef __finite
__MATH_INLINE int __finite (double __x);
__MATH_INLINE int
__finite (double __x)
{
  register int __result;
  __asm__ __volatile__
    ("orl	$0x800fffff, %0\n\t"
     "incl	%0\n\t"
     "shrl	$31, %0"
     : "=q" (__result) : "0" (((int *) &__x)[1]));
  return __result;
}

/* Miscellaneous functions */

__inline_mathcode (__coshm1, __x,
  register long double __exm1 = __expm1l (__fabsl (__x));
  return 0.5 * (__exm1 / (__exm1 + 1.0)) * __exm1)

__inline_mathcode (__acosh1p, __x, \
  return log1pl (__x + __sqrtl (__x) * __sqrtl (__x + 2.0)))


#endif /* __USE_MISC  */

/* Undefine some of the large macros which are not used anymore.  */
#undef __expm1_code
#undef __exp_code
#undef __atan2_code

#endif /* __NO_MATH_INLINES  */
#undef __sincos_code
#endif /* __GNUC__  */

#endif /* _BITS_MATHINLINE_H  */
#endif
