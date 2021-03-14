#ifndef JASS_UTILITIES_H
#define JASS_UTILITIES_H

namespace Jass {

	namespace Utilities {

		namespace Strings {

			std::vector<std::string> Split(const std::string& str, char separator);
			std::vector<std::string> Split(const std::string& str, const std::string& separator);

			std::wstring ToWstring(const std::string& str);
		}

	}

}

#endif // !JASS_UTILITIES_H
