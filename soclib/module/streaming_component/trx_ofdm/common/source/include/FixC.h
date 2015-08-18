/* -*- c++ -*-
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 *
 * Copyright (c) CEA-LETI, MINATEC, 2008
 *
 * Authors : Julien BARLETTA (CEA-LETI)
 * 
 */
/* ###############################################################################################################
##                      :
## File name            : FixC.h (header file for 'FixC.cpp')
## Language             : C++
## Short description    : This file contains the declaration of the fixed-point class and the code of the
##                      : inline operators and functions.
##                      :
## History              : 01/09/03 created  by Julien BARLETTA (CEA-LETI)
##                      : 10/12/03 modified by Julien BARLETTA (CEA-LETI). The following bugs has been corrected:
##                      :          'Display()' method: the sign of the floating-point value is now displayed.
##                      :          'Display()' method: some values were not correctly displayed.
##                      :          Shift operators '>>' and '<<' did not work in some cases.
##                      :          A second 'InitFixC2d' function allows to initialize two-dimensions arrays
##                      :          dynamically allocated (the type of the first argument is 'FixC**').
##                      :          The '=' operators can now display a warning message when an overflow occurs.
##                      : 27/01/04 modified by Julien BARLETTA (CEA-LETI). Correction of the following bug:
##                      :          Assignment of 2 FixC variables: in some cases the rounding is wrong.
##                      : 18/02/04 modified by Julien BARLETTA (CEA-LETI).
##                      :          The 'GetMantissa()' method has been added.
##                      : 30/03/04 modified by Julien BARLETTA (CEA-LETI).
##                      :          The 'rounding' test is done before the 'underflow' one.
##                      : 04/06/04 modified by Julien BARLETTA (CEA-LETI).
##                      :          Constructors #2, #4, #6 and #8 and '=" operators: modification of the test
##                      :          used to detect an overflow. The test takes in account the precision.
##                      : 11/06/04 modified by Julien BARLETTA (CEA-LETI).
##                      :          Modification of the rounding for the negative numbers. 
##                      :          Rounding and saturation (overflow, underflow) are now done in this order.
##                      :
## Detailed description : This class is derived from the code produced by Seehyun Kim (Seoul National University)
##                      : The operators defined in this file are:
##                      :       - unary operators (+,-)
##                      :       - shift operators (<<,>>,<<=,>>=)
##                      :       - arithmetic-assignment operators (+=,-=,*=,/=)
##                      :       - comparison operators (==, !=, <, <=, >, >=)
##                      :
##                      : MATRICE: don't put in this file 'ifdef' or 'define' used to configure a particular
##                      : implementation to avoid a complete compilation of the simulator.
##                      :
## ############################################################################################################ */

#ifndef FIXC_H
#define FIXC_H

// ===============
// INCLUDE SECTION
// ===============

#include <iostream>
#include <sstream>		// string streams used by the 'Display()' method
#include <iomanip>		// needed by use of 'setprecision()' in 'Display()' method
#include <string>
#include <stdio.h>		// needed by use of 'printf()' and 'sprintf' in 'Display()' method (see the method source code)
#include <math.h>
#include <limits.h>		// CHAR_BIT definition

// ==================
// DEFINITION SECTION
// ==================

#define MANT_TYPE	long long										// mantissa type (use to initialize MAX_WL)
//#define MANT_TYPE	long											// mantissa type (use to initialize MAX_WL)
#define MAX_WL		( (short) ( sizeof(MANT_TYPE) * CHAR_BIT ) )	// word length: maximum number of bits

#define DEFAULT_WL	MAX_WL					// word length: default number of bits
#define DEFAULT_IWL	0						// integer word length: default number of bits
#define DEFAULT_FMT	"sr"					// default overflow (saturation) and quantization (rounding) modes
#define FIXC_ERROR	( (int) -1 )			// value passed to 'exit' function
#define EPSILON_VAL	( 1.0e-6 )				// used by the 'pow(double, FixC&)' method
#define MINUS_ONE	( (MANT_TYPE) -1 )
#define PLUS_ONE	( (MANT_TYPE) 1 )

// -----
// Macros
// -----

#define LOG2(X)		( log(X) / log(2.0) )
#define EXP2(X)		( exp(log(2.0) * (X)) )

// =====
// Declaration section
// =====

using namespace std;

// ###########################################
// Class for fixed-point format representation
// ###########################################

class FixC
{
private:

