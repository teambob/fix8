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
//-------------------------------------------------------------------------------------------------
#ifndef _FIX8_TRAITS_HPP_
#define _FIX8_TRAITS_HPP_

#include <set>

//-------------------------------------------------------------------------------------------------
namespace FIX8 {

//-------------------------------------------------------------------------------------------------
/// FIX field traits - hold specific traits for each field.
struct FieldTrait
{
	enum FieldType
	{
		ft_untyped,
		ft_int,
		ft_Length, ft_TagNum, ft_SeqNum, ft_NumInGroup, ft_DayOfMonth, ft_end_int=ft_DayOfMonth,
		ft_char,
		ft_Boolean, ft_end_char=ft_Boolean,
		ft_float,
		ft_Qty, ft_Price, ft_PriceOffset, ft_Amt, ft_Percentage, ft_end_float=ft_Percentage,
		ft_string,
		ft_MultipleCharValue, ft_MultipleStringValue, ft_Country, ft_Currency, ft_Exchange,
		ft_MonthYear, ft_UTCTimestamp, ft_UTCTimeOnly, ft_UTCDateOnly, ft_LocalMktDate, ft_TZTimeOnly, ft_TZTimestamp,
		ft_data, ft_XMLData,
		ft_pattern,
		ft_Tenor, ft_Reserved100Plus, ft_Reserved1000Plus, ft_Reserved4000Plus,
		ft_Language, ft_end_string=ft_Language
	};

	enum TraitTypes { mandatory, present, position, group, component, suppress, automatic, count };

	/*! Check if this FieldType is an int.
	  \param ftype field to check
	  \return true if an int */
	static bool is_int(const FieldType ftype) { return ft_int <= ftype && ftype <= ft_end_int; }

	/*! Check if this FieldType is a char.
	  \param ftype field to check
	  \return true if a char */
	static bool is_char(const FieldType ftype) { return ft_char <= ftype && ftype <= ft_end_char; }

	/*! Check if this FieldType is a string.
	  \param ftype field to check
	  \return true if a string */
	static bool is_string(const FieldType ftype) { return ft_string <= ftype && ftype <= ft_end_string; }

	/*! Check if this FieldType is a float.
	  \param ftype field to check
	  \return true if a float */
	static bool is_float(const FieldType ftype) { return ft_float <= ftype && ftype <= ft_end_float; }

	FieldTrait() {}

	FieldTrait(const FieldTrait& from) : _fnum(from._fnum), _ftype(from._ftype), _pos(from._pos),
		_subpos(from._subpos), _field_traits(from._field_traits) {}

	FieldTrait(unsigned short fnum, unsigned ftype, unsigned short pos, short field_traits)
		: _fnum(fnum), _ftype(FieldType(ftype)), _pos(pos), _subpos(), _field_traits(field_traits | (pos ? 1 : 0) << position)  {}

	/*! Ctor.
	  \param field field num (tag number)
	  \param ftype field type
	  \param pos field position (in FIX message)
	  \param ismandatory true if mandatory
	  \param isgroup true if this is a group
	  \param subpos field sub-position (in FIX message)
	  \param ispresent true if field is present (should be false until set). */
	FieldTrait(const unsigned short field, const FieldType ftype=ft_untyped, const unsigned short pos=0,
		bool ismandatory=false, bool isgroup=false, const unsigned subpos=0, bool ispresent=false) :
		_fnum(field), _ftype(ftype), _pos(pos), _subpos(subpos),
		_field_traits((ismandatory ? 1 : 0) | (ispresent ? 1 : 0) << present
		| (pos ? 1 : 0) << position | (isgroup ? 1 : 0) << group | (subpos ? 1 : 0) << component) {}

	unsigned short _fnum;
	FieldType _ftype;
	mutable unsigned short _pos, _subpos;
	mutable ebitset<TraitTypes, unsigned short> _field_traits;

	/// Binary comparitor functor.
	struct Compare
	{
		/*! Comparitor operator.
		  \param p1 lhs to compare
		  \param p2 rhs to compare
		  \return true if p1 < p2 */
		bool operator()(const FieldTrait& p1, const FieldTrait& p2) const { return p1._fnum < p2._fnum; }
	};

