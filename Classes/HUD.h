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
    
    // implement the "static node()" method manually
    CREATE_FUNC(HUD);

protected:
    cocos2d::MenuItem *_upArrow;
    cocos2d::MenuItem *_downArrow;
    cocos2d::MenuItem *_leftArrow;
    cocos2d::MenuItem *_rightArrow;
    cocos2d::Menu *_dpad;
};

#endif /* defined(__TheOtherWorld__HUD__) */
