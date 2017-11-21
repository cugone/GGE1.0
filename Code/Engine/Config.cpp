#include "Engine/Config.hpp"

#include <fstream>
#include <filesystem>
#include <sstream>
#include <locale>
#include <algorithm>

#include "Engine/Core/ErrorWarningAssert.hpp"

Config::Config(const std::string& configFilePath, const std::string& cmdLineParams)
: _config()
, _configFilePath(configFilePath)
, _configCmdParams(cmdLineParams)

{

    if (_configFilePath.empty()) {
        if (!CreateDefaultConfig()) {
            DebuggerPrintf("\nWARNING: Default Config file failed to parse correctly.");
        }
    } else {
        std::ifstream input;
        input.open(_configFilePath);
        bool successful = Parse(input);
        input.close();

        if (!successful) {
            DebuggerPrintf("\nWARNING: Config file failed to parse correctly. Recreating Default.");
            if(!CreateDefaultConfig()) {
                DebuggerPrintf("\nWARNING: Default Config file failed to parse correctly.");
            }
        }
    }
    if(!cmdLineParams.empty()) {
        bool cmdline_successful = Parse(cmdLineParams);
        if(!cmdline_successful) {
            DebuggerPrintf("\nWARNING: Command line params failed to parse correctly.");
        }
    }

}

Config::~Config() {
    /* DO NOTHING */
}

bool Config::CreateDefaultConfig() {
std::string default_config =
R"(

window_res_x = 1280
window_res_y = 720
window_title = "CURRENT ASSIGNMENT"

config_path = "Data/Config/default.config"
shaderprogram_path = "Data/Shaders/" #Folder to shaders

image_path = "Data/Images/" #Folder to images
font_path = "Data/Fonts/" #Folder to fonts

device_font = "trebuchetMS32"
system_font = "system32"

)";

std::ofstream f;
f.open("Data/Config/default.config");
if(!(f << default_config)) {
    DebuggerPrintf("Failed to create default config file.");
}
f.close();
    std::istringstream ss;
    ss.str(default_config);
    return Parse(ss);
}

void Config::PrintConfigs(std::ostream& output /*= std::cout*/) const {
    for(auto iter = _config.begin(); iter != _config.end(); ++iter) {
        bool value_has_space = iter->second.find(' ') != std::string::npos;
        output << iter->first << '=';
        if(value_has_space) {
            output << '"';
        }
        output << iter->second;
        if(value_has_space) {
            output << '"';
        }
        output << '\n';
#ifdef _DEBUG
        { //Temporary string stream
            std::ostringstream ss;
            value_has_space = iter->second.find(' ') != std::string::npos;
            ss << iter->first << '=';
            if(value_has_space) {
                ss << '"';
            }
            ss << iter->second;
            if(value_has_space) {
                ss << '"';
            }
            ss << '\n';
            DebuggerPrintf(ss.str().c_str());
        }
#endif
    }
}

bool Config::ProcessSystemMessage(const SystemMessage& /*msg*/) {
    return false;
}
void Config::GetValue(const std::string& key, char& value) {
    value = *_config[key].begin();
}

void Config::GetValue(const std::string& key, unsigned char& value) {
    value = static_cast<unsigned char>(std::stoul(_config[key]));
}

void Config::GetValue(const std::string& key, signed char& value) {
    value = static_cast<signed char>(std::stoi(_config[key]));
}

void Config::GetValue(const std::string& key, bool& value) {
    if(_config[key] == "true") {
        value = true;
    } else if(_config[key] == "false") {
        value = false;
    }
}

void Config::GetValue(const std::string& key, unsigned int& value) {
    value = static_cast<unsigned int>(std::stoul(_config[key]));
}

void Config::GetValue(const std::string& key, int& value) {
    value = std::stoi(_config[key]);
}

void Config::GetValue(const std::string& key, long& value) {
    value = std::stol(_config[key]);
}

void Config::GetValue(const std::string& key, unsigned long& value) {
    value = std::stoul(_config[key]);
}

void Config::GetValue(const std::string& key, long long& value) {
    value = std::stoll(_config[key]);
}

void Config::GetValue(const std::string& key, unsigned long long& value) {
    value = std::stoull(_config[key]);
}

void Config::GetValue(const std::string& key, float& value) {
    value = std::stof(_config[key]);
}

void Config::GetValue(const std::string& key, double& value) {
    value = std::stod(_config[key]);
}

