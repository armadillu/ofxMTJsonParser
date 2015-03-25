//
//  ofxMtJsonParser.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#ifndef __BaseApp__ofxMtJsonParser__
#define __BaseApp__ofxMtJsonParser__

#include "ofMain.h"
#include "ofxSimpleHttp.h"
#include "ofxJSON.h"
#include "ofxMtJsonParseConfig.h"
#include "ofxMtJsonParserThread.h"

class ofxParseableObject;

template <class T>
class ofxMtJsonParser: public ofThread{

public:

	ofxMtJsonParser();

	void downloadAndParse(string jsonURL_,
						  string jsonDownloadDir_,
						  int numThreads,
						  ofxMtJsonParserConfig config);

	void checkLocalJsonAndSplitWorkloads();

	void update();
	void isBusy();

	ofEvent<bool> eventDownloadFailed;
	ofEvent<bool> eventDontentReady;

	vector<ofxParseableObject*> getParsedObjects();


protected:

	enum State{
		IDLE = 0,
		DOWNLOADING_JSON,
		DOWNLOAD_FAILED,
		CHECKING_JSON,
		JSON_PARSE_FAILED,
		PARSING_JSON_IN_SUBTHREADS,
		MERGE_THREAD_RESULTS,
		FINISHED
	};

	ofxSimpleHttp http;
	State state;

	string jsonURL;
	string jsonDownloadDir;
	string jsonAbsolutePath;
	ofxJSONElement * json;

	ofMutex mutex;

	int numThreads;
	vector<ofxMtJsonParserThread*> threads;
	vector<ofxMtJsonParserConfig> threadConfigs;

	void setState(State s);

	void updateParsing();
	void startParsingInSubThreads();
	void mergeThreadResults();

	void onJsonDownload(ofxSimpleHttpResponse & arg);

	void threadedFunction();

	vector<ofxParseableObject*> parsedObjects;
};

#endif /* defined(__BaseApp__ofxMtJsonParser__) */