	/// Binary position comparitor functor.
	struct PosCompare
	{
		/*! Comparitor operator.
		  \param p1 lhs to compare
		  \param p2 rhs to compare
		  \return true if p1 position not equal to p2 position; will use subpos if necessary */
		bool operator()(const FieldTrait* p1, const FieldTrait* p2) const
			{ return p1->_pos < p2->_pos || (p1->_pos == p2->_pos && p1->_subpos < p2->_subpos); }
	};

	/*! Inserter friend.
	    \param os stream to send to
	    \param what FieldTrait
	    \return stream */
	friend std::ostream& operator<<(std::ostream& os, const FieldTrait& what);
};

//-------------------------------------------------------------------------------------------------
struct FieldTrait_Hash_Array
{
   const unsigned _els, _sz;
   unsigned short *_arr;

   FieldTrait_Hash_Array(const FieldTrait *from, const size_t els)
      : _els(els), _sz((from + _els - 1)->_fnum + 1), _arr(new unsigned short [_sz])
   {
		for (unsigned ii(0); ii < _sz; _arr[ii++] = 0)
			;
      for (unsigned offset(0); offset < _els; ++offset)
         _arr[from[offset]._fnum] = offset;
   }

   ~FieldTrait_Hash_Array() { delete[] _arr; }
};

//-------------------------------------------------------------------------------------------------
/// Specialisation of Presorted set using hash array lookup
/// Search complexity is O(1), ctor complexity approaches O(1), no insert
template<>
class presorted_set<unsigned short, FieldTrait, FieldTrait::Compare>
{
public:
	typedef FieldTrait* iterator;
	typedef const FieldTrait* const_iterator;
	typedef std::pair<iterator, bool> result;

	/*! Calculate the amount of space to reserve in set
	  \param sz number of elements currently in set; if 0 retun reserve elements as size to reserve
	  \param res percentage of sz to keep in reserve
	  \return number of elements to additionally reserve (at least 1) */
	static size_t calc_reserve(size_t sz, size_t res)
	{
		if (!sz)  // special case - reserve means number to reserve, not %
			return res;
		const size_t val(sz * res / 100);
		return val ? val : 1;
	}

private:
	const size_t _reserve;
	size_t _sz, _rsz;
	FieldTrait *_arr;
	const FieldTrait_Hash_Array *_ftha;

	typedef std::pair<iterator, iterator> internal_result;
	typedef std::pair<const_iterator, const_iterator> const_internal_result;

public:
	/*! ctor - initialise from static sorted set
	  \param arr_start pointer to start of static array to copy elements from
	  \param sz number of elements in set to copy
	  \param ftha pointer to field hash array
	  \param reserve percentage of sz to keep in reserve */
	presorted_set(const_iterator arr_start, const size_t sz, const FieldTrait_Hash_Array *ftha, const size_t reserve=RESERVE_PERCENT)
		: _reserve(reserve), _sz(sz), _rsz(_sz + calc_reserve(_sz, _reserve)), _arr(new FieldTrait[_rsz]), _ftha(ftha)
			{ memcpy(_arr, arr_start, _sz * sizeof(FieldTrait)); }

	presorted_set(const_iterator arr_start, const size_t sz, const size_t reserve=RESERVE_PERCENT) : _reserve(reserve),
		_sz(sz), _rsz(_sz + calc_reserve(_sz, _reserve)), _arr(new FieldTrait[_rsz]), _ftha()
			{ memcpy(_arr, arr_start, _sz * sizeof(FieldTrait)); }

	/*! ctor - initialise an empty set; defer memory allocation;
	  \param sz number of elements to initially allocate
	  \param reserve percentage of sz to keep in reserve */
	explicit presorted_set(const size_t sz=0, const size_t reserve=RESERVE_PERCENT) : _reserve(reserve),
		_sz(sz), _rsz(_sz + calc_reserve(_sz, _reserve)), _arr(), _ftha() {}

	/// dtor
	~presorted_set() { delete[] _arr; }

	/*! Find an element with the given value
	  \param what element to find
	  \param answer true if element is found
	  \return pointer to found element or pointer to location where element would be inserted */
	iterator find(const FieldTrait what, bool& answer)
	{
		if (_ftha)
			return (answer = what._fnum < _ftha->_sz && (_arr + _ftha->_arr[what._fnum])->_fnum == what._fnum) ? _arr + _ftha->_arr[what._fnum] : 0;
		const internal_result res(std::equal_range (_arr, _arr + _sz, what, FieldTrait::Compare()));
		answer = res.first != res.second;
		return res.first;
	}

