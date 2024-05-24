//
//  MyJsonParserThread.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 25/03/15.
//
//

#ifndef BaseApp_MyJsonParserThread_h
#define BaseApp_MyJsonParserThread_h


#include "ofxMtJsonParser.h"
#include "MyParseableObject.h"
#include "MyParsingConfig.h"

class MyJsonParserThread : public ofxMtJsonParserThread<MyParseableObject>{

public:

	void parseJsonSubsetThread(){

		const ofJson & jsonRef = *json; //pointers mess up the json syntax somehow

		//both included
		int start = config.startIndex;
		int end = config.endIndex;

		//force cast to your config subclass to get to your custom parsing params
		MyParsingConfig * myArgs = (MyParsingConfig *)args;

		//only parse our subset of the JSON objects: [start .. end]
		for(int i = start; i <= end; i++){

			//verbose according to your config
			if (myArgs->verbose){
				printMutex->lock();
				ofLogNotice("MyJsonParserThread") << "Thread " << config.threadID <<" parsing OBJ #" << i;
				printMutex->unlock();
			}

			// Parse stuff from json object at index i //
			string title;
			string description;
			try{

				title = ofxMtJsonParserUtils::initFromJsonString(jsonRef[i], "title_raw", true, printMutex);
				description = ofxMtJsonParserUtils::initFromJsonString(jsonRef[i], "dimensions", true, printMutex);

			}catch(exception exc){ //JSON parsing can throw exceptions
				printMutex->lock();
				ofLogError("MyJsonParserThread") << exc.what() << " WHILE PARSING OBJ " << i;
				printMutex->unlock();
			}

			bool addThisObject = true;

			//filter out objects that are incomplete (ie no title) according yo your rules (MyParsingConfig)
			if(myArgs->ignoreObjectsWithNoTitle && title.size() == 0){
				addThisObject = false;
			}

			if(addThisObject){
				//make new object of your supplied class
				MyParseableObject * o = new MyParseableObject();

				o->setTitle( title );
				o->setDescription( description );

				////////////////////////////////////////////////////////////////////////////////////
				// THIS IS KEY! store the new parsed object in the superclass array "parsedObjects"
				parsedObjects.push_back(o);
				///////////////////////////////////////////////////////////////////////////////////
			}

			////////////////////////////////////////////////////////////////////////////////////
			// AND THIS TOO, WE NEED TO KEEP TRACK OF HOW MANY WE PARSED SO FAR
			numParsedObjects = i - start;
			////////////////////////////////////////////////////////////////////////////////////

		}
	}


	int getNumEntriesInJson(ofJson* json_){

		const ofJson & jsonRef = *json_; //pointers mess up the json syntax somehow

		//CH JSON
		if(jsonRef.is_array()){
			return jsonRef.size();
		}


		ofLogError("MyJsonParserThread") << "JSON has unexpected format";
		return 0;
	}

private:


};

#endif