void Config::GetValue(const std::string& key, long double& value) {
    value = std::stold(_config[key]);
}

void Config::GetValue(const std::string& key, std::string& value) {
    value = _config[key];
}

void Config::SetValue(const std::string& key, const char& value) {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const unsigned char& value) {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const signed char& value) {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const bool& value) {
    if(value) {
        _config[key] = "true";
    } else {
        _config[key] = "false";
    }
}

void Config::SetValue(const std::string& key, const unsigned int& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const int& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const long& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const unsigned long& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const long long& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const unsigned long long& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const float& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const double& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const long double& value) {
    _config[key] = std::to_string(value);
}

void Config::SetValue(const std::string& key, const std::string& value) {
    _config[key] = value;
}

void Config::SetValue(const std::string& key, const char* value) {
    SetValue(key, std::string(value));
}
bool Config::LoadFromFile(const std::string& filepath) {
    namespace FS = std::experimental::filesystem;
    FS::path p(filepath);
    if(p.has_extension() == false) {
        std::ostringstream ss;
        ss << "\nConfig files must be of type .config.";
        DebuggerPrintf(ss.str().c_str());
        ss.str("");
        return false;
    }
    std::string ext = p.extension().string();
    if(ext != ".config") {
        std::ostringstream ss;
        ss << "\nConfig files must be of type .config.";
        DebuggerPrintf(ss.str().c_str());
        ss.str("");
        return false;
    }
    
    std::string buffer(GetFileContents(p.string()));

    return Parse(buffer);
}

std::string Config::GetFileContents(const std::string& filepath) {

    std::ifstream input;
    input.open(filepath);

    std::stringstream ss;
    ss << input.rdbuf();
    input.close();

    ss.seekg(0);
    ss.seekp(0);
    ss.clear();

    std::string stream_contents(ss.str());
    return stream_contents;
}

bool Config::Parse(std::ifstream& input) {
    if(input.is_open() == false) {
        DebuggerPrintf("\nFile stream not initially open.");
        return false;
    }
    if(input.good() == false) {
        DebuggerPrintf("\nFile stream not initially in a good state.");
        return false;
    }
    return Parse(static_cast<std::istream&>(input));
}

bool Config::Parse(std::istream& input) {

    std::string cur_line;
    while(std::getline(input, cur_line)) {
        bool did_parse = Parse(cur_line);
        if(!did_parse) {
            return false;
        }
    }
    return true;
}

