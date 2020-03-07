#pragma once
#include <string>

class Exception
{
public:
	Exception(const std::string& message) : m_message(message){}
	virtual ~Exception() {}

	const std::string& getMessage() const { return m_message; }
private:
	std::string m_message;
};

