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
#ifndef _XML_ENTITY_HPP_
#define _XML_ENTITY_HPP_

//----------------------------------------------------------------------------------------
/// A simple xml parser with Xpath style lookup.
class XmlElement
{
	/// Maximum depth levels supported.
	enum { MaxDepth = 128 };
	static int errors_, line_, incline_, maxdepth_, seq_;
	static FIX8::RegExp rCE_, rCX_, rIn_, rEn_, rEv_;
	static XmlElement *root_;
	static std::string inclusion_;

	std::string tag_, *value_, *decl_;
	int depth_, sequence_, txtline_, chldcnt_, subidx_;

	/// Comparitor.
	struct EntityOrderComp
	{
		bool operator()(const XmlElement *a, const XmlElement *b) const
			{ return a->GetSequence() < b->GetSequence(); }
	};

public:
	/*! XmlSet ordering preserved from source file */
	typedef std::set<const XmlElement *, EntityOrderComp> XmlSet;

private:
	typedef std::multimap<std::string, XmlElement *> XmlSubEls;
	/// simple n-ary tree
	XmlSubEls *children_;
	bool _was_include;

	/// Set of all child elements in file order
	XmlSet *ordchildren_;
	static XmlSet emptyset_;

public:
	typedef std::map<std::string, std::string> XmlAttrs;
	XmlAttrs *attrs_;
	static XmlAttrs emptyattrs_;

	/// Copy Ctor. Non-copyable.
	XmlElement(const XmlElement&);

	/// Assignment operator. Non-copyable.
	XmlElement& operator=(const XmlElement&);

public:
	/*! Ctor.
	  \param ifs input file stream
	  \param subidx the subindex for this child
	  \param txtline xml sourcefile line number
	  \param depth depth nesting level
	  \param rootAttr root attribute string */
	XmlElement(std::istream& ifs, int subidx, int txtline=0, int depth=0, const char *rootAttr=0);

	/// Dtor.
	virtual ~XmlElement();

	/*! Parse the xml attributes from an element.
	  \param attlst string of attributes
	  \return number of attributes extracted */
	int ParseAttrs(const std::string& attlst);

	/*! Find an element with a given name, attribute name and attribute value.
	  \param what the name to search for
	  \param ignorecase if true ignore case of name
	  \param atag the attribute name
	  \param aval the attribute value
	  \param delim the Xpath delimiter
	  \return the found or 0 if not found */
	const XmlElement *find(const std::string& what, bool ignorecase=false,
		const std::string *atag=0, const std::string *aval=0, const char delim='/') const;

	/*! Recursively find all elements with a given name, attribute name and attribute value.
	  \param what the name to search for
	  \param eset target XmlSet to place results
	  \param ignorecase if true ignore case of name
	  \param atag the attribute name
	  \param aval the attribute value
	  \param delim the Xpath delimiter
	  \return the number of found elements */
	int find(const std::string& what, XmlSet& eset, bool ignorecase=false,
		const std::string *atag=0, const std::string *aval=0, const char delim='/') const;

	/*! Find an attribute's with the given name.
	  \param what attribute to find
	  \param target where to place value
	  \return true if found */
	bool GetAttr(const std::string& what, std::string& target) const;

	/*! Find an attribute's value with the name "value".
	  \param target where to place value
	  \return true if found */
	bool GetAttrValue(std::string& target) const
	{
		static const std::string valstr("value");
		return GetAttr(valstr, target);
	}

	/*! Find an element and obtain the attribute's value with the name "value".
	  \param what name to find
	  \param target where to place value
	  \return true if found */
	bool FindAttrGetValue(const std::string& what, std::string& target)
	{
		 const XmlElement *inst(find(what));
		 return inst ? inst->GetAttrValue(target) : false;
	}

	/*! Find an attribute with the given name and value.
	  \param what attribute to find
	  \param value attribute value
	  \return true if found */
	bool findAttrByValue(const std::string& what, const std::string& value) const;

