//
//  Player.h
//  TheOtherWorld
//
//  Created by Zhan Huang on 12/13/13.
//
//

#ifndef __TheOtherWorld__Player__
#define __TheOtherWorld__Player__

#include <iostream>
#include "cocos2d.h"

#define MOVE_TIME 0.4

class Player : public cocos2d::Sprite
{
public:
    static Player* createWithSpriteFrameName(const std::string& spriteFrameName);
    ~Player();
    
    void animateMove(int direction);
    void faceDirection(int direction);
    int facingDirection;
    bool isMoving;
    cocos2d::Point coord; //saves space of player
    
private:
    std::string _character;
    cocos2d::Array *_upframes;
    cocos2d::Array *_downframes;
    cocos2d::Array *_leftframes;
    cocos2d::Array *_rightframes;
    
    void makeAnimations(const std::string& startFrameName);
};

#endif /* defined(__TheOtherWorld__Player__) */
