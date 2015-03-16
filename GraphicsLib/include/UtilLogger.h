/********************************************//**
 * \copyright (c) 2014 
 * \file   	UtilLogger.h
 * \Author 	Tim Lewis (tim@totga-games.com)
 * \see    	Project: Anthracite
 * \date   	11-09-2014
 * \brief  	UtilLogger
 * UtilLogger contains a set of utilities useful for
 * logging. Most significantly, a std::cout interface over
 * the windows console (OutputDebugStringX) is provided, giving convienient (and cross platform) 
 * logging capabilities.
 ***********************************************/

#ifndef _logger_h_
#define _logger_h_
#include <iostream>

/********************************************//**
* \brief 	CBufferedStringBuf implements a buffered text stream
* \details 	CBufferedStringBuf is used to buffer string output from another source
***********************************************/
class CBufferedStringBuf : public std::streambuf
{
public:
    CBufferedStringBuf(int bufferSize = 1024) ;
	const char *c_str() const;
    virtual ~CBufferedStringBuf();

	virtual void writeString(const std::string &str);

private:
    int	overflow(int c);

    int	sync();
};

/********************************************//**
* \brief 	CDupBuf is used to duplicate textual output streams
* \details 	CDupBuf is a class used to create dual output streams.
* 			For example, one to a log file, one to standard output
***********************************************/
class CDupBuf : public CBufferedStringBuf
{
public:

	CDupBuf(std::streambuf *stream1, std::streambuf *stream2, size_t buffer_size = 1024);
    CDupBuf(std::ostream *stream1, std::ostream *stream2, size_t buffer_size = 1024);
    
    virtual void writeString(const std::string &str);

private:

    std::streambuf *buffer1; 
    std::streambuf *buffer2;
};

/********************************************//**
* \brief 	COutputdebugStringStream provides a stream interface over OutputDebugStringX
* \details 	COutputdebugStringStream is a class derived
*			from CBufferedStringBuf that provides a stream
*			compatible interface to Visual Studio's OutputDebugStringX
***********************************************/
class COutputdebugStringStream: public CBufferedStringBuf
{
public:
	 COutputdebugStringStream(int bufferSize);

	 virtual void writeString(const std::string &str);

};

namespace tio
{
	enum {error_string_size=4096};
	extern char error_string[error_string_size];
	extern std::ostream  *logStream;
	extern std::ostream  *dumpStream;
	extern char error_string[error_string_size];	
	extern int output_debug_string(const char *pat,...);
	extern int sprintf(char *dest, size_t dest_size,const char *pat,...);	
};


#define LOG std::cout
#define TESTLOG std::cout
#define VISLOG std::cout
#define ERRLOG std::cout
#define DUMPLOG std::cout

//reports error code in manner that visual studio understands - allows double clicking on error
#define VS_FILE_LINE_ERROR_REPORT(ERROR_CODE) tio::sprintf(tio::error_string,tio::error_string_size,"%s(%d): warning CWERR_%d: ", __FILE__, __LINE__, ERROR_CODE); LOG<<(tio::error_string);
void LOGinit(std::ostream *stream);
void LOGshutdown();

#endif
