//
//  ofxMtJsonParserThread.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#ifndef __BaseApp__ofxMtJsonParserThread__
#define __BaseApp__ofxMtJsonParserThread__

#include "ofThread.h"
#include "ofxJSON.h"
#include "ofxMtJsonParserConfig.h"
#include "ofxMtJsonParserUtils.h"

struct ofxMtJsonParserThreadConfig{
	int threadID;
	int startIndex;
	int endIndex;
};

template <class O>
class ofxMtJsonParserThread: public ofThread{

public:

	ofxMtJsonParserThread();

	void startParsing(ofxJSONElement* json_,
					  ofxMtJsonParserThreadConfig config,
					  ofxMtJsonParserConfig *args,
					  ofMutex * printMutex_);


	// YOUR CUSTOM PARSING HERE /////////////////////////////////////////
	virtual void parseJsonSubsetThread() = 0;

	// HOW MANY ELEMENTS TO PARSE IN JSON ///////////////////////////////
	virtual int getNumEntriesInJson(ofxJSONElement* json_) = 0;

	int getNumParsedObjects();
	int getNumObjectsToParse(); //total # of objects to parse
	int getNumObjectsLeftToParse();
	float getPercentDone();

	//only call when Thread is finished, or you will get crashes
	vector<O*> getParsedObjects(){return parsedObjects;}

protected:

	// SUBCLASS SHOULD STORE LIST OF PARSED OBJECTS HERE ///////////////
	vector<O*> parsedObjects;

	// SUBCLASS SHOULD UPDATE THIS AS IT PARSES ///////////////////////
	int numParsedObjects;

	// TO BE USED BY SUBCLASS ////////////////////////////////////////
	ofxJSONElement* json;
	ofMutex * printMutex;

	ofxMtJsonParserConfig * args;
	ofxMtJsonParserThreadConfig config;

private:

	int numObjectsToParse;
	void threadedFunction();
};


#endif /* defined(__BaseApp__ofxMtJsonParserThread__) */
