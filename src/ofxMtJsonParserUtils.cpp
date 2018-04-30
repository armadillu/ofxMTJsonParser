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


std::string ofxMtJsonParserUtils::initFromJsonString(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex){
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
	return "";
}


int ofxMtJsonParserUtils::initFromJsonInt(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex){
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
	return 0;
}


float ofxMtJsonParserUtils::initFromJsonFloat(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex){
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
	ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonInt '" << key << "'" ;
	UNLOCK_PRINT_MUTEX
	return 0.0f;
}


bool ofxMtJsonParserUtils::initFromJsonBool(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex){

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
	ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonInt '" << key << "'" ;
	UNLOCK_PRINT_MUTEX
	return false;
}


#undef LOCK_PRINT_MUTEX
#undef UNLOCK_PRINT_MUTEX
