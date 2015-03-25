//
//  ofxMtJsonParserThread.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//


#include "ofxMtJsonParserThread.h"

ofxMtJsonParserThread::ofxMtJsonParserThread(){
	numParsedObjects = 0;
	json = NULL;
	printMutex = NULL;
};

void ofxMtJsonParserThread::startParsing(ofxJSONElement* json_,
										 ofxMtJsonParserConfig config_,
										 ofMutex * printMutex_){
	json = json_;
	config = config_;
	printMutex = printMutex_;
	startThread();
}


void ofxMtJsonParserThread::threadedFunction(){

	try {
		getPocoThread().setName("ofxMtJsonParserThread");
	} catch (Poco::SystemException exc) {
		ofLogError("ofxMtJsonParserThread") << exc.what() << " " << exc.message()
		<< " " << exc.displayText();
	}
	numObjectsToParse = getNumEntriesInJson(json);
	parseJsonSubset();
	ofLogNotice("ofxMtJsonParserThread")<< "Parsing Thread " << config.threadID << " finished";
	stopThread();
}


void ofxMtJsonParserThread::parseJsonSubset(){

	ofLogError("ofxMtJsonParserThread") << "your subclass must implement this!";
	//in your subclass, use json*, printMutex* and config to parse
	//the JSON elements from config.startIndex to config.endIndex
}


int ofxMtJsonParserThread::getNumParsedObjects(){
	return numParsedObjects;

}


int ofxMtJsonParserThread::getNumObjectsToParse(){
	return config.endIndex - config.startIndex;
}


int ofxMtJsonParserThread::getNumObjectsLeftToParse(){
	ofLogNotice("ofxMtJsonParserThread") << "getNumObjectsLeftToParse " << numParsedObjects<< " " <<numObjectsToParse ;
	return numParsedObjects - numObjectsToParse;
}


float ofxMtJsonParserThread::getPercentDone(){
	return float(numParsedObjects) / float(numObjectsToParse);
}




