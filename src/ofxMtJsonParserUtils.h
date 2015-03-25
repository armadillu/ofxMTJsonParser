//
//  ofxMtJsonParserUtils.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#ifndef BaseApp_ofxMtJsonParserUtils_h
#define BaseApp_ofxMtJsonParserUtils_h

#include "ofxJson.h"

class ofxMtJsonParserUtils{

public:
	
	static inline string initFromJsonString(const ofxJSONElement& json, const string& key, bool verbose){
		if(json[key].isString()) {
			string ret = json[key].asString();
			if(verbose){
				ofLogNotice("ofxMtJsonParserUtils") << key << " = " << ret;
			}
			return ret;
		}
		ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonString '" << key << "'" ;
		return "";
	}

	static inline int initFromJsonInt(const ofxJSONElement& json, const string& key, bool verbose){
		if(!json[key].isNull()) {
			int v = json[key].asInt();
			if(verbose){
				ofLogNotice("ofxMtJsonParserUtils") << key << " = " << v;
			}
			return v;
		}
		ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonInt '" << key << "'" ;
		return 0;
	}

	static inline float initFromJsonFloat(const ofxJSONElement& json, const string& key, bool verbose){
		if(!json[key].isNull()) {
			float f = json[key].asFloat();
			if(verbose){
				ofLogNotice("ofxMtJsonParserUtils") << key << " = " << f;
			}
			return f;
		}
		ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonInt '" << key << "'" ;
		return 0.0f;
	}

	static inline bool initFromJsonBool(const ofxJSONElement& json, const string& key, bool verbose){
		if(!json[key].isNull()) {
			bool b = json[key].asBool();
			if(verbose){
				ofLogNotice("ofxMtJsonParserUtils") << key << " = " << b;
			}
			return b;
		}
		ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonInt '" << key << "'" ;
		return false;
	}

};



#define initFromJsonBool(key) if(!entry[#key].isNull()) {						\
									key = entry[#key].asBool();					\
								}else{											\
									/*LOG_WARNING << "Key is Null: " << #key;*/	\
								}


#endif
