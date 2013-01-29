//-----------------------------------------------------------------------------------------
#if 0

Fix8 is released under the GNU LESSER GENERAL PUBLIC LICENSE Version 3.

Fix8 Open Source FIX Engine.
Copyright (C) 2010-13 David L. Dight <fix@fix8.org>

Fix8 is free software: you can  redistribute it and / or modify  it under the  terms of the
GNU Lesser General  Public License as  published  by the Free  Software Foundation,  either
version 3 of the License, or (at your option) any later version.

Fix8 is distributed in the hope  that it will be useful, but WITHOUT ANY WARRANTY;  without
even the  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

You should  have received a copy of the GNU Lesser General Public  License along with Fix8.
If not, see <http://www.gnu.org/licenses/>.

BECAUSE THE PROGRAM IS  LICENSED FREE OF  CHARGE, THERE IS NO  WARRANTY FOR THE PROGRAM, TO
THE EXTENT  PERMITTED  BY  APPLICABLE  LAW.  EXCEPT WHEN  OTHERWISE  STATED IN  WRITING THE
COPYRIGHT HOLDERS AND/OR OTHER PARTIES  PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY
KIND,  EITHER EXPRESSED   OR   IMPLIED,  INCLUDING,  BUT   NOT  LIMITED   TO,  THE  IMPLIED
WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO
THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE,
YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

IN NO EVENT UNLESS REQUIRED  BY APPLICABLE LAW  OR AGREED TO IN  WRITING WILL ANY COPYRIGHT
HOLDER, OR  ANY OTHER PARTY  WHO MAY MODIFY  AND/OR REDISTRIBUTE  THE PROGRAM AS  PERMITTED
ABOVE,  BE  LIABLE  TO  YOU  FOR  DAMAGES,  INCLUDING  ANY  GENERAL, SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT
NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR
THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH
HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#endif
//-----------------------------------------------------------------------------------------
#ifndef _F8_UTILS_HPP_
#define _F8_UTILS_HPP_

#include <tbb/atomic.h>
#include <tbb/mutex.h>
#include <Poco/DateTime.h>
#include <sys/ioctl.h>
#include <termios.h>

