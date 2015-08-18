/*

  This software is governed by the CeCILL  license under French law and
  abiding by the rules of distribution of free software.  You can  use,
  modify and/ or redistribute the software under the terms of the CeCILL
  license as circulated by CEA, CNRS and INRIA at the following URL
  "http://www.cecill.info".

  As a counterpart to the access to the source code and  rights to copy,
  modify and redistribute granted by the license, users are provided only
  with a limited warranty  and the software's author,  the holder of the
  economic rights,  and the successive licensors  have only  limited
  liability.

  In this respect, the user's attention is drawn to the risks associated
  with loading,  using,  modifying and/or developing or reproducing the
  software by the user in light of its specific status of free software,
  that may mean  that it is complicated to manipulate,  and  that  also
  therefore means  that it is reserved for developers  and  experienced
  professionals having in-depth computer knowledge. Users are therefore
  encouraged to load and test the software's suitability as regards their
  requirements in conditions enabling the security of their systems and/or
  data to be ensured and,  more generally, to use and operate it in the
  same conditions as regards security.

  The fact that you are presently reading this means that you have had
  knowledge of the CeCILL license and that you accept its terms.

  Copyright (c) 2010-2012
    Alexandre Becoulet <alexandre.becoulet@telecom-partistech.fr>,
  Copyright (c) 2010-2012 Institut Telecom / Telecom ParisTech

*/

#ifndef SOCLIB_FIXED_POINT_HH_
#define SOCLIB_FIXED_POINT_HH_

#include <stdint.h>
#include <ostream>
#include <cstdlib>

#include "static_assert.h"

/**
   @short Fixed point values template classes
   @file
 */


/**
   @short Real fixed point value template class
 */
template <unsigned int_len, unsigned fract_len>
class fxp_real
{
  typedef int64_t T;
  template <unsigned, unsigned> friend class fxp_real;

  static const unsigned int sh_mask = sizeof(T) * 8 - 1;

  /** @hidden error if int_len + fract_len > 64 */
  typedef char _assert_t[64 - int_len - fract_len];

  static inline T mask(T r)
  {
#if 1
    static const size_t padsize = (64 - int_len - fract_len);
    return (r << padsize) >> padsize;
#else
    return r;
#endif
  }

  fxp_real(T r)
    : _r(mask(r))
  {
  }

public:

  fxp_real()
  {
  }

  /** Create a fixed point value from raw integer bits value */
  static fxp_real from_int(T i)
  {
    return fxp_real(i);
  }

  /** Return raw integer bits value */
  T to_int() const
  {
    return _r;
  }

  /**
   * This function assign a fixed point value and shift as needed
   * depending on fractional parts width.
   */
  template <unsigned i_len, unsigned f_len>
  fxp_real operator=(const fxp_real<i_len, f_len> &r)
  {
    _r = f_len > fract_len ? mask(r._r >> ((f_len - fract_len) & sh_mask))
                           : mask(r._r << ((fract_len - f_len) & sh_mask));

    // some MSB are droped, must use saturated, rescale or truncate
    soclib_static_assert(i_len <= int_len);

    return *this;
  }

  template <unsigned i_len, unsigned f_len>
  fxp_real(const fxp_real<i_len, f_len> &r)
  {
    _r = f_len > fract_len ? mask(r._r >> ((f_len - fract_len) & sh_mask))
                           : mask(r._r << ((fract_len - f_len) & sh_mask));

    // some MSB are droped, must use saturated, rescale or truncate
    soclib_static_assert(i_len <= int_len);
  }

  /**
   * This function converts to given format droping some MSB if needed.
   */
  template <unsigned i_len, unsigned f_len>
  fxp_real<i_len, f_len> truncate() const
  {
    return fract_len > f_len ? mask(_r >> ((fract_len - f_len) & sh_mask))
                             : mask(_r << ((f_len - fract_len) & sh_mask));
  }