	/*! Find an element with the given key
	  \param key to find
	  \param answer true if element is found
	  \return pointer to found element or pointer to location where element would be inserted */
	iterator find(const unsigned short key, bool& answer) { return find(FieldTrait(key), answer); }

	/*! Find an element with the given key (const version)
	  \param key to find
	  \return pointer to found element or end() */
	const_iterator find(const unsigned short key) const { return find(FieldTrait(key)); }

	/*! Find an element with the given value (const version)
	  \param what element to find
	  \return pointer to found element or end() */
	const_iterator find(const FieldTrait what) const
	{
		if (_ftha)
			return what._fnum < _ftha->_sz && (_arr + _ftha->_arr[what._fnum])->_fnum == what._fnum ? _arr + _ftha->_arr[what._fnum] : end();
		const const_internal_result res(std::equal_range (_arr, _arr + _sz, what, FieldTrait::Compare()));
		return res.first != res.second ? res.first : end();
	}

	/*! Find an element with the given key
	  \param key to find
	  \return pointer to found element or end() */
	iterator find(const unsigned short key) { return find(FieldTrait(key)); }

	/*! Find an element with the given value
	  \param what value to find
	  \return pointer to found element or end() */
	iterator find(const FieldTrait what)
	{
		if (_ftha)
			return what._fnum < _ftha->_sz && (_arr + _ftha->_arr[what._fnum])->_fnum == what._fnum ? _arr + _ftha->_arr[what._fnum] : end();
		internal_result res(std::equal_range (_arr, _arr + _sz, what, FieldTrait::Compare()));
		return res.first != res.second ? res.first : end();
	}

	/*! Get the element at index location
	  \param idx of the pair to retrieve
	  \return const_iterator to element or end() if not found */
	const_iterator at(const size_t idx) const { return idx < _sz ? _arr + idx : end(); }

	/*! Insert an element into the set
	  \param what pointer to element to insert
	  \return result with iterator to insert location and true or end() and false */
	result insert(const_iterator what)
	{
		if (!_sz)
		{
			_arr = new FieldTrait[_rsz];
			memcpy(_arr, what, sizeof(FieldTrait));
			++_sz;
			return result(_arr, true);
		}

		bool answer;
		iterator where(find(*what, answer));
		if (answer) // sorry already here
			return result(end(), false);

		if (_sz < _rsz) // we have space
		{
			memmove(where + 1, where, (end() - where) * sizeof(FieldTrait));
			memcpy(where, what, sizeof(FieldTrait));
		}
		else // we have to make space
		{
			iterator new_arr(new FieldTrait[_rsz = _sz + calc_reserve(_sz, _reserve)]);
			size_t wptr(where - _arr);
			if (wptr > 0)
				memcpy(new_arr, _arr, sizeof(FieldTrait) * wptr);
			memcpy(new_arr + wptr, what, sizeof(FieldTrait));
			memcpy(new_arr + wptr + 1, where, (end() - where) * sizeof(FieldTrait));
			delete[] _arr;
			_arr = new_arr;
		}
		++_sz;
		return result(where, true);
	}

	/*! Find the distance between two iterators
	  \param what_begin start iterator
	  \param what_end end iterator (must be >= to what_begin
	  \return distance in elements */
	static size_t distance(const_iterator what_begin, const_iterator what_end)
		{ return what_end - what_begin; }

	/*! Insert a range of elements into the set
	  \param what_begin pointer to 1st element to insert
	  \param what_end pointer to nth element + 1 to insert */
	void insert(const_iterator what_begin, const_iterator what_end)
	{
		for (const_iterator ptr(what_begin); ptr < what_end; ++ptr)
			if (!insert(ptr).second)
				break;
	}

	/*! Clear the set (does not delete) */
	void clear() { _sz = 0; }

	/*! Obtain the number of elements in the set
	  \return the number of elements */
	size_t size() const { return _sz; }

	/*! Check if the set is empty
	  \return true if empty */
	bool empty() const { return _sz == 0; }

