//
//  ofxMtJsonParserThread.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//


#include "ofxMtJsonParserThread.h"
#include "ofxMtJsonParsedObject.h"
#include "ofMain.h"

ofxMtJsonParserThread::ofxMtJsonParserThread(){
	numParsedObjects = 0;
	json = NULL;
	printMutex = NULL;
};


void ofxMtJsonParserThread::startParsing(	ofxJSONElement* json_,
											ofxMtJsonParserThread::Config config_,
											ofMutex * printMutex_,
										 	std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseSingleObjectUserLambda
										 ){

	this->parseSingleObjectUserLambda = parseSingleObjectUserLambda;
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
	if(config.endIndex < 0 || config.startIndex < 0){ //no work to do for this thread!
		return;
	}
	numObjectsToParse = config.endIndex - config.startIndex;
	//parseJsonSubsetThread();
	int start = config.startIndex;
	int end = config.endIndex;
	SingleObjectParseData arg;
	arg.printMutex = printMutex;
	const ofxJSONElement & jsonRef = *json;
	bool isArray = jsonRef.isArray();
	int c = 0;
	for( Json::ValueIterator itr = jsonRef.begin(); itr != jsonRef.end(); itr++ ) {

		if(c >= start && c <= end){

			arg.threadID = config.threadID;
			if(isArray){
				arg.objectID = "Object_" + ofToString(c);
				arg.jsonObj = (ofxJSONElement *) &(*itr);
			}else{
				arg.jsonObj = (ofxJSONElement *) &(*itr);
				if(itr.key().isConvertibleTo(Json::stringValue)){
					arg.objectID = itr.key().asString();
				}else{
					arg.objectID = ofToString(c);
				}
			}

			arg.object = nullptr; //user is supposed to create that object

			parseSingleObjectUserLambda(arg);

			if(arg.object != nullptr){
				parsedObjects.push_back(arg.object);
				arg.object->setObjectUUID(arg.objectID); //we assign one unique object ID to that object
			}
			numParsedObjects = 1 + c - start;

		}
		if(c > end){
			break;
		}
		c++;
	}
	ofSleepMillis(16);
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




