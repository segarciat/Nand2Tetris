#pragma once
#include <exception>
#include <string>

namespace jack {
class JackTokenError : public std::exception
{
protected:
	std::string msg_;
public:
	JackTokenError(const std::string message) :
		msg_{ "JackTokenError: " + message } {}
	virtual const char* what() const noexcept override
	{
		return msg_.c_str();
	}
};
} // namespace jack
