#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);

	//subscribe to parsing events - event list still wip
	ofAddListener(jsonParser.eventJsonDownloaded, this, &ofApp::jsonDownloaded);
	ofAddListener(jsonParser.eventJsonDownloadFailed, this, &ofApp::jsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonInitialCheckOK, this, &ofApp::jsonInitialCheckOK);
	ofAddListener(jsonParser.eventJsonParseFailed, this, &ofApp::jsonParseFailed);
	ofAddListener(jsonParser.eventAllObjectsParsed, this, &ofApp::jsonContentReady);

	ofAddListener(jsonParser.eventCalcNumEntriesInJson, this, &ofApp::onCalcNumEntriesInJson);
	ofAddListener(jsonParser.eventParseObject, this, &ofApp::onParseObject);

	string jsonURL_CH = "http://ch-localprojects.s3.amazonaws.com/json_data/api.objects.latest.json";
	string jsonURL_CWRU = "http://129.22.220.12/api/data/";

	//config the http downloader if you need to (proxy, etc)
	jsonParser.getHttp().setSpeedLimit(50000); // kb/sec

	//start the download and parse process
	jsonParser.downloadAndParse(jsonURL_CH, 	//json url
								"json", 		//directory where to save
								8, 				//num threads
								this,
								&ofApp::onCalcNumEntriesInJson,
								&ofApp::onParseObject
								);
}

void ofApp::onCalcNumEntriesInJson(ofxMtJsonParserThread::ObjectCountData & data){
	ofLogNotice("ofApp") << "onCalcNumEntriesInJson!";
	const ofxJSONElement & jsonRef = *(data.jsonObj); //pointers mess up the json syntax somehow
	//here I am getting the plain json file, bc i know its schema, I can dig into what
	//i want to find the # of objects in the json
	if(jsonRef.isArray()){
		data.numObjects = jsonRef.size();
	}else{
		ofLogError("MyJsonParserThread") << "JSON has unexpected format";
		data.numObjects = 0;
	}
}

void ofApp::onParseObject(ofxMtJsonParserThread::ParseInputOutput & data){

	const ofxJSONElement & jsonRef = *(data.jsonObj); //pointers mess up the json syntax somehow

	data.printMutex->lock();//as this method gets called from concurrent threads, you might want to
							//lock and unlock the provided mutex b4 printing - or even better,
							//dont print at all to keep it faster
	ofLogNotice("ofApp") << "Thread " << data.threadID <<" parsing OBJ #" << data.objectID;
	data.printMutex->unlock();

	//start parsing;
	string title;
	string description;
	try{
		title = ofxMtJsonParserUtils::initFromJsonString(jsonRef, "title_raw", true, data.printMutex);
		description = ofxMtJsonParserUtils::initFromJsonString(jsonRef, "dimensions", true, data.printMutex);
	}catch(Exception exc){ //JSON parsing can throw exceptions - catch them!
		data.printMutex->lock();
		ofLogError("ofApp") << exc.what() << " " << exc.message() << " " << exc.displayText() << " WHILE PARSING OBJ " << data.objectID;
		data.printMutex->unlock();
	}

	//filter out objects that are incomplete (ie no title) according yo your rules (MyParsingConfig)
	if(title.size() && description.size()){
		//make new object (must be subclass of ParsedObject) and "fill it in"
		MyParseableObject * o = new MyParseableObject();
		o->setTitle( title );
		o->setDescription( description );
		data.object = o; //this is how we "return" the object to the parser;
	}else{
		//by not giving a valid pointer to data.object, we are effectively rejecting the object in that
		//json value... This is how you discard objects that you don't want in the final list.
	}

}

void ofApp::jsonDownloaded(ofxSimpleHttpResponse & arg){
	ofLogNotice("ofApp") << "download json ok!";
}

void ofApp::jsonDownloadFailed(ofxSimpleHttpResponse & arg){
	ofLogError("ofApp") << "download failed!";
}


void ofApp::jsonInitialCheckOK(bool & arg){
	ofLogNotice("ofApp") << "json file seems ok!";
}

void ofApp::jsonParseFailed(bool & arg){
	ofLogError("ofApp") << "json Parse Failed!";
}


void ofApp::jsonContentReady(vector<ParsedObject*> & parsedObjects_){

	ofLogNotice("ofApp") << "content ready!";

	//sadly we need to cast the objects in the array to MyParseableObject* by hand
	for(auto p : parsedObjects_){
		parsedObjects.push_back((MyParseableObject*) p);
	}

	//print all parsed objects
	for(int i = 0; i < parsedObjects.size(); i++){
		parsedObjects[i]->print();
		delete parsedObjects[i];
	}
	parsedObjects.clear();
}


void ofApp::update(){
	float dt = 1./60.;
	jsonParser.update();
}


void ofApp::draw(){
	ofDrawBitmapString(jsonParser.getDrawableState(), 20, 20);
}


