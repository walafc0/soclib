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
##
## File name            : FixC.cpp
## Language             : C++
## Short description    : This file contains the definition of the methods of the fixed-point class.
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
##                      : The operators defined in the 'FixC.h' file are:
##                      :       - unary operators (+,-)
##                      :       - shift operators (<<,>>,<<=,>>=)
##                      :       - arithmetic-assignment operators (+=,-=,*=,/=)
##                      :       - comparison operators (==, !=, <, <=, >, >=)
##                      :
## ############################################################################################################ */

// ===============
// INCLUDE SECTION
// ===============

#include "FixC.h"

// MATRICE: don't put the following 'define' in the header file of the class
// to avoid the complete compilation of the simulator.

#define FIXC_WARNING	0		// used to manage the display of warning messages
								// display is done when FIXC_WARNING > 1

// #######################
// Constructor definitions
// #######################

// ********************************************************
// Constructor #1
// Constructor arguments: [overflow and quantization modes]
// ********************************************************

FixC::FixC(const string& Fmt) : Name("UnNamed")
{
	// -------------------------------------
	// Set mantissa, WL, IWL and real values
	// -------------------------------------

	Mantissa = (MANT_TYPE) 0;
	Iwl      = DEFAULT_IWL;
	Wl       = DEFAULT_WL;
	Rvalue   = RealValue();

	// -------------------------------------------------
    // Set the value for overflow and quantization modes
	// -------------------------------------------------

	SetFmt(Fmt);

}	// end of constructor #1: FixC::FixC(const string&)

// *******************************************************************************
// Constructor #2
// Constructor arguments: floating point value [, overflow and quantization modes]
// *******************************************************************************

FixC::FixC(double DoubleVal, const string& Fmt) : Name("UnNamed")
{
	double TmpVal;		// used to calculate IWL and mantissa values
	double Delta;		// used for rounding calculus

	// ----------------------------------------------------
    // Set the value for overflow and quantization modes
	// These settings must be done before the overflow test
	// ----------------------------------------------------

	SetFmt(Fmt);

	// ---------------
	// Set word length
	// ---------------

	Wl = DEFAULT_WL;

	// -------------------------------------------
	// Set integer word length and mantissa values
	// -------------------------------------------

	if ( DoubleVal == 0.0 )
	{
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;
		Iwl      = DEFAULT_IWL;
		return;
	}

	// -------------------
	// Integer word length
	// -------------------

   	TmpVal = LOG2(fabs(DoubleVal));
	Iwl    = (short) ( fabs(TmpVal) + 1);

	if ( Iwl >= Wl )
	{
		// To force overflow detection

		Iwl = Wl - 1;
	}

	// -----------------------------
	// Test if rounding must be done
	// -----------------------------

	if ( Round == 'r' )
	{
	  double comp_value;
	  MANT_TYPE comp_value_int;
	  comp_value = DoubleVal*EXP2(Wl-Iwl-1);
	  comp_value_int = (MANT_TYPE)comp_value;
	  comp_value -= comp_value_int;
	  Delta = 0.0;
	  if (comp_value > 0 && comp_value < 0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // -0.x
	  if (comp_value >= 0.5)
	    Delta = EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1);   // 1 - 0.x
	  if (comp_value < 0 && comp_value >= -0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // +0.x 
	  if (comp_value < -0.5)
	    Delta = -EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1) ; // -1 + 0.x
	  
	  DoubleVal = DoubleVal + Delta;
		
	  //Delta     = EXP2(-Wl + Iwl);
	  //DoubleVal = DoubleVal + Delta;		// FAUST
	}

	// ----------------------------
	// Test if there is an overflow
	// ----------------------------

	if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) )
         ||
         ( DoubleVal <  ( -pow(2.0, Iwl) + pow(2.0, -Wl + Iwl + 1) ) )
       )
	{
#if FIXC_WARNING > 1
        cerr << "Constructor #2 - overflow at " << Name << ". Not enough IWL (" << Iwl << " bits) ";
        cerr << "for a real value of "<< DoubleVal;
#endif
		//
		// Test if saturation is required
		//

		if ( Saturation == 's' )
		{
#if FIXC_WARNING > 1
			cerr << ": Saturate value.";
#endif
			if ( DoubleVal > 0.0 )
			{
				Mantissa = ( PLUS_ONE << ( Wl - 1 ) ) - 1;
			}
			else
			{
				Mantissa = MINUS_ONE << ( Wl - 1 );
			}

			//
			// Set the real value that this object represents
			//

			Rvalue = RealValue();

		}	// end of "if ( Saturation == 's' )"

#if FIXC_WARNING > 1
		cerr << endl;
#endif
		return;
		
	}	// end of "if ( ( DoubleVal >= pow(2.0, Iwl) ) || ( DoubleVal < -pow(2.0, Iwl) ) )"

	// -----------------------------
	// Test if there is an underflow
	// -----------------------------

	if ( ( ( DoubleVal > 0.0 ) ? DoubleVal : -DoubleVal ) < pow(2.0, -Wl + Iwl + 1) )
	{
#if FIXC_WARNING > 1
		cerr << "Constructor #2 - underflow at " << Name;
		cerr << "not enough FWL (" << Wl - Iwl - 1 << " bits)";
		cerr << " for a real value of " << DoubleVal << ". Instance value is set to 0" << endl;
#endif
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;
		return;
	}

	// ---------------------------------------------------------------
	// There is neither overflow nor underflow: set the mantissa value
	// ---------------------------------------------------------------

	if ( ( Wl - Iwl - 1 ) == 0 )
	{
		// The fractional word length is nil

		Mantissa = (MANT_TYPE) DoubleVal;
	}
	else
	{
		// If necessary, rounding has already be done

		Mantissa = (MANT_TYPE) ( DoubleVal * EXP2(Wl - Iwl - 1) );
	}

	// ----------------------------------------------
	// Set the real value that this object represents
	// ----------------------------------------------

	Rvalue = RealValue();

}	// end of constructor #2: FixC::FixC(double, const string&)

