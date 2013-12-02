//
//  DPad.h
//  TheOtherWorld
//
//  Created by Zhan Huang on 12/1/13.
//
//

#ifndef __TheOtherWorld__DPad__
#define __TheOtherWorld__DPad__

#include "cocos2d.h"

class DPad : public cocos2d::Menu
{
public:
    DPad(){};
    ~DPad();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    int getDirection();
    
    // implement the "static node()" method manually
    CREATE_FUNC(DPad);

protected:
    cocos2d::MenuItem *_upArrow;
    cocos2d::MenuItem *_downArrow;
    cocos2d::MenuItem *_leftArrow;
    cocos2d::MenuItem *_rightArrow;
};

#endif /* defined(__TheOtherWorld__DPad__) */
