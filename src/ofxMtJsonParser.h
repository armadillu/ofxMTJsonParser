//
//  ofxMtJsonParser.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxSimpleHttp.h"
#include "ofxJSON.h"
#include "ofxMtJsonParserConfig.h"

#include "ofxMtJsonParserThread.h"


class JsonParser : public ofThread{

public:

	virtual void downloadAndParse(	string jsonURL_,
								  string jsonDownloadDir_,
								  int numThreads,
								  ofxMtJsonParserConfig* args) = 0;

	virtual void update() = 0;

	virtual float getTotalProgress() = 0;
	bool isBusy(){ return state != IDLE && state != DOWNLOAD_FAILED && state != FINISHED; };
	bool isDownloadingJson(){ return state == DOWNLOADING_JSON; }
	bool isCheckingJson(){ return state == CHECKING_JSON; }
	bool isParsingJson(){ return state == PARSING_JSON_IN_SUBTHREADS; }
	virtual string getDrawableState() = 0;

	ofxSimpleHttp & getHttp(){return http;} //in case you want to config it

	ofEvent<ofxSimpleHttpResponse> eventJsonDownloaded;
	ofEvent<ofxSimpleHttpResponse> eventJsonDownloadFailed;
	ofEvent<bool> eventJsonInitialCheckOK;
	ofEvent<bool> eventJsonParseFailed;

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

	virtual void setState(State s) = 0;

	State state;
	int numThreads;
	ofxSimpleHttp http;
	ofxMtJsonParserConfig *args;

	string jsonURL;
	string jsonDownloadDir;

};


//Parser, Object
template <class P,class O>
class ofxMtJsonParser: public JsonParser{

public:

	ofxMtJsonParser();

	void downloadAndParse(string jsonURL_,
						  string jsonDownloadDir_,
						  int numThreads,
						  ofxMtJsonParserConfig* args);

	void update();

	// STATUS //
	vector<float> getPerThreadProgress(); //returns a vector of size NumThreads with a float with [0..1]
	float getTotalProgress();
	string getDrawableState();

	int getNumEntriesInJson(){return numEntriesInJson;}

	// EVENTS //
	//see superclass events too!
	ofEvent<vector<O*> > eventAllObjectsParsed;

	vector<O*> getParsedObjects(); //use only after you got the "eventDontentReady" callback

protected:


	string jsonAbsolutePath;
	ofxJSONElement * json;

	int numEntriesInJson;

	ofMutex printMutex;

	vector<ofxMtJsonParserThread<O>*> threads;
	vector<ofxMtJsonParserThreadConfig> threadConfigs;

	void setState(State s);

	void updateParsing();
	void startParsingInSubThreads();
	void checkLocalJsonAndSplitWorkload();
	void mergeThreadResults();

	void onJsonDownload(ofxSimpleHttpResponse & arg);

	void threadedFunction();

	vector<O*> parsedObjects;
};

#include "ofxMtJsonParser.inl"
