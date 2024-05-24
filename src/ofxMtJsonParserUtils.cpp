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


const std::string & ofxMtJsonParserUtils::initFromJsonString(const ofJson& json, const std::string& key, 
															 bool verbose, ofMutex * mutex, const std::string & defaultVal){
	if(json.contains(key) && json[key].is_string()) {
		const string & data = json[key].get_ref<const std::string&>();
		if(verbose){
			LOCK_PRINT_MUTEX
			ofLogNotice("ofxMtJsonParserUtils") << key << " = " << data;
			UNLOCK_PRINT_MUTEX
		}
		return data;
	}
	LOCK_PRINT_MUTEX
	ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonString '" << key << "'" ;
	UNLOCK_PRINT_MUTEX
	return defaultVal;
}


int ofxMtJsonParserUtils::initFromJsonInt(const ofJson& json, const std::string& key, bool verbose, 
										  ofMutex * mutex, int defaultVal){
	if(json.contains(key) && json[key].is_number()) {
		int v = json[key];
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


float ofxMtJsonParserUtils::initFromJsonFloat(const ofJson& json, const std::string& key, bool verbose, 
											  ofMutex * mutex, float defaultVal){
	if(json.contains(key) && json[key].is_number()) {
		float f = json[key];
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


bool ofxMtJsonParserUtils::initFromJsonBool(const ofJson& json, const std::string& key, bool verbose, 
											ofMutex * mutex, bool defaultVal){

	if(json.contains(key) && json.is_boolean() ){
		bool b = json[key];
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
