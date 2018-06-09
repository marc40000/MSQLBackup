#pragma once

#include <string>
#include <codecvt>

#include "MHashable32.h"
#include "MSerialize.h"
#include "MSerializeBuffer.h"
#include "MUnserializeBuffer.h"


class MString : public std::string
{
public:
	inline MString()
	{
	}
	inline MString(const MString & s)
	{
		assign(s);
	}
	inline MString(const char * s)
	{
		//*((std::string*)this) = s;
		assign(s);
	}
	inline MString(const std::string & s)
	{
		//*((std::string*)this) = s;
		assign(s);
	}
	inline MString(const std::u16string & s)
	{
		// from https://stackoverflow.com/questions/11086183/encode-decode-stdstring-to-utf-16
		//static std::wstring_convert<std::codecvt<char16_t, char, std::mbstate_t>, char16_t> convert;
		// from https://github.com/ThePhD/sol2/issues/571
		//std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
		//*this = convert.to_bytes(s);
		*this = utf16toutf8(s);
	}
	/*inline MString(const MStringU16 & s)
	{
		// from https://stackoverflow.com/questions/11086183/encode-decode-stdstring-to-utf-16
		//static std::wstring_convert<std::codecvt<char16_t, char, std::mbstate_t>, char16_t> convert;
		// from https://github.com/ThePhD/sol2/issues/571
		//std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
		//*this = convert.to_bytes((const std::u16string &)s);
		*this = utf16toutf8((const std::u16string &)s);
	}*/


	// because of a bug in vs https://stackoverflow.com/questions/32055357/visual-studio-c-2015-stdcodecvt-with-char16-t-or-char32-t
#if _MSC_VER >= 1900
	static std::string utf16toutf8(const std::u16string & utf16_string)
	{
		//std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
		//auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
		//return convert.to_bytes(p, p + utf16_string.size());

		/*
		// this seems to convert to ascii and not utf8
		unsigned int pn = utf16_string.size() + 1;
		char * p = new char[sizeof(char) * pn];
		std::wcstombs(p, (wchar_t *)utf16_string.c_str(), pn);
		std::string s = p;
		delete[] p;
		return s;
		*/

		// https://stackoverflow.com/questions/32055357/visual-studio-c-2015-stdcodecvt-with-char16-t-or-char32-t
		std::wstring wide_string(utf16_string.begin(), utf16_string.end());
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
		return convert.to_bytes(wide_string);
	}
#else
	static std::string utf16toutf8(const std::u16string & utf16_string)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
		return convert.to_bytes(utf16_string);
	}
#endif	


	inline const bool operator== (const MString & o) const
	{
		//return strcmp(c_str(), o.c_str()) == 0;
		return compare(o) == 0;
	}

	inline const unsigned int Hash32() const
	{
		unsigned int hash = 0;
		unsigned int i = 0;
		while ((*this)[i] != 0)
		{
			hash ^= ((*this)[i] << ((i % 4) * 8));
			++i;
		}
		return hash;
	}


	inline void Serialize(class MSerializeBuffer & b)
	{
		b.string(this->c_str());
	}

	inline void Unserialize(class MUnserializeBuffer & b)
	{
		char * s = (char*)(b.GetPointerCurrent());
		this->assign(s);
		b.Skip(sizeof(char) * (strlen(s) + 1));
	}


	inline MString ToUpper() const
	{
		MString t = *this;
		for (auto & c : t) c = toupper(c);		// from https://stackoverflow.com/questions/735204/convert-a-string-in-c-to-upper-case
		return t;
	}

	inline MString ToLower() const
	{
		MString t = *this;
		for (auto & c : t) c = tolower(c);		// from https://stackoverflow.com/questions/735204/convert-a-string-in-c-to-upper-case
		return t;
	}

	// doesn't work :(
/*	template<T>
	inline MString operator+(T t)
	{
		return (*this) + std::to_string(t);
	}*/


};

#define MToString(t) std::to_string(t)


template < > class MHashable32Helper < MString >
{
public:
	inline const unsigned int HashFunction32(const MString & v) const
	{
		return v.Hash32();
	}
};


inline void operator<< (MSerializeBuffer & b, MString & s)
{
	s.Serialize(b);
}

inline void operator<< (MUnserializeBuffer & b, MString & s)
{
	s.Unserialize(b);
}
