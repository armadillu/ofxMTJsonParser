#pragma once

#include "ofMain.h"
#include "ofxMtJsonParser.h"

/////////////////////////////////////////////////////////////////
//define a simple object Class that can holds some data
class MyParseableObject : public ParsedObject {

	public:
		void setTitle(const string& t){ title = t;}
		void setDescription(const string& d){ description = d;}
		void print(){ ofLog() << "title: " << title << "  desc: " << description;}

	private:
		string title;
		string description;
};


/////////////////////////////////////////////////////////////////

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key){}
		void keyReleased(int key){}

		// STATE CALLBACKS /////////////////////////////////////////////////////////////////////////////

		void jsonDownloaded(ofxSimpleHttpResponse & arg);
		void jsonDownloadFailed(ofxSimpleHttpResponse & arg);
		void jsonInitialCheckOK();
		void jsonParseFailed();
		void jsonContentReady(vector<ParsedObject*> & arg);

		// APP STUFF ////////////////////////////////////////////

		ofxMtJsonParser jsonParser;

};