// ******************************************************************
// Constructor #3
// Constructor arguments; WL, IWL [, overflow and quantization modes]
// ******************************************************************

FixC::FixC(short WordLength, short IntWordLength, const string& Fmt) : Name("UnNamed")
{
	// ----------------------------
	// Set mantissa and real values
	// ----------------------------

	Mantissa = (MANT_TYPE) 0;
	Rvalue   = 0.0;

	// -------------------------
	// Set the word length value
	// -------------------------

	Wl = WordLength;

	if ( Wl > MAX_WL )
	{
		cerr << "Variable " << Name << " - Constructor #3 error ; WL must be lower than " << MAX_WL << endl;
		exit (FIXC_ERROR);
	}

	// ---------------------------------
	// Set the integer word length value
	// ---------------------------------

	Iwl = IntWordLength;

	if ( Iwl >= Wl )
	{
		cerr << "Variable " << Name << " - Constructor #3 error ; IWL must be lower than WL." << endl;
		exit (FIXC_ERROR);
	}

	// -----------------------------------
	// Set overflow and quantization modes
	// -----------------------------------

	SetFmt(Fmt);

}	// end of constructor #3: FixC::FixC(short, short, const string&)

// ****************************************************************************************
// Constructor #4
// Constructor arguments: floating point value, WL, IWL [, overflow and quantization modes]
// ****************************************************************************************

FixC::FixC(double DoubleVal, short WordLength, short IntWordLength, const string& Fmt) : Name("UnNamed")
{
	double Delta;		// used for rounding calculus

	// -------------------------
	// Set the word length value
	// -------------------------

	Wl = WordLength;

	if ( Wl > MAX_WL )
	{
		cerr << "Variable " << Name << " - Constructor #4 error ; WL must be lower than " << MAX_WL << endl;
		exit (FIXC_ERROR);
	}

	// ---------------------------------
	// Set the integer word length value
	// ---------------------------------

	Iwl = IntWordLength;

	if ( Iwl >= Wl )
	{
		cerr << "Variable " << Name << " - Constructor #4 error ; IWL must be lower than WL." << endl;
		exit (FIXC_ERROR);
	}

	// ----------------------------------------------------
	// Set overflow and quantization modes
	// These settings must be done before the overflow test
	// ----------------------------------------------------

	SetFmt(Fmt);

	// ----------------------------------------------------------------------
	// Test if the value that must be used to instantiate the variable is '0'
	// ----------------------------------------------------------------------

	if ( DoubleVal == 0.0 )
	{
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;
		return;
	}

	// -----------------------------
	// Test if rounding must be done
	// -----------------------------

	if ( Round == 'r' )
	{
	  double comp_value;
	  MANT_TYPE comp_value_int;
	  comp_value = DoubleVal*EXP2(Wl-Iwl-1);
	  comp_value_int = (MANT_TYPE)comp_value;
	  comp_value -= comp_value_int;
	  Delta = 0.0;
	  if (comp_value > 0 && comp_value < 0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // -0.x
	  if (comp_value >= 0.5)
	    Delta = EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1);   // 1 - 0.x
	  if (comp_value < 0 && comp_value >= -0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // +0.x 
	  if (comp_value < -0.5)
	    Delta = -EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1) ; // -1 + 0.x
	  
	  DoubleVal = DoubleVal + Delta;
	  //Delta     = EXP2(-Wl + Iwl);
	  //DoubleVal = DoubleVal + Delta;		// FAUST
	}

	// ----------------------------
	// Test if there is an overflow
	// ----------------------------
	
	if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) )
         ||
         ( DoubleVal <  ( -pow(2.0, Iwl) + pow(2.0, -Wl + Iwl + 1) ) )
       )
	{
#if FIXC_WARNING > 1
        cerr << "Constructor #4 - overflow at " << Name << ". Not enough IWL (" << Iwl << " bits) ";
        cerr << "for a real value of "<< DoubleVal;
#endif
		//
		// Test if saturation is required
		//

		if ( Saturation == 's' )
		{
#if FIXC_WARNING > 1
			cerr << ": Saturate value.";
#endif
			if ( DoubleVal > 0.0 )
			{
				Mantissa = ( PLUS_ONE << ( Wl - 1 ) ) - 1;
			}
			else
			{
				Mantissa = MINUS_ONE << ( Wl - 1 );
			}

			//
			// Set the real value that this object represents
			//

			Rvalue = RealValue();

		}	// end of "if ( Saturation == 's' )"

#if FIXC_WARNING > 1
		cerr << endl;
#endif
		return;
		
	}	// end of "if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) ) || ..." test

	// -----------------------------
	// Test if there is an underflow
	// -----------------------------

	if ( ( ( DoubleVal > 0.0 ) ? DoubleVal : -DoubleVal ) < pow(2.0, -Wl + Iwl + 1) )
	{
#if FIXC_WARNING > 1
		cerr << "Constructor #4 - underflow at " << Name;
		cerr << "not enough FWL (" << Wl - Iwl - 1 << " bits)";
		cerr << " for a real value of " << DoubleVal << ". Instance value is set to 0" << endl;
#endif
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;
		return;
	}

	// ---------------------------------------------------------------
	// There is neither overflow nor underflow: set the mantissa value
	// ---------------------------------------------------------------

	if ( ( Wl - Iwl - 1 ) == 0 )
	{
		// The fractional word length is nil

		Mantissa = (MANT_TYPE) DoubleVal;
	}
	else
	{
		// If necessary, rounding has already be done

		Mantissa = (MANT_TYPE) ( DoubleVal * EXP2(Wl - Iwl - 1) );
	}

	// ----------------------------------------------
	// Set the real value that this object represents
	// ----------------------------------------------

	Rvalue = RealValue();

}	// end of constructor #4: FixC::FixC(double, short, short, const string&)

// ************************************************************************
// Constructor #5
// Constructor arguments: instance name [, overflow and quantization modes]
// ************************************************************************