	/*! Find an attribute with the given name and return its typed value.
	  \tparam type of target attribute
	  \param what attribute to find
	  \param defValue value to return if attribute was not found
	  \return the value of the found attribute */
	template<typename T>
	T FindAttr(const std::string& what, const T defValue) const
	{
		if (attrs_)
		{
			XmlAttrs::iterator itr(attrs_->find(what));
			if (itr != attrs_->end())
				return FIX8::GetValue<T>(itr->second);
		}

		return defValue;
	}

	/*! Find an attribute with the given name and populate supplied target with its typed value.
	  \tparam type of target attribute
	  \param what attribute to find
	  \param target location to return value
	  \return reference to value of the found attribute */
	template<typename T>
	T& FindAttrRef(const std::string& what, T& target) const
	{
		if (attrs_)
		{
			XmlAttrs::iterator itr(attrs_->find(what));
			if (itr != attrs_->end())
				target = FIX8::GetValue<T>(itr->second);
		}

		return target;
	}

	/*! Perform xml translation on the supplied string inplace.
	  Translate predefined entities and numeric character references.
	  \param what source string to translate
	  \return the translated string */
	const std::string& InplaceXlate (std::string& what);

	/*! Get the depth of this element
	  \return the depth */
	int GetDepth() const { return depth_; }

	/*! Get the global error count.
	  \return the error count */
	int GetErrorCnt() const { return errors_; }

	/*! Get the actual current source line.
	  \return the source line */
	int GetLineCnt() const { return line_; }

	/*! Get the count of children this element has.
	  \return the number of children */
	int GetChildCnt() const { return chldcnt_; }

	/*! Get the source line of this element (element order).
	  \return the line */
	int GetLine() const { return txtline_; }

	/*! Get the subindex.
	  \return the subindex */
	int GetSubIdx() const { return subidx_; }

	/*! Get the sequence number for this element (incremented for each new element).
	  \return the sequence number */
	int GetSequence() const { return sequence_; }

	/*! Get the maximum depth supported.
	  \return the maximum depth */
	int GetMaxDepth() const { return maxdepth_; }


	/*! Get the element tag name.
	  \return the tag */
	const std::string& GetTag() const { return tag_; }

	/*! Get the element value.
	  \return the value */
	const std::string *GetVal() const { return value_; }

	/*! Get the declaration if available.
	  \return the depth */
	const std::string *GetDecl() const { return decl_; }

	/// Reset all counters, errors and sequences.
	static void Reset() { errors_ = maxdepth_ = seq_ = 0; line_ = 1; root_ = 0; }

	/*! Create a new root element (and recursively parse) from a given xml filename.
	  \param fname the xml filename
	  \return the depth */
	static XmlElement *Factory(const std::string& fname);

	/*! Get an iterator to the first child attribute.
	  \return const_iterator to first attribute */
	XmlAttrs::const_iterator abegin() const { return attrs_ ? attrs_->begin() : emptyattrs_.end(); }

	/*! Get an iterator to the last+1 child attribute.
	  \return const_iterator to last+1 attribute */
	XmlAttrs::const_iterator aend() const { return attrs_ ? attrs_->end() : emptyattrs_.end(); }

	/*! Get an iterator to the first child element.
	  \return const_iterator to first element */
	XmlSet::const_iterator begin() const { return ordchildren_ ? ordchildren_->begin() : emptyset_.end(); }

	/*! Get an iterator to the last+1 child element.
	  \return const_iterator to last+1 element */
	XmlSet::const_iterator end() const { return ordchildren_ ? ordchildren_->end() : emptyset_.end(); }

	/*! Inserter friend.
	    \param os stream to send to
	    \param en XmlElement REFErence
	    \return stream */
	friend std::ostream& operator<<(std::ostream& os, const XmlElement& en);
};

#endif // _XML_ENTITY_HPP_