	/*! Obtain the number of elements that can be inserted before reallocating
	  \return reserved + sz */
	size_t rsize() const { return _rsz; }

	/*! Get a pointer to the first element
	  \return the first element */
	iterator begin() { return _arr; }

	/*! Get a pointer to the last element + 1
	  \return the last element + 1 */
	iterator end() { return _arr + _sz; }

	/*! Get a const pointer to the first element
	  \return the first element */
	const_iterator begin() const { return _arr; }

	/*! Get a const pointer to the last element + 1
	  \return the last element + 1 */
	const_iterator end() const { return _arr + _sz; }
};

//-------------------------------------------------------------------------------------------------
typedef presorted_set<unsigned short, FieldTrait, FieldTrait::Compare> Presence;

/// A collection of FieldTraits for a message. Which fields are required, which are present.
class FieldTraits
{
	Presence _presence;

public:
	/*! Ctor.
	  \tparam InputIterator input iterator to construct from
	  \param begin start iterator to input
	  \param cnt number of elements to input */
	template<typename InputIterator>
	FieldTraits(const InputIterator begin, const size_t cnt

#if defined PERMIT_CUSTOM_FIELDS
			) : _presence(begin, cnt) {}
#else
		, const FieldTrait_Hash_Array *ftha) : _presence(begin, cnt, ftha) {}
#endif
	/// Ctor.
	FieldTraits() {}

	/*! Check if a field is present
	  \param field to check
	  \param itr hint iterator: set to itr of found element
	  \return true if present */
	bool has(const unsigned short field, Presence::const_iterator& itr) const
	{
		if (itr == _presence.end())
			itr = _presence.find(field);
		return itr != _presence.end();
	}

	/*! Check if a field is present
	  \param field to check
	  \return true if present */
	bool has(const unsigned short field) const
	{
		Presence::const_iterator itr(_presence.find(field));
		return itr != _presence.end();
	}

	/*! Get the traits for a field.
	  \param field to get
	  \return traits as an unsigned short */
	unsigned getval(const unsigned short field)
	{
		Presence::const_iterator itr(_presence.find(field));
		return itr != _presence.end() ? itr->_field_traits.get() : 0;
	}

	/*! Get the number of possible fields
	  \return number of fields */
	size_t size() const { return _presence.size(); }

	/*! Check if a field has a specified trait.
	  \param field to check
	  \param type TraitType to check (default present)
	  \return true if field has trait */
	bool get(const unsigned short field, FieldTrait::TraitTypes type=FieldTrait::present) const
	{
		Presence::const_iterator itr(_presence.find(field));
		return itr != _presence.end() ? itr->_field_traits.has(type) : false;
	}

	/*! Check if a field has a specified trait.
	  \param field to check
	  \param itr hint iterator: if end, set to itr of found element, if not end use it to locate element
	  \param type TraitType to check (default present)
	  \return true if field has trait */
	bool get(const unsigned short field, Presence::const_iterator& itr, FieldTrait::TraitTypes type) const
	{
		if (itr != _presence.end())
			return itr->_field_traits.has(type);
		itr = _presence.find(field);
		return itr != _presence.end() ? itr->_field_traits.has(type) : false;
	}

	/*! Find the first field that does not have the specified trait.
	  \param type TraitType to check (default mandatory)
	  \return field number of field, 0 if none */
	unsigned short find_missing(FieldTrait::TraitTypes type=FieldTrait::mandatory) const
	{
		for (Presence::const_iterator itr(_presence.begin()); itr != _presence.end(); ++itr)
			if ((itr->_field_traits & type) && (itr->_field_traits & FieldTrait::present) == 0)
				return itr->_fnum;
		return 0;
	}

	/*! Set a trait for a specified field.
	  \param field to set
	  \param itr hint iterator: if end, set to itr of found element, if not end use it to locate element
	  \param type TraitType to set (default present) */
	void set(const unsigned short field, Presence::const_iterator& itr, FieldTrait::TraitTypes type)
	{
		if (itr == _presence.end())
		{
			itr = _presence.find(field);
			if (itr != _presence.end())
				itr->_field_traits.set(type);
		}
		else
			itr->_field_traits.set(type);
	}

