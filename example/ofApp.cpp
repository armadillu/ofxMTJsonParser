#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);

	//subscribe to parsing events - event list still wip
	ofAddListener(jsonParser.eventDownloadFailed, this, &ofApp::jsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonParseFailed, this, &ofApp::jsonParseFailed);
	ofAddListener(jsonParser.eventDontentReady, this, &ofApp::jsonContentReady);

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


void ofApp::jsonDownloadFailed(bool & arg){
	ofLogError("ofApp") << "download failed!";
}


void ofApp::jsonParseFailed(bool & arg){
	ofLogError("ofApp") << "json Parse Failed!";
}


void ofApp::jsonContentReady(bool & arg){

	ofLogNotice("ofApp") << "content ready!";

	//get your parsed object *
	parsedObjects = jsonParser.getParsedObjects();

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


