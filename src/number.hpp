/*
 * STX Expression Parser C++ Framework v0.7
 * Copyright (C) 2007 Timo Bingmann
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/** \file AnyScalar.h
 * Definition of a typed scalar value class AnyScalar used by the parser to
 * represent values.
 */

#ifndef _STX_AnyScalar_H_
#define _STX_AnyScalar_H_

#include <sstream>
#include <string>
#include <stdexcept>
#include <functional>
#include <ostream>
#include <assert.h>
#include <cstdlib>

#include "object.hpp"
#include "arith_error.hpp"
#include <iostream>
namespace lisp 
{

    //I don't know where to put this, so I leave it here, as everything includes number.hpp
    template <class T>
    std::string to_string(T Converted)
    {
    	std::stringstream buffer;
    	buffer << Converted;
    	return buffer.str();
    }

    template <class T>
    T from_string(std::string Converted)
    {
        std::stringstream buffer;
        T ret;
    	buffer << Converted;
        buffer >> ret;
    	return ret;
    }

    /** number constructs objects holding a typed scalar value. It supports
     * boolean values, integer values both signed and unsigned, floating point
     * values and strings. The class provides operators which will compare scalars
     * between other scalars by converting them into a common domain. Furthermore
     * arithmetic operator will compose one or two scalars where the calculation is
     * done in the "higher" domain. */
    class number : public object
    {
    public:

        /// Enumeration establishing identifiers for all supported types. All
        /// "small" integer types are included, because the class use originally
        /// designed for serializing large amounts of data.
        enum attrtype_t
        {
            /// Long (long) signed integer type, 8 bytes long.
            ATTRTYPE_LONG = 0,

            /// Double precision floating point type, 8 bytes long.
            ATTRTYPE_DOUBLE = 1,

            /// Fraction type
            ATTRTYPE_FRACTION = 2
        };

    private:
        /// The currently set type in the union.
        attrtype_t          atype;

        struct fraction {
            fraction operator+(const fraction& other) const
            {
                if(n == 0 || other.n == 0)
                    throw arith_error("division by zero");

                fraction f = {z*other.n + n*other.z,
                                n*other.n};
                f.reduce();
                return f;
            }

            fraction operator-(const fraction& other) const
            {
                if(n == 0 || other.n == 0)
                    throw arith_error("division by zero");

                fraction f = {z*other.n - n*other.z,
                                n*other.n};
                f.reduce();
                return f;
            }

            fraction operator*(const fraction& other) const
            {
                if(n == 0 || other.n == 0)
                    throw arith_error("division by zero");

                fraction f = {z*other.z, n*other.n};
                f.reduce();
                return f;
            }

            fraction operator/(const fraction& other) const
            {
                if(n == 0 || other.z == 0)
                    throw arith_error("division by zero");

                fraction f = {z*other.n, n*other.z};
                f.reduce();
                return f;
            }

            fraction operator-() const
            {
                fraction f = {-z, n};
                f.reduce();
                return f;
            }

            void reduce()
            {
                if(n < 0) {
                    n*=-1;
                    z*=-1;
                }

                int t = ggt(abs(n),abs(z));

                z/=t;
                n/=t;
            }

            int ggt(int a, int b)
            {
                while(b != 0)
                {
                    int temp = a % b;
                    a = b;
                    b = temp;
                }

                return a;
            }

            int z;
            int n;

        };

        /// Union type to holding the current value of an number.
        union value_t
        {
            /// Used for ATTRTYPE_LONG
            long long               _long;

            /// Used for ATTRTYPE_DOUBLE
            double                  _double;

            /// Used for ATTRTYPE_FRACTION
            fraction                _fraction;
        };

        /// Union holding the current value of set type.
        union value_t       val;
    
    public:
        /// Create a new empty number object of given type.
        explicit inline number(attrtype_t t = ATTRTYPE_LONG)
            : atype(t)
            { 
                val._long = 0;
            }

