//
//  MyParseableObject.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#ifndef BaseApp_MyParseableObject_h
#define BaseApp_MyParseableObject_h

class MyParseableObject {

public:

	void setTitle(const string& t){ title = t;}
	void setDescription(const string& d){ description = d;}

	void print(){
		ofLog() << "title: " << title << "  desc: " << description;
	}

private:

	//my parsed object stuff;
	string title;
	string description;

};

#endif