namespace FIX8 {

//----------------------------------------------------------------------------------------
/*! In place string to upper case.
  \param src source string
  \return reference to modified string */
std::string& InPlaceStrToUpper(std::string& src);

/*! In place string to lower case.
  \param src source string
  \return reference to modified string */
std::string& InPlaceStrToLower(std::string& src);

/*! Check if string has trailing slash, if not add.
  \param source source string
  \return reference to modified string */
std::string& CheckAddTrailingSlash(std::string& source);

/*! Replace any character found in the supplied set in string with supplied character
  \param iset set of characters
  \param src source string
  \param repl character to replace
  \return reference to modified string */
std::string& InPlaceReplaceInSet(const std::string& iset, std::string& src, const char repl='_');

/*! Find standard error string for given errno.
  \param err errno value
  \param str if not 0, prepend string to error string
  \return error string */
std::string Str_error(const int err, const char *str=0);

/*! Format Tickval into string.
  \param result target string
  \param tv tickval to use or 0 for current time
  \param dplaces number of decimal places to report seconds (default 6)
  \return reference to target string */
const std::string& GetTimeAsStringMS(std::string& result, const class Tickval *tv=0, const unsigned dplaces=6);

/*! Trim leading and trailing whitespace from a string, inplace.
  \param source source string
  \param ws string containing whitespace characters to trim out
  \return trimmed string */
const std::string& trim(std::string& source, const std::string& ws = " \t");

//----------------------------------------------------------------------------------------
/*! Rotate a value by the specified number of bits
  \tparam T type
  \param val source value
  \param bits number of bits to rotate by
  \return the rotated value */
template<typename T>
inline T rotl(const T val, const int bits) { return val << bits | val >> (sizeof(T) * 8 - bits); }

/*! Generate a rot13 hash. No multiplication, algorithm by Serge Vakulenko. See http://vak.ru/doku.php/proj/hash/sources.
  \param str source string
  \return hash value */
inline unsigned ROT13Hash (const std::string& str)
{
	unsigned int hash(0);

	for (std::string::const_iterator itr(str.begin()); itr != str.end(); ++itr)
	{
		hash += *itr;
		hash -= rotl(hash, 13);
	}

	return hash;
}

//----------------------------------------------------------------------------------------
/*! case insensitive std::string == std::string operator
  \tparam _CharT char type
  \tparam _Traits char traits
  \tparam _Alloc allocator
  \param __lhs left hand value
  \param __rhs right hand value
  \return true if strings are equivalent */
template<typename _CharT, typename _Traits, typename _Alloc>
	inline bool operator% (const std::basic_string<_CharT, _Traits, _Alloc>& __lhs,
		const std::basic_string<_CharT, _Traits, _Alloc>& __rhs)
			{ return strcasecmp(__lhs.c_str(), __rhs.c_str()) == 0; }

/*! case insensitive char* == std::string operator
  \tparam _CharT char type
  \tparam _Traits char traits
  \tparam _Alloc allocator
  \param __lhs left hand value
  \param __rhs right hand value
  \return true if strings are equivalent */
template<typename _CharT, typename _Traits, typename _Alloc>
	inline bool operator% (const _CharT* __lhs, const std::basic_string<_CharT, _Traits, _Alloc>& __rhs)
		{ return strcasecmp(__lhs, __rhs.c_str()) == 0; }

/*! case insensitive std::string == char* operator
  \tparam _CharT char type
  \tparam _Traits char traits
  \tparam _Alloc allocator
  \param __lhs left hand value
  \param __rhs right hand value
  \return true if strings are equivalent */
template<typename _CharT, typename _Traits, typename _Alloc>
	inline bool operator% (const std::basic_string<_CharT, _Traits, _Alloc>& __lhs, const _CharT* __rhs)
		{ return strcasecmp(__lhs.c_str(), __rhs) == 0; }

/*! case insensitive std::string < std::string operator
  \tparam _CharT char type
  \tparam _Traits char traits
  \tparam _Alloc allocator
  \param __lhs left hand value
  \param __rhs right hand value
  \return true if lhs < rhs */
template<typename _CharT, typename _Traits, typename _Alloc>
	inline bool operator^ (const std::basic_string<_CharT, _Traits, _Alloc>& __lhs,
		const std::basic_string<_CharT, _Traits, _Alloc>& __rhs)
			{ return strcasecmp(__lhs.c_str(), __rhs.c_str()) < 0; }

//----------------------------------------------------------------------------------------
/// C++11 inspired scoped pointer.
/*! \tparam T typename */
template <typename T>
class scoped_ptr
{
	T *ptr_;

	/// Copy Ctor. Non-copyable.
	scoped_ptr(const scoped_ptr&);

	/// Assignment operator. Non-copyable.
	void operator=(const scoped_ptr&);

public:
	/*! Ctor.
	  \param p pointer to T */
	explicit scoped_ptr(T *p=0) : ptr_(p) {}

	/// Dtor. Destroys object.
	~scoped_ptr() { delete ptr_; }

	/*! Equivalence operator (other is scoped_ptr)
	  \tparam U type of that object
	  \return true if objects are equivalent */
	template <typename U>
	bool operator==(const scoped_ptr<U>& that) const { return ptr_ == that.get(); }

	/*! Equivalence operator (other is ptr)
	  \tparam U type of that object
	  \return true if objects are equivalent */
	template <typename U>
	bool operator==(const scoped_ptr<U> *that) const { return ptr_ == that; }

	/*! Non-equivalence operator (other is scoped_ptr)
	  \tparam U type of that object
	  \return true if objects are not equal */
	template <typename U>
	bool operator!=(const scoped_ptr<U>& that) const { return ptr_ != that.get(); }

	/*! Non-equivalence operator (other is scoped_ptr)
	  \tparam U type of that object
	  \return true if objects are not equal */
	template <typename U>
	bool operator!=(const scoped_ptr<U> *that) const { return ptr_ != that; }

