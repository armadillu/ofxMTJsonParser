//
//  MyParseableObject.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#ifndef BaseApp_MyParseableObject_h
#define BaseApp_MyParseableObject_h

#include "ofxParseableObject.h"

class MyParseableObject : public ofxParseableObject{

	friend class MyJsonParserThread;
	
public:

	void print(){
		ofLog() << "title: " << title << "  desc: " << description << "  loc: " << location;
	}

private:
	//my parsed object stuff;
	string title;
	string description;
	string location;

};

#endif