        // *** Constructors for the various types

        /// Construct a new number object of type ATTRTYPE_LONG and set the
        /// given long value.
        inline number(long long l)
            : atype(ATTRTYPE_LONG)
            {
                val._long = l;
            }

        /// Construct a new number object of type ATTRTYPE_DOUBLE and set the
        /// given floating point value.
        inline number(double d)
            : atype(ATTRTYPE_DOUBLE)
            {
                val._double = d;
            }

        /// Construct a new number object of type ATTRTYPE_FRACTION and set the
        /// given value.
        inline number(int z, int n)
            : atype(ATTRTYPE_FRACTION)
            {
                val._fraction.z = z;
                val._fraction.n = n;
            }

        number(fraction f) : atype(ATTRTYPE_FRACTION)
            {
                val._fraction.z = f.z;
                val._fraction.n = f.n;
            }
    
        /// Copy-constructor to deal with enclosed strings. Transfers type and
        /// value.
        inline number(const number &a)
            : atype(a.atype)
            {
                //val = a.val;

                switch(atype)
                {
                case ATTRTYPE_LONG:
                    val._long = a.val._long;
                    break;

                case ATTRTYPE_DOUBLE:
                    val._double = a.val._double;
                    break;

                case ATTRTYPE_FRACTION:
                    val._fraction = a.val._fraction;
                    break;
                }
            }

        /// Assignment operator to deal with enclosed strings. Transfers type and
        /// value.
        inline number& operator=(const number &a)
            {
                // check if we are to assign ourself
                if (this == &a) return *this;

                atype = a.atype;
        
                switch(atype)
                {
                case ATTRTYPE_LONG:
                    val._long = a.val._long;
                    break;

                case ATTRTYPE_DOUBLE:
                    val._double = a.val._double;
                    break;

                case ATTRTYPE_FRACTION:
                    val._fraction = a.val._fraction;
                    break;
                }

                return *this;
            }

        /// Return the type identifier of the object.
        inline attrtype_t   getType() const
            {
                return atype;
            }

        /// Returns true if this object contains one of the integer types.
        inline bool         isIntegerType() const
            {
                return (atype == ATTRTYPE_LONG);
            }

        /// Returns true if this object contains one of the floating point types.
        inline bool         isFloatingType() const
            {
                return (atype == ATTRTYPE_DOUBLE);
            }

        /// Returns true if this object contains one of the fraction types.
        inline bool         isFractionType() const
            {
                return (atype == ATTRTYPE_FRACTION);
            }

        /// Attempts to convert the current type/value into the given type. Returns
        /// false if the value could not be represented in the new type, the new
        /// type is set nonetheless. See the getXXX below on how the new value is
        /// computed from the old type.
        bool        convert_type(attrtype_t t);

        /// Returns a const char* for each attrtype_t.
        static std::string get_type_string(attrtype_t at);

        /// Returns a string for this number's type.
        inline std::string get_type_string() const
            {
                return get_type_string(atype);
            }

        // *** Setters

        // Sets the value
        number& operator=(const long long &l);
        number& operator=(const double &d);
        number& set_fraction(int z, int n);

        /** Return the value converted to a long integer. If the enclosed value
         * could not be converted this function throws a ConversionException.
         *
         * - boolean: returns 0 or 1.
         * - char/short/int: return the integer (possibly representing a negative number as positive)
         * - byte/word/dword: return the unsigned integer
         * - long: no conversion
         * - qword: return the integer (possibly converting a large long to a negative)
         * - float/double: machine-conversion floating point to long long.
         * - string: Attempt to strtoll the string, throws ConversionException if this fails.
         */
        long long           as_long() const;

        /** Return the value converted to a double. If the enclosed value could not
         * be converted this function throws a ConversionException.
         *
         * - boolean: returns 0 or 1.0.
         * - char/short/int/long: return the integer as a float
         * - byte/word/dword/qword: return the unsigned integer as a float
         * - float: cast to double.
         * - double: no conversion
         * - string: Attempt to strtod the string, throws ConversionException if this fails.
         */
        double              as_double() const;

