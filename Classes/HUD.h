//
//  HUD.h
//  TheOtherWorld
//
//  Created by Zhan Huang on 12/1/13.
//
//

#ifndef __TheOtherWorld__HUD__
#define __TheOtherWorld__HUD__

#include "cocos2d.h"

class HUD : public cocos2d::Layer
{
public:
    HUD(){};
    ~HUD();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    int getDirection();
    int getActionPressed();
    
//    // TODO: implement better touch control
//    int directionPressed;
//    bool actionPressed;
//	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
//	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
//	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HUD);

protected:
    cocos2d::MenuItem *_upArrow;
    cocos2d::MenuItem *_downArrow;
    cocos2d::MenuItem *_leftArrow;
    cocos2d::MenuItem *_rightArrow;
    cocos2d::MenuItem *_circle;
    cocos2d::Menu *_dpad;
    cocos2d::Menu *_actionButton;
};

#endif /* defined(__TheOtherWorld__HUD__) */
