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
    
    _upArrow->setPosition(Point(0,48));
    _downArrow->setPosition(Point(0,-48));
    _leftArrow->setPosition(Point(-48,0));
    _rightArrow->setPosition(Point(48,0));
    
    _dpad = Menu::create();
    _dpad->addChild(_upArrow);
    _dpad->addChild(_downArrow);
    _dpad->addChild(_leftArrow);
    _dpad->addChild(_rightArrow);
    
    _dpad->setPosition(Point(64, 64));
    _dpad->setOpacity(128);
    this->addChild(_dpad);
    
    // add action button
    _circle = MenuItemImage::create("circle.png","circle.png");
    
    _actionButton = Menu::create();
    _actionButton->addChild(_circle);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    _actionButton->setPosition(visibleSize.width - 64, 64);
    _actionButton->setOpacity(128);
    this->addChild(_actionButton);
    
//    // add touch listener
//    auto dispatcher = Director::getInstance()->getEventDispatcher();
//    auto listener = EventListenerTouchAllAtOnce::create();
//    listener->onTouchesBegan = CC_CALLBACK_2(HUD::onTouchesBegan, this);
//    listener->onTouchesMoved = CC_CALLBACK_2(HUD::onTouchesMoved, this);
//    listener->onTouchesEnded = CC_CALLBACK_2(HUD::onTouchesEnded, this);
//    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
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

int HUD::getActionPressed(){
    if (_circle && _circle->isSelected()) {
        return 1;
    }
    return 0;
}

//void HUD::onTouchesBegan(const std::vector<Touch*>& touches, Event* event){
//    auto visibleSize = Director::getInstance()->getVisibleSize();
//	for(Touch* touch : touches){
//        Point location = touch->getLocation();
//        if (location.x < 150 && location.y < 150) {
//            // dpad
//            if(location.y > abs(location.x)){
//                directionPressed = 1;
//            } else if(-location.y > abs(location.x)){
//                directionPressed = 2;
//            } else if(location.x > abs(location.y)){
//                directionPressed = 4;
//            } else if(-location.x > abs(location.y)){
//                directionPressed = 3;
//            }
//        } else if (location.x > visibleSize.width - 128 && location.y < 128){
//            // action button
//            actionPressed = true;
//        }
//    }
//}
//
//void HUD::onTouchesMoved(const std::vector<Touch*>& touches, Event* event){
//    auto visibleSize = Director::getInstance()->getVisibleSize();
//	for(Touch* touch : touches){
//        Point location = touch->getLocation();
//        if (location.x < 150 && location.y < 150) {
//            // dpad
//            if(location.y > abs(location.x)){
//                directionPressed = 1;
//            } else if(-location.y > abs(location.x)){
//                directionPressed = 2;
//            } else if(location.x > abs(location.y)){
//                directionPressed = 4;
//            } else if(-location.x > abs(location.y)){
//                directionPressed = 3;
//            }
//        } else if (location.x > visibleSize.width - 128 && location.y < 128){
//            // action button
//            actionPressed = true;
//        }
//    }
//}
//
//void HUD::onTouchesEnded(const std::vector<Touch*>& touches, Event* event){
//	auto visibleSize = Director::getInstance()->getVisibleSize();
//	for(Touch* touch : touches){
//        Point location = touch->getLocation();
//        if (location.x < 250 && location.y < 250) {
//            // dpad
//            directionPressed = 0;
//        } else if (location.x > visibleSize.width - 200 && location.y < 200){
//            // action button
//            actionPressed = false;
//        }
//    }
//}
