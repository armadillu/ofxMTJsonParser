#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);

	//subscribe to parsing events - event list still wip
	ofAddListener(jsonParser.eventJsonDownloaded, this, &ofApp::jsonDownloaded);
	ofAddListener(jsonParser.eventJsonDownloadFailed, this, &ofApp::jsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonInitialCheckOK, this, &ofApp::jsonInitialCheckOK);
	ofAddListener(jsonParser.eventJsonParseFailed, this, &ofApp::jsonParseFailed);
	ofAddListener(jsonParser.eventAllObjectsParsed, this, &ofApp::jsonContentReady);

	string jsonURL = "file://api.objects.latest.json";
	//jsonURL = "http://ch-localprojects.s3.amazonaws.com/json_data/api.objects.latest.json"

	//config the http downloader if you need to (proxy, etc)
	//jsonParser.getHttp().setSpeedLimit(50000); // kb/sec

	// LAMBDA DEFINITIONS /////////////////////////////////////////////////////////////////////////////////////

	// 1 - create your lambda for "Stage 1" >> describle where the object array is inside the JSON ////////////
	//you will have to do minor parsing here, just let us know where the object array is inside the JSON
	auto describeJsonStructureLambda = [](ofxMtJsonParserThread::JsonStructureData & inOutData){

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
	};


	// 2 - Create your lambda for "Stage 2" >> Parse a single object inside the JSON //////////////////////////
	//you will get N calls to this lambda, one per each JSON object in the JSON object array you pointed us to.
	//the lambda will be called concurrently from different threads, so be aware of that.
	//you will have to alloc and "fill in" data into a new object from the json obj we provide
	auto parseSingleObjectLambda = [](ofxMtJsonParserThread::SingleObjectParseData & inOutData){

		const ofxJSONElement & jsonRef = *(inOutData.jsonObj); //pointers mess up the json syntax somehow

		//syncronized logging possible
		//inOutData.printMutex->lock();//as this method gets called from concurrent threads, you might want to
		//lock and unlock the provided mutex b4 printing - or even better, dont print at all to keep it faster
		//ofLogNotice("ofApp") << "Thread " << inOutData.threadID <<" parsing OBJ #" << inOutData.objectID;
		//inOutData.printMutex->unlock();

		string title, description;

		try{ //do some parsing - catching exceptions
			title = ofxMtJsonParserUtils::initFromJsonString(jsonRef, "title_raw", false, inOutData.printMutex);
			description = ofxMtJsonParserUtils::initFromJsonString(jsonRef, "dimensions", false, inOutData.printMutex);
		}catch(std::exception exc){
			inOutData.printMutex->lock();
			ofLogError("ofApp") << exc.what() << " " << " WHILE PARSING OBJ " << inOutData.objectID;
			inOutData.printMutex->unlock();
		}

		//in this example, wefilter out objects that are incomplete (ie no title)
		if(title.size() && description.size()){
			//make new object (must be subclass of "ParsedObject") and "fill it in"
			MyParseableObject * o = new MyParseableObject();			
			o->setTitle( title );
			o->setDescription( description );
			inOutData.object = o; //this is how we "return" the object to the parser;
		}else{
			//by not giving a valid pointer to inOutData.object, we are effectively rejecting the object in that
			//Json value... This is how you discard objects that you don't want in the final object list.
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////


	ofxJSON userData;
	//start the download and parse process, providing your custom lambdas
	jsonParser.downloadAndParse(jsonURL, 	//json url
								"json", 		//directory where to save the json file
								8, 				//num threads to parse on
								describeJsonStructureLambda,//your lambda to describe the JSON structure
								parseSingleObjectLambda,	//your lambda to parse each of the objects in the JSON
								userData					//pass any data you require to each individual parser
								);
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

	//This will hold your vector of parsed objects, MyParseableObject*
	vector<MyParseableObject*> parsedObjects;

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


