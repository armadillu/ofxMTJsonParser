//
//  MyJsonParserThread.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#ifndef BaseApp_MyJsonParserThread_h
#define BaseApp_MyJsonParserThread_h


#include "ofxMtJsonParser.h"
#include "ofxJSON.h"
#include "MyParseableObject.h"

class MyJsonParserThread : public ofxMtJsonParserThread<MyParseableObject>{

public:

	void parseJsonSubsetThread(){

		ofxJSONElement & jsonRef = *json; //pointers mess up the json syntax somehow

		vector<string>allKeys = jsonRef["data"].getMemberNames();

		int start = config.startIndex;
		int end = config.endIndex;

		//only parse our subset of the JSON objects

		for(int i = start; i < end; i++){
			string key = allKeys[i];
			Json::Value &thisObject = jsonRef["data"][key];

			MyParseableObject * o = new MyParseableObject();

			o->title = thisObject["title"].asString();
			o->description = thisObject["description"].asString();
			o->location = thisObject["location"].asString();

			////////////////////////////////////////////////////////////////////
			// THIS IS KEY! store the new parsed object in the superclass array
			// and update progress!/////////////////////////////////////////////
			parsedObjects.push_back(o);
			numParsedObjects = i - start + 1;
			////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////
		}
	}

	int getNumEntriesInJson(ofxJSONElement* json_){

		ofxJSONElement & jsonRef = *json_; //pointers mess up the json syntax somehow

		if(jsonRef.isObject()){
			if(jsonRef["data"].isObject()){
				return jsonRef["data"].size();
			}
		}
		ofLogError("MyJsonParserThread") << "JSON has unexpected format";
		return 0;
	}

private:


};

#endif