	/*! Set a trait for a specified field.
	  \param field to set
	  \param type TraitType to set (default present) */
	void set(const unsigned short field, FieldTrait::TraitTypes type=FieldTrait::present)
	{
		Presence::iterator itr(_presence.find(field));
		if (itr != _presence.end())
			itr->_field_traits.set(type);
	}

	/*! Clear a trait for a specified field.
	  \param field to set
	  \param itr hint iterator: if end, set to itr of found element, if not end use it to locate element
	  \param type TraitType to set (default present) */
	void clear(const unsigned short field, Presence::const_iterator& itr, FieldTrait::TraitTypes type=FieldTrait::present)
	{
		if (itr == _presence.end())
		{
			itr = _presence.find(field);
			if (itr != _presence.end())
				itr->_field_traits.clear(type);
		}
		else
			itr->_field_traits.clear(type);
	}

	/*! Clear a trait for a specified field.
	  \param field to set
	  \param type TraitType to set (default present) */
	void clear(const unsigned short field, FieldTrait::TraitTypes type=FieldTrait::present)
	{
		Presence::iterator itr(_presence.find(field));
		if (itr != _presence.end())
			itr->_field_traits.clear(type);
	}

	/*! Add a FieldTrait.
	  \param what TraitType to add
	  \return true on success (false already present) */
	bool add(const FieldTrait& what) { return _presence.insert(&what).second; }

	/*! Add from a range of traits.
	  \param begin start iterator to input
	  \param cnt number of elements to input */
	template<typename InputIterator>
	void add(const InputIterator begin, const size_t cnt) { _presence.insert(begin, begin + cnt); }

	/*! Clear a trait from all traits.
	  \param type TraitType to clear */
	void clear_flag(FieldTrait::TraitTypes type=FieldTrait::present)
		{ for (Presence::const_iterator itr(_presence.begin()); itr != _presence.end(); itr++->_field_traits.clear(type)); }

	/*! Check if a specified field has the present bit set (is present).
	  \param field field to check
	  \return true if present */
	bool is_present(const unsigned short field) const { return get(field, FieldTrait::present); }

	/*! Check if a specified field has the mandator bit set.
	  \param field field to check
	  \return true if mandatory set */
	bool is_mandatory(const unsigned short field) const { return get(field, FieldTrait::mandatory); }

	/*! Check if a specified field has the group bit set (is a group).
	  \param field field to check
	  \param itr hint iterator: if end, set to itr of found element, if not end use it to locate element
	  \return true if a group */
	bool is_group(const unsigned short field, Presence::const_iterator& itr) const { return get(field, itr, FieldTrait::group); }

	/*! Check if a specified field has the group bit set (is a group).
	  \param field field to check
	  \return true if a group */
	bool is_group(const unsigned short field) const { return get(field, FieldTrait::group); }

	/*! Check if a specified field has the component bit set (is a component).
	  \param field field to check
	  \return true if a component */
	bool is_component(const unsigned short field) const { return get(field, FieldTrait::component); }

	/*! Get the field position of a specified field.
	  \param field field to get
	  \param itr hint iterator: if end, set to itr of found element, if not end use it to locate element
	  \return position of field, 0 if no pos or not found */
	unsigned short getPos(const unsigned short field, Presence::const_iterator& itr) const
	{
		if (itr != _presence.end())
			return itr->_field_traits.has(FieldTrait::position) ? itr->_pos : 0;
		itr = _presence.find(field);
		return itr != _presence.end() && itr->_field_traits.has(FieldTrait::position) ? itr->_pos : 0;
	}

	/*! Get the field position of a specified field.
	  \param field field to get
	  \return position of field, 0 if no pos or not found */
	unsigned short getPos(const unsigned short field) const
	{
		Presence::const_iterator itr(_presence.find(field));
		return itr != _presence.end() && itr->_field_traits.has(FieldTrait::position) ? itr->_pos : 0;
	}

	/*! Get the Presence set
	  \return the Presence set */
	const Presence& get_presence() const { return _presence; }

	/*! Inserter friend.
	    \param os stream to send to
	    \param what FieldTraits
	    \return stream */
	friend std::ostream& operator<<(std::ostream& os, const FieldTraits& what);
};

} // FIX8

#endif // _FIX8_TRAITS_HPP_
