//
//  ofxMtJsonParserUtils.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#include "ofxMtJsonParserUtils.h"

#define LOCK_PRINT_MUTEX 		if(mutex) mutex->lock();
#define UNLOCK_PRINT_MUTEX		if(mutex) mutex->unlock();


std::string ofxMtJsonParserUtils::initFromJsonString(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex, std::string defaultVal){
	if(json[key].isString()) {
		std::string ret = json[key].asString();
		if(verbose){
			LOCK_PRINT_MUTEX
			ofLogNotice("ofxMtJsonParserUtils") << key << " = " << ret;
			UNLOCK_PRINT_MUTEX
		}
		return ret;
	}
	LOCK_PRINT_MUTEX
	ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonString '" << key << "'" ;
	UNLOCK_PRINT_MUTEX
	return defaultVal;
}


int ofxMtJsonParserUtils::initFromJsonInt(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex, int defaultVal){
	if(!json[key].isNull()) {
		int v = json[key].asInt();
		if(verbose){
			LOCK_PRINT_MUTEX
			ofLogNotice("ofxMtJsonParserUtils") << key << " = " << v;
			UNLOCK_PRINT_MUTEX
		}
		return v;
	}
	LOCK_PRINT_MUTEX
	ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonInt '" << key << "'" ;
	UNLOCK_PRINT_MUTEX
	return defaultVal;
}


float ofxMtJsonParserUtils::initFromJsonFloat(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex, float defaultVal){
	if(!json[key].isNull()) {
		float f = json[key].asFloat();
		if(verbose){
			LOCK_PRINT_MUTEX
			ofLogNotice("ofxMtJsonParserUtils") << key << " = " << f;
			UNLOCK_PRINT_MUTEX
		}
		return f;
	}
	LOCK_PRINT_MUTEX
	ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonFloat '" << key << "'" ;
	UNLOCK_PRINT_MUTEX
	return defaultVal;
}


bool ofxMtJsonParserUtils::initFromJsonBool(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex, bool defaultVal){

	if(json.isMember(key)) {
		bool b = json[key].asBool();
		if(verbose){
			LOCK_PRINT_MUTEX
			ofLogNotice("ofxMtJsonParserUtils") << key << " = " << b;
			UNLOCK_PRINT_MUTEX
		}
		return b;
	}
	LOCK_PRINT_MUTEX
	ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonBool '" << key << "'" ;
	UNLOCK_PRINT_MUTEX
	return defaultVal;
}


#undef LOCK_PRINT_MUTEX
#undef UNLOCK_PRINT_MUTEX