	/*! Equivalence operator (other is scoped_ptr)
	  \return true if objects are equivalent */
	bool operator==(const scoped_ptr<T>& that) const { return ptr_ == that.get(); }

	/*! Equivalence operator (other is ptr)
	  \return true if objects are equivalent */
	bool operator==(const T *that) const { return (ptr_ == that); }

	/*! Non-equivalence operator (other is scoped_ptr)
	  \return true if objects are not equal */
	bool operator!=(const scoped_ptr<T>& that) const { return ptr_ != that.get(); }

	/*! Non-equivalence operator (other is ptr)
	  \return true if objects are not equal */
	bool operator!=(const T *that) const { return ptr_ != that; }

	/*! Member selection operator.
	  \return pointer to object */
	T *operator->() const { return ptr_; }

	/*! Member dereference operator.
	  \return object */
	T& operator*() const { return *ptr_; }

	/*! Member dereference operator.
	  \return object */
	T *release() { T *tmp(ptr_); ptr_ = 0; return tmp; }

	/*! Replace the pointer with the supplied pointer.
	  \return the original pointer */
	T *Reset(T *p=0) { delete ptr_; return ptr_ = p; }

	/*! Get the object pointer.
	  \return object */
	T *get() const { return ptr_; }
};

//----------------------------------------------------------------------------------------
/// A class to contain regex matches using RegExp.
class RegMatch
{
	/// Maximum number of sub-expressions.
	enum { SubLimit_ = 32 };

	regmatch_t subexprs_[SubLimit_];
	int subCnt_;

public:
	/// Ctor.
	RegMatch() : subexprs_(), subCnt_() {}

	/// Ctor.
	virtual ~RegMatch() {}

	/*! Get the number of sub-expressions found.
	  \return number of sub-expression */
	unsigned SubCnt() const { return subCnt_; }

	/*! Get the size (length) of the specified sub-expression.
	  \param which sub-expression index (0 based)
	  \return size of sub-expression, -1 if not found */
	size_t SubSize(const int which=0) const
		{ return which < subCnt_ ? subexprs_[which].rm_eo - subexprs_[which].rm_so : -1; }

	/*! Get the starting offset of the specified sub-expression.
	  \param which sub-expression index (0 based)
	  \return offset of the sub-expression, -1 if not found */
	unsigned SubPos(const int which=0) const
		{ return which < subCnt_ ? subexprs_[which].rm_so : -1; }

	friend class RegExp;
};

//----------------------------------------------------------------------------------------
/// POSIX regex wrapper class.
class RegExp
{
	/// Maximum length of an error message.
	enum { MaxErrLen_ = 256 };

	regex_t reg_;
	const std::string pattern_;
	std::string errString;
	int errCode_;

public:
	/*! Ctor.
	  \param pattern regular expression to compile. errCode and errString set on error.
	  \param flags POSIX regcomp flags */
	RegExp(const char *pattern, const int flags=0) : pattern_(pattern)
	{
		if ((errCode_ = regcomp(&reg_, pattern_.c_str(), REG_EXTENDED|flags)) != 0)
		{
			char rbuf[MaxErrLen_];
			regerror(errCode_, &reg_, rbuf, MaxErrLen_);
			errString = rbuf;
		}
	}

	/// Dtor. Destroys internal compiled expression.
	virtual ~RegExp() { if (errCode_ == 0) regfree(&reg_); }

	/*! Search a string.
	  \param match reference to a RegMatch object
	  \param source string to search
	  \param subExpr number of sub-expression
	  \param offset to start searching
	  \return number of sub-expressions found (0=none) */
	int SearchString(RegMatch& match, const std::string& source, const int subExpr, const int offset=0) const
	{
		match.subCnt_ = 0;
		if (regexec(&reg_, source.c_str() + offset, subExpr <= RegMatch::SubLimit_ ? subExpr : RegMatch::SubLimit_, match.subexprs_, 0) == 0)
			while (match.subCnt_ < subExpr && match.subexprs_[match.subCnt_].rm_so != -1)
				++match.subCnt_;
		return match.subCnt_;
	}

