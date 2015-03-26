#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);

	//subscribe to events
	ofAddListener(jsonParser.eventDownloadFailed, this, &ofApp::jsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonParseFailed, this, &ofApp::jsonParseFailed);
	ofAddListener(jsonParser.eventDontentReady, this, &ofApp::jsonContentReady);

	//Send this custom object (MyParsingArgs) to your MyJsonParserThread Threads
	//to be able to tweak their behavior if you need to
	myArgs.verbose = true;

	string jsonURL_CH = "http://ch-localprojects.s3.amazonaws.com/json_data/api.objects.latest.json";
	string jsonURL_CWRU = "http://129.22.220.12/api/data/";

	//config the download if you need to (proxy, etc)
	jsonParser.getHttp().setSpeedLimit(20000); // kb/sec

	//start the process
	jsonParser.downloadAndParse(jsonURL_CH, //json url
								"json", //directory where to save
								8, //num threads
								&myArgs //my config to pass to the threads
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
	parsedObjects = jsonParser.getParsedObjects();

	//print all parsed objects
	for(int i = 0; i < parsedObjects.size(); i++){
		MyParseableObject * obj = (MyParseableObject *)parsedObjects[i];
		obj->print();
	}
}


void ofApp::update(){
	float dt = 1./60.;
	jsonParser.update();
}


void ofApp::draw(){
	ofDrawBitmapString(jsonParser.getDrawableState(), 20, 20);
}


