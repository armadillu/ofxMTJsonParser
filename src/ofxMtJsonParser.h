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
#include "ofxMtJsonParserThread.h"
#include "ofxMtJsonParsedObject.h"
#include "ofxMtJsonParserUtils.h"
#include "ofEvents.h"
#include "ofEventUtils.h"

class ofxMtJsonParser: public ofThread{

public:

	ofxMtJsonParser();

	void downloadAndParse(string jsonURL_,
						  string jsonDownloadDir_,
						  int numThreads_,
						  std::function<void (ofxMtJsonParserThread::JsonStructureData &)> describeJsonFunc,
						  std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseSingleObjectFunc,
						  const ofJson & userData
						);

	void update();

	// STATUS //
	vector<float> getPerThreadProgress(); //returns a vector of size NumThreads with a float with [0..1]
	float getTotalProgress();
	bool isBusy(){return parsing;}
	string getDrawableState();

	int getNumEntriesInJson(){return numEntriesInJson;}
	bool isDownloadingJson(){ return state == DOWNLOADING_JSON; }
	bool isCheckingJson(){ return state == CHECKING_JSON; }
	bool isParsingJson(){ return state == PARSING_JSON_IN_SUBTHREADS; }
	string getJsonLocalPath(){return jsonAbsolutePath;}
	ofxSimpleHttp & getHttp(){return http;} //in case you want to config it

	vector<ParsedObject*> getParsedObjects(); //use only after you got the "eventDontentReady" callback

	// STATUS EVENTS ////// you probably want to subscribe to these
	ofEvent<ofxSimpleHttpResponse> 	eventJsonDownloaded;
	ofEvent<ofxSimpleHttpResponse> 	eventJsonDownloadFailed;
	ofEvent<void> 					eventJsonInitialCheckOK;
	ofEvent<void> 					eventJsonParseFailed;
	ofEvent<vector<ParsedObject*> > eventAllObjectsParsed;


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
	ofJson json;
	ofJson * jsonObjArray = nullptr; //points somewhere inside the json above, into its array of obj to parse


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

	std::function<void (ofxMtJsonParserThread::JsonStructureData &)> pointToObjects;
	std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseOneObject;

	vector<ParsedObject*> parsedObjects;
	bool parsing = false;
	bool shouldStartParsingInSubThreads; //var shared with thread
	ofJson userData; //holds any data the user wants to be able to get from within the thread
};