FixC::FixC(const string& InstanceName, const string& Fmt)
{
	// -------------------------------------
	// Set mantissa, WL, IWL and real values
	// -------------------------------------

	Name     = InstanceName;
	Iwl      = DEFAULT_IWL;
	Wl       = DEFAULT_WL;
	Mantissa = (MANT_TYPE) 0;
	Rvalue   = 0.0;

	// -------------------------------------------------
    // Set the value for overflow and quantization modes
	// -------------------------------------------------

	SetFmt(Fmt);

}	// end of constructor #5: FixC::FixC(const string&, const string&)

// **********************************************************************************************
// Constructor #6
// Constructor arguments: instance name, floating point value [, overflow and quantization modes]
// **********************************************************************************************

FixC::FixC(const string& InstanceName, double DoubleVal, const string& Fmt)
{
	double TmpVal;		// used to set IWL and mantissa values
	double Delta;		// used for rounding calculus

	//-----------------------------------------------------
    // Set the value for overflow and quantization modes
	// These settings must be done before the overflow test
	//-----------------------------------------------------

	SetFmt(Fmt);

	// ------------------------------
	// Set instance name and WL value
	// ------------------------------

	Name = InstanceName;
	Wl   = DEFAULT_WL;

	// ----------------------------------------------------------------------
	// Set the integer word length and the mantissa value
	// Test if the value that must be used to instantiate the variable is '0'
	// ----------------------------------------------------------------------

	if ( DoubleVal == 0.0 )
	{
		Mantissa = (MANT_TYPE) 0;
		Iwl      = DEFAULT_IWL;
		Rvalue   = 0.0;
		return;
	}

	TmpVal = LOG2(fabs(DoubleVal));
	Iwl    = (short) ( fabs(TmpVal) + 1 );

	if ( Iwl >= Wl )
	{
		// To force overflow detection

		Iwl = Wl - 1;
	}

	// -----------------------------
	// Test if rounding must be done
	// -----------------------------

	if ( Round == 'r' )
	{
	  double comp_value;
	  MANT_TYPE comp_value_int;
	  comp_value = DoubleVal*EXP2(Wl-Iwl-1);
	  comp_value_int = (MANT_TYPE)comp_value;
	  comp_value -= comp_value_int;
	  Delta = 0.0;
	  if (comp_value > 0 && comp_value < 0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // -0.x
	  if (comp_value >= 0.5)
	    Delta = EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1);   // 1 - 0.x
	  if (comp_value < 0 && comp_value >= -0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // +0.x 
	  if (comp_value < -0.5)
	    Delta = -EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1) ; // -1 + 0.x
	  
	  DoubleVal = DoubleVal + Delta;
	  //		Delta     = EXP2(-Wl + Iwl);
	  //DoubleVal = DoubleVal + Delta;		// FAUST
	}

	// ----------------------------
	// Test if there is an overflow
	// ----------------------------

	if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) )
         ||
         ( DoubleVal <  ( -pow(2.0, Iwl) + pow(2.0, -Wl + Iwl + 1) ) )
       )
	{
#if FIXC_WARNING > 1
        cerr << "Constructor #6 - overflow at " << Name << ". Not enough IWL (" << Iwl << " bits) ";
        cerr << "for a real value of "<< DoubleVal;
#endif
		//
		// Test if saturation is required
		//

		if ( Saturation == 's' )
		{
#if FIXC_WARNING > 1
			cerr << ": Saturate value.";
#endif
			if ( DoubleVal > 0.0 )
			{
				Mantissa = ( PLUS_ONE << ( Wl - 1 ) ) - 1;
			}
			else
			{
				Mantissa = MINUS_ONE << ( Wl - 1 );
			}

			//
			// Set the real value that this object represents
			//

			Rvalue = RealValue();

		}	// end of "if ( Saturation == 's' )"

#if FIXC_WARNING > 1
		cerr << endl;
#endif
		return;
		
	}	// end of "if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) ) || ..." test

	// -----------------------------
	// Test if there is an underflow
	// -----------------------------

	if ( ( ( DoubleVal > 0.0 ) ? DoubleVal : -DoubleVal ) < pow(2.0, -Wl + Iwl + 1) )
	{
#if FIXC_WARNING > 1
		cerr << "Constructor #6 - underflow at " << Name;
		cerr << "not enough FWL (" << Wl - Iwl - 1 << " bits)";
		cerr << " for a real value of " << DoubleVal << ". Instance value is set to 0" << endl;
#endif
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;
		return;
	}

	// ---------------------------------------------------------------
	// There is neither overflow nor underflow: set the mantissa value
	// ---------------------------------------------------------------

	if ( ( Wl - Iwl - 1 ) == 0 )
	{
		// The fractional word length is nil

		Mantissa = (MANT_TYPE) DoubleVal;
	}
	else
	{
		// If necessary, rounding has already be done

		Mantissa = (MANT_TYPE) ( DoubleVal * EXP2(Wl - Iwl - 1) );
	}

	// ----------------------------------------------
	// Set the real value that this object represents
	// ----------------------------------------------

	Rvalue = RealValue();

}	// end of constructor #6: FixC::FixC(const string&, double, const string&)

// *********************************************************************************
// Constructor #7
// Constructor arguments: instance name, WL, IWL [, overflow and quantization modes]
// *********************************************************************************

FixC::FixC(const string& InstanceName, short WordLength, short IntWordLength, const string& Fmt)
{
	// ---------------------------------------------------
	// Set the instance name, the mantissa and real values
	// ---------------------------------------------------

	Name     = InstanceName;
	Mantissa = (MANT_TYPE) 0;
	Rvalue   = 0.0;

	//
	// Set the word length value
	//

	Wl = WordLength;

	if ( Wl > MAX_WL )
	{
		cerr << "Variable " << Name << " - Constructor #7 error ; WL must be lower than " << MAX_WL << endl;
		exit (FIXC_ERROR);
	}

	// ---------------------------------
	// Set the integer word length value
	// ---------------------------------

	Iwl = IntWordLength;

	if ( Iwl >= Wl )
	{
		cerr << "Variable " << Name << " - Constructor #7 error ; IWL must be lower than WL." << endl;
		exit (FIXC_ERROR);
	}

	// -----------------------------------
    // Set overflow and quantization modes
	// -----------------------------------

	SetFmt(Fmt);

}	// end of constructor #7: FixC::FixC(const string&, short, short, const string&)

