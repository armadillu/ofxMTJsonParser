#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);

	ofAddListener(jsonParser.eventDownloadFailed, this, &ofApp::jsonDownloadFailed);
	ofAddListener(jsonParser.eventDontentReady, this, &ofApp::jsonContentReady);

	ofxMtJsonParserConfig myConfig;
	jsonParser.downloadAndParse("http://129.22.220.12/api/data/", "json", 1, myConfig);
}

void ofApp::jsonDownloadFailed(bool & arg){

	ofLogNotice() << "download failed";
}

void ofApp::jsonContentReady(bool & arg){

	ofLogNotice() << "content ready";
	parsedObjects = jsonParser.getParsedObjects();

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

}


