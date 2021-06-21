#pragma once
#include <exception>

namespace Hack {
	class CommandError : public std::exception {
	public:
		virtual const char* what() const noexcept override
		{
			return "Invalid Hack command";
		}
	};
}