// *******************************************************************************************************
// Constructor #8
// Constructor arguments: instance name, floating point value, WL, IWL [, overflow and quantization modes]
// *******************************************************************************************************

FixC::FixC(const string& InstanceName, double DoubleVal, short WordLength, short IntWordLength, const string& Fmt)
{
	double Delta;		// used for rounding calculus

	// ---------------------
	// Set the instance name
	// ---------------------

	Name = InstanceName;

	// -------------------------
	// Set the word length value
	// -------------------------

	Wl = WordLength;

	if ( Wl > MAX_WL )
	{
		cerr << "Variable " << Name << " - Constructor #8 error ; WL must be lower than " << MAX_WL << endl;
		exit (FIXC_ERROR);
	}

	// ---------------------------------
	// Set the integer word length value
	// ---------------------------------

	Iwl = IntWordLength;

	if ( Iwl >= Wl )
	{
		cerr << "Variable " << Name << " - Constructor #8 error ; IWL must be lower than WL." << endl;
		exit (FIXC_ERROR);
	}

	//-----------------------------------------------------
    // Set the value for overflow and quantization modes
	// These settings must be done before the overflow test
	//-----------------------------------------------------

	SetFmt(Fmt);

	// ----------------------------------------------------------------------
	// Test if the value that must be used to instantiate the variable is '0'
	// ----------------------------------------------------------------------

	if ( DoubleVal == 0.0 )
	{
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;
		return;
	}

	// -----------------------------
	// Test if rounding must be done
	// -----------------------------

	if ( Round == 'r' )
	{
	  double comp_value;
	  MANT_TYPE comp_value_int;
	  comp_value = DoubleVal*EXP2(Wl-Iwl-1);
	  comp_value_int = (MANT_TYPE)comp_value;
	  comp_value -= comp_value_int;
	  Delta = 0.0;
	  if (comp_value > 0 && comp_value < 0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // -0.x
	  if (comp_value >= 0.5)
	    Delta = EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1);   // 1 - 0.x
	  if (comp_value < 0 && comp_value >= -0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // +0.x 
	  if (comp_value < -0.5)
	    Delta = -EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1) ; // -1 + 0.x
	  
	  DoubleVal = DoubleVal + Delta;

	  //		Delta     = EXP2(-Wl + Iwl);
	  //DoubleVal = DoubleVal + Delta;		// FAUST
	}

	// ----------------------------
	// Test if there is an overflow
	// ----------------------------

	if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) )
         ||
         ( DoubleVal <  ( -pow(2.0, Iwl) + pow(2.0, -Wl + Iwl + 1) ) )
       )
	{
#if FIXC_WARNING > 1
        cerr << "Constructor #8 - overflow at " << Name << ". Not enough IWL (" << Iwl << " bits) ";
        cerr << "for a real value of "<< DoubleVal;
#endif
		//
		// Test if saturation is required
		//

		if ( Saturation == 's' )
		{
#if FIXC_WARNING > 1
			cerr << ": Saturate value.";
#endif
			if ( DoubleVal > 0.0 )
			{
				Mantissa = ( PLUS_ONE << ( Wl - 1 ) ) - 1;
			}
			else
			{
				Mantissa = MINUS_ONE << ( Wl - 1 );
			}

			//
			// Set the real value that this object represents
			//

			Rvalue = RealValue();

		}	// end of "if ( Saturation == 's' )"

#if FIXC_WARNING > 1
		cerr << endl;
#endif
		return;
		
	}	// end of "if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) ) || ..." test

	// -----------------------------
	// Test if there is an underflow
	// -----------------------------

	if ( ( ( DoubleVal > 0.0 ) ? DoubleVal : -DoubleVal ) < pow(2.0, -Wl + Iwl + 1) )
	{
#if FIXC_WARNING > 1
		cerr << "Constructor #8 - underflow at " << Name;
		cerr << "not enough FWL (" << Wl - Iwl - 1 << " bits)";
		cerr << " for a real value of " << DoubleVal << ". Instance value is set to 0" << endl;
#endif
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;
		return;
	}

	// ---------------------------------------------------------------
	// There is neither overflow nor underflow: set the mantissa value
	// ---------------------------------------------------------------

	if ( ( Wl - Iwl - 1 ) == 0 )
	{
		// The fractional word length is nil

		Mantissa = (MANT_TYPE) DoubleVal;
	}
	else
	{
		// If necessary, rounding has already be done

		Mantissa = (MANT_TYPE) ( DoubleVal * EXP2(Wl - Iwl - 1) );
	}

	// ----------------------------------------------
	// Set the real value that this object represents
	// ----------------------------------------------

	Rvalue = RealValue();

}	// end of constructor #8: "FixC::FixC(const string&, double, short, short, const string&)

// #################
// Destructor: empty
// #################

FixC::~FixC() {}

// ####################
// Operator definitions
// ####################

// **************
// Cast operators
// **************

FixC::operator float() const
{
  return (float) Rvalue;
}

FixC::operator double() const
{
  return Rvalue;
}

// *******************
// Assignment operator
// *******************

// -----------------------------
// Assignment of a FixC instance
// -----------------------------

