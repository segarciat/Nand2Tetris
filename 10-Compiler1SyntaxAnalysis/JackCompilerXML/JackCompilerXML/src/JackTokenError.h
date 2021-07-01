#pragma once
#include <exception>
#include <string>

class JackTokenError: public std::exception
{
private:
	std::string msg_;
public:
	JackTokenError(const std::string message) :
		msg_{ "Token Error: " + message } {}
	virtual const char* what() const noexcept override
	{
		return msg_.c_str();
	}
};