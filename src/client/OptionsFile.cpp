#include "OptionsFile.h"
#include <stdio.h>
#include <string.h>
#include <cctype>
#include <cstdlib>
#ifdef _WIN32
#include <stdlib.h>
#else
#include <sys/stat.h>
#endif

OptionsFile::OptionsFile() {
#ifdef __APPLE__
	settingsPath = "./Documents/options.txt";
#elif defined(ANDROID)
	settingsPath = "options.txt";
#else
	char* home = getenv("HOME");
	if (home) {
		settingsPath = std::string(home) + "/.minecraft/mcpe/options.txt";
	} else {
		settingsPath = "options.txt";
	}
#endif
}

void OptionsFile::setPath(const std::string& path) {
	settingsPath = path;
}

void OptionsFile::save(const StringVector& settings) {
	// Create directory if it doesn't exist
	size_t lastSlash = settingsPath.find_last_of('/');
	if (lastSlash != std::string::npos) {
		std::string dirPath = settingsPath.substr(0, lastSlash);
		// Create directories recursively
		for (size_t i = 1; i <= dirPath.length(); i++) {
			if (i == dirPath.length() || dirPath[i] == '/') {
				std::string subDir = dirPath.substr(0, i);
#ifdef _WIN32
				_mkdir(subDir.c_str());
#else
				mkdir(subDir.c_str(), 0755);
#endif
			}
		}
	}
	
	FILE* pFile = fopen(settingsPath.c_str(), "w");
	if(pFile != NULL) {
		for(StringVector::const_iterator it = settings.begin(); it != settings.end(); ++it) {
			fprintf(pFile, "%s\n", it->c_str());
		}
		fclose(pFile);
	}
}

StringVector OptionsFile::getOptionStrings() {
	StringVector returnVector;
	FILE* pFile = fopen(settingsPath.c_str(), "r");
	if(pFile != NULL) {
		char lineBuff[128];
		while(fgets(lineBuff, sizeof lineBuff, pFile)) {
			if(strlen(lineBuff) > 2) {
				// Remove newline character
				char* newline = strchr(lineBuff, '\n');
				if (newline) *newline = '\0';
				
				// Split line into key=value or key = value
				char* equals = strchr(lineBuff, '=');
				if (equals) {
					*equals = '\0';
					// Trim whitespace from key
					char* key = lineBuff;
					while(*key && isspace(*key)) key++;
					char* keyEnd = key + strlen(key) - 1;
					while(keyEnd > key && isspace(*keyEnd)) keyEnd--;
					*(keyEnd + 1) = '\0';
					
					// Trim whitespace from value
					char* value = equals + 1;
					while(*value && isspace(*value)) value++;
					char* valueEnd = value + strlen(value) - 1;
					while(valueEnd > value && isspace(*valueEnd)) valueEnd--;
					*(valueEnd + 1) = '\0';
					
					returnVector.push_back(std::string(key));
					returnVector.push_back(std::string(value));
				}
			}
		}
		fclose(pFile);
	}
	return returnVector;
}