FixC& FixC::operator = (const FixC& X)
{
	short     Shift;			// used to align the binary points of the lhs and rhs instances
	double    DoubleVal;		// real value of the rhs instance
	double    Delta;			// used for rounding calculus
	MANT_TYPE LongTmp;			// used to calculate the mantissa value of the lhs instance

	// ---------------------------------------------------------------------------------
	// Get the real value of the rhs instance and set the real value of the lhs instance
	// ---------------------------------------------------------------------------------

	DoubleVal = X.Rvalue;

	// ----------------------------------------------
	// Test if the value that must be assigned is '0'
	// ----------------------------------------------

	if ( DoubleVal == 0.0 )
	{
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;

		return *this;
	}

	// -----------------------------
	// Test if rounding must be done
	// -----------------------------

	if ( Round == 'r' )
	{
	  double comp_value;
	  MANT_TYPE comp_value_int;
	  comp_value = DoubleVal*EXP2(Wl-Iwl-1);
	  comp_value_int = (MANT_TYPE)comp_value;
	  comp_value -= comp_value_int;
	  //cout << "equality operator : DoubleVal= " << hex << DoubleVal ;
	  Delta = 0.0;
	  if (comp_value > 0 && comp_value < 0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // -0.x
	  if (comp_value >= 0.5)
	    Delta = EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1);   // 1 - 0.x
	  if (comp_value < 0 && comp_value >= -0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // +0.x 
	  if (comp_value < -0.5)
	    Delta = -EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1) ; // -1 + 0.x
	  
	  DoubleVal = DoubleVal + Delta;
	  //cout << ", comp_value = " << comp_value << ", delta = " << Delta << " and after rounding DoubleVal = " << DoubleVal << dec << endl ;
	}

	// ----------------------------
	// Test if there is an overflow
	// ----------------------------

	if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) )
         ||
         ( DoubleVal <  ( -pow(2.0, Iwl) + pow(2.0, -Wl + Iwl + 1) ) )
       )
	{
#if FIXC_WARNING > 1
		cerr << Name << ": assignment (of a 'FixC' instance) overflow - ";
        cerr << "not enough IWL (" << Iwl << " bits) for a real value of " << DoubleVal;
#endif
		// ------------------------------
		// Test if saturation is required
		// ------------------------------

		if ( Saturation == 's' )
		{
#if FIXC_WARNING > 1
			cerr << ": saturate value.";
#endif
			if ( X.Mantissa > 0 )
			{
				Mantissa = ( PLUS_ONE << ( Wl - 1 ) ) - 1;
			}
			else
			{
				Mantissa = MINUS_ONE << ( Wl - 1 );
			}

			// ----------------------------------------------
			// Set the real value that this object represents
			// ----------------------------------------------

			Rvalue = RealValue();

		}	// end of "if ( Saturation == 's' )"

#if FIXC_WARNING > 1
		cerr << endl;
#endif
		
		return *this;

	}	// end of "if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) ) || ..." test

	// -----------------------------
	// Test if there is an underflow
	// -----------------------------

	if ( ( ( DoubleVal > 0.0 ) ? DoubleVal : -DoubleVal ) < pow(2.0, -Wl + Iwl + 1) )
	{
		// An underflow has been detected.
		// The members 'Mantissa' and 'Rvalue' has been set to '0'

#if FIXC_WARNING > 1
		cerr << Name << ": assignment (of a 'FixC' instance) underflow - ";
		cerr << "not enough FWL (" << Wl - Iwl - 1 << " bits)";
		cerr << " for a real value of " << DoubleVal << ". Instance value is set to 0" << endl;
#endif
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;

		return *this;
	}

	// -------------------------------------------------------------------------------
	// There is neither overflow nor underflow: set the mantissa value
	// Assign after aligning as much as difference between the fractional word lengths
	// -------------------------------------------------------------------------------

	LongTmp = X.Mantissa;

	//
	// FWL minus X.FWL: number of left shifts
	//

	Shift = ( Wl - Iwl ) - ( X.Wl - X.Iwl);

	if ( Shift < 0 )
	{
		//
		// The FWL of the lhs instance is lower than the FWL of the rhs instance
		// If necessary, rounding has already be done
		//

		if ( Round == 'r' )
		{
			Mantissa  = (MANT_TYPE) ( DoubleVal * EXP2(Wl - Iwl - 1) );
		}
		else
		{
			Mantissa = LongTmp >> ( -Shift );
		}
	}
	else
	{
		Mantissa = LongTmp << Shift;
	}

	// ----------------------------------------------
	// Set the real value that this object represents
	// ----------------------------------------------

	Rvalue = RealValue();

	return *this;

}	// end of '=' operator: FixC::operator = (const FixC&)

// ===============================================
// Assignment of a double value to a FixC instance
// ===============================================

