#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
inline void breakable_assert(const char *message)
{
	std::cout<< "Assert failed " << message << std::endl;
}

#define ASSERT(x) \
{ \
	static bool assert_called = false; \
	if ((!assert_called) && (!(x))) \
	{ \
		assert_called = true; \
		breakable_assert(#x); \
	} \
}


class CException
{
public:
	CException(const char *filename, int line, int column, int index)
		:m_src_file_name(filename)
		, m_line(line)
		, m_column(column)
		, m_index(index)	
	{
	}
	const char *m_src_file_name;
	int m_line, m_column, m_index;
};

class COutOfBoundsException
{
    public:
	COutOfBoundsException(const char *filename, int line, int column, int index, int capacity)
		:m_src_file_name(filename)
		,m_line(line)
		,m_column(column)
		,m_index(index)
		,m_capacity(capacity)
		{
		}
    const char *m_src_file_name;
	int m_line, m_column, m_index, m_capacity;
};

#endif // UTILITY_H