	string    Name;			// instance name
	MANT_TYPE Mantissa;		// mantissa
	short     Iwl;			// integer word-length
	short     Wl;			// total word-length = integer word-length + fractional word-length + 1 (sign bit)
	char      Saturation;	//                        overflow treatement: 's' for saturation, 'o' for no treatment
	char      Round;		// quantization of the lower significant bits: 'r' for rounding,   't' for truncation
	double    Rvalue;		// real value that this object represents

public:

	// ============
	// Constructors
	// ============

	FixC(const string& Fmt = DEFAULT_FMT);									// #1
	FixC(double D, const string& Fmt = DEFAULT_FMT);						// #2
	FixC(short Wl, short Iwl, const string& Fmt = DEFAULT_FMT);				// #3
	FixC(double D, short Wl, short Iwl, const string& Fmt = DEFAULT_FMT);	// #4

	FixC(const string& name, const string& Fmt = DEFAULT_FMT);									// #5
	FixC(const string& name, double D, const string& Fmt = DEFAULT_FMT);						// #6
	FixC(const string& name, short Wl, short Iwl, const string& Fmt = DEFAULT_FMT);				// #7
	FixC(const string& name, double D, short Wl, short Iwl, const string& Fmt = DEFAULT_FMT);	// #8

	// ==========
	// Destructor
	// ==========

	~FixC();

	// =========
	// Operators
	// =========

	// --------------
	// Cast operators
	// --------------

	operator float() const;
	operator double() const;

	// -------------------
	// Assignment operator
	// -------------------

	FixC& operator = (const FixC& X);
	FixC& operator = (double D);

	// ---------------
	// Unary operators
	// ---------------

	FixC& operator + ();	// unary '+' returns itself
	FixC operator - ();

	// -------------------------------------------------------
	// Arithmetic operators : binary operators
	// -------------------------------------------------------

	friend FixC operator + (const FixC& X, const FixC& Y);
	friend FixC operator + (const FixC& X, double D);
	friend FixC operator + (double D, const FixC& X);
	friend FixC operator - (const FixC& X, const FixC& Y);
	friend FixC operator - (const FixC& X, double D);
	friend FixC operator - (double D, const FixC& X);
	friend FixC operator * (const FixC& X, const FixC& Y);
	friend FixC operator * (const FixC& X, double D);
	friend FixC operator * (double D, const FixC& X);
	friend FixC operator / (const FixC& X, const FixC& Y);
	friend FixC operator / (const FixC& X, double D);
	friend FixC operator / (double D, const FixC& X);

	// ---------------
	// Shift operators
	// ---------------

	friend FixC operator << (const FixC& X, short S);
	friend FixC operator >> (const FixC& X, short S);

	void operator <<= (short S);
	void operator >>= (short S);


	// -------------------------------------------------------
	// Arithmetic-assignment operators
	// -------------------------------------------------------

	void operator += (const FixC& X);
	void operator += (double D);

	void operator -= (const FixC& X);
	void operator -= (double D);

	void operator *= (const FixC&);
	void operator *= (double D);

	void operator /= (const FixC& X);
	void operator /= (double D);

	// --------------------
	// Comparison operators
	// --------------------

	friend short operator == (const FixC& X, const FixC& Y);
	friend short operator == (const FixC& X, double D);
	friend short operator != (const FixC& X, const FixC& Y);
	friend short operator != (const FixC& X, double D);
	friend short operator >= (const FixC& X, const FixC& Y);
	friend short operator <= (const FixC& X, const FixC& Y);
	friend short operator > (const FixC& X, const FixC& Y);
	friend short operator < (const FixC& X, const FixC& Y);

	// ----------------
	// Stream operators
	// ----------------

	friend istream& operator >> (istream& s, FixC& X);
	friend ostream& operator << (ostream& s, const FixC& X);

	// =======
	// Methods
	// =======

	void      SetName(const string&);
	void      SetWl(short);
	void      SetIwl(short);
	void      SetFmt(const string&);
	double    RealValue() const;
	void      Display() const;
	MANT_TYPE GetMantissa() const;	

	// --------------
	// Friend methods
	// --------------

    friend void InitFixC1d(FixC* X, int Size, const string& InstanceName, short Wl, short Iwl,
                           const string& Fmt = DEFAULT_FMT);
    friend void InitFixC2d(FixC* X, int SizeI, int SizeJ, const string& InstanceName, short Wl, short Iwl,
                           const string& Fmt = DEFAULT_FMT);
    friend void InitFixC2d(FixC** X, int SizeI, int SizeJ, const string& InstanceName, short Wl, short Iwl,
                           const string& Fmt = DEFAULT_FMT);

};	// FixC class

