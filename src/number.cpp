// $Id: AnyScalar.cc 59 2007-07-17 14:43:23Z tb $

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

/** \file AnyScalar.cc
 * Implementation of the typed scalar value class AnyScalar used by the parser
 * to represent values.
 */

#include "number.hpp"

#include <stdlib.h>
#include <functional>
#include <algorithm>

#include <boost/lexical_cast.hpp>

namespace lisp {
    std::string number::get_type_string(attrtype_t at)
    {
        switch(at)
        {
        case ATTRTYPE_LONG:		return "long";
        case ATTRTYPE_DOUBLE:	return "double";
        case ATTRTYPE_FRACTION:	return "fraction";
        }
        return "unknown";
    }

    number& number::operator=(const long long &l)
    {
        atype = ATTRTYPE_LONG;
        val._long = l;
        return *this;
    }

    number& number::operator=(const double &d)
    {
        atype = ATTRTYPE_DOUBLE;
        val._double = d;
        return *this;
    }

    number& number::set_fraction(int z, int n)
    {
        atype = ATTRTYPE_FRACTION;
        val._fraction.z = z;
        val._fraction.n = n;
        return *this;
    }

    long long number::as_long() const
    {
        switch(atype)
        {
        case ATTRTYPE_LONG:
            return val._long;

        case ATTRTYPE_DOUBLE:
            return static_cast<long long>(val._double);

        case ATTRTYPE_FRACTION:
            return val._fraction.z/val._fraction.n;
        }

        assert(0);
        return false;
    }

    double number::as_double() const
    {
        switch(atype)
        {
        case ATTRTYPE_LONG:
            return static_cast<double>(val._long);

        case ATTRTYPE_DOUBLE:
            return val._double;

        case ATTRTYPE_FRACTION:
            return  static_cast<double>(val._fraction.z)/
                static_cast<double>(val._fraction.n);
        }
        assert(0);
        return false;
    }

    std::string number::str() const
    {
        switch(atype)
        {
        case ATTRTYPE_LONG:
            return boost::lexical_cast<std::string>(val._long);
    
        case ATTRTYPE_DOUBLE:
            return boost::lexical_cast<std::string>(val._double);

        case ATTRTYPE_FRACTION:
            return  boost::lexical_cast<std::string>(val._fraction.z)
                + "/" +
                boost::lexical_cast<std::string>(val._fraction.n);
        }
        assert(0);
        return "";
    }

    bool number::convert_type(attrtype_t t)
    {
        // same source and target type?
        if (atype == t) return true;

        switch(t)
        {
    
        case ATTRTYPE_LONG:
        {
            long long v = as_long();
            val._long = v;
            atype = t;
            return true;
        }

        case ATTRTYPE_DOUBLE:
        {
            double d = as_double();
            val._double = d;
            atype = t;
            return true;
        }

        //This doesn't convert double back to fraction,
        //as it shouldn't be necessary
        case ATTRTYPE_FRACTION:
        {
            int z = as_long();
            val._fraction.z = z;
            val._fraction.n = 1;
            atype = t;
            return true;
        }
 
        }
        assert(0);
        return false;
    }

    number number::operator-() const
    {
        number at = *this;
    
        switch(at.atype)
        {
        case ATTRTYPE_LONG:
            at.val._long = - at.val._long;
            break;

        case ATTRTYPE_DOUBLE:
            at.val._double = - at.val._double;
            break;

        case ATTRTYPE_FRACTION:
            at.val._fraction.z = -at.val._fraction.z;
        }

        return at;
    }