	/*! Extract a sub-expression.
	  \param match reference to a RegMatch object
	  \param source source string
	  \param target location to place sub-experssion
	  \param offset to start searching
	  \param num desired sub-expression
	  \return the target string */
	static std::string& SubExpr(RegMatch& match, const std::string& source, std::string& target, const int offset=0, const int num=0)
	{
		if (num < match.subCnt_)
			target = source.substr(offset + match.subexprs_[num].rm_so, match.subexprs_[num].rm_eo - match.subexprs_[num].rm_so);
		else
			target.empty();
		return target;
	}

	/*! Erase a sub-expression from a string.
	  \param match reference to a RegMatch object
	  \param source source string
	  \param num desired sub-expression
	  \return the source string */
	static std::string& Erase(RegMatch& match, std::string& source, const int num=0)
	{
		if (num < match.subCnt_)
			source.erase(match.subexprs_[num].rm_so, match.subexprs_[num].rm_eo - match.subexprs_[num].rm_so);
		return source;
	}

	/*! Replace a sub-expression with a string.
	  \param match reference to a RegMatch object
	  \param source source string
	  \param with replacement string
	  \param num desired sub-expression
	  \return the source string */
	static std::string& Replace(RegMatch& match, std::string& source, const std::string& with, const int num=0)
	{
		if (num < match.subCnt_)
			source.replace(match.subexprs_[num].rm_so, match.subexprs_[num].rm_eo - match.subexprs_[num].rm_so, with);
		return source;
	}

	/*! Get the regular expression pattern.
	  \return the pattern string */
	const std::string& GetPattern() const { return pattern_; }

	/*! Get the error string (set when Ctor fails to compile).
	  \return the error string */
	const std::string& ErrString() const { return errString; }

	/*! Check if a pattern did not compile ok.
	  \return true on failure */
	bool operator!() const { return errCode_; }

