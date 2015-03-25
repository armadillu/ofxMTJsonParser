//
//  ofxMtJsonParser.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#include "ofxMtJsonParser.h"

template <class P,class O>
ofxMtJsonParser<P,O>::ofxMtJsonParser(){
	state = IDLE;
	ofAddListener(http.httpResponse, this, &ofxMtJsonParser::onJsonDownload);
}


template <class P,class O>
void ofxMtJsonParser<P,O>::downloadAndParse(string jsonURL_,
										  string jsonDownloadDir_,
										  int numThreads_,
										  ofxMtJsonParserArgs* args_){
	numThreads = numThreads_;
	args = args_;
	jsonDownloadDir = jsonDownloadDir_;
	jsonURL = jsonURL_;
	setState(DOWNLOADING_JSON);
}


template <class P,class O>
void ofxMtJsonParser<P,O>::onJsonDownload(ofxSimpleHttpResponse & arg){

	if(arg.ok){
		jsonAbsolutePath = arg.absolutePath;
		setState(CHECKING_JSON);
	}else{
		ofLogError("ofxMtJsonParser") << "download failed! " << arg.reasonForStatus;
		setState(DOWNLOAD_FAILED);
	}
}

template <class P,class O>
vector<O*> ofxMtJsonParser<P,O>::getParsedObjects(){
	if (state == FINISHED){
		return parsedObjects;
	}
	vector<O*> empty;
	return empty;
}


template <class P,class O>
void ofxMtJsonParser<P,O>::checkLocalJsonAndSplitWorkloads(){

	bool ok = true;
	json = new ofxJSONElement();
	bool parsingSuccessful;
	parsingSuccessful = json->open(jsonAbsolutePath);
	if(parsingSuccessful){

		//use a temp JsonParser subclass instance to count how many objects are there to parse
		ofxMtJsonParserThread<O> * temp = new P();
		int numEntries = temp->getNumEntriesInJson(json);
		delete temp;

		float numObjectsPerThread = numEntries / float(numThreads);

		threadConfigs.resize(numThreads);

		for(int i = 0; i < numThreads; i++){
			ofxMtJsonParserThread<O> * pjt = new P();
			threads.push_back(pjt);

			int start, end;
			start = floor(i * numObjectsPerThread);

			if (i < numThreads -1){
				end = floor((i+1) * numObjectsPerThread) - 1;
			}else{ //special case for last core, int division might not be even
				end = numEntries - 1 ;
			}
			ofxMtJsonParserConfig tConfig;
			tConfig.threadID = i;
			tConfig.startIndex = start;
			tConfig.endIndex = end;

			threadConfigs[i] = tConfig;
		}
		setState(PARSING_JSON_IN_SUBTHREADS);
	}else{
		setState(JSON_PARSE_FAILED);
	}
}


template <class P,class O>
void ofxMtJsonParser<P,O>::startParsingInSubThreads(){
	for(int i = 0; i < threads.size(); i++){
		ofxMtJsonParserThread<O>* pjt = threads[i];
		pjt->startParsing(json, threadConfigs[i], args, &mutex);
	}
}


template <class P,class O>
void ofxMtJsonParser<P,O>::mergeThreadResults(){

	if (parsedObjects.size()){
		ofLogError("ofxMtJsonParser") << "parsed object list not empty?";
	}
	parsedObjects.clear();

	for(int i = 0; i < threads.size(); i++){
		vector<O*> threadObjects = threads[i]->getParsedObjects();
		parsedObjects.insert(parsedObjects.begin(), threadObjects.begin(), threadObjects.end());
	}
}


template <class P,class O>
void ofxMtJsonParser<P,O>::setState(State s){

	state = s;
	switch(state){
		case DOWNLOADING_JSON:
			http.fetchURLToDisk(jsonURL, true, jsonDownloadDir, "ofxMtJsonParser");
			break;

		case DOWNLOAD_FAILED:{
			bool ok = false;
			ofNotifyEvent(eventDownloadFailed, ok, this);
			ofLogError("ofxMtJsonParser")<< "DOWNLOAD_FAILED! " << jsonURL;
			}break;

		case CHECKING_JSON:
			startThread();
			break;

		case JSON_PARSE_FAILED:
			stopThread();
			ofLogError("ofxMtJsonParser")<< "JSON_PARSE_FAILED! " << jsonDownloadDir;
			break;

		case PARSING_JSON_IN_SUBTHREADS:
			startParsingInSubThreads();
			break;

		case MERGE_THREAD_RESULTS:
			startThread();
			break;

		case FINISHED:{
			bool ok = true;
			ofNotifyEvent(eventDontentReady, ok, this);
			}break;
	}
}


template <class P,class O>
void ofxMtJsonParser<P,O>::update(){

	switch (state) {

		case DOWNLOADING_JSON:
			http.update();
			break;

		case CHECKING_JSON:
			break;

		case PARSING_JSON_IN_SUBTHREADS:
			updateParsing();
			break;

		case MERGE_THREAD_RESULTS:
			if(!isThreadRunning()){
				setState(FINISHED);
			}
			break;
	}
}


template <class P,class O>
void ofxMtJsonParser<P,O>::updateParsing(){

	int numRunning = 0;
	int numJsonObjectsRemaining = 0;
	for(int i = 0; i < threads.size(); i++){
		numJsonObjectsRemaining += threads[i]->getNumObjectsLeftToParse();
		if(threads[i]->isThreadRunning()) numRunning++;
	}
	if(numRunning == 0 && numJsonObjectsRemaining == 0){ //json parse finished, all theads done!
		setState(MERGE_THREAD_RESULTS);
	}
	//TODO this can easily stay looping forever on a json parse exception

	ofLog() << "updateParsing numrunning:" << numRunning << "  numJsonObjectsRemaining: " << numJsonObjectsRemaining;
}


template <class P,class O>
void ofxMtJsonParser<P,O>::threadedFunction(){

	try {
		getPocoThread().setName("ofxMtJsonParser");
		getPocoThread().setOSPriority(Poco::Thread::getMinOSPriority());
	} catch (Poco::SystemException exc) {
		ofLogError("ofxMtJsonParser") << exc.what() << " " << exc.message() << " " << exc.displayText();
	}

	while(isThreadRunning()){
		switch (state) {
			case CHECKING_JSON:
				checkLocalJsonAndSplitWorkloads();
				stopThread();
				break;

			case MERGE_THREAD_RESULTS:
				mergeThreadResults();
				stopThread();
				break;
		}
	}
}