    template <template <typename Type> class Operator, char OpName>
    number number::binary_arith_op(const number &b) const
    {
        switch(atype)
        {
        case ATTRTYPE_LONG:
        {
            switch(b.atype)
            {
            case ATTRTYPE_LONG:
            {
                Operator<long long> op;
                if (OpName == '/') {
                    if(b.val._long == 0)
                        throw arith_error("division by zero");
                    if(val._long%b.val._long != 0) {
                        number b2(b);
                        b2.convert_type(ATTRTYPE_FRACTION);
                        number a2(*this);
                        a2.convert_type(ATTRTYPE_FRACTION);
                        Operator<fraction> op2;
                        return number(op2(a2.val._fraction, b2.val._fraction));
                    }
                }

                return number( op(val._long, b.val._long) );
            }

            case ATTRTYPE_DOUBLE:
            {
                Operator<double> op;
                return number( op(static_cast<double>(val._long), b.val._double) );
            }

            case ATTRTYPE_FRACTION:
            {
                Operator<fraction> op;
                if (OpName == '/' && b.val._long == 0)
                    throw arith_error("division by zero");

                number a(*this);
                a.convert_type(ATTRTYPE_FRACTION);
                return number(op(a.val._fraction, b.val._fraction));
            }

            }
            break;
        }

        case ATTRTYPE_DOUBLE:
        {
            switch(b.atype)
            {
	    
            case ATTRTYPE_LONG:
            {
                Operator<double> op;
                return number( op(val._double, static_cast<double>(b.val._long)) );
            }

            case ATTRTYPE_DOUBLE:
            {
                Operator<double> op;
                return number( op(val._double, b.val._double) );
            }

            case ATTRTYPE_FRACTION:
            {
                Operator<double> op;
                number a(b);
                a.convert_type(ATTRTYPE_DOUBLE);
                return number( op(val._double, a.val._double) );
            }

            }

            break;
        }

        case ATTRTYPE_FRACTION:
        {
            switch(b.atype)
            {
	    
            case ATTRTYPE_LONG:
            {
                Operator<fraction> op;
                if (OpName == '/' && b.val._long == 0)
                    throw arith_error("division by zero");

                number a(b);
                a.convert_type(ATTRTYPE_FRACTION);
                return number(op(val._fraction, a.val._fraction));
            }

            case ATTRTYPE_DOUBLE:
            {
                Operator<double> op;
                number a(*this);
                a.convert_type(ATTRTYPE_DOUBLE);
                return number( op(a.val._double, b.val._double) );
            }

            case ATTRTYPE_FRACTION:
            {
                Operator<fraction> op;
                return number( op(val._fraction, b.val._fraction) );
            }

            }

            break;
        }

        }
        assert(0);
        //throw(ConversionException("Invalid binary operator call. (PROGRAM ERROR)"));
    }

// *** Force instantiation of binary_op for the four arithmetic operators

/// Forced instantiation of binary_arith_op for number::operator+()
    template number number::binary_arith_op<std::plus, '+'>(const number &b) const;

/// Forced instantiation of binary_arith_op for number::operator-()
    template number number::binary_arith_op<std::minus, '-'>(const number &b) const;

/// Forced instantiation of binary_arith_op for number::operator*()
    template number number::binary_arith_op<std::multiplies, '*'>(const number &b) const;

/// Forced instantiation of binary_arith_op for number::operator/()
    template number number::binary_arith_op<std::divides, '/'>(const number &b) const;

    template <template <typename Type> class Operator, int OpNum>
    bool number::binary_comp_op(const number &b) const
    {
        // static const char *OpNameArray[] = { "==", "!=", "<", ">", "<=", ">=" };

        switch(atype)
        {
        case ATTRTYPE_LONG:
        {
            switch(b.atype)
            {
            case ATTRTYPE_LONG:
            {
                Operator<long long> op;
                return op(val._long, b.val._long);
            }
            case ATTRTYPE_DOUBLE:
            {
                Operator<double> op;
                return op(static_cast<double>(val._long), b.val._double);
            }
	    case ATTRTYPE_FRACTION:
	    {
		Operator<const fraction&> op;
		fraction tmp = {val._long, 1};

		return op(tmp, b.val._fraction);
	    }
            }
            break;
        }

        case ATTRTYPE_DOUBLE:
        {
            switch(b.atype)
            {
            case ATTRTYPE_LONG:
            {
                Operator<double> op;
                return op(val._double, static_cast<double>(b.val._long));
            }

            case ATTRTYPE_DOUBLE:
            {
                Operator<double> op;
                return op(val._double, b.val._double);
            }

	    case ATTRTYPE_FRACTION:
	    {
		Operator<double> op;
		double tmp = val._double;

		return op(tmp, b.val._double);
	    }
            }
            break;
        }

        case ATTRTYPE_FRACTION:
        {
            switch(b.atype)
            {
            case ATTRTYPE_LONG:
            {
		Operator<const fraction&> op;
		fraction tmp = {b.val._long, 1};

		return op(val._fraction, tmp);
            }
            case ATTRTYPE_DOUBLE:
            {
                Operator<double> op;
                number a(*this);
                a.convert_type(ATTRTYPE_DOUBLE);
                return op(a.val._double, b.val._double);
            }
	    case ATTRTYPE_FRACTION:
	    {
		Operator<const fraction&> op;

		return op(val._fraction, b.val._fraction);
	    }
            }

            break;
        }
        }
        assert(0);
        //throw(ConversionException("Invalid binary operator call. (PROGRAM ERROR)"));
    }

/// Forced instantiation of binary_comp_op for number::equal_to()
    template bool number::binary_comp_op<std::equal_to, 0>(const number &b) const;

/// Forced instantiation of binary_comp_op for number::not_equal_to()
    template bool number::binary_comp_op<std::not_equal_to, 1>(const number &b) const;

/// Forced instantiation of binary_comp_op for number::less()
    template bool number::binary_comp_op<std::less, 2>(const number &b) const;

/// Forced instantiation of binary_comp_op for number::greater()
    template bool number::binary_comp_op<std::greater, 3>(const number &b) const;

/// Forced instantiation of binary_comp_op for number::less_equal()
    template bool number::binary_comp_op<std::less_equal, 4>(const number &b) const;

/// Forced instantiation of binary_comp_op for number::greater_equal()
    template bool number::binary_comp_op<std::greater_equal, 5>(const number &b) const;
}
