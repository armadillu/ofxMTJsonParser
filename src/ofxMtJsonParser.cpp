//
//  ofxMtJsonParser.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#include "ofxMtJsonParser.h"

string ofxMtJsonParser::getDrawableState(){

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


ofxMtJsonParser::ofxMtJsonParser(){
	state = IDLE;
	json = NULL;
	numEntriesInJson = numThreads = 0;
	ofAddListener(http.httpResponse, this, &ofxMtJsonParser::onJsonDownload);
	http.setNeedsChecksumMatchToSkipDownload(true);
}


void ofxMtJsonParser::onJsonDownload(ofxSimpleHttpResponse & arg){

	if(arg.ok){
		jsonAbsolutePath = arg.absolutePath;
		ofLogNotice("ofxMtJsonParser") << "JSON downloaded to " << jsonAbsolutePath;
		ofNotifyEvent(eventJsonDownloaded, arg, this);
		setState(CHECKING_JSON);
	}else{
		ofLogError("ofxMtJsonParser") << "download failed! " << arg.reasonForStatus;
		setState(DOWNLOAD_FAILED);
		ofNotifyEvent(eventJsonDownloadFailed, arg, this);
	}
}



vector<ParsedObject*> ofxMtJsonParser::getParsedObjects(){
	if (state == FINISHED){
		return parsedObjects;
	}
	vector<ParsedObject*> empty;
	return empty;
}



void ofxMtJsonParser::checkLocalJsonAndSplitWorkload(){

	bool ok = true;
	json = new ofxJSONElement();
	bool parsingSuccessful;
	ofLogNotice("ofxMtJsonParser") << "Checking JSON...";
	parsingSuccessful = json->open(jsonAbsolutePath);

	if(parsingSuccessful){

		ofLogNotice("ofxMtJsonParser") << "JSON Check Successful!";
		ofNotifyEvent(eventJsonInitialCheckOK, parsingSuccessful, this);

		//use a temp JsonParser subclass instance to count how many objects are there to parse
		ofxMtJsonParserThread * temp = new ofxMtJsonParserThread();
		temp->eventCalcNumEntriesInJson = eventCalcNumEntriesInJson;
		//numEntriesInJson = temp->getNumEntriesInJson(json);
		ofxMtJsonParserThread::ObjectCountData args;
		args.jsonObj = json;
		ofNotifyEvent(temp->eventCalcNumEntriesInJson, args);
		delete temp;
		numEntriesInJson = args.numObjects;
		float numObjectsPerThread = numEntriesInJson / float(numThreads);

		threadConfigs.resize(numThreads);

		for(int i = 0; i < numThreads; i++){
			ofxMtJsonParserThread * pjt = new ofxMtJsonParserThread();
			pjt->eventParseObject = eventParseObject;
			pjt->eventCalcNumEntriesInJson = eventCalcNumEntriesInJson;
			threads.push_back(pjt);

			int start, end;
			start = floor(i * numObjectsPerThread);

			if (i < numThreads - 1){
				end = floor((i+1) * numObjectsPerThread) - 1;
			}else{ //special case for last core, int division might not be even
				end = numEntriesInJson - 1;
			}
			ofxMtJsonParserThread::Config tConfig;
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



void ofxMtJsonParser::startParsingInSubThreads(){

	ofLogNotice("ofxMtJsonParser") << "Starting " << threads.size() << " JSON parsing threads";
	for(int i = 0; i < threads.size(); i++){
		ofxMtJsonParserThread * pjt = threads[i];
		pjt->startParsing(json, threadConfigs[i], &printMutex);
	}
}



void ofxMtJsonParser::mergeThreadResults(){

	ofLogNotice("ofxMtJsonParser") << "Merging Thread Parsing results";
	if (parsedObjects.size()){
		ofLogError("ofxMtJsonParser") << "parsed object list not empty?";
	}
	parsedObjects.clear();

	for(int i = 0; i < threads.size(); i++){
		vector<ParsedObject*> threadObjects = threads[i]->getParsedObjects();
		parsedObjects.insert(parsedObjects.begin(), threadObjects.begin(), threadObjects.end());
	}
}



void ofxMtJsonParser::setState(State s){

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
			ofLogError("ofxMtJsonParser")<< "JSON PARSE FAILED! " << jsonAbsolutePath;
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
			ofLogNotice("ofxMtJsonParser") << "Finished! " << jsonAbsolutePath;
			delete json;
			json = NULL;
			ofNotifyEvent(eventAllObjectsParsed, parsedObjects, this);
			}break;
	}
}



void ofxMtJsonParser::update(){

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
				setState(FINISHED);
			}
			break;
	}
}



void ofxMtJsonParser::updateParsing(){

	int numRunning = 0;
	for(int i = 0; i < threads.size(); i++){
		if(threads[i]->isThreadRunning()) numRunning++;
	}
	if(numRunning == 0 && !isThreadRunning()){ //json parse finished, all theads done!
		setState(MERGE_THREAD_RESULTS);
	}
}



vector<float> ofxMtJsonParser::getPerThreadProgress(){
	vector<float> p;

	if(state == PARSING_JSON_IN_SUBTHREADS || state == MERGE_THREAD_RESULTS || state == FINISHED ){
		for(int i = 0; i < threads.size(); i++){
			p.push_back(threads[i]->getPercentDone());
		}
	}
	return p;
}


float ofxMtJsonParser::getTotalProgress(){
	vector<float> ps = getPerThreadProgress();
	float threadsP = 0.0f;
	for(int i = 0; i < ps.size(); i++){ threadsP += ps[i]; }
	return threadsP / ps.size();
}



void ofxMtJsonParser::threadedFunction(){

	#if( OF_VERSION_MINOR <= 9 )
	try {
		getPocoThread().setName("ofxMtJsonParser");
		getPocoThread().setOSPriority(Poco::Thread::getMinOSPriority());
	} catch (Poco::SystemException exc) {
		ofLogError("ofxMtJsonParser") << exc.what() << " " << exc.message() << " " << exc.displayText();
	}
	#endif

	bool running = true;
	while(running){
		switch (state) {
			case CHECKING_JSON:
				checkLocalJsonAndSplitWorkload();
				running = false;
				break;

			case MERGE_THREAD_RESULTS:
				mergeThreadResults();
				running = false;
				break;
		}
	}
}