FixC& FixC::operator = (double DoubleVal)
{
	double Delta;		// used for rounding calculus

	// ----------------------------------------------
	// Test if the value that must be assigned is '0'
	// ----------------------------------------------

	if ( DoubleVal == 0.0 )
	{
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;

		return *this;
	}

	// -----------------------------
	// Test if rounding must be done
	// -----------------------------

	if ( Round == 'r' )
	{
	  double comp_value;
	  MANT_TYPE comp_value_int;
	  comp_value = DoubleVal*EXP2(Wl-Iwl-1);
	  comp_value_int = (MANT_TYPE)comp_value;
	  comp_value -= comp_value_int;
	  Delta = 0.0;
	  if (comp_value > 0 && comp_value < 0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // -0.x
	  if (comp_value >= 0.5)
	    Delta = EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1);   // 1 - 0.x
	  if (comp_value < 0 && comp_value >= -0.5)
	    Delta = -comp_value*EXP2(-Wl+Iwl+1);                    // +0.x 
	  if (comp_value < -0.5)
	    Delta = -EXP2(-Wl+Iwl+1) - comp_value*EXP2(-Wl+Iwl+1) ; // -1 + 0.x
	  
	  DoubleVal = DoubleVal + Delta;

	  //		Delta     = EXP2(-Wl + Iwl);
	  //DoubleVal = DoubleVal + Delta;		// FAUST
	}

	// ----------------------------
	// Test if there is an overflow
	// ----------------------------

	if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) )
         ||
         ( DoubleVal <  ( -pow(2.0, Iwl) + pow(2.0, -Wl + Iwl + 1) ) )
       )
	{
#if FIXC_WARNING > 1
		cerr << Name << ": assignment (of a 'double' value) overflow - ";
        cerr << "not enough IWL (" << Iwl << " bits) for a real value of " << DoubleVal;
#endif
		// ------------------------------
		// Test if saturation is required
		// ------------------------------

		if ( Saturation == 's' )
		{
#if FIXC_WARNING > 1
			cerr << ": saturate value.";
#endif
			if ( DoubleVal > 0.0 )
			{
				Mantissa = ( PLUS_ONE << ( Wl - 1 ) ) - 1;
			}
			else
			{
				Mantissa = MINUS_ONE << ( Wl - 1 );
			}

			// ----------------------------------------------
			// Set the real value that this object represents
			// ----------------------------------------------

			Rvalue = RealValue();

		}	// end of "if ( Saturation == 's' )"

#if FIXC_WARNING > 1
		cerr << endl;
#endif
		
		return *this;
		
	}	// end of "if ( ( DoubleVal >= (  pow(2.0, Iwl) - pow(2.0, -Wl + Iwl + 1) ) ) || ..." test

	// -----------------------------
	// Test if there is an underflow
	// -----------------------------

	if ( ( ( DoubleVal > 0.0 ) ? DoubleVal : -DoubleVal ) < pow(2.0, -Wl + Iwl + 1) )
	{
		// An underflow has been detected.
		// The members 'Mantissa' and 'Rvalue' has been set to '0'

#if FIXC_WARNING > 1
		cerr << Name << ": assignment (of a 'double' value) underflow - ";
		cerr << "not enough FWL (" << Wl - Iwl - 1 << " bits)";
		cerr << " for a real value of " << DoubleVal << ". Instance value is set to 0" << endl;
#endif
		Mantissa = (MANT_TYPE) 0;
		Rvalue   = 0.0;

		return *this;
	}

	// ---------------------------------------------------------------
	// There is neither overflow nor underflow: set the mantissa value
	// If necessary, rounding has already be done
	// ---------------------------------------------------------------

	Mantissa = (MANT_TYPE) ( DoubleVal * EXP2(Wl - Iwl - 1) );

	// ----------------------------------------------
	// Set the real value that this object represents
	// ----------------------------------------------

	Rvalue = RealValue();

	return *this;

}	// end of '=' operator: FixC::operator = (double)

// *******************************************************
// Arithmetic operators
// *******************************************************

// ============
// Operator '+'
// ============

// ---------------------------------------------------------------------------
// Addition of 2 FixC instances
// Assume that:   Result.Iwl = max(X.Iwl, Y.Iwl) + 1;
//                Result.Wl  = max(Result.Iwl + max(X.fWl, Y.fWl) + 1, MAX_WL)
// where "Fwl" means fractional word length
// ---------------------------------------------------------------------------

FixC operator + (const FixC& X, const FixC& Y)
{
	short  XfWl;			// X fraction length
	short  YfWl;			// Y fraction length
	short  MaxFwl;			// max value of fraction length
	short  IntWordLength;	// integer word length of the result
	short  WordLength;		// word length of the result
	double Result;			// floating-point representation of the result

	// ----------------------------------------------------
	// Calculus of the fractional word length of the result
	// ----------------------------------------------------

	XfWl   = X.Wl - X.Iwl - 1;
	YfWl   = Y.Wl - Y.Iwl - 1;
	MaxFwl = ( XfWl > YfWl ) ? XfWl : YfWl;

	// ---------------------------------------
	// Set the WL and IWL values of the result
	// ---------------------------------------

	IntWordLength = ( X.Iwl > Y.Iwl ) ? ( X.Iwl + 1 ) : ( Y.Iwl + 1 );
	WordLength    = IntWordLength + MaxFwl + 1;

	//
	// Word length can't exceed the max value
	//

	if ( WordLength > MAX_WL )
	{
		WordLength = MAX_WL;

		if ( IntWordLength >= WordLength )
		{
			IntWordLength = WordLength - MaxFwl - 1;
		}
	}

	// ----------------------------------------------------
	// Set the real value that this instance will represent
	// ----------------------------------------------------

	Result = X.Rvalue + Y.Rvalue;

	// ---------------------------
	// Instantiation of the result
	// ---------------------------

	return FixC(Result, WordLength, IntWordLength);

}	// '+' operator

// ============
// Operator '-'
// ============

// ---------------------------------------------------------------------------
// Substraction of 2 FixC instances
// Assume that:   Result.Iwl = max(X.Iwl, Y.Iwl) + 1;
//                Result.Wl  = max(Result.Iwl + max(X.Fwl, Y.Fwl) + 1, MAX_WL)
// where "Fwl" means fractional word length
// ---------------------------------------------------------------------------

FixC operator - (const FixC& X, const FixC& Y)
{
	short  XfWl;			// X fraction length
	short  YfWl;			// Y fraction length
	short  MaxFwl;			// max value of fractional word length
	short  IntWordLength;	// integer word length of the result
	short  WordLength;		// word length of the result
	double Result;			// floating-point representation of the result

	// ----------------------------------------------------
	// Calculus of the fractional word length of the result
	// ----------------------------------------------------

	XfWl   = X.Wl - X.Iwl - 1;
	YfWl   = Y.Wl - Y.Iwl - 1;
	MaxFwl = ( XfWl > YfWl ) ? XfWl : YfWl;

	// ---------------------------------------
	// Set the WL and IWL values of the result
	// ---------------------------------------

	IntWordLength = ( X.Iwl > Y.Iwl ) ? ( X.Iwl + 1 ) : ( Y.Iwl + 1 );
	WordLength    = IntWordLength + MaxFwl + 1;

	//
	// Word length can't exceed the max value
	//

	if ( WordLength > MAX_WL )
	{
		WordLength = MAX_WL;

		if ( IntWordLength >= WordLength )
		{
			IntWordLength = WordLength - MaxFwl - 1;
		}
	}

	// ----------------------------------------------------
	// Set the real value that this instance will represent
	// ----------------------------------------------------

	Result = X.Rvalue - Y.Rvalue;

	// ---------------------------
	// Instantiation of the result
	// ---------------------------

	return FixC(Result, WordLength, IntWordLength);

}	// '-' operator

