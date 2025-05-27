#include <string>
#include <stdexcept>
#include <cerrno>
#include <cstdlib>

class StringConverter
{
public:
	static long toLong(const std::string& str) {
		if (str.empty()) {
			throw std::invalid_argument("Input string is empty");
		}

		char* end;
		errno = 0;
		long result = std::strtol(str.c_str(), &end, 10);

		// Check for conversion errors
		if (errno == ERANGE)
		{
			throw std::out_of_range("Value out of range for long");
		}
		if (*end != '\0')
		{
			throw std::invalid_argument("Invalid characters in input string");
		}

		return result;
	}
};