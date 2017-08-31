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
		std::string getObjectUUID(){return uuid;}
		void setObjectUUID(const std::string & uiid){ this->uuid = uiid;}

	private:
		std::string uuid; //must be unique!
};

