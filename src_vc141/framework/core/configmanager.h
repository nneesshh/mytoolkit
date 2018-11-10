#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "cocostudio/CocoStudio.h" // for rapid json
#include "declarations.h"

typedef rapidjson::Value JSONValue;
typedef rapidjson::Document JSONDocument;
typedef rapidjson::SizeType JSONSizeType;
typedef std::shared_ptr<JSONDocument> JSONDocumentPtr;

// @bindsingleton g_configs
class ConfigManager
{
public:
	bool load();

	inline JSONDocumentPtr getSettings() { return _settings; }
	inline const JSONValue& getSettingsValue(const char *key) { return (*(_settings.get()))[key]; }

private:
	JSONDocumentPtr _settings;
};

extern ConfigManager g_configs;

#endif // _CONFIG_H_
