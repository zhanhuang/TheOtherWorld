//
//  DPad.cpp
//  TheOtherWorld
//
//  Created by Zhan Huang on 12/1/13.
//
//

#include "DPad.h"

USING_NS_CC;

DPad::~DPad(){
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

bool DPad::init(){
    if ( !Menu::init() )
    {
        return false;
    }
    
    // add dpad
    _upArrow = MenuItemImage::create("arrow_up.png","arrow_up.png");
    _downArrow = MenuItemImage::create("arrow_down.png","arrow_down.png");
    _leftArrow = MenuItemImage::create("arrow_left.png","arrow_left.png");
    _rightArrow = MenuItemImage::create("arrow_right.png","arrow_right.png");
    
    _upArrow->setPosition(Point(40,64));
    _downArrow->setPosition(Point(40,16));
    _leftArrow->setPosition(Point(16,40));
    _rightArrow->setPosition(Point(64,40));
    
    this->addChild(_upArrow);
    this->addChild(_downArrow);
    this->addChild(_leftArrow);
    this->addChild(_rightArrow);
    
    return true;
}

int DPad::getDirection(){
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


