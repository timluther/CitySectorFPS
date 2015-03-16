

#include <iostream>
#include <stdio.h>

#include <stdarg.h>//varargs.h>
#include <string.h>
#include "UtilLogger.h"

#ifndef _WIN32

#else
#include "Windows.h"
#endif

/*
# '__DATE__' -> a string literal of the form "Mmm dd yyyy"

# '__TIME__' -> a string literal of the form "hh:mm:ss"

# '__TIMESTAMP__' -> a string literal of the form "Mmm dd yyyy hh:mm:ss"

# '__FUNCTION__' function name in current scope
*/
#ifndef _WIN32
	void OutputDebugString(const char *string)
	{
		//std::cout << string << std::cout::endl;
	}
	#endif


    CBufferedStringBuf::CBufferedStringBuf(int bufferSize) 
    {
        if (bufferSize)
        {
            char *ptr = new char[bufferSize];
            setp(ptr, ptr + bufferSize);
        }
        else
            setp(0, 0);
    }
	const char *CBufferedStringBuf::c_str() const {return pbase();}
    CBufferedStringBuf::~CBufferedStringBuf() 
    {
        sync();
        delete[] pbase();
    }

	void CBufferedStringBuf::writeString(const std::string &str) {}

	int	CBufferedStringBuf::overflow(int c)
    {
        sync();
	
        if (c != EOF)
        {
            if (pbase() == epptr() || (c == '\n'))
            {
                std::string temp;
                temp += char(c);
                writeString(temp);
            }
            else
                sputc(c);
        }

        return 0;
    }

    int	CBufferedStringBuf::sync()
    {
        if (pbase() != pptr())
        {
            int len = int(pptr() - pbase());
            std::string temp(pbase(), len);
            writeString(temp);
            setp(pbase(), epptr());
        }
		
        return 0;
    }


	CDupBuf::CDupBuf(std::streambuf *stream1, std::streambuf *stream2, size_t buffer_size) :
      CBufferedStringBuf(buffer_size), buffer1(stream1), buffer2(stream2) 
    {
    }

    CDupBuf::CDupBuf(std::ostream *stream1, std::ostream *stream2, size_t buffer_size) :
      CBufferedStringBuf(buffer_size), buffer1(stream1->rdbuf()), buffer2(stream2->rdbuf()) 
    {
    }
    
    void CDupBuf::writeString(const std::string &str)
    {
        const char *ptr = str.c_str();
        std::streamsize size = std::streamsize(str.size());

        buffer1->sputn(ptr, size);
        buffer2->sputn(ptr, size);
        buffer1->pubsync();
        buffer2->pubsync();
    }

	COutputdebugStringStream::COutputdebugStringStream(int bufferSize) : CBufferedStringBuf(bufferSize) 
	{
	}

	void COutputdebugStringStream::writeString(const std::string &str)
	{
		#ifdef _WIN32
		OutputDebugStringA(str.c_str());
		#else
		//std::cout<<str;
		#endif
	}


namespace tio
{	
	std::ostream *logStream;
	std::ostream *dumpStream;	
};

void LOGinit(std::ostream *stream)
{

	tio::logStream=stream;


}

void LOGshutdown()
{		
}