bool Config::Parse(const std::string& input) {
    //Strip comments.
    std::string cur_line = input.substr(0, input.find_first_of('#'));
    if(cur_line.empty()) {
        return true;
    }
    int eq_count = std::count(cur_line.begin(), cur_line.end(), '=');
    int true_count = std::count(cur_line.begin(), cur_line.end(), '+');
    int false_count = std::count(cur_line.begin(), cur_line.end(), '-');
    int nl_count = std::count(cur_line.begin(), cur_line.end(), '\n');
    bool probably_multiline = nl_count == 0 && (eq_count > 1 || true_count > 1 || false_count > 1);
    if(probably_multiline) {
        return ParseMultiParams(cur_line);
    }

    auto key_iter = std::begin(cur_line);
    //Shorthand cases
    if(*key_iter == '-') {
        ++key_iter;
        std::string key = cur_line.substr(std::distance(key_iter, key_iter + 1));
        _config[key] = "false";
        return true;
    }
    if(*key_iter == '+') {
        ++key_iter;
        std::string key = cur_line.substr(std::distance(key_iter, key_iter + 1));
        _config[key] = "true";
        return true;
    }
    //Get raw key-value pairs split on equals.
    while(key_iter != std::end(cur_line) && *key_iter != '=') {
        ++key_iter;
    }
    //Get Key
    std::string key = cur_line.substr(0, std::distance(std::begin(cur_line), key_iter));
    if(key_iter != std::end(cur_line) && *key_iter == '=') {
        ++key_iter; //skip '=';
    }
    auto value_iter = key_iter;
    while(value_iter != std::end(cur_line)) {
        ++value_iter;
    }
    //Get value
    std::string value = cur_line.substr(std::distance(std::begin(cur_line), key_iter), std::distance(key_iter, value_iter));

    //Trim whitespace
    std::string::iterator first_key_non_whitespace = std::begin(key);
    if(first_key_non_whitespace != std::end(key)) {
        while(std::isspace(*first_key_non_whitespace, std::locale(""))) {
            first_key_non_whitespace = std::find_if_not(first_key_non_whitespace,
                                                        std::end(key),
                                                        [](char c) {
                return std::isspace(c, std::locale(""));
            });
        }
    }
    key = key.substr(std::distance(std::begin(key), first_key_non_whitespace));

    std::string::reverse_iterator last_key_non_whitespace = std::rbegin(key);
    if(last_key_non_whitespace != std::rend(key)) {
        while(std::isspace(*last_key_non_whitespace, std::locale(""))) {
            last_key_non_whitespace = std::find_if_not(last_key_non_whitespace,
                                                       std::rend(key),
                                                       [](char c) {
                return std::isspace(c, std::locale(""));
            });
        }
    }
    key = key.substr(0, std::distance(std::begin(key), last_key_non_whitespace.base()));


    std::string::iterator first_value_non_whitespace = std::begin(value);
    if(first_value_non_whitespace != std::end(value)) {
        while(std::isspace(*first_value_non_whitespace, std::locale(""))) {
            first_value_non_whitespace = std::find_if_not(first_value_non_whitespace,
                                                          std::end(value),
                                                          [](char c) {
                return std::isspace(c, std::locale(""));
            });
        }
    }
    value = value.substr(std::distance(std::begin(value), first_value_non_whitespace));

    std::string::reverse_iterator last_value_non_whitespace = std::rbegin(value);
    if(last_value_non_whitespace != std::rend(value)) {
        while(std::isspace(*last_value_non_whitespace, std::locale(""))) {
            last_value_non_whitespace = std::find_if_not(last_value_non_whitespace,
                                                         std::rend(value),
                                                         [](char c) {
                return std::isspace(c, std::locale(""));
            });
        }
    }
    value = value.substr(0, std::distance(std::begin(value), last_value_non_whitespace.base()));

    if(key.find('"') != std::string::npos) {
        key = key.substr(key.find_first_not_of("\""), key.find_last_not_of("\""));
    }
    if(value.find('"') != std::string::npos) {
        auto ffno = value.find_first_not_of('"');
        auto flno = value.find_last_not_of('"');
        if (ffno == std::string::npos || flno == std::string::npos) {
            if (ffno == std::string::npos) {
                value = value.substr(1);
            }
            if (flno == std::string::npos) {
                value.pop_back();
            }
        } else {
            value = value.substr(ffno, flno);
        }
    }

    SetValue(key, value);
    return true;
}

bool Config::ParseMultiParams(const std::string& input) {
    std::string whole_line = input;
    //Remove spaces around equals
    auto eq_loc = std::find(whole_line.begin(), whole_line.end(), '=');
    while(eq_loc != std::end(whole_line)) {
        auto left_space_eq = eq_loc - 1;
        while(std::isspace(*left_space_eq, std::locale(""))) {
            left_space_eq--;
        }
        eq_loc = whole_line.erase(left_space_eq + 1, eq_loc);
        auto right_space_eq = eq_loc + 1;
        while(std::isspace(*right_space_eq, std::locale(""))) {
            right_space_eq++;
        }
        eq_loc = whole_line.erase(eq_loc + 1, right_space_eq);
        eq_loc = std::find(eq_loc + 1, whole_line.end(), '=');
    }
    //Remove consecutive spaces
    whole_line.erase(std::unique(whole_line.begin(), whole_line.end(),
                              [](char lhs, char rhs) {
                                    return lhs == rhs && lhs == ' ';
                              }),
                   whole_line.end());
    //Replace space-delimited KVPs with newlines;
    bool inQuote = false;
    for(auto iter = whole_line.begin(); iter != whole_line.end(); ++iter) {
        if(*iter == '"') {
            inQuote = !inQuote;
            continue;
        }
        if(!inQuote) {
            if(*iter == ' ') {
                whole_line.replace(iter, iter + 1, "\n");
            }
        }
    }
    std::istringstream ss;
    ss.str(whole_line);
    std::string cur_line;
    while(std::getline(ss, cur_line)) {
        bool did_parse = Parse(cur_line);
        if(!did_parse) {
            return false;
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream& output, const Config& config) {
    config.PrintConfigs(output);
    return output;
}

std::istream& operator >> (std::istream& input, Config& config) {
    config.Parse(input);
    return input;
}