// ============
// Operator '*'
// ============

// ----------------------------------
// Multiplication of 2 FixC instances
// ----------------------------------

FixC operator * (const FixC& X, const FixC& Y)
{
	short  IntWordLength;	// integer word length of the result
	short  WordLength;		// word length of the result
	double Result;			// real value of the result
	double TmpVal;			// used to calculate the IWL value

	// --------------------------------
	// Set the real value of the result
	// --------------------------------

	Result = X.Rvalue * Y.Rvalue;

	// ----------------------------------
	// Calculus of the result word length
	// ----------------------------------

	WordLength = X.Wl + Y.Wl - 1;

	//
	// Word length can't exceed the max value
	//

	if ( WordLength > MAX_WL )
	{
		 WordLength = MAX_WL;
	}

	// -----------------------------
	// Calculus of IWL of the result
	// -----------------------------

	IntWordLength = X.Iwl + Y.Iwl;

	if ( IntWordLength >= WordLength )
	{
    	TmpVal        = LOG2(fabs(Result));
		IntWordLength = (short) ( fabs(TmpVal) + 1 );
	}

	// ---------------------------
	// Instantiation of the result
	// ---------------------------

	return FixC(Result, WordLength, IntWordLength);

}	// '*' operator

// ============
// Operator '/'
// ============

// ------------------------------
// Division of two FixC instances
// ------------------------------

FixC operator / (const FixC& X, const FixC& Y)
{
	short  XfWl;			// X fraction length
	short  YfWl;			// Y fraction length
	short  MaxFwl;			// max value of fraction length
	short  IntWordLength;	// integer word length of the result
	short  WordLength;		// word length of the result
	double Result;			// real value of the division
	double TmpVal;			// used to calculate the IWL value

	//
	// Calculus of the real result of the division
	//

	Result = X.Rvalue / Y.Rvalue;

	//
	// Calculus of IWL of the result
	//

   	TmpVal        = LOG2(fabs(Result));
	IntWordLength = (short) ( fabs(TmpVal) + 1);

	//
	// Calculus of the FWL of the result
	//

	XfWl   = X.Wl - X.Iwl - 1;
	YfWl   = Y.Wl - Y.Iwl - 1;
	MaxFwl = ( XfWl > YfWl ) ? XfWl : YfWl;

	if ( MaxFwl < 0 )
	{
		MaxFwl = 0;
	}

	// --------------------
	// Set WL of the result
	// --------------------

	WordLength = IntWordLength + MaxFwl + 1;

	//
	// Word length can't exceed the max value
	//

	if ( WordLength > MAX_WL )
	{
		WordLength = MAX_WL;
	}

	// ---------------------------
	// Instantiation of the result
	// ---------------------------

	return FixC(Result, WordLength, IntWordLength);

}	// '/' operator

// *************
// Class methods
// *************

// ================================
// Set the value of the word length
// ================================

void FixC::SetWl(short WordLength)
{
	Wl = WordLength;
}

// ========================================
// Set the value of the integer word length
// ========================================

void FixC::SetIwl(short IntWordLength)
{
	Iwl = IntWordLength;
}

// =======================================
// Set the overflow and quantization modes
// =======================================

void FixC::SetFmt(const string& Fmt)
{
	Saturation = Fmt[0];
	Round      = Fmt[1];
}

// =====================================================
// Set the real value that the class instance represents
// =====================================================

double FixC::RealValue() const
{
	bool               PositiveValue;		// true if the mantissa is positive
	int                NoBit;				// mantissa bit number
	unsigned MANT_TYPE Mask;				// to get the value of the mantissa bit
	unsigned MANT_TYPE AbsMantissa;			// mantissa absolute value
	double             RealValue;			// real value of the class instance

	// ------------------------------
	// Absolute value of the mantissa
	// ------------------------------

	if ( Mantissa < 0 )
	{
		AbsMantissa   = (unsigned MANT_TYPE) ( -Mantissa );
		PositiveValue = false;
	}
	else
	{
		AbsMantissa   = (unsigned MANT_TYPE) Mantissa;
		PositiveValue = true;
	}

	// ------------------------------------------
	// Calculus of the real value of the instance
	// ------------------------------------------

	RealValue = 0.0;
	Mask      = (unsigned MANT_TYPE) ( (MANT_TYPE) 1 << ( sizeof(unsigned MANT_TYPE) * CHAR_BIT) - 1 );

	for ( NoBit = sizeof(MANT_TYPE) * CHAR_BIT - 1 ; NoBit >= 0 ; NoBit-- )
	{
		RealValue *= 2.0;
		if ( ( AbsMantissa & Mask )  != (unsigned MANT_TYPE) 0 )
		{
			RealValue += 1.0;
		}

		Mask >>= 1;
	}

	RealValue /= EXP2((double) ( Wl - Iwl - 1 ));

	return PositiveValue ? RealValue : -RealValue;
}

// ===========================
// Set the class instance name
// ===========================

void FixC::SetName(const string& InstanceName)
{
	Name = InstanceName;
}

// =============================
// Get the value of the mantissa
// =============================

MANT_TYPE FixC::GetMantissa() const
{
	return Mantissa;
}

// ========================================================
// Display the variable member values of the class instance
// ========================================================

