#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);

	//subscribe to parsing events - event list still wip
	ofAddListener(jsonParser.eventJsonDownloaded, this, &ofApp::jsonDownloaded);
	ofAddListener(jsonParser.eventJsonDownloadFailed, this, &ofApp::jsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonInitialCheckOK, this, &ofApp::jsonInitialCheckOK);
	ofAddListener(jsonParser.eventJsonParseFailed, this, &ofApp::jsonParseFailed);
	ofAddListener(jsonParser.eventAllObjectsParsed, this, &ofApp::jsonContentReady);

	//subscribe to do the actual parsing bits; there's 2 stages:
	// 1 - eventCalcNumEntriesInJson >> you need to report back how many elements are there in the json
	ofAddListener(jsonParser.eventDescribeJsonStructure, this, &ofApp::onDescribeJsonStructure);
	ofAddListener(jsonParser.eventParseSingleObject, this, &ofApp::onParseSingleObject);

	string jsonURL_CH = "http://ch-localprojects.s3.amazonaws.com/json_data/api.objects.latest.json";

	//config the http downloader if you need to (proxy, etc)
	jsonParser.getHttp().setSpeedLimit(50000); // kb/sec

	//start the download and parse process
	jsonParser.downloadAndParse(jsonURL_CH, 	//json url
								"json", 		//directory where to save the json file
								8 				//num threads to parse on
								);
}

void ofApp::onDescribeJsonStructure(ofxMtJsonParserThread::JsonStructureData & inOutData){

	ofLogNotice("ofApp") << "onDescribeJsonStructure!";
	ofxJSONElement & jsonRef = *(inOutData.fullJson); //pointers mess up the json syntax somehow

	//here I am getting the full plain json file; bc I know its schema, I can dig into what
	//I want to find the # of objects in the json, and to point the parser into that big array
	//of objects
	if(jsonRef.isArray()){
		inOutData.objectArray = &(jsonRef);//in this case, the object array is at root level in the JSON
										//in other cases, it might be deeper into the structure, you would
										//need to navigate ie data.objectArray = jsonRef["data"]["objects"]

	}else{

		ofLogError("MyJsonParserThread") << "JSON has unexpected format!";
		//if the json is not what we exepcted it to be, let the parser know by filling it the data like this:
		inOutData.objectArray = NULL;
	}
}

void ofApp::onParseSingleObject(ofxMtJsonParserThread::SingleObjectParseData & inOutData){

	ofLogNotice("ofApp") << "onParseSingleObject!";
	const ofxJSONElement & jsonRef = *(inOutData.jsonObj); //pointers mess up the json syntax somehow

	inOutData.printMutex->lock();//as this method gets called from concurrent threads, you might want to
								//lock and unlock the provided mutex b4 printing - or even better,
								//dont print at all to keep it faster
	ofLogNotice("ofApp") << "Thread " << inOutData.threadID <<" parsing OBJ #" << inOutData.objectID;
	inOutData.printMutex->unlock();

	//start parsing
	string title, description;
	try{
		title = ofxMtJsonParserUtils::initFromJsonString(jsonRef, "title_raw", true, inOutData.printMutex);
		description = ofxMtJsonParserUtils::initFromJsonString(jsonRef, "dimensions", true, inOutData.printMutex);
	}catch(Exception exc){ //JSON parsing can throw exceptions - catch them!
		inOutData.printMutex->lock();
		ofLogError("ofApp") << exc.what() << " " << exc.message() << " " << exc.displayText() << " WHILE PARSING OBJ " << inOutData.objectID;
		inOutData.printMutex->unlock();
	}

	//filter out objects that are incomplete (ie no title) according yo your rules (MyParsingConfig)
	if(title.size() && description.size()){
		//make new object (must be subclass of ParsedObject) and "fill it in"
		MyParseableObject * o = new MyParseableObject();
		o->setTitle( title );
		o->setDescription( description );
		inOutData.object = o; //this is how we "return" the object to the parser;
	}else{
		//by not giving a valid pointer to inOutData.object, we are effectively rejecting the object in that
		//json value... This is how you discard objects that you don't want in the final list.
	}
}


void ofApp::jsonDownloaded(ofxSimpleHttpResponse & arg){
	ofLogNotice("ofApp") << "download json ok!";
}

void ofApp::jsonDownloadFailed(ofxSimpleHttpResponse & arg){
	ofLogError("ofApp") << "download failed!";
}

void ofApp::jsonInitialCheckOK(){
	ofLogNotice("ofApp") << "json file seems ok!";
}

void ofApp::jsonParseFailed(){
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
	}
}


void ofApp::update(){
	float dt = 1./60.;
	jsonParser.update();
}


void ofApp::draw(){
	ofDrawBitmapString(jsonParser.getDrawableState(), 20, 20);
}


