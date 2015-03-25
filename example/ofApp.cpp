#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);

	ofAddListener(jsonParser.eventDownloadFailed, this, &ofApp::jsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonParseFailed, this, &ofApp::jsonParseFailed);
	ofAddListener(jsonParser.eventDontentReady, this, &ofApp::jsonContentReady);

	//send this custom object to the threads
	//to tweak their behavior
	myArgs.verbose = true;

	jsonParser.downloadAndParse("http://129.22.220.12/api/data/", //json url
								"json", //directory where to save
								8, //num threads
								&myArgs //my config to pass to the threads
								);
}


void ofApp::jsonDownloadFailed(bool & arg){
	ofLogError("ofApp") << "download failed";
}


void ofApp::jsonParseFailed(bool & arg){
	ofLogError("ofApp") << "json Parse Failed!";
}


void ofApp::jsonContentReady(bool & arg){

	ofLogNotice("ofApp") << "content ready!";
	parsedObjects = jsonParser.getParsedObjects();

	//print all objects
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


