#pragma once

#include "ofMain.h"


#include "ofxMtJsonParser.h"

// my custom code
#include "MyParseableObject.h"
#include "MyParsingConfig.h"

class ofApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key){}
	void keyReleased(int key){}

	// APP CALLBACKS ////////////////////////////////////////

	//you will have to do minor parsing here, just let us know how many objects there are in
	//the json we provide
	void onCalcNumEntriesInJson(ofxMtJsonParserThread::ObjectCountData &); //minor parsing

	//you will get N calls to this method, one per each JSON object in your json
	//you will have to alloc and "fill in" data into a new object from the json obj we provide
	void onParseObject(ofxMtJsonParserThread::ParseInputOutput &);

	void jsonDownloaded(ofxSimpleHttpResponse & arg);
	void jsonDownloadFailed(ofxSimpleHttpResponse & arg);
	void jsonInitialCheckOK(bool &);
	void jsonParseFailed(bool & arg);
	void jsonContentReady(vector<ParsedObject*> & arg);

	// APP STUFF ////////////////////////////////////////////

	//ofxMtJsonParser does all the work.
	//Create a Parser object, specifying your custom:
	//	1 - Parsing Class: "MyJsonParserThread"
	//	2 - Parsed Object Class: "MyParseableObject"
	ofxMtJsonParser jsonParser;


	//This will hold your vector of parsed objects, MyParseableObject*
	vector<MyParseableObject*> parsedObjects;

};
