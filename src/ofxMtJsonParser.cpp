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
				msg += "  Thread (" + ofToString(i) + ") : " + ofToString(100 * progress[i], 1) +
				"% parsed. (" + ofToString((int)threads[i]->getNumParsedObjects()) + " object out of " +
				ofToString((int)threads[i]->getNumObjectsToParse()) + " parsed)\n";
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
	json = jsonObjectArray = nullptr;
	numEntriesInJson = numThreads = 0;
	ofAddListener(http.httpResponse, this, &ofxMtJsonParser::onJsonDownload);
	http.setNeedsChecksumMatchToSkipDownload(true);
}


void ofxMtJsonParser::onJsonDownload(ofxSimpleHttpResponse & arg){

	jsonAbsolutePath = arg.absolutePath;
	if(arg.ok){
		ofLogNotice("ofxMtJsonParser") << "JSON downloaded to " << jsonAbsolutePath;
		ofNotifyEvent(eventJsonDownloaded, arg, this);
		setState(CHECKING_JSON);
	}else{
		ofLogError("ofxMtJsonParser") << "download failed! " << arg.reasonForStatus;
		ofNotifyEvent(eventJsonDownloadFailed, arg, this);
		setState(DOWNLOAD_FAILED);
	}
}



vector<ParsedObject*> ofxMtJsonParser::getParsedObjects(){
	if (state == FINISHED){
		return parsedObjects;
	}
	vector<ParsedObject*> empty;
	return empty;
}


void ofxMtJsonParser::downloadAndParse(string jsonURL_, string jsonDownloadDir_, int numThreads_ ,
									   std::function<void (ofxMtJsonParserThread::JsonStructureData &)> describeJsonFunc,
									   std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseSingleObjectFunc){

	if(describeJsonFunc == nullptr || parseSingleObjectFunc == nullptr){
		ofLogError("ofxMtJsonParser") << "Can't start! Please provide your parsing lambdas!";
		return;
	}
	if(parsing){
		ofLogError("ofxMtJsonParser") << "Can't start! Already handling another request!";
	}
	shouldStartParsingInSubThreads = false;
	parsing = true;
	describeJsonUserLambda = describeJsonFunc;
	parseSingleObjectUserLambda = parseSingleObjectFunc;
	numEntriesInJson = 0;
	jsonObjectArray = json = nullptr;
	numThreads = ofClamp(numThreads_, 1, INT_MAX);
	jsonDownloadDir = jsonDownloadDir_;
	jsonURL = jsonURL_;
	ofLogNotice("ofxMtJsonParser") << "start download and parse of JSON '" << jsonURL << "' across " << numThreads << " threads.";
	setState(DOWNLOADING_JSON);
}


void ofxMtJsonParser::checkLocalJsonAndSplitWorkload(){ //this runs on a thread

	bool ok = true;
	json = new ofxJSONElement();
	bool parsingSuccessful;
	ofLogNotice("ofxMtJsonParser") << "Checking JSON...";
	parsingSuccessful = json->open(jsonAbsolutePath);

	if(parsingSuccessful){

		ofLogNotice("ofxMtJsonParser") << "JSON Check Successful!";
		ofNotifyEvent(eventJsonInitialCheckOK, this);

		//run the user lambda!
		ofxMtJsonParserThread::JsonStructureData args;
		args.fullJson = json;
		try{
			describeJsonUserLambda(args);
		}catch(Exception E){
			ofLogError("ofxMtJsonParser") << E.what();
		}

		if(args.objectArray == nullptr){ //user couldnt point us to the object array - abort!

			ofLogError("ofxMtJsonParser") << "Did not get an object location in the JSON from the User Lambda (objectArray == NULL)!";
			ofLogError("ofxMtJsonParser") << "You either forgot to assaing \"objectArray\" a valid value, or the JSON had an unexpected format";
			setState(JSON_PARSE_FAILED);

		}else{ //user gave us a json reference to parse

			if(args.objectArray->isArray() || args.objectArray->isObject() ){

				numEntriesInJson = args.objectArray->size();
				float numObjectsPerThread = numEntriesInJson / float(numThreads);
				jsonObjectArray = args.objectArray; //store a ptr to that json obj array structure
				threadConfigs.resize(numThreads);

				for(int i = 0; i < numThreads; i++){

					ofxMtJsonParserThread * pjt = new ofxMtJsonParserThread();
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
				//setState(PARSING_JSON_IN_SUBTHREADS); //lets not spawn threads from the soon dying thread - seems to cause issues sometimes
				shouldStartParsingInSubThreads = true;
			}else{
				ofLogError("ofxMtJsonParser") << "eventDescribeJsonStructure listener provided wrong data (objectArray IS NOT an OBJECT ARRAY!)!";
				setState(JSON_PARSE_FAILED);
			}
		}
	}else{
		setState(JSON_PARSE_FAILED);
	}
}


void ofxMtJsonParser::startParsingInSubThreads(){

	ofLogNotice("ofxMtJsonParser") << "Starting " << threads.size() << " JSON parsing threads";
	for(int i = 0; i < threads.size(); i++){
		ofxMtJsonParserThread * pjt = threads[i];
		pjt->startParsing(jsonObjectArray, threadConfigs[i], &printMutex, parseSingleObjectUserLambda);
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
			parsing = false;
			}break;

		case CHECKING_JSON:
			try{
				startThread();
			}catch(Exception e){
				ofLogError("ofxMtJsonParser") << e.message();
			}
			break;

		case JSON_PARSE_FAILED:
			stopThread();
			ofNotifyEvent(eventJsonParseFailed, this);
			ofLogError("ofxMtJsonParser")<< "JSON PARSE FAILED! " << jsonAbsolutePath;
			delete json;
			json = nullptr;
			parsing = false;
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
			ofLogNotice("ofxMtJsonParser") << "Finished parsing '" << jsonAbsolutePath << "'";
			ofLogNotice("ofxMtJsonParser") << parsedObjects.size() << " objects delivered, " << jsonObjectArray->size() - parsedObjects.size() << " objects rejected by the user.";
			delete json;
			json = jsonObjectArray = nullptr;
			ofNotifyEvent(eventAllObjectsParsed, parsedObjects, this);
			parsing = false;
			}break;

		default: break;
	}
}


void ofxMtJsonParser::update(){

	switch (state) {
		case DOWNLOADING_JSON: http.update(); break;
		case PARSING_JSON_IN_SUBTHREADS:
			updateParsing();
			break;

		case CHECKING_JSON:
			if(shouldStartParsingInSubThreads){
				setState(PARSING_JSON_IN_SUBTHREADS);
				shouldStartParsingInSubThreads = false;
			}break;

		case MERGE_THREAD_RESULTS:
			if(!isThreadRunning()){
				for(int i = 0; i < threads.size(); i++){ //delete threads that are long dead
					delete threads[i];
				}
				threads.clear();
				threadConfigs.clear();
				setState(FINISHED);
			}
			break;
		default: break;
	}
}



void ofxMtJsonParser::updateParsing(){

	int numRunning = 0;
	for(int i = 0; i < threads.size(); i++){
		if(threads[i]->isThreadRunning()){
			numRunning++;
		}
	}
	if(numRunning == 0){ //json parse finished, all theads done!
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
	#else
	setThreadName("ofxMtJsonParser");
	#endif

	switch (state) {
		case CHECKING_JSON:
			checkLocalJsonAndSplitWorkload();
			ofSleepMillis(16);
			break;

		case MERGE_THREAD_RESULTS:
			mergeThreadResults();
			ofSleepMillis(16);
			break;

		default: break;
	}
}

