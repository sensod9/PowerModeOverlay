#include "../include/ConfigParser.hpp"
#include <algorithm>
#include <fstream>

void ConfigParser::trim(std::string& str)
{
	if (std::isspace(str[0]))
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](char ch) {
			return !std::isspace(ch);
		}));
	str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), str.end());
}

bool ConfigParser::load(const std::string& filename)
{
	std::ifstream file(filename);
	
	std::string line;
	if (file.is_open()) {
		while (std::getline(file, line))
		{
			std::size_t found = line.find(':');
			if (found != std::string::npos) {
				std::string key = line.substr(0, found);
				std::string value = line.substr(found + 1);
				trim(key);
				trim(value);
				if (!key.empty() && !value.empty()) {
					config[key] = value;
				}
			}
		}
		file.close();
		return true;
	}
	else {
		return false;
	}
}

std::string ConfigParser::getString(const std::string& key, const std::string& defaultValue)
{
	auto it = config.find(key);
	if (it != config.end()) {
		if (config[key] != "default")
			return config[key];
		return defaultValue;
	}
	return defaultValue;
}

int ConfigParser::getInt(const std::string& key, const int defaultValue)
{
	auto it = config.find(key);
	if (it != config.end()) {
		if (config[key] != "default")
			return stoi(config[key]);
		return defaultValue;
	}
	return defaultValue;
}
