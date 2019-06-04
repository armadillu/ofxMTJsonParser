//
//  ofxMtJsonParserUtils.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#pragma once

#include "ofxJSON.h"
#include "ofTypes.h"

class ofxMtJsonParserUtils{

public:
	static std::string initFromJsonString(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex = NULL, std::string defaultVal = "");
	static int initFromJsonInt(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex = NULL, int defaultVal = -1);
	static float initFromJsonFloat(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex = NULL, float defaultVal = -1.0f);
	static bool initFromJsonBool(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex = NULL, bool defaultVal = false);
};
