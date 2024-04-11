#include "Config.h"

ServerConfig ServerConfigLoad(
	CString FilePath
) {
	ServerConfig Config = { 0 };
	memset(&Config, 0, sizeof(struct _ServerConfig));

#define CONFIG_BEGIN(__NAMESPACE__) \
	{ \
		struct _Config##__NAMESPACE__ *Namespace = &Config.__NAMESPACE__;

#define CONFIG_PARAMETER(__TYPE__, __NAME__, __PATH__, __DEFAULT__) \
        CONCAT(ReadConfig, __TYPE__)(FilePath, __PATH__, #__DEFAULT__, &Namespace->__NAME__);

#define CONFIG_PARAMETER_ARRAY(__TYPE__, __LENGTH__, __NAME__, __PATH__, __DEFAULT__) \
        CONCAT(ReadConfig, __TYPE__ ## Array)(FilePath, __PATH__, #__DEFAULT__, (__TYPE__*)&Namespace->__NAME__, __LENGTH__);

#define CONFIG_END(__NAMESPACE__) \
	};

#include "ConfigDefinition.h"

	return Config;
}
