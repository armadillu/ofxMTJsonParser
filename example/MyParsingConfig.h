//
//  MyParsingConfig.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#ifndef BaseApp_MyParsingConfig_h
#define BaseApp_MyParsingConfig_h

#include "ofxMtJsonParserConfig.h"

class MyParsingConfig: public ofxMtJsonParserConfig{

public:
	
	bool verbose;
	bool ignoreObjectsWithNoTitle;

};

#endif
