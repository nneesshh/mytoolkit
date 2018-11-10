#include "ConfigManager.h"

#include "cocos2d.h"

ConfigManager g_configs;

bool ConfigManager::load() {
	std::string fullPathFile = cocos2d::FileUtils::getInstance()->fullPathForFilename("settings.json");
	if (fullPathFile.empty())
		return false;

	// read config file
	std::string fileContent = cocos2d::FileUtils::getInstance()->getStringFromFile(fullPathFile);

	if (fileContent.empty())
		return false;

	_settings = std::make_shared <JSONDocument>();
	if (_settings->Parse<0>(fileContent.c_str()).HasParseError()) {
		CCLOG("read settings json file %s failed because of %s", fullPathFile.c_str(), _settings->GetParseError());
		return false;
	}
	return true;
}
