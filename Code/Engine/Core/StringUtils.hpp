#pragma once

#include <string>
#include <vector>

const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

std::vector<std::string> Split(const char* string, char delim = ',');
std::vector<std::string> Split(const std::string& string, char delim = ',');

std::string ToUpperCase(std::string str);
std::string ToLowerCase(std::string str);