	/*! Check if a pattern compiled ok.
	  \return true onsuccess */
	operator void*() { return errCode_ ? 0 : this; }
};

//----------------------------------------------------------------------------------------
/// Case-insensitive string comparison, pointer version.
struct StringPtrLessThanNoCase
{
	bool operator()(const std::string *a, const std::string *b) const
		{ return *a ^ *b; }
};

/// Case-sensitive string comparison, pointer version.
struct StringPtrLessThan
{
	bool operator()(const std::string *a, const std::string *b) const
		{ return *a < *b; }
};

/// Case-insensitive string comparison.
struct StringLessThanNoCase
{
	bool operator()(const std::string& a, const std::string& b) const
		{ return a ^ b; }
};

//----------------------------------------------------------------------------------------
/*! Extract a typed value from a string.
  \tparam typename
  \param source source string
  \param defval value to return if source string is empty
  \return the extracted value. */
template<typename T>
inline T GetValue(const std::string& source, T defval)
{
	if (source.empty())
		return defval;
	std::istringstream istr(source);
	T result(defval);
	istr >> result;
	return result;
}

/*! Extract a typed value from a string.
  \tparam typename
  \param source source string
  \return the extracted value. */
template<typename T>
inline T GetValue(const std::string& source)
{
	std::istringstream istr(source);
	T result;
	istr >> result;
	return result;
}

/*! Extract a bool value from a string.
  \tparam typename
  \param source source string
  \return the extracted value. */
template<>
inline bool GetValue(const std::string& source)
{
	if (source.empty())
		return false;
#if !defined XMLENTITY_STRICT_BOOL
	return source % "true" || source % "yes" || source % "y" || source == "1";
#else
	bool result(false);
	std::istringstream istr(source);
	istr >> std::boolalpha >> result;
	return result;
#endif
}

/*! Insert a typed value into a string.
  \tparam typename
  \param a source value
  \param target string to place result
  \return the inserted string */
template<typename T>
inline const std::string& PutValue(const T& a, std::string& target)
{
	std::ostringstream ostr;
	ostr << a;
	return target = ostr.str();
}

//----------------------------------------------------------------------------------------
/*! Decode a string into an int or unsigned.
  \tparam typename
  \param str source string
  \return the converted value */
template<typename T>
T fast_atoi(const char *str)
{
	T retval(0);
	for (; *str; ++str)
		retval = (retval << 3) + (retval << 1) + *str - '0';
	return retval;
}


//----------------------------------------------------------------------------------------
/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukas Chmela
 * Released under GPLv3.
 * see http://www.strudel.org.uk/itoa
 */
inline size_t itoa(int value, char *result, const int base=10)
{
	// check that the base if valid
	if (base < 2 || base > 36)
	{
		*result = 0;
		return 0;
	}

	char *ptr(result), *ptr1(result);
	int tmp_value;

	do
	{
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	}
	while (value);

	// Apply negative sign
	if (tmp_value < 0)
		*ptr++ = '-';
	*ptr-- = 0;
	while(ptr1 < ptr)
	{
		const char tmp_char(*ptr);
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
	return ::strlen(result);
}

//----------------------------------------------------------------------------------------
/*! Simple and fast atof (ascii to float) function.
Executes about 5x faster than standard MSCRT library atof().
An attractive alternative if the number of calls is in the millions.
Assumes input is a proper integer, fraction, or scientific format.
Matches library atof() to 15 digits (except at extreme exponents).
Follows atof() precedent of essentially no error checking.
09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
	\param p source string
	\return double converted value */
inline double fast_atof (const char *p)
{
	bool frac(false);
	double sign(1.), value(0.), scale(1.);

	while (isspace(*p))
		++p;

	// Get sign, if any.
	if (*p == '-')
	{
		sign = -1.;
		++p;
	}
	else if (*p == '+')
		++p;

	// Get digits before decimal point or exponent, if any.
	while (isdigit(*p))
	{
		value = value * 10. + (*p - '0');
		++p;
	}

	// Get digits after decimal point, if any.
	if (*p == '.')
	{
		++p;
		double pow10(10.);
		while (isdigit(*p))
		{
			value += (*p - '0') / pow10;
			pow10 *= 10.;
			++p;
		}
	}

	// Handle exponent, if any.
	if (toupper(*p) == 'E')
	{
		unsigned int expon(0);
		++p;

	// Get sign of exponent, if any.
		if (*p == '-')
		{
			frac = true;
			++p;
		}
		else if (*p == '+')
			++p;

	// Get digits of exponent, if any.
		while (isdigit(*p))
		{
			expon = expon * 10 + (*p - '0');
			++p;
		}
		if (expon > 308)
			expon = 308;

	// Calculate scaling factor.
		while (expon >= 50)
		{
			scale *= 1E50;
			expon -= 50;
		}
		while (expon >= 8)
		{
			scale *= 1E8;
			expon -=  8;
		}
		while (expon > 0)
		{
			scale *= 10.0;
			expon -=  1;
		}
	}

	// Return signed and scaled floating point result.
	return sign * (frac ? (value / scale) : (value * scale));
}

//----------------------------------------------------------------------------------------
/// Bitset for enums.
/*! \tparam T the enum type
    \tparam B the integral type of the enumeration */
template<typename T, typename B=unsigned int>
class ebitset
{
	typedef B integral_type;
	integral_type a_;

public:
	/// Ctor.
	ebitset() : a_() {}

	/*! Ctor.
	    \param from ebitset_r to copy */
	ebitset(const ebitset<T, B>& from) : a_(from.a_) {}

	/*! Ctor.
	    \param a integral type to construct from */
	explicit ebitset(const integral_type a) : a_(a) {}

	/*! Ctor.
	    \param sbit enum to construct from */
	explicit ebitset(const T sbit) : a_((1 << sbit) - 1) {}

	/*! Assignment operator.
	    \param that ebitset_r to assign from
	    \return  this */
	ebitset<T, B>& operator=(const ebitset<T, B>& that)
	{
		if (this != &that)
			a_ = that.a_;
		return *this;
	}

	/*! Check if an enum is in the set.
	    \param sbit enum to check
	    \return integral_type of bits if found */
	integral_type has(const T sbit) { return a_ & 1 << sbit; }

	/*! Check if an enum is in the set.
	    \param sbit enum to check
	    \return integral_type of bits if found */
	integral_type operator&(const T sbit) { return a_ & 1 << sbit; }

	/*! Set a bit on or off.
	    \param sbit enum to set
	    \param on set on or off */
	void set(const T sbit, bool on=true) { if (on) a_ |= 1 << sbit; else a_ &= ~(1 << sbit); }

	/*! Set a bit on or off.
	    \param bset integral_type to set */
	void set(const integral_type bset) { a_ = bset; }

	/*! From a set of strings representing the names of each bit in order, set the named bit on.
	    \param els number of elements in set
	    \param sset the set of strings
	    \param what the string to find and set
	    \return true if found and set */
	bool set(const unsigned els, const std::string *sset, const std::string& what)
	{
		const std::string *last(sset + els), *result(std::find(sset, last, what));
		if (result == last)
			return false;
		set(static_cast<T>(std::distance(sset, result)));
		return true;
	}

	/*! Clear a bit on or off.
	    \param sbit enum to set */
	void clear(const T sbit) { a_ &= ~(1 << sbit); }

	/// Clear all bits.
	void clearall() { a_ = 0; }

	/*! Set all bits to a value.
	    \param sbit value to set to */
	void setall(const T sbit) { a_ = (1 << sbit) - 1; }

	/*! Get the enum integral_type.
	    \return integral_type of enum */
	integral_type get() const { return a_; }

	/*! Or a bit value with the current set.
	    \param sbit to set */
	void operator|=(const T sbit) { a_ |= 1 << sbit; }

	/*! Or a bit value with the current set.
	    \param sbit to set
	    \return ebitset */
	ebitset& operator<<(const T sbit) { a_ |= 1 << sbit; return *this; }
	//friend ebitset operator|(const T lbit, const T rbit) { return ebitset(lbit) |= 1 << rbit; }
};

/// Atomic bitset for enums.
/*! \tparam T the enum type
    \tparam B the integral type of the enumeration */
template<typename T, typename B=unsigned int>
class ebitset_r
{
	typedef B integral_type;
	tbb::atomic<integral_type> a_;

public:
	/// Ctor.
	ebitset_r() { a_ = 0; }

	/*! Ctor.
	    \param from ebitset_r to copy */
	ebitset_r(const ebitset_r<T, B>& from) { a_ = from.a_; }

	/*! Ctor.
	    \param a integral type to construct from */
	explicit ebitset_r(const integral_type a) { a_ = a; }

	/*! Ctor.
	    \param sbit enum to construct from */
	explicit ebitset_r(const T sbit) { a_ = (1 << sbit) - 1; }

	/*! Assignment operator.
	    \param that ebitset_r to assign from
	    \return  this */
	ebitset_r<T, B>& operator=(const ebitset_r<T, B>& that)
	{
		if (this != &that)
			a_ = that.a_;
		return *this;
	}

	/*! Check if an enum is in the set.
	    \param sbit enum to check
	    \return integral_type of bits if found */
	integral_type has(const T sbit) { return a_ & 1 << sbit; }

	/*! Check if an enum is in the set.
	    \param sbit enum to check
	    \return integral_type of bits if found */
	integral_type operator&(const T sbit) { return a_ & 1 << sbit; }

	/*! Set a bit on or off.
	    \param sbit enum to set
	    \param on set on or off */
	void set(const T sbit, bool on=true) { if (on) a_ |= 1 << sbit; else a_ &= ~(1 << sbit); }

	/*! From a set of strings representing the names of each bit in order, set the named bit on.
	    \param els number of elements in set
	    \param sset the set of strings
	    \param what the string to find and set
	    \return true if found and set */
	bool set(const unsigned els, const std::string *sset, const std::string& what)
	{
		const std::string *last(sset + els), *result(std::find(sset, last, what));
		if (result == last)
			return false;
		set(static_cast<T>(std::distance(sset, result)));
		return true;
	}

	/*! Set a bit on or off.
	    \param bset integral_type to set */
	void set(const integral_type bset) { a_ = bset; }

	/*! Clear a bit on or off.
	    \param sbit enum to set */
	void clear(const T sbit) { integral_type a = a_; a &= ~(1 << sbit); a_ = a; }

	/// Clear all bits.
	void clearall() { a_ = 0; }

	/*! Set all bits to a value.
	    \param sbit value to set to */
	void setall(const T sbit) { a_ = (1 << sbit) - 1; }

	/*! Get the enum integral_type.
	    \return integral_type of enum */
	integral_type get() const { return a_; }

	/*! Or a bit value with the current set.
	    \param sbit to set */
	void operator|=(const T sbit) { integral_type a = a_; a |= 1 << sbit; a_ = a; }

	/*! And a bit value with the current set.
	    \param sbit to set */
	void operator&=(const T sbit) { integral_type a = a_; a &= 1 << sbit; a_ = a; }

	/*! Or a bit value with the current set.
	    \param sbit to set
	    \return ebitset_r */
	ebitset_r& operator<<(const T sbit) { a_ |= 1 << sbit; return *this; }
};

//----------------------------------------------------------------------------------------
/*! Check for file existance.
    \param fname filename to check
    \return true if file exists */
inline bool exist(const std::string& fname)
{
	return access(fname.c_str(), F_OK) == 0;
}

//----------------------------------------------------------------------------------------
/*! Copy a string safely to a target.
    \param src source string
    \param target target location
    \param limit maximum bytes to copy, 0 for no limit
    \return pointer to target */
inline char *CopyString(const std::string& src, char *target, unsigned limit=0)
{
   if (!target)
      return 0;
   unsigned sz(limit && src.size() > limit ? limit : src.size() + 1);
   src.copy(target, sz - 1);
   target[sz - 1] = 0;
   return target;
}

//----------------------------------------------------------------------------------------
/*! A more reliable nanosleep.
    \param tspec timespec to sleep
    \return 0 on success */
inline int rnanosleep (const timespec& tspec)
{
#if defined HAVE_CLOCK_NANOSLEEP
   timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   ts.tv_sec += tspec.tv_sec;
   ts.tv_nsec += tspec.tv_nsec;
   if (ts.tv_nsec >= 1000000000)
   {
      ++ts.tv_sec;
      ts.tv_nsec -= 1000000000;
   }
	return clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, 0);
#else
	return nanosleep(&tspec, 0);
#endif
}

//----------------------------------------------------------------------------------------
/*! Sleep the specified number of milliseconds.
    \param ms time to sleep in milliseconds
    \return 0 on success */
inline int millisleep (const unsigned ms)
{
	const timespec tspec = { ms / 1000, 1000 * 1000 * (ms % 1000) };
	return rnanosleep(tspec);
}

//----------------------------------------------------------------------------------------
/*! Sleep the specified number of microseconds.
    \param us time to sleep in microseconds
    \return 0 on success */
inline int microsleep (const unsigned us)
{
	const timespec tspec = { us / (1000 * 1000), 1000 * (us % (1000 * 1000)) };
	return rnanosleep(tspec);
}

//----------------------------------------------------------------------------------------
/// Delete ptr.
struct DeleteObject
{
	/*! delete ptr operator
		 \tparam T typename
		 \param m object to delete */
	template<typename T>
	void operator()(const T& m) const { delete m; }
};

/// Delete array.
struct DeleteArrayObject
{
	/*! delete array operator
		 \tparam T typename
		 \param m object to delete */
	template<typename T>
	void operator()(const T& m) const { delete[] m; }
};

/// Delete 1st of pair.
template <typename Deleter = DeleteObject>
struct Delete1stPairObject
{
	/*! delete 1st of a std::pair operator
		 \tparam T typename
		 \param m object to delete */
	template<typename A, typename B>
	void operator()(const std::pair<A, B>& m) const { Deleter()(m.first); }
};

/// Delete 2nd of pair.
template <typename Deleter = DeleteObject>
struct Delete2ndPairObject
{
	/*! delete 2nd of a std::pair operator
		 \tparam T typename
		 \param m object to delete */
	template<typename A, typename B>
	void operator()(const std::pair<A, B>& m) const { Deleter()(m.second); }
};

/// Parameterisable deleter functor.
/*! \tparam Deleter the desired deleter (see above) */
template <typename Deleter = DeleteObject>
struct free_ptr
{
	/*! function operator
		 \tparam T typename
		 \param ptr object to delete */
	template<typename T>
	void operator()(const T& ptr) const { Deleter()(ptr); }
};

//----------------------------------------------------------------------------------------
/// A lockfree Singleton.
/*! \tparam T the instance object type */
template <typename T>
class Singleton
{
	static tbb::atomic<T*> _instance;
	static tbb::mutex _mutex;

	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);

public:
	/// Ctor.
	Singleton() {}

