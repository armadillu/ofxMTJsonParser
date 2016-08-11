//
//  ofxMtJsonParserThread.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#pragma once

#include "ofThread.h"
#include "ofxJSON.h"
#include "ofxMtJsonParserConfig.h"
#include "ofxMtJsonParserUtils.h"

//every parsed object will have to inherit this class
class ParsedObject{};


class ofxMtJsonParserThread: public ofThread{

public:

	struct Config{
		int threadID;
		int startIndex;
		int endIndex;
	};

	// We need you to intervene in 2 places, for 2 actions to take place //////////////////////////
	// 1 - LOCATE JSON OBJECT ARRAY /////////////////////////////////////////

	struct JsonStructureData{
		ofxJSONElement * fullJson; //this will provide you the full json data
		ofxJSONElement * objectArray; 	//you are supposed to send back a ptr to the json structure that has
		//the object array you want to parse
		JsonStructureData(){
			objectArray = objectArray = nullptr;
		}
	};

	// 2 - YOUR CUSTOM PARSING HERE /////////////////////////////////////////

	struct SingleObjectParseData{
		int threadID;
		int objectID;
		ofxJSONElement * jsonObj;
		ofMutex * printMutex;
		ParsedObject * object; 	//its the event listener's job to allocate a new ParsedObject,
		//"fill it in" with data from the json, and assign it to object.
		SingleObjectParseData(){
			printMutex = nullptr;
			object = nullptr;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////


	ofxMtJsonParserThread();

	
	void startParsing(ofxJSONElement* json_,
					  ofxMtJsonParserThread::Config config,
					  ofMutex * printMutex_,
					  std::function<void (SingleObjectParseData &)> parseSingleObjectUserLambda
					  );





	int getNumParsedObjects();
	int getNumObjectsToParse(); //total # of objects to parse
	int getNumObjectsLeftToParse();
	float getPercentDone();

	//only call when Thread is finished, or you will get crashes
	vector<ParsedObject*> getParsedObjects(){return parsedObjects;}

protected:

	// SUBCLASS SHOULD STORE LIST OF PARSED OBJECTS HERE ///////////////
	vector<ParsedObject*> parsedObjects;

	// SUBCLASS SHOULD UPDATE THIS AS IT PARSES ///////////////////////
	int numParsedObjects;

	// TO BE USED BY SUBCLASS ////////////////////////////////////////
	ofxJSONElement* json;
	ofMutex * printMutex;

	ofxMtJsonParserConfig * args;
	Config config;

	std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseSingleObjectUserLambda;

private:

	int numObjectsToParse;
	void threadedFunction();
};

