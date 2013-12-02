//
//  GameScene.h
//  TheOtherWorld
//
//  Created by Zhan Huang on 11/30/13.
//
//

#ifndef __TheOtherWorld__GameScene__
#define __TheOtherWorld__GameScene__

#include "cocos2d.h"
#include "DPad.h"

//#include "SimpleAudioEngine.h"

class GameScene : public cocos2d::Layer
{
public:
	GameScene();
	~GameScene();
    
	// Here's a difference. Method 'init' in cocos2d-x returns bool,
    // instead of returning 'id' in cocos2d-iphone
	virtual bool init();
    
	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::Scene* scene();
    
	// implement the "static node()" method manually
	CREATE_FUNC(GameScene);
    
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    
    cocos2d::Point tileCoordForPosition(cocos2d::Point position);
    
	void updateGame(float dt);
    
    
protected:
    int TILE_SIZE;
    cocos2d::TMXTiledMap *_levelMap;
    cocos2d::TMXLayer *_background;
    cocos2d::TMXLayer *_meta;
    cocos2d::Sprite *_player1;
    
    DPad *_dpad;
    
    bool _playerIsMoving;
    void playerMoveFinished();
};

#endif /* defined(__TheOtherWorld__GameScene__) */
