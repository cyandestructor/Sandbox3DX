#include "jasspch.h"
#include "JassUtilities.h"

#include <locale>
#include <codecvt>

namespace Jass {

	namespace Utilities {

		namespace Strings {

			std::vector<std::string> Split(const std::string& str, char separator)
			{
				std::vector<std::string> strings;

				size_t begin = 0;
				size_t end = str.find_first_of(separator, begin);
				end = (end == std::string::npos) ? str.size() : end;
				while (end != str.size())
				{
					strings.push_back(str.substr(begin, end - begin));
					begin = end;
					end = str.find_first_of(separator, begin);
					end = (end == std::string::npos) ? str.size() : end;
				}

				return strings;
			}

			std::vector<std::string> Split(const std::string& str, const std::string& separator)
			{
				std::vector<std::string> strings;

				size_t begin = 0;
				size_t end = str.find_first_of(separator, begin);
				end = (end == std::string::npos) ? str.size() : end;
				while (end != str.size())
				{
					strings.push_back(str.substr(begin, end - begin));
					begin = end;
					end = str.find_first_of(separator, begin);
					end = (end == std::string::npos) ? str.size() : end;
				}

				return strings;
			}

			std::wstring ToWstring(const std::string& str)
			{
				// https://stackoverflow.com/a/18597384
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				return converter.from_bytes(str);
			}

		}

	}

}
