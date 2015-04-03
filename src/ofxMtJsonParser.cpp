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
	json = NULL;
	numEntriesInJson = numThreads = 0;
	ofAddListener(http.httpResponse, this, &ofxMtJsonParser::onJsonDownload);
	http.setNeedsChecksumMatchToSkipDownload(true);
}

template <class P,class O>
bool ofxMtJsonParser<P,O>::isBusy(){
	return state != IDLE && state != DOWNLOAD_FAILED && state != FINISHED;
}


template <class P,class O>
string ofxMtJsonParser<P,O>::getDrawableState(){

	string msg = "State: ";

	switch (state) {
		case IDLE:
			msg += "IDLE";
			break;
		case DOWNLOADING_JSON:
			msg += "DOWNLOADING_JSON";
			msg += "\n\n" + http.drawableString();
			break;
		case DOWNLOAD_FAILED:
			msg += "DOWNLOAD_FAILED";
			break;
		case CHECKING_JSON:{
			msg += "CHECKING_JSON";
			}break;
		case JSON_PARSE_FAILED:
			msg += "JSON_PARSE_FAILED";
			break;
		case PARSING_JSON_IN_SUBTHREADS:{
			msg += "PARSING_JSON_IN_SUBTHREADS";
			msg += "\n\n";
			vector<float> progress = getPerThreadProgress();
			for(int i = 0; i < progress.size(); i++){
				msg += "  Thread " + ofToString(i) + ": " + ofToString(100 * progress[i], 1) +
				"% parsed. (" + ofToString((int)threads[i]->getNumParsedObjects()) + "/" +
				ofToString((int)threads[i]->getNumObjectsToParse()) + ")\n";
			}
			}break;
		case MERGE_THREAD_RESULTS:
			msg += "MERGE_THREAD_RESULTS";
			break;
		case FINISHED:
			msg += "FINISHED";
			break;
	}
	return msg;
}


template <class P,class O>
void ofxMtJsonParser<P,O>::downloadAndParse(string jsonURL_,
										  string jsonDownloadDir_,
										  int numThreads_,
										  ofxMtJsonParserConfig* args_){
	numThreads = ofClamp(numThreads_, 1, INT_MAX);
	args = args_;
	jsonDownloadDir = jsonDownloadDir_;
	jsonURL = jsonURL_;
	setState(DOWNLOADING_JSON);
}


template <class P,class O>
void ofxMtJsonParser<P,O>::onJsonDownload(ofxSimpleHttpResponse & arg){

	if(arg.ok){
		jsonAbsolutePath = arg.absolutePath;
		ofNotifyEvent(eventJsonDownloaded, arg, this);
		setState(CHECKING_JSON);
	}else{
		ofLogError("ofxMtJsonParser") << "download failed! " << arg.reasonForStatus;
		ofNotifyEvent(eventJsonDownloadFailed, arg, this);
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
void ofxMtJsonParser<P,O>::checkLocalJsonAndSplitWorkload(){

	bool ok = true;
	json = new ofxJSONElement();
	bool parsingSuccessful;
	parsingSuccessful = json->open(jsonAbsolutePath);
	if(parsingSuccessful){

		ofNotifyEvent(eventJsonInitialCheckOK, parsingSuccessful, this);

		//use a temp JsonParser subclass instance to count how many objects are there to parse
		ofxMtJsonParserThread<O> * temp = new P();
		numEntriesInJson = temp->getNumEntriesInJson(json);
		delete temp;

		float numObjectsPerThread = numEntriesInJson / float(numThreads);

		threadConfigs.resize(numThreads);

		for(int i = 0; i < numThreads; i++){
			ofxMtJsonParserThread<O> * pjt = new P();
			threads.push_back(pjt);

			int start, end;
			start = floor(i * numObjectsPerThread);

			if (i < numThreads -1){
				end = floor((i+1) * numObjectsPerThread) - 1;
			}else{ //special case for last core, int division might not be even
				end = numEntriesInJson - 1;
			}
			ofxMtJsonParserThreadConfig tConfig;
			tConfig.threadID = i;
			tConfig.startIndex = start;
			tConfig.endIndex = end;
			//printf("%d > %d - %d\n", i, start, end);

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
		pjt->startParsing(json, threadConfigs[i], args, &printMutex);
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
			ofLogError("ofxMtJsonParser")<< "DOWNLOAD_FAILED! " << jsonURL;
			}break;

		case CHECKING_JSON:
			startThread();
			break;

		case JSON_PARSE_FAILED:{
			stopThread();
			bool ok = false;
			ofNotifyEvent(eventJsonParseFailed, ok, this);
			ofLogError("ofxMtJsonParser")<< "JSON_PARSE_FAILED! " << jsonDownloadDir;
			}
			break;

		case PARSING_JSON_IN_SUBTHREADS:
			startParsingInSubThreads();
			break;

		case MERGE_THREAD_RESULTS:
			try{
			startThread();
			}catch(Exception e){
				ofLogError("ofxMtJsonParser") << e.message();
			}
			break;

		case FINISHED:{
			bool ok = true;
			delete json;
			json = NULL;
			ofNotifyEvent(eventAllObjectsParsed, ok, this);
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
				for(int i = 0; i < threads.size(); i++){
					delete threads[i];
				}
				threads.clear();
				threadConfigs.clear();
				args = NULL;
				setState(FINISHED);
			}
			break;
	}
}


template <class P,class O>
void ofxMtJsonParser<P,O>::updateParsing(){

	int numRunning = 0;
	for(int i = 0; i < threads.size(); i++){
		if(threads[i]->isThreadRunning()) numRunning++;
	}
	if(numRunning == 0){ //json parse finished, all theads done!
		setState(MERGE_THREAD_RESULTS);
	}
}


template <class P,class O>
vector<float> ofxMtJsonParser<P,O>::getPerThreadProgress(){
	vector<float> p;

	if(state == PARSING_JSON_IN_SUBTHREADS || state == MERGE_THREAD_RESULTS || state == FINISHED ){
		for(int i = 0; i < threads.size(); i++){
			p.push_back(threads[i]->getPercentDone());
		}
	}
	return p;
}

template <class P,class O>
float ofxMtJsonParser<P,O>::getTotalProgress(){
	vector<float> ps = getPerThreadProgress();
	float threadsP = 0.0f;
	for(int i = 0; i < ps.size(); i++){ threadsP += ps[i]; }
	return threadsP / ps.size();
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
				checkLocalJsonAndSplitWorkload();
				stopThread();
				break;

			case MERGE_THREAD_RESULTS:
				mergeThreadResults();
				stopThread();
				break;
		}
	}
}

