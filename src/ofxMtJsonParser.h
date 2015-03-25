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
#include "ofxMtJsonParserThread.cpp"//NOTE THE INCLUSION OF .CPP!! Bc of the templated code
//http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file


//Parser, Object
template <class P,class O>
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

	vector<O*> getParsedObjects();


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
	vector<ofxMtJsonParserThread<O>*> threads;
	vector<ofxMtJsonParserConfig> threadConfigs;

	void setState(State s);

	void updateParsing();
	void startParsingInSubThreads();
	void mergeThreadResults();

	void onJsonDownload(ofxSimpleHttpResponse & arg);

	void threadedFunction();

	vector<O*> parsedObjects;
};

#endif /* defined(__BaseApp__ofxMtJsonParser__) */
