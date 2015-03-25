//
//  ofxMtJsonParserConfig.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#ifndef __BaseApp__ofxMtJsonParserConfig__
#define __BaseApp__ofxMtJsonParserConfig__


//Subclass this if you need extra config params
class ofxMtJsonParserConfig{
public:
	ofxMtJsonParserConfig(){
		threadID = -1;
		startIndex = endIndex = 0;
	}
	
	int threadID;
	int startIndex;
	int endIndex;
};

#endif
