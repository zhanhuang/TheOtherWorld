//
//  Player.cpp
//  TheOtherWorld
//
//  Created by Zhan Huang on 12/13/13.
//
//

#include "Player.h"

using namespace cocos2d;

Player* Player::createWithSpriteFrameName(const std::string& spriteFrameName){
    Player* thePlayer = new Player();
    if (thePlayer && thePlayer->initWithSpriteFrameName(spriteFrameName)) {
        thePlayer->autorelease();
        
        // always start facing down
        thePlayer->facingDirection = 2;
        thePlayer->isMoving = false;
        thePlayer->coord = Point(-1,-1);
        thePlayer->makeAnimations(spriteFrameName);
        return thePlayer;
    }
    CC_SAFE_DELETE(thePlayer);
    return NULL;
}

Player::~Player()
{
	if (_upframes)
	{
		_upframes->release();
		_upframes = NULL;
	}
    
	if (_downframes)
	{
		_downframes->release();
		_downframes = NULL;
	}
    
    if (_leftframes)
	{
		_leftframes->release();
		_leftframes = NULL;
	}
    
	if (_rightframes)
	{
		_rightframes->release();
		_rightframes = NULL;
	}
    
	// cpp don't need to call super dealloc
	// virtual destructor will do this
}

void Player::makeAnimations(const std::string& startFrameName){
    if (startFrameName.find("girl") != std::string::npos) {
        _character = "girl";
    } else{
        _character = "man";
    }
    
    _upframes = new Array();
    _upframes->init();
    _downframes = new Array();
    _downframes->init();
    _leftframes = new Array();
    _leftframes->init();
    _rightframes = new Array();
    _rightframes->init();
    
    char frameName[20];
    SpriteFrameCache* sharedFrameCache = SpriteFrameCache::getInstance();
    for (int i = 0; i < 4; i++) {
        sprintf(frameName,"%s_back%d.png",_character.c_str(),i);
        _upframes->addObject(sharedFrameCache->getSpriteFrameByName(frameName));
        sprintf(frameName,"%s_front%d.png",_character.c_str(),i);
        _downframes->addObject(sharedFrameCache->getSpriteFrameByName(frameName));
        sprintf(frameName,"%s_left%d.png",_character.c_str(),i);
        _leftframes->addObject(sharedFrameCache->getSpriteFrameByName(frameName));
        sprintf(frameName,"%s_right%d.png",_character.c_str(),i);
        _rightframes->addObject(sharedFrameCache->getSpriteFrameByName(frameName));
    }
}

void Player::animateMove(int direction){
    Animation* anim;
    switch (direction) {
        case 1:
            anim = Animation::createWithSpriteFrames(_upframes);
            break;
        case 2:
            anim = Animation::createWithSpriteFrames(_downframes);
            break;
        case 3:
            anim = Animation::createWithSpriteFrames(_leftframes);
            break;
        case 4:
            anim = Animation::createWithSpriteFrames(_rightframes);
            break;
        default:
            break;
    }
    anim->setDelayPerUnit(MOVE_TIME/4);
    anim->setRestoreOriginalFrame(true);
    this->runAction(Animate::create(anim));
}

void Player::faceDirection(int direction){
    this->facingDirection = direction;
    
    char frameName[20];
    SpriteFrameCache* sharedFrameCache = SpriteFrameCache::getInstance();
    switch (direction) {
        case 1:
            sprintf(frameName,"%s_back0.png",_character.c_str());
            break;
        case 2:
            sprintf(frameName,"%s_front0.png",_character.c_str());
            break;
        case 3:
            sprintf(frameName,"%s_left0.png",_character.c_str());
            break;
        case 4:
            sprintf(frameName,"%s_right0.png",_character.c_str());
            break;
        default:
            break;
    }
    this->setDisplayFrame(sharedFrameCache->getSpriteFrameByName(frameName));
}
