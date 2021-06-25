#pragma once
#include <exception>
#include <string>

namespace HackVM
{
	class InvalidCommand: public std::exception
	{
	private:
		std::string msg_;
	public:
		InvalidCommand(const std::string& message) :
			msg_(message + ": Parsing Error")
		{

		}
		virtual const char* what() const noexcept override
		{
			return msg_.c_str();
		}
	};
};