void FixC::Display() const
{
	double        FractPart;					// decimal value of the fractional part
	MANT_TYPE     IntPart;						// decimal value of the integer part
	MANT_TYPE     TmpVal;						// used to calculate the decimal value of the integer and fractional parts
	MANT_TYPE     AbsMantissa;					// absolute value of the mantissa
	short         IdxBit;						// loop index
	short         Exponent;						// used to calculate the decimal value of the integer and fractional parts
	short         Fwl;							// number of bits of the fractional word
	ostringstream TmpString1;					// used to edit the object
	ostringstream TmpSign;						// used to edit the object: sign of the instance value
	const int     StringSize = 63;				// see WARNING below
	char          FractPartStr[StringSize +1];	// see WARNING below

	// ------------------------------
	// Absolute value of the mantissa
	// ------------------------------

	if ( Mantissa < 0 )
	{
		AbsMantissa = -Mantissa;
		TmpSign << "-";
	}
	else
	{
		AbsMantissa = Mantissa;
		TmpSign << "";
	}

	// -----------------------------------------
	// Calculus of the FWL of the class instance
	// -----------------------------------------

	Fwl = Wl - Iwl - 1;

	// -------------------------------------------------------------
	// Calculus of the decimal representation of the fractional part
	// -------------------------------------------------------------

	Exponent  = -Fwl;
	FractPart = 0.0;
	TmpVal    = AbsMantissa;

	for ( IdxBit = 0 ; IdxBit < Fwl ; IdxBit++ )
	{
		FractPart += ( TmpVal & PLUS_ONE ) * pow(2.0, Exponent);
		TmpVal >>= (MANT_TYPE) 1;
		Exponent++;
	}

	// ----------------------------------------------------------
	// Calculus of the decimal representation of the integer part
	// ----------------------------------------------------------

	Exponent = (MANT_TYPE) 0;
	IntPart  = (MANT_TYPE) 0;
	TmpVal   = AbsMantissa >> Fwl;

	for ( IdxBit = 0 ; IdxBit < Iwl ; IdxBit++  )
	{
		IntPart += ( TmpVal & PLUS_ONE ) * (MANT_TYPE) pow(2.0, Exponent);
		TmpVal >>= (MANT_TYPE) 1;
		Exponent++;
	}

	// -------------------------------------------------
	// Display the characteristics of the class instance
	// -------------------------------------------------

	TmpString1 << Name << " ; WL: " << Wl << " ; IWL: " << Iwl << " ; Mantissa: " << Mantissa;

	TmpString1 << " ; Floating-point value: " << TmpSign.str() << IntPart;

	// -----
	// WARNING:
	// To avoid a scientific conversion of the fractional part, the following code must be written:
	//
	// ostringstream TmpString2;				// used to edit the object (fractional part)
	// const int     DisplayPrecision = 10;
	//
	//     TmpString2.setf(ios_base::fixed, ios_base::floatfield);
	//
	// However, this code can't be compile with the gnu compiler 'g++ 2.95.3'
	// So, the code normally used to display the FixC instance characteristics:
	//
	//     TmpString2.setf(ios_base::fixed, ios_base::floatfield);
	//     TmpString2 << setprecision(DisplayPrecision) << FractPart;
	//     cout << TmpString1.str() << TmpString2.str().erase(0, 1) << endl;
	//
	// is replaced by a mix of C and C++ styles to format and display the characteristics.
	//
	// Note: the fixed representation of the fractional part gives '0.xxxxxx...'.
	//       The '0' character will not be displayed
	// -----

	sprintf(FractPartStr, "%.10f", FractPart);
	cout << TmpString1.str();
	printf("%s\n", &FractPartStr[1]);

}	// end of 'Display' method

// **************
// Friend methods
// **************

// ========================================================
// Initialization of a one-dimension array of FixC elements
// ========================================================

void InitFixC1d(FixC* X, int Size, const string& InstanceName, short Wl, short Iwl, const string& Fmt)
{
	int           I;			// loop index and array index
	ostringstream TmpString;	// used to generate the instance name

	// --------------------------------------------------------------------------------------
	// For each instance, initialization of: instance name, word length, integer word length,
	// management of overflow and quantization flags
	// --------------------------------------------------------------------------------------

	for ( I= 0 ; I < Size ; I++ )
	{
		TmpString.str("");
		TmpString << "_" << I;

		X[I].SetName(InstanceName + TmpString.str());
		X[I].SetWl(Wl);
		X[I].SetIwl(Iwl);
		X[I].SetFmt(Fmt);
	}
}

// =========================================================
// Initialization of a two-dimensions array of FixC elements
// =========================================================

void InitFixC2d(FixC* X, int SizeI, int SizeJ,  const string& InstanceName, short Wl, short Iwl, const string& Fmt)
{
	int           I;			// loop index and first dimension index
	int           J;			// loop index and second dimension index
	ostringstream TmpString;	// used to generate the instance name

	// --------------------------------------------------------------------------------------
	// For each instance, initialization of: instance name, word length, integer word length,
	// management of overflow and quantization flags 
	// --------------------------------------------------------------------------------------

	for ( I = 0 ; I < SizeI ; I++ )
	{
		for ( J = 0 ; J < SizeJ ; J++ )
		{
			TmpString.str("");
			TmpString << "_" << I << "_" << J;

			X[SizeJ * I + J].SetName(InstanceName + TmpString.str());
			X[SizeJ * I + J].SetWl(Wl);
			X[SizeJ * I + J].SetIwl(Iwl);
			X[SizeJ * I + J].SetFmt(Fmt);
		}
	}
}

// =========================================================
// Initialization of a two-dimensions array of FixC elements
// =========================================================

void InitFixC2d(FixC** X, int SizeI, int SizeJ,  const string& InstanceName, short Wl, short Iwl, const string& Fmt)
{
	int           I;			// loop index and first dimension index
	int           J;			// loop index and second dimension index
	ostringstream TmpString;	// used to generate the instance name

	// --------------------------------------------------------------------------------------
	// For each instance, initialization of: instance name, word length, integer word length,
	// management of overflow and quantization flags 
	// --------------------------------------------------------------------------------------

	for ( I = 0 ; I < SizeI ; I++ )
	{
		for ( J = 0 ; J < SizeJ ; J++ )
		{
			TmpString.str("");
			TmpString << "_" << I << "_" << J;

			X[I][J].SetName(InstanceName + TmpString.str());
			X[I][J].SetWl(Wl);
			X[I][J].SetIwl(Iwl);
			X[I][J].SetFmt(Fmt);
		}
	}
}
