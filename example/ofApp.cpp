#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);

	//subscribe to parsing events - event list still wip
	ofAddListener(jsonParser.eventJsonDownloaded, this, &ofApp::jsonDownloaded);
	ofAddListener(jsonParser.eventJsonDownloaded, this, &ofApp::jsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonInitialCheckOK, this, &ofApp::jsonInitialCheckOK);

	ofAddListener(jsonParser.eventJsonParseFailed, this, &ofApp::jsonParseFailed);
	ofAddListener(jsonParser.eventAllObjectsParsed, this, &ofApp::jsonContentReady);




	//Send custom parsing params (MyParsingConfig) to your MyJsonParserThread Threads
	//to be able to tweak their parsing behavior if you need to
	//(ie ignore objects with no images, etc)
	myParseConfig.verbose = true;
	myParseConfig.ignoreObjectsWithNoTitle = true;

	string jsonURL_CH = "http://ch-localprojects.s3.amazonaws.com/json_data/api.objects.latest.json";
	string jsonURL_CWRU = "http://129.22.220.12/api/data/";

	//config the http downloader if you need to (proxy, etc)
	jsonParser.getHttp().setSpeedLimit(20000); // kb/sec

	//start the download and parse process
	jsonParser.downloadAndParse(jsonURL_CH, 	//json url
								"json", 		//directory where to save
								8, 				//num threads
								&myParseConfig 		//my config to pass to the threads
								);
}


void ofApp::jsonDownloaded(ofxSimpleHttpResponse & arg){
	ofLogError("ofApp") << "download json ok!";
}

void ofApp::jsonDownloadFailed(ofxSimpleHttpResponse & arg){
	ofLogError("ofApp") << "download failed!";
}


void ofApp::jsonInitialCheckOK(bool & arg){
	ofLogError("ofApp") << "json file seems ok!";
}

void ofApp::jsonParseFailed(bool & arg){
	ofLogError("ofApp") << "json Parse Failed!";
}


void ofApp::jsonContentReady(vector<MyParseableObject*> & parsedObjects_){

	ofLogNotice("ofApp") << "content ready!";

	//get your parsed object *
	parsedObjects = parsedObjects_;

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