        /** Return the value converted to a string. This function never throws
         * ConversionException.
         *
         * - boolean: returns "false" or "true"
         * - char/short/int/long: return the integer as a string using boost::lexical_cast
         * - byte/word/dword/qword: return the unsigned integer as a string using boost::lexical_cast
         * - float/double: return float as a string using boost::lexical_cast
         * - string: return the string.
         */
        std::string         str() const;

        // *** Unary Operators
    
        number           operator-() const;

        // *** Binary Operators

        // These will convert the two operands to the largest common type of the
        // same field.

    private:
        /** Binary arithmetic template operator. Converts the two AnyScalars into the
         * largest type of their common field. If a string cannot be converted to a
         * numeric of the same field as the other operand a ConversionException is
         * thrown.
     
         * Conversion chart: * signifies the operator
     
         - long * long: returned number is a signed long (long).
         - fraction * long|fraction: returned number is a fraction
         - double * long|double|fraction: calculate as floating point number
        */
        template <template <typename Type> class Operator, char OpName>
        number           binary_arith_op(const number &b) const;
    
    public:
        /// Instantiation of binary_arith_op for "+" plus.
        inline number    operator+(const number &b) const
            {
                return binary_arith_op<std::plus, '+'>(b);
            }

        /// Instantiation of binary_arith_op for "-" minus.
        inline number    operator-(const number &b) const
            {
                return binary_arith_op<std::minus, '-'>(b);
            }

        /// Instantiation of binary_arith_op for "*" multiplication.
        inline number    operator*(const number &b) const
            {
                return binary_arith_op<std::multiplies, '*'>(b);
            }

        /// Instantiation of binary_arith_op for "/" division.
        inline number    operator/(const number &b) const
            {
                return binary_arith_op<std::divides, '/'>(b);
            }

    private:
        /** Binary comparison template operator. Converts the two AnyScalars into the
         * largest type of their common field. If a string cannot be converted to a
         * numeric of the same field as the other operand a ConversionException is
         * thrown. Operator is the STL operator class. OpNum is an identifier for
         * the operator name string within the template.

         * Conversion chart: * signifies the operator
     
         - long * long: compare as signed long integer
         - double * long: compare as floating point numbers
        */
        template <template <typename Type> class Operator, int OpNum>
        bool                binary_comp_op(const number &b) const;

    public:
        /// Instantiation of binary_comp_op for "==" equality.
        inline bool         operator==(const number &b) const
            {
                return binary_comp_op<std::equal_to, 0>(b);
            }

        /// Instantiation of binary_comp_op for "!=" inequality.
        inline bool         operator!=(const number &b) const
            {
                return binary_comp_op<std::not_equal_to, 1>(b);
            }

        /// Instantiation of binary_comp_op for "<" less-than.
        inline bool         operator<(const number &b) const
            {
                return binary_comp_op<std::less, 2>(b);
            }

        /// Instantiation of binary_comp_op for ">" greater-than.
        inline bool         operator>(const number &b) const
            {
                return binary_comp_op<std::greater, 3>(b);
            }

        /// Instantiation of binary_comp_op for "<=" less-or-equal-than.
        inline bool         operator<=(const number &b) const
            {
                return binary_comp_op<std::less_equal, 4>(b);
            }

        /// Instantiation of binary_comp_op for ">=" greater-or-equal-than.
        inline bool         operator>=(const number &b) const
            {
                return binary_comp_op<std::greater_equal, 5>(b);
            }

        // virtual bool is_int() const
        //     {
        //         return false;
        //     }

        // virtual bool is_float() const
        //     {
        //         return false;
        //     }

        bool is_number() const
            {
                return true;
            }
    };

    typedef boost::shared_ptr<number> number_ptr_t;
}

#endif // VGS_AnyScalar_H
