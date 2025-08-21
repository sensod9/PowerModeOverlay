#ifndef CONFIGPARSERCLASS_H
#define CONFIGPARSERCLASS_H

#include <string>
#include <unordered_map>

class ConfigParser
{
public:
	bool load(const std::string& filename);
	std::string getString(const std::string& key, const std::string& defaultValue = "");
	int getInt(const std::string& key, const int defaultValue = 0);
private:
	std::unordered_map<std::string, std::string> config;
	void trim(std::string& str);
};

#endif // CONFIGPARSERCLASS_H