  /**
   * This function converts to given format with saturation.
   */
  template <unsigned i_len, unsigned f_len>
  fxp_real<i_len, f_len> saturate() const
  {
    static const T mask = ~((1 << (i_len - 1 + f_len)) - 1);
    T s = fract_len > f_len ? _r >> ((fract_len - f_len) & sh_mask)
                            : _r << ((f_len - fract_len) & sh_mask);
    if (((s & mask) == 0) || ((s & mask) == mask))
      return fxp_real<i_len, f_len>(s);

    if (s <= 0)
      return fxp_real<i_len, f_len>(mask);
    else
      return fxp_real<i_len, f_len>(~mask);
  }

  /**
   * This function constructs a fixed point value from @tt{double}.
   */
  explicit fxp_real(double r)
  {
    const T s = (T)(r * (1ULL << fract_len));
    static const T mask = ~((1 << (int_len - 1 + fract_len)) - 1);

    if (((s & mask) == 0) || ((s & mask) == mask))
      _r = s;
    else
      _r = (s <= 0) ? _r = mask : ~mask;
  }

#define FPMAX(a, b) (a >= b ? a : b)

#define FXP_ADDOP(typeret, typearg, op, expr)			\
  template <unsigned i_len, unsigned f_len>			\
  typeret<FPMAX(i_len,int_len) + 1, FPMAX(f_len,fract_len)>	\
  op(const typearg<i_len, f_len> &r) const			\
  {								\
    const unsigned imax = FPMAX(i_len,int_len);			\
    const unsigned fmax = FPMAX(f_len,fract_len);		\
    typedef typeret<imax + 1, fmax> ret_t;			\
    return expr;						\
  }

#define FXP_MULOP(typeret, typearg, op, expr)			\
  template <unsigned i_len, unsigned f_len>			\
  typeret<i_len + int_len, f_len + fract_len>			\
  operator*(const typearg<i_len, f_len> &r) const		\
  {								\
    typedef typeret<i_len + int_len, f_len + fract_len> ret_t;	\
    return expr;						\
  }

  /**
   * This function adds two real values.
   *
   * Format: Q(a.b) + Q(c.d) => Q(max(a,c)+1 . max(b,d))
   */
  FXP_ADDOP(fxp_real, fxp_real, operator+, ret_t((_r << (fmax - fract_len)) + (r._r << (fmax - f_len))));

  /**
   * This function subtracts two real values.
   *
   * Format: Q(a.b) - Q(c.d) => Q(max(a,c)+1 . max(b,d))
   */
  FXP_ADDOP(fxp_real, fxp_real, operator-, ret_t((_r << (fmax - fract_len)) - (r._r << (fmax - f_len))));

  /**
   * This function multiplies two real values.
   *
   * Format: Q(a.b) * Q(c.d) => Q(a+c . b+d)
   */
  FXP_MULOP(fxp_real, fxp_real, operator*, ret_t(_r * r._r));

  /**
   * This function negate a real value.
   *
   * Format: Q(a.b) * Q(c.d) => Q(a+c . b+d)
   */
  fxp_real<int_len + 1, fract_len> operator-() const
  {
    return fxp_real<int_len + 1, fract_len>(-_r);
  }

  fxp_real operator+() const
  {
    return fxp_real(_r);
  }

  fxp_real<int_len + 1, fract_len> abs() const
  {
    return fxp_real<int_len + 1, fract_len>(std::abs(_r));
  }

  /**
   * This function multiply or divide the real by a power of two by shifting the point.
   *
   * Format: Q(a.b) => Q(a+shift_left . b-shift_left)
   */
  template <int shift_left>
  fxp_real<int_len + shift_left, fract_len - shift_left> rescale() const
  {
    soclib_static_assert((int)int_len + shift_left >= 0 &&
		      (int)fract_len - shift_left >= 0);

    return fxp_real<int_len + shift_left, fract_len - shift_left>(_r);
  }

  fxp_real operator>>(unsigned int n) const
  {
    return fxp_real(_r >> n);
  }

