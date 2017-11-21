#pragma once

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "Engine/Core/EngineSubsystem.hpp"

constexpr const int DEFAULT_CLIENT_WIDTH = 800;
constexpr const int DEFAULT_CLIENT_HEIGHT = 600;

constexpr const int DEFAULT_WINDOW_WIDTH = 800;
constexpr const int DEFAULT_WINDOW_HEIGHT = 600;

class Config : public EngineSubsystem {
public:
	Config(const std::string& configFilePath, const std::string& cmdLineParams = std::string(""));
	~Config();

    void GetValue(const std::string& key, char& value);
    void GetValue(const std::string& key, unsigned char& value);
    void GetValue(const std::string& key, signed char& value);
    void GetValue(const std::string& key, bool& value);
    void GetValue(const std::string& key, unsigned int& value);
    void GetValue(const std::string& key, int& value);
    void GetValue(const std::string& key, long& value);
    void GetValue(const std::string& key, unsigned long& value);
    void GetValue(const std::string& key, long long& value);
    void GetValue(const std::string& key, unsigned long long& value);
    void GetValue(const std::string& key, float& value);
    void GetValue(const std::string& key, double& value);
    void GetValue(const std::string& key, long double& value);
    void GetValue(const std::string& key, std::string& value);
    
    void SetValue(const std::string& key, const char& value);
    void SetValue(const std::string& key, const unsigned char& value);
    void SetValue(const std::string& key, const signed char& value);
    void SetValue(const std::string& key, const bool& value);
    void SetValue(const std::string& key, const unsigned int& value);
    void SetValue(const std::string& key, const int& value);
    void SetValue(const std::string& key, const long& value);
    void SetValue(const std::string& key, const unsigned long& value);
    void SetValue(const std::string& key, const long long& value);
    void SetValue(const std::string& key, const unsigned long long& value);
    void SetValue(const std::string& key, const float& value);
    void SetValue(const std::string& key, const double& value);
    void SetValue(const std::string& key, const long double& value);
    void SetValue(const std::string& key, const std::string& value);
    void SetValue(const std::string& key, const char* value);

    bool LoadFromFile(const std::string& filepath);
    void PrintConfigs(std::ostream& output = std::cout) const;

    friend std::ostream& operator<<(std::ostream& output, const Config& config);
    friend std::istream& operator>>(std::istream& input, Config& config);


    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;
protected:
    bool Parse(std::ifstream& input);
    bool Parse(std::istream& input);
    bool Parse(const std::string& input);
    bool ParseMultiParams(const std::string& input);
    std::string GetFileContents(const std::string& filepath);
    bool CreateDefaultConfig();
private:

	std::map<std::string, std::string> _config;
    std::string _configFilePath;
    std::string _configCmdParams;

};