	/// Dtor.
	virtual ~Singleton() { delete _instance.fetch_and_store(0); }

	/*! Get the instance of the underlying object. If not created, create.
	    \return the instance */
	static T *instance()
	{
		if (_instance) // cast operator performs atomic load with acquire
			return _instance;

		tbb::mutex::scoped_lock guard(_mutex);
		if (_instance == 0)
			_instance = new T;
		return _instance;
	}


	/*! Get the instance of the underlying object. If not created, create.
	    \return the instance */
	T *operator->() const { return instance(); }

	/*! Replace the instance object with a new instance.
	    \param what the new instance
	    \return the original instance */
	static T *reset(T *what) { return _instance.fetch_and_store(what); }

	/*! Get the instance of the underlying object removing it from the singleton.
	    \return the instance */
	static T *release() { return _instance.fetch_and_store(0); }
};

//---------------------------------------------------------------------------------------------------
/// Create a streambuf from an open file descriptor.
class fdinbuf : public std::streambuf
{
   enum { _buffer_size = 16 };

protected:
   char _buffer[_buffer_size];
   int _fd;

   virtual int_type underflow()
   {
      if (gptr() < egptr())
         return *gptr();
      int put_back_cnt(gptr() - eback());
      if (put_back_cnt > 4)
         put_back_cnt = 4;
		std::memcpy(_buffer + (4 - put_back_cnt), gptr() - put_back_cnt, put_back_cnt);
      int num_read(read (_fd, _buffer + 4, _buffer_size - 4));
      if (num_read <= 0)
         return -1;
      setg(_buffer + (4 - put_back_cnt), _buffer + 4, _buffer + 4 + num_read);
      return *gptr();
   }

public:
   fdinbuf(int infd) : _buffer(), _fd(infd) { setg(_buffer + 4, _buffer + 4, _buffer + 4); }
};

