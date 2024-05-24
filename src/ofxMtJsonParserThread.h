//
//  ofxMtJsonParserThread.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofThread.h"

class ParsedObject;

class ofxMtJsonParserThread: public ofThread{

public:

	struct Config{
		int threadID;
		int startIndex;
		int endIndex;
	};

	// We need you to intervene in 2 places, for 2 actions to take place //////////////////////////
	// 1 - LOCATE JSON OBJECT ARRAY OR DICTIONARY /////////////////////////////////////////

	struct JsonStructureData{
		ofJson * fullJson = nullptr;	//this will provide you the full json data
		ofJson * objectArray = nullptr;	//you are supposed to send back a ptr to the json structure
												//that has the object array OR dictionary you want to parse
	};

	// 2 - YOUR CUSTOM PARSING HERE /////////////////////////////////////////

	struct SingleObjectParseData{ //data sent to the user for him/her to create a single object from json
		int threadID;
		std::string objectID;
		ofJson * jsonObj = nullptr;
		ofMutex * printMutex = nullptr;
		ParsedObject * object = nullptr;
		//its the event listener's job to allocate a new ParsedObject,
		//"fill it in" with data from the json, and assign it to object.

		ofJson * userData = nullptr;
		//custom user data you might need inside the thread.
		//be extra careful NOT TO WRITE into it from the thread!
		//READ ONLY!
	};

	////////////////////////////////////////////////////////////////////////////////////////////////


	ofxMtJsonParserThread();
	
	void startParsing(ofJson * json_,
					  ofxMtJsonParserThread::Config config,
					  ofMutex * printMutex_,
					  std::function<void (SingleObjectParseData &)> parseOneObject,
					  ofJson * userData
					  );


	int getNumParsedObjects();
	int getNumObjectsToParse(); //total # of objects to parse
	int getNumObjectsLeftToParse();
	float getPercentDone();

	//only call when Thread is finished, or you will get crashes
	std::vector<ParsedObject*> getParsedObjects(){return parsedObjects;}

protected:

	// SUBCLASS SHOULD STORE LIST OF PARSED OBJECTS HERE ///////////////
	std::vector<ParsedObject*> parsedObjects;

	// SUBCLASS SHOULD UPDATE THIS AS IT PARSES ///////////////////////
	int numParsedObjects;

	// TO BE USED BY SUBCLASS ////////////////////////////////////////
	ofJson* json = nullptr;
	ofMutex * printMutex;

	Config config;

	std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseOneObject;
	ofJson * userData = nullptr;

private:

	int numObjectsToParse;
	void threadedFunction();
};

