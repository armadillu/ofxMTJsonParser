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
#include "ofEvents.h"
#include "ofEventUtils.h"


class ofxMtJsonParser: public ofThread{

public:

	ofxMtJsonParser();

	//You need to provide 2 call backs and a listener, similar to to ofAddListener()
	//downloadAndParse( "url",
	//					"dir",
	//					4,
	//					this,
	//					&myClass::onCalcNumEntriesInJson,
	//					&myClass::onCalcNumEntriesInJson
	//					);

	template <typename ObjectCountData, typename ParseData, class ListenerClass>
	void downloadAndParse(string jsonURL_,
						  string jsonDownloadDir_,
						  int numThreads_,
						  ListenerClass * listener,
						  void (ListenerClass::*onCalcNumEntriesMethod)(ObjectCountData&),
						  void (ListenerClass::*onParseObjectMethod)(ParseData&)
						  ){

		numThreads = ofClamp(numThreads_, 1, INT_MAX);
		jsonDownloadDir = jsonDownloadDir_;
		jsonURL = jsonURL_;
		ofLogNotice("ofxMtJsonParser") << "start download and parse of JSON '" << jsonURL <<
		"' across " << numThreads << " threads.";
		setState(DOWNLOADING_JSON);

	}

	void update();

	// STATUS //
	vector<float> getPerThreadProgress(); //returns a vector of size NumThreads with a float with [0..1]
	float getTotalProgress();
	string getDrawableState();

	int getNumEntriesInJson(){return numEntriesInJson;}
	bool isBusy(){ return state != IDLE && state != DOWNLOAD_FAILED && state != FINISHED; };
	bool isDownloadingJson(){ return state == DOWNLOADING_JSON; }
	bool isCheckingJson(){ return state == CHECKING_JSON; }
	bool isParsingJson(){ return state == PARSING_JSON_IN_SUBTHREADS; }
	ofxSimpleHttp & getHttp(){return http;} //in case you want to config it

	// EVENTS //
	ofEvent<ofxSimpleHttpResponse> eventJsonDownloaded;
	ofEvent<ofxSimpleHttpResponse> eventJsonDownloadFailed;

	ofEvent<ofxMtJsonParserThread::ObjectCountData> eventCalcNumEntriesInJson;
	ofEvent<ofxMtJsonParserThread::ParseInputOutput> eventParseObject;

	ofEvent<bool> eventJsonInitialCheckOK;
	ofEvent<bool> eventJsonParseFailed;
	ofEvent<vector<ParsedObject*> > eventAllObjectsParsed;


	vector<ParsedObject*> getParsedObjects(); //use only after you got the "eventDontentReady" callback

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


	string jsonAbsolutePath;
	ofxJSONElement * json;

	int numEntriesInJson;

	ofMutex printMutex;
	State state;
	int numThreads;
	ofxSimpleHttp http;

	string jsonURL;
	string jsonDownloadDir;

	vector<ofxMtJsonParserThread*> threads;
	vector<ofxMtJsonParserThread::Config> threadConfigs;

	void setState(State s);

	void updateParsing();
	void startParsingInSubThreads();
	void checkLocalJsonAndSplitWorkload();
	void mergeThreadResults();

	void onJsonDownload(ofxSimpleHttpResponse & arg);

	void threadedFunction();

	vector<ParsedObject*> parsedObjects;

	
};