//---------------------------------------------------------------------------------------------------
class tty_save_state
{
	bool _raw_mode;
	int _fd;
	termio _tty_state;

public:
	explicit tty_save_state(int fd) : _raw_mode(), _fd(fd) {}

	tty_save_state& operator=(const tty_save_state& from)
	{
		if (&from != this)
		{
			_raw_mode = from._raw_mode;
			_fd = from._fd;
			_tty_state = from._tty_state;
		}
		return *this;
	}

	void unset_raw_mode()
	{
		if (_raw_mode)
		{
			if (ioctl(_fd, TCSETA, &_tty_state) < 0)
				std::cerr << Str_error(errno, "Cannot reset ioctl") << std::endl;
			else
				_raw_mode = false;
		}
	}

	void set_raw_mode()
	{
		if (!_raw_mode)
		{
			if (ioctl(_fd, TCGETA, &_tty_state) < 0)
			{
				std::cerr << Str_error(errno, "Cannot get ioctl") << std::endl;
				return;
			}
			termio tty_state(_tty_state);
			tty_state.c_lflag = 0;
			tty_state.c_cc[VTIME] = 0;
			tty_state.c_cc[VMIN] = 1;
			if (ioctl(_fd, TCSETA, &tty_state) < 0)
				std::cerr << Str_error(errno, "Cannot reset ioctl") << std::endl;
			else
				_raw_mode = true;
		}
	}
};

//----------------------------------------------------------------------------------------
} // namespace FIX8

#endif // _F8_UTILS_HPP_

