//
//  MenuScene.h
//  TheOtherWorld
//
//  Created by Zhan Huang on 11/28/13.
//
//

#ifndef __TheOtherWorld__MenuScene__
#define __TheOtherWorld__MenuScene__

#include "cocos2d.h"


class MenuScene : public cocos2d::Layer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* scene();
    
    // a selector callback
    void menuCloseCallback(Object* sender);
    
    void menuStartCallback(Object* sender);
    
    void menuConnectCallback(Object* sender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(MenuScene);
};


#endif /* defined(__TheOtherWorld__MenuScene__) */
