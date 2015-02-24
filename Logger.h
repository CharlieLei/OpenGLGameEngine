/*
 * Logger.h
 *
 *  Created on: Feb 21, 2015
 *      Author: ericwadkins
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <vector>
#include <string>
#include <iostream>

class Logger {
public:
	Logger();
	virtual ~Logger() {
	}
	Logger& log(const std::string str);
	//template<typename T>
	//Logger& log(const T t);
	Logger& log(const char* c);
	Logger& log(const unsigned char* c);
	Logger& log(const int i);
	Logger& log(const float f);
	Logger& log(const double d);
	Logger& log(const long l);
	Logger& endLine();
	Logger& increaseIndent();
	Logger& decreaseIndent();
	Logger& setIndent(int i);
	Logger& clear();
	std::vector<std::string> getLog();
private:
	std::string indent(std::string str);
	std::vector<std::string> _log;
	bool newLine;
	unsigned int numIndents;
	std::string indentString;
};

#endif /* LOGGER_H_ */
