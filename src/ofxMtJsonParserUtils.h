//
//  ofxMtJsonParserUtils.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#ifndef BaseApp_ofxMtJsonParserUtils_h
#define BaseApp_ofxMtJsonParserUtils_h

#define PRINT_STRING_VAR(var) ofLogNotice() << "  " << #var << ": " << var.substr(0, MIN(var.length(), 80) );
#define PRINT_BOOL_VAR(var) ofLogNotice() << "  " << #var << ": " << string(var ? "TRUE" : "FALSE");
#define PRINT_INT_VAR(var) ofLogNotice() << "  " << #var << ": " << var;
#define NAME_OF_VAR(var) #var

#include "ofxJson.h"

class ofxMtJsonParserUtils{

	static inline string initFromJsonString(const ofxJSONElement& json, const string& key, bool verbose){
		if(json[key].isString()) {
			return json[key].asString();
			if( key.size() == 0){
				/*LOG_ERROR << "entry has empty '" << #key <<"' field";*/\
			}
			if(verbose){
				PRINT_STRING_VAR(key);
			}
		}
		ofLogError("ofxMtJsonParserUtils") << "Can't initFromJsonString '" << key << "'" ;
		return "";
	}

};


#define initFromJsonString(key, verbose)\
								if(!entry[#key].isNull()) {						\
									key = entry[#key].asString();				\
									if( key.size() == 0){						\
										/*LOG_ERROR << "entry has empty '" << #key <<"' field";*/\
									}											\
									if(verbose){PRINT_STRING_VAR(key);}			\
								}else{											\
									/*LOG_WARNING << "Key is Null: " << #key;*/	\
								}

#define initFromJsonMongoDate(key) if(!entry[#key].isNull()) {						\
										key = entry[#key]["$date"].asUInt64();		\
									}else{											\
										/*LOG_WARNING << "Key is Null: " << #key;*/		\
									}


#define initFromJsonInt(key) if(!entry[#key].isNull()) {						\
									key = entry[#key].asInt();					\
								}else{											\
									/*LOG_ERROR << "Key is Null: " << #key;*/	\
								}

#define initFromJsonFloat(key) if(!entry[#key].isNull()) {						\
									key = entry[#key].asFloat();				\
								}else{											\
									/*LOG_ERROR << "Key is Null: " << #key;*/	\
								}

#define initFromJsonBool(key) if(!entry[#key].isNull()) {						\
									key = entry[#key].asBool();					\
								}else{											\
									/*LOG_WARNING << "Key is Null: " << #key;*/	\
								}


#endif
