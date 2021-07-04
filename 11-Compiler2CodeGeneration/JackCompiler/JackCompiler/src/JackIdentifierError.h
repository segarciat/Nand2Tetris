#pragma once
#include <exception>
#include <string>

namespace jack {
class JackIdentifierError : public std::exception
{
protected:
	std::string msg_;
public:
	JackIdentifierError(const std::string message) :
		msg_{ "JackIdentifierError: " + message } {}
	virtual const char* what() const noexcept override
	{
		return msg_.c_str();
	}
};
} // namespace jack