// ###########################
// Inline operator definitions
// ###########################

// ***************
// Unary operators
// ***************

inline FixC& FixC::operator + ()
{
	return *this;
}

inline FixC FixC::operator - ()
{
	return FixC(-Rvalue, Wl, Iwl);
}

// ***************
// Shift operators
// ***************

inline FixC operator << (const FixC& X, short S)
{
	return FixC(X.Rvalue * EXP2(S), X.Wl, X.Iwl);
}

inline FixC operator >> (const FixC& X, short S)
{
	return FixC(X.Rvalue / EXP2(S), X.Wl, X.Iwl);
}

inline void FixC::operator <<= (short S)
{
	*this = *this << S;
}

inline void FixC::operator >>= (short S)
{
	*this = *this >> S;
}

// ********************************************************************************************************
// Arithmetic operators: operations between FixC instances and 'double' value
// ********************************************************************************************************

// ------------------------------------------------
// Addition of a FixC instance and a 'double' value
// ------------------------------------------------

inline FixC operator + (const FixC& X, double D)
{
	return X + FixC(D);
}

inline FixC operator + (double D, const FixC& X)
{
	return FixC(D) + X;
}

// ----------------------------------
// FixC instance minus 'double' value
// ----------------------------------

inline FixC operator - (const FixC& X, double D)
{
	return X - FixC(D);
}

// ----------------------------------
// 'double' value minus FixC instance
// ----------------------------------

inline FixC operator - (double D, const FixC& X)
{
	return FixC(D) - X;
}

// -----------------------------------------------------
// Multiplication of a FixC instance by a 'double' value
// -----------------------------------------------------

inline FixC operator * (const FixC& X, double D)
{
  return X * FixC(D);
}

// -----------------------------------------------------
// Multiplication of a 'double' value by a FixC instance
// -----------------------------------------------------

inline FixC operator * (double D, const FixC& X)
{
  return FixC(D) * X;
}

// -----------------------------------------------
// Division of a FixC instance by a 'double' value
// -----------------------------------------------

inline FixC operator / (const FixC& X, double D)
{
	return X / FixC(D);
}

// -----------------------------------------------
// Division of a 'double' value by a FixC instance
// -----------------------------------------------

inline FixC operator / (double D, const FixC& X)
{
	return FixC(D) / X;
}

// ************************************************************************************************
// Arithmetic-assignment operators
// ************************************************************************************************

inline void FixC::operator += (const FixC& X)
{
	*this = *this + X;
}

inline void FixC::operator += (double D)
{
    *this = *this + D;
}

inline void FixC::operator -= (const FixC& X)
{
	*this = *this - X;
}

inline void FixC::operator -= (double D)
{
	*this = *this - D;
}

inline void FixC::operator *= (const FixC& X)
{
	*this = *this * X;
}

inline void FixC::operator *= (double D)
{
	*this = *this * D;
}

inline void FixC::operator /= (const FixC& X)
{
	*this = *this / X;
}

inline void FixC::operator /= (double D)
{
	*this = *this / D;
}

// ********************
// Comparison operators
// ********************

inline short operator == (const FixC& X, const FixC& Y)
{
	return (double) X == (double) Y;
}

inline short operator == (const FixC& X, double D)
{
	return (double) X == D;
}

inline short operator != (const FixC& X, const FixC& Y)
{
	return (double) X != (double) Y;
}

inline short operator != (const FixC& X, double D)
{
	return (double) X != D;
}

inline short operator >= (const FixC& X, const FixC& Y)
{
	return (double) X >= (double) Y;
}

inline short operator <= (const FixC& X, const FixC& Y)
{
	return (double) X <= (double) Y;
}

inline short operator > (const FixC& X, const FixC& Y)
{
	return (double) X > (double) Y;
}

inline short operator < (const FixC& X, const FixC& Y)
{
	return (double) X < (double) Y;
}

// ****************
// Stream operators
// ****************

inline istream&  operator>>(istream& InputStream, FixC& X)
{ 
	double DoubleVal;

	InputStream >> DoubleVal;
	X = DoubleVal; 

	return InputStream; 
}

inline ostream&  operator<<(ostream& OutputStream, const FixC& x)
{ 
	return OutputStream;
}

#endif
