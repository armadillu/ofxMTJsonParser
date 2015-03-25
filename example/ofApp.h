#pragma once

#include "ofMain.h"


#include "ofxMtJsonParser.h"
#include "ofxMtJsonParser.cpp" //NOTE THE INCLUSION OF .CPP!! Bc of the templated code
//http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file

#include "MyJsonParserThread.h"
#include "MyParseableObject.h"
#include "MyParsingArgs.h"

class ofApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key){}
	void keyReleased(int key){}


	// APP CALLBACKS ////////////////////////////////////////

	void jsonDownloadFailed(bool & arg);
	void jsonContentReady(bool & arg);

	// APP SETUP ////////////////////////////////////////////


	ofxMtJsonParser<MyJsonParserThread, MyParseableObject> jsonParser;
	vector<MyParseableObject*> parsedObjects;

	MyParsingArgs myArgs;
};
