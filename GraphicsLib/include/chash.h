/********************************************//**
 * \file   	CHash.h
 * \Author 	Tim Lewis (tim@totga-games.com)
 * \see    	Project: Anthracite
 * \date   	11-09-2014
 * \copyright (c) 2014 
 * \brief  	Hash class
 * The CHash class is used to hash strings in order to facilitate 
 * quicker comparisons and easier map stoarge
 ***********************************************/

#ifndef CHash_h_included
#define CHash_h_included

#define CONVERT_BACKSLASH
#define CASE_INSENSITIVE

#include "stddef.h"
#include <functional>

/********************************************//**
 * \brief  	CHash is the hash of a string
 * \details The The CHash class is used to hash strings in order to facilitate 
 * quicker comparisons and easier map stoarge
 ***********************************************/

class CHash
{
public:
	enum { INVALID_HASH = 0xFFFFFFFF, HASH_INIT = 0x811c9dc5, HASH_PRIME = 0x01000193, HASH_MASK = 0x7FFFFFFF };

	CHash(int val) : mHash(val){}
	CHash() :mHash(INVALID_HASH){}
	CHash(const CHash &other) : mHash(other.mHash){}
	CHash(const char *str) :mHash(make_hash(str))
	{
	}

	CHash(const char *str, size_t size) :mHash(make_hash(str, size))
	{
	}

	

	CHash operator = (const CHash &rhs) { mHash = rhs.mHash; return *this;}
	operator unsigned int() const {return mHash;}

	bool operator <  (const CHash &other) const {return mHash <  other.mHash;}
	bool operator >  (const CHash &other) const {return mHash >  other.mHash;}	
	bool operator <= (const CHash &other) const {return mHash <= other.mHash;}
	bool operator >= (const CHash &other) const {return mHash >= other.mHash;}
	bool operator == (const CHash &other) const {return mHash == other.mHash;}
	bool operator != (const CHash &other) const {return mHash != other.mHash;}

	bool is_valid() const {return mHash != INVALID_HASH;}

	void next_hash_int(int val)
	{		
		mHash = (mHash * HASH_PRIME) ^ (unsigned int)val;			
	}

	void start_hash()
	{
		mHash = HASH_INIT;
	}

	static inline char normalise_char(char ch)
	{
#ifdef CONVERT_BACKSLASH
		if (ch == '\\')
		{
			ch = '/';
		}
#endif

#ifdef CASE_INSENSITIVE
		if ((ch >= 'a') && (ch <= 'z'))
		{
			ch -= 'a' - 'A';
		}
#endif
		return ch;
	}

	unsigned int next_hash(char c)
	{
		
		mHash = (mHash * HASH_PRIME) ^ (unsigned int)normalise_char(c);
		return mHash;
	}

	static unsigned int make_hash(const char* str, size_t size)
	{
		if (!str )
			return INVALID_HASH;

		
		unsigned int hash = HASH_INIT;

		for (size_t i = 0; i < size; ++i)
		{				
			hash *= HASH_PRIME;
			hash ^= (unsigned int)normalise_char(str[i]);
		}
		return hash;// & 0x7FFFFFFF;
	}	

	static unsigned int make_hash(const char* str)
	{
		if (!str || !str[0])
		return INVALID_HASH;

		const unsigned char* string = (const unsigned char*)str;
		unsigned int hash = HASH_INIT;

		while (*string)
		{
			hash *= HASH_PRIME;
			hash ^= (unsigned int)normalise_char(*string++);
		}
		return hash;// & 0x7FFFFFFF;
	}
	//waiting for the day when constexpr is available widely...

#ifdef CONST_EXPR_SUPPORTED
	
	static constexpr char convert_backslash(char c) {return c == '\\' ? '/' : c;}
	static constexpr char upcase (char c) {return ((c >= 'a') && (c <= 'z'))? c - 'a' - 'A' : c;}

	
	static constexpr size_t make_hash_inner(const char* input, uint64_t value_so_far)
	{
	 return *input ? make_hash_inner(input + 1, 
	 
	 (value_so_far * HASH_PRIME) ^ 
	 #ifdef CONVERT_BACKSLASH
		#ifdef CASE_INSENSITIVE
		   convert_backslash(upcase(*input))
		#else
		   convert_backslash(*input)
		#endif
	 #elif CASE_INSENSITIVE
		upcase(*input)
	 #else

		*input
	 #endif
	 ) 
	 
	 : value_so_far;
	}

	static constexpr size_t make_hash_const(const char* input)
	{
		return make_hash_inner(input, HASH_INIT) & 0x7FFFFFFF;
	}
#endif
	unsigned int mHash;
};

//

/********************************************//**
* \brief 	SHashablePredicate is used to find objects that can be compared to a hash
* \details 	use for find_if and other functions referencing objects that can be compared to a hash
***********************************************/
#ifdef DEREFERENCE
struct SHashablePredicate
{
public:
	SHashablePredicate(CHash hash):mHash(hash){}
	SHashablePredicate(const SHashablePredicate &other):mHash(other.mHash){}
	template <class T>
	bool operator ()(const T &object) const
	{
		return meta::dereference<T>::eval(object) == mHash;
	}
private:
	CHash mHash;
};
#endif

//#ifndef _WIN32
namespace std
{
    template <> struct hash<CHash>
    {
        typedef size_t result_type;
        typedef CHash argument_type;

		//#ifndef noexcept
		size_t operator()(CHash const & hash) const 
        {
            return hash.mHash;
        }
    };
}
//#endif

#endif