  fxp_real operator<<(unsigned int n) const
  {
    return fxp_real(_r << n);
  }

#if 0
  fxp_real operator/(const fxp_real& r) const
  {
    return fxp_real( (T)((_r << fract_len) / r._r) );
  }
#endif

  double to_double() const
  {
    return (double)_r / (1ULL << fract_len);
  }

  bool operator>(const fxp_real &r) const
  {
    return _r > r._r;
  }

  bool operator>=(const fxp_real &r) const
  {
    return _r >= r._r;
  }

  bool operator<(const fxp_real &r) const
  {
    return _r < r._r;
  }

  bool operator<=(const fxp_real &r) const
  {
    return _r <= r._r;
  }

  bool operator==(const fxp_real &r) const
  {
    return _r == r._r;
  }

private:
  T _r;
};



/**
   @short Complex fixed point value template class
 */
template <unsigned int_len, unsigned fract_len>
class fxp_complex
{
  template <unsigned, unsigned> friend class fxp_complex;

  typedef int64_t T;
  typedef fxp_real<int_len, fract_len> R;

public:

  /** Create a complex fixed point value from 2 raw integer bits values */
#if 0
  static fxp_complex from_int(T i, T r)
  {
    return fxp_complex(R::from_int(i), R::from_int(r));
  }
#endif

  /** Create a complex fixed point value from single raw integer bits value.
   Real and imaginary parts are specified with @tt im_shift and @tt r_shift .*/
  template <unsigned r_shift, unsigned im_shift>
  static fxp_complex from_int(T i)
  {
    static const T mask = (1ULL << (int_len + fract_len)) - 1;
    return fxp_complex(R::from_int((i >> r_shift) & mask),
		       R::from_int((i >> im_shift) & mask));
  }

  /** Return raw integer bits value */
  template <unsigned r_shift, unsigned im_shift>
  T to_int() const
  {
    static const T mask = (1ULL << (int_len + fract_len)) - 1;
    return ((_r.to_int() & mask) << r_shift) | ((_i.to_int() & mask) << im_shift);
  }

  fxp_complex()
  {
  }

  explicit fxp_complex(double r, double i)
    : _r(r), _i(i)
  {
  }

  fxp_complex(R r, R i)
    : _r(r), _i(i)
  {
  }

  template <unsigned i_len, unsigned f_len>
  fxp_complex(const fxp_complex<i_len, f_len> &c)
    : _r(c._r), _i(c._i)
  {
  }

  /**
   * This function converts to given format with saturation.
   */
  template <unsigned i_len, unsigned f_len>
  fxp_complex<i_len, f_len> saturate() const
  {
    return fxp_complex<i_len, f_len>(_r.saturate<i_len, f_len>(),
				     _i.saturate<i_len, f_len>());
  }

  /**
   * This function converts to given format and may truncate MSB.
   */
  template <unsigned i_len, unsigned f_len>
  fxp_complex<i_len, f_len> truncate() const
  {
    return fxp_complex<i_len, f_len>(_r.truncate<i_len, f_len>(),
				     _i.truncate<i_len, f_len>());
  }

 /**
   * This function adds a complex with an real.
   *
   * Format: Q(a.b) + Q(c.d) => Q(max(a,c)+1 . max(b,d))
   */
  FXP_ADDOP(fxp_complex, fxp_real, operator+, ret_t(_r + r, _i));

  /**
   * This function subtracts a real from a complex.
   *
   * Format: Q(a.b) - Q(c.d) => Q(max(a,c)+1 . max(b,d))
   */
  FXP_ADDOP(fxp_complex, fxp_real, operator-, ret_t(_r - r, _i));

  /**
   * This function multiplies a complex with an real.
   *
   * Format: Q(a.b) * Q(c.d) => Q(a+c . b+d)
   */
  FXP_MULOP(fxp_complex, fxp_real, operator*, ret_t(_r * r, _i * r));

