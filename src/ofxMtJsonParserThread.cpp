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
	args = NULL;
};


void ofxMtJsonParserThread::startParsing(ofxJSONElement* json_,
											ofxMtJsonParserThread::Config config_,
										 	ofMutex * printMutex_){
	json = json_;
	config = config_;
	printMutex = printMutex_;
	startThread();
}


void ofxMtJsonParserThread::threadedFunction(){

	#if( OF_VERSION_MINOR <= 9 )
	try {
		getPocoThread().setName("ofxMtJsonParserThread");
	} catch (Poco::SystemException exc) {
		printMutex->lock();
		ofLogError("ofxMtJsonParserThread") << exc.what() << " " << exc.message()
		<< " " << exc.displayText();
		printMutex->unlock();
	}
	#endif
	numObjectsToParse = config.endIndex - config.startIndex;
	//parseJsonSubsetThread();
	int start = config.startIndex;
	int end = config.endIndex;
	ParseInputOutput arg;
	arg.printMutex = printMutex;
	for(int i = start; i <= end; i++){
		const ofxJSONElement & jsonRef = *json;
		arg.objectID = i;
		arg.threadID = config.threadID;
		arg.jsonObj = (ofxJSONElement *) &(jsonRef[i]);
		arg.object = nullptr;

		ofNotifyEvent(eventParseObject, arg);
		if(arg.object != nullptr){
			parsedObjects.push_back(arg.object);
		}
		numParsedObjects = i - start;
	}
}


//void ofxMtJsonParserThread::parseJsonSubsetThread(){
//
//	ofLogError("ofxMtJsonParserThread") << "Your subclass must implement this!";
//	//in your subclass, use json*, printMutex* and config to parse
//	//the JSON elements from config.startIndex to config.endIndex
//}


int ofxMtJsonParserThread::getNumParsedObjects(){
	return numParsedObjects;

}


int ofxMtJsonParserThread::getNumObjectsToParse(){
	return config.endIndex - config.startIndex + 1;
}


int ofxMtJsonParserThread::getNumObjectsLeftToParse(){
	return numObjectsToParse - numParsedObjects;
}


float ofxMtJsonParserThread::getPercentDone(){
	return float(numParsedObjects) / float(numObjectsToParse);
}




