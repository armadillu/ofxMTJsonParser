//
//  ofxMtJsonParserArgs.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#ifndef __BaseApp__ofxMtJsonParserArgs__
#define __BaseApp__ofxMtJsonParserArgs__


//Subclass this if you need extra config params
class ofxMtJsonParserArgs{

public:

	ofxMtJsonParserArgs(){
		threadID = -1;
		startIndex = endIndex = 0;
	}

	int threadID;

	//both included
	int startIndex; //defines which indices to parse (per thread)
	int endIndex;

};

#endif
