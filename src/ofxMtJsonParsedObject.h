//
//  ofxMtParsedObject.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 24/03/15.
//
//

#pragma once

//every parsed object will have to inherit from this class
class ParsedObject{

	public:
		string getObjectUUID(){return uuid;}
		void setObjectUUID(const string & uiid){ this->uuid = uiid;}

	private:
		string uuid; //must be unique!
};

