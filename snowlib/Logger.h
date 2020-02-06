#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <stdarg.h>

class Logger
{
private:
	static Logger* logger;

	Logger(){}

public:
	static Logger* getLogger()
	{
		if (logger == nullptr){
			logger = new Logger();
		}
		return logger;
	}

	void log(const char* message)
	{
		std::cerr << message;
#ifdef WIN32
		OutputDebugStringA(message);
#endif
	}

	void logFormat(const char* format, ...)
	{
		char buf[256];
		va_list args;
		va_start (args, format);
		vsnprintf(buf, 256, format, args);
		va_end(args);
		log(buf);
	}
};