  /**
   * This function negate a complex value.
   *
   * Format: Q(a.b) * Q(c.d) => Q(a+c . b+d)
   */
  fxp_complex<int_len + 1, fract_len> operator-() const
  {
    return fxp_complex<int_len + 1, fract_len>(-_r, -_i);
  }

  fxp_complex operator+() const
  {
    return fxp_complex(_r, _i);
  }

  fxp_complex<int_len + 1, fract_len> conjugate() const
  {
    return fxp_complex<int_len + 1, fract_len>(_r, -_i);
  }

  fxp_real<int_len * 2, fract_len * 2> modulus() const
  { // fxp_real<int_len * 2, fract_len * 2>
    return (_r * _r + _i * _i).truncate<int_len * 2, fract_len * 2>();
  }

  /**
   * This function multiply or divide the complex by a power of two by shifting the point.
   *
   * Format: Q(a.b) => Q(a+shift_left . b-shift_left)
   */
  template <int shift_left>
  fxp_complex<int_len + shift_left, fract_len - shift_left> rescale() const
  {
    return fxp_complex<int_len + shift_left, fract_len - shift_left>(_r.rescale<shift_left>(),
								     _i.rescale<shift_left>());
  }

  fxp_complex operator>>(unsigned int n) const
  {
    return fxp_complex(_r >> n, _i >> n);
  }

  fxp_complex operator<<(unsigned int n) const
  {
    return fxp_complex(_r << n, _i << n);
  }

#if 0
  fxp_complex operator/(R r) const
  {
    return fxp_complex(_r / r, _i / r);
  }
#endif

  /**
   * This function adds two complex.
   *
   * Format: Q(a.b) + Q(c.d) => Q(max(a,c)+1 . max(b,d))
   */
  FXP_ADDOP(fxp_complex, fxp_complex, operator+, ret_t(_r + r._r, _i + r._i));

  /**
   * This function subtracts two complex.
   *
   * Format: Q(a.b) - Q(c.d) => Q(max(a,c)+1 . max(b,d))
   */
  FXP_ADDOP(fxp_complex, fxp_complex, operator-, ret_t(_r - r._r, _i - r._i));

  /**
   * This function multiplies two complex.
   *
   * Format: Q(a.b) * Q(c.d) => Q(a+c+1 . b+d)
   */
  template <unsigned i_len, unsigned f_len>
  fxp_complex<i_len + int_len + 1, f_len + fract_len>
  operator*(const fxp_complex<i_len, f_len> &e) const
  {
    typedef fxp_complex<i_len + int_len + 1, f_len + fract_len> ret_t;
    const R &a = _r;
    const R &b = _i;
    const typename fxp_complex<i_len, f_len>::R &c = e._r;
    const typename fxp_complex<i_len, f_len>::R &d = e._i;
    return ret_t(a*c - b*d, a*d + c*b);
  }

#if 0
  fxp_complex operator/(const fxp_complex& e) const
  {
    const R &a = _r;
    const R &b = _i;
    const R &c = e._r;
    const R &d = e._i;
    R q = c*c + d*d;
    return fxp_complex((a*c + b*d) / q, (b*c - a*d) / q);
  }
#endif

  /**
   * This function returns real part.
   */
  const R & real() const
  {
    return _r;
  }

  /**
   * This function returns real part.
   */
  R & real()
  {
    return _r;
  }

  /**
   * This function returns imaginary part.
   */
  const R & imag() const
  {
    return _i;
  }

  /**
   * This function returns imaginary part.
   */
  R & imag()
  {
    return _i;
  }

private:
  R _r, _i;
};

template <unsigned int_len, unsigned fract_len>
inline std::ostream & operator<<(std::ostream &o, const fxp_real<int_len, fract_len> &r)
{
  o << "[" << int_len << "." << fract_len << "]" << r.to_double();
  return o;
}

template <unsigned int_len, unsigned fract_len>
inline std::ostream & operator<<(std::ostream &o, const fxp_complex<int_len, fract_len> &e)
{
  o << "(" << e.real() << ", " << e.imag() << ")";
  return o;
}

#endif

