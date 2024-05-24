//
//  ofxMtJsonParserUtils.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofTypes.h"

class ofxMtJsonParserUtils{

public:
	static const std::string & initFromJsonString(const ofJson& json, const std::string& key, bool verbose, ofMutex * mutex = NULL, const std::string & defaultVal = "");
	static int initFromJsonInt(const ofJson& json, const std::string& key, bool verbose, ofMutex * mutex = NULL, int defaultVal = -1);
	static float initFromJsonFloat(const ofJson& json, const std::string& key, bool verbose, ofMutex * mutex = NULL, float defaultVal = -1.0f);
	static bool initFromJsonBool(const ofJson& json, const std::string& key, bool verbose, ofMutex * mutex = NULL, bool defaultVal = false);
};
