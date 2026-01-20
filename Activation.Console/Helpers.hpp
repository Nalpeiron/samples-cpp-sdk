#include <string>
#include <stdexcept>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <limits>

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

namespace InputHelper
{
	inline std::string TrimCopy(const std::string& input)
	{
		size_t start = 0;
		size_t end = input.size();

		while (start < end && std::isspace(static_cast<unsigned char>(input[start])))
		{
			++start;
		}

		while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1])))
		{
			--end;
		}

		return input.substr(start, end - start);
	}

	inline std::string ToLowerCopy(const std::string& input)
	{
		std::string output;
		output.reserve(input.size());
		for (unsigned char ch : input)
		{
			output.push_back(static_cast<char>(std::tolower(ch)));
		}
		return output;
	}

	inline bool TryParseSizeT(const std::string& input, size_t& value)
	{
		std::string trimmed = TrimCopy(input);
		if (trimmed.empty() || trimmed[0] == '-')
		{
			return false;
		}

		char* end = nullptr;
		errno = 0;
		unsigned long long parsed = std::strtoull(trimmed.c_str(), &end, 10);

		if (errno == ERANGE || end == trimmed.c_str() || *end != '\0')
		{
			return false;
		}

		if (parsed > std::numeric_limits<size_t>::max())
		{
			return false;
		}

		value = static_cast<size_t>(parsed);
		return true;
	}

	inline bool TryParseInt(const std::string& input, int& value)
	{
		std::string trimmed = TrimCopy(input);
		if (trimmed.empty())
		{
			return false;
		}

		char* end = nullptr;
		errno = 0;
		long parsed = std::strtol(trimmed.c_str(), &end, 10);

		if (errno == ERANGE || end == trimmed.c_str() || *end != '\0')
		{
			return false;
		}

		if (parsed < std::numeric_limits<int>::min() || parsed > std::numeric_limits<int>::max())
		{
			return false;
		}

		value = static_cast<int>(parsed);
		return true;
	}
}
