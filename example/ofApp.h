#pragma once

#include "ofMain.h"


#include "ofxMtJsonParser.h"
#include "ofxMtJsonParser.cpp" //NOTE THE INCLUSION OF .CPP!! Bc of the templated code
//http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file

#include "MyJsonParserThread.h"
#include "MyParseableObject.h"

class ofApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);


	// APP CALLBACKS ////////////////////////////////////////

	void jsonDownloadFailed(bool & arg);
	void jsonContentReady(bool & arg);

	// APP SETUP ////////////////////////////////////////////


	ofxMtJsonParser<MyJsonParserThread> jsonParser;

	//it's a vector of MyParsedObject's really
	vector<ofxParseableObject*> parsedObjects;

};
