/******************************************************************************

 ENIGMA
 Copyright (C) 2008-2013 Enigma Strike Force

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/

#ifndef ERT_MATH_HPP_
#define ERT_MATH_HPP_

#include "ert/variant.hpp"

namespace ert {
  double math_set_epsilon(double);

  // Random Functions
  variant_t choose(const varargs_t&);
  double random(double);
  double random_range(double, double);
  double irandom(double);
  double irandom_range(double, double);
  double random_set_seed(double);
  double random_get_seed();
  double randomize();

  // Trigonometric Functions
  double arccos(double);
  double arcsin(double);
  double arctan(double);
  double arctan2(double, double);
  double sin(double);
  double tan(double);
  double cos(double);
  double degtorad(double);
  double radtodeg(double);
  double lengthdir_x(double, double);
  double lengthdir_y(double, double);

  // Rounding Functions
  double round(double);
  double floor(double);
  double frac(double);
  double abs(double);
  double sign(double);
  double ceil(double);
  variant_t max(const varargs_t&);
  variant_t mean(const varargs_t&);
  variant_t median(const varargs_t&);
  variant_t min(const varargs_t&);
  double lerp(double, double, double);
  double clamp(double, double, double);

  // Miscellaneous Functions
  double is_real(const variant_t&);
  double is_string(const variant_t&);
  double exp(double);
  double ln(double);
  double power(double, double);
  double sqr(double);
  double sqrt(double);
  double log2(double);
  double log10(double);
  double logn(double, double);
}

#endif // ERT_MATH_HPP_
