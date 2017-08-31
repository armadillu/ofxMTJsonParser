//
//  ofxMtJsonParserUtils.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#ifndef BaseApp_ofxMtJsonParserUtils_h
#define BaseApp_ofxMtJsonParserUtils_h

#include "ofxJSON.h"
#include "ofTypes.h"

#define LOCK_PRINT_MUTEX 		if(mutex) mutex->lock();
#define UNLOCK_PRINT_MUTEX		if(mutex) mutex->unlock();


class ofxMtJsonParserUtils{

public:

	static inline std::string initFromJsonString(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex = NULL){
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

	static inline int initFromJsonInt(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex = NULL){
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

	static inline float initFromJsonFloat(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex = NULL){
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

	static inline bool initFromJsonBool(const ofxJSONElement& json, const std::string& key, bool verbose, ofMutex * mutex = NULL){

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
};

#endif
