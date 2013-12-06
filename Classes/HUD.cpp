//
//  HUD.cpp
//  TheOtherWorld
//
//  Created by Zhan Huang on 12/1/13.
//
//

#include "HUD.h"

USING_NS_CC;

HUD::~HUD(){
    if (_upArrow) {
        _upArrow->release();
        _upArrow = NULL;
    }
    if (_downArrow) {
        _downArrow->release();
        _downArrow = NULL;
    }
    if (_leftArrow) {
        _leftArrow->release();
        _leftArrow = NULL;
    }
    if (_rightArrow) {
        _rightArrow->release();
        _rightArrow = NULL;
    }
}

bool HUD::init(){
    if ( !Layer::init() )
    {
        return false;
    }
    
    // add dpad
    _upArrow = MenuItemImage::create("arrow_up.png","arrow_up.png");
    _downArrow = MenuItemImage::create("arrow_down.png","arrow_down.png");
    _leftArrow = MenuItemImage::create("arrow_left.png","arrow_left.png");
    _rightArrow = MenuItemImage::create("arrow_right.png","arrow_right.png");
    
    _upArrow->setPosition(Point(0,32));
    _downArrow->setPosition(Point(0,-32));
    _leftArrow->setPosition(Point(-32,0));
    _rightArrow->setPosition(Point(32,0));
    
    _dpad = Menu::create();
    _dpad->addChild(_upArrow);
    _dpad->addChild(_downArrow);
    _dpad->addChild(_leftArrow);
    _dpad->addChild(_rightArrow);
    
    _dpad->setPosition(Point(64, 64));
    _dpad->setOpacity(128);
    
    this->addChild(_dpad);
    
    return true;
}

int HUD::getDirection(){
    if (_upArrow && _upArrow->isSelected()) {
        return 1;
    }
    if (_downArrow && _downArrow->isSelected()) {
        return 2;
    }
    if (_leftArrow && _leftArrow->isSelected()) {
        return 3;
    }
    if (_rightArrow && _rightArrow->isSelected()) {
        return 4;
    }
    return 0;
}


