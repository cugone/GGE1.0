#include "Engine/Core/StringUtils.hpp"

#include <stdarg.h>

#include <algorithm>
#include <locale>
#include <sstream>

const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;

const std::string Stringf( const char* format, ... ) {
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}

const std::string Stringf( const int maxLength, const char* format, ... ) {
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

std::vector<std::string> Split(const char* string, char delim /*= ','*/) {
    return Split(std::string(string ? string : ""), delim);
}

std::vector<std::string> Split(const std::string& string, char delim /*= ','*/) {

    std::stringstream ss;

    ss.str(string);

    ss.seekg(0);
    ss.seekp(0);
    ss.clear();

    std::vector<std::string> result;
    std::string curString;
    while(std::getline(ss, curString, delim)) {
        if(curString.empty()) continue;
        result.push_back(curString);
    }

    return std::move(result);
}

std::string ToUpperCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) -> unsigned char { return std::toupper(c, std::locale("")); });
    return str;
}

std::string ToLowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) -> unsigned char { return std::tolower(c, std::locale("")); });
    return str;
}


