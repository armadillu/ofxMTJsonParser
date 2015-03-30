#pragma once

#include "ofMain.h"


#include "ofxMtJsonParser.h"
#include "ofxMtJsonParser.cpp" //NOTE THE INCLUSION OF .CPP!! Bc of the templated code
//http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file

// my custom code
#include "MyJsonParserThread.h"
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


	void jsonDownloaded(ofxSimpleHttpResponse & arg);
	void jsonDownloadFailed(ofxSimpleHttpResponse & arg);
	void jsonInitialCheckOK(bool &);
	void jsonParseFailed(bool & arg);
	void jsonContentReady(bool & arg);

	// APP STUFF ////////////////////////////////////////////

	//ofxMtJsonParser does all the work.
	//Create a Parser object, specifying your custom:
	//	1 - Parsing Class: "MyJsonParserThread"
	//	2 - Parsed Object Class: "MyParseableObject"
	ofxMtJsonParser<MyJsonParserThread, MyParseableObject> jsonParser;

	//If you need to send some parameters to the parsing,
	//create a subclass of ofxMtJsonParserConfig
	//and fill it in with what you need, and feed it to the jsonParser
	MyParsingConfig myParseConfig;

	//This will hold your vector of parsed objects, MyParseableObject*
	vector<MyParseableObject*> parsedObjects;

};
