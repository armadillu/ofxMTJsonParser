//
//  ofxMtJsonParserThread.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//


#include "ofxMtJsonParserThread.h"

template <class O>
ofxMtJsonParserThread<O>::ofxMtJsonParserThread(){
	numParsedObjects = 0;
	json = NULL;
	printMutex = NULL;
};

template <class O>
void ofxMtJsonParserThread<O>::startParsing(ofxJSONElement* json_,
										 ofxMtJsonParserConfig config_,
										 ofMutex * printMutex_){
	json = json_;
	config = config_;
	printMutex = printMutex_;
	startThread();
}

template <class O>
void ofxMtJsonParserThread<O>::threadedFunction(){

	try {
		getPocoThread().setName("ofxMtJsonParserThread");
	} catch (Poco::SystemException exc) {
		ofLogError("ofxMtJsonParserThread") << exc.what() << " " << exc.message()
		<< " " << exc.displayText();
	}
	numObjectsToParse = getNumEntriesInJson(json);
	parseJsonSubsetThread();
	ofLogNotice("ofxMtJsonParserThread")<< "Parsing Thread " << config.threadID << " finished";
	stopThread();
}

template <class O>
void ofxMtJsonParserThread<O>::parseJsonSubsetThread(){

	ofLogError("ofxMtJsonParserThread") << "your subclass must implement this!";
	//in your subclass, use json*, printMutex* and config to parse
	//the JSON elements from config.startIndex to config.endIndex
}

template <class O>
int ofxMtJsonParserThread<O>::getNumParsedObjects(){
	return numParsedObjects;

}

template <class O>
int ofxMtJsonParserThread<O>::getNumObjectsToParse(){
	return config.endIndex - config.startIndex;
}

template <class O>
int ofxMtJsonParserThread<O>::getNumObjectsLeftToParse(){
	ofLogNotice("ofxMtJsonParserThread") << "getNumObjectsLeftToParse " << numParsedObjects<< " " <<numObjectsToParse ;
	return numParsedObjects - numObjectsToParse;
}

template <class O>
float ofxMtJsonParserThread<O>::getPercentDone(){
	return float(numParsedObjects) / float(numObjectsToParse);
}




