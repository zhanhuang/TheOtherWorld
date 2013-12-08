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
#include "HUD.h"

//#include "SimpleAudioEngine.h"

class GameScene : public cocos2d::Scene
{
public:
	GameScene();
	~GameScene();
    
	// Here's a difference. Method 'init' in cocos2d-x returns bool,
    // instead of returning 'id' in cocos2d-iphone
	virtual bool init();
    
	// implement the "static node()" method manually
	CREATE_FUNC(GameScene);
    
    void alignViewPosition(cocos2d::Point playerPosition);
    
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    
    cocos2d::Point tileCoordForPosition(cocos2d::Point position);
    
	void updateGame(float dt);
    
    
protected:
    int TILE_SIZE;
    cocos2d::TMXTiledMap *_levelMap;
    cocos2d::TMXTiledMap *_levelOverlay;
    cocos2d::TMXLayer *_meta;
    cocos2d::TMXLayer *_movables;
    cocos2d::Sprite *_player1;
    cocos2d::Layer *_gameLayer;
    HUD *_hud;
    
    
    bool _playerIsMoving;
    void collide(cocos2d::Point moveVector);
    void playerMoveFinished();
    void tileMoveFinished(cocos2d::TMXLayer *layer, cocos2d::Point fromCoord, cocos2d::Point toCoord);
    bool passTilePropertyCheck(cocos2d::TMXTiledMap *map, cocos2d::TMXLayer *layer, cocos2d::Point coord, const char *property);
    bool failBoundsCheck(cocos2d::TMXTiledMap *map, cocos2d::Point coord);
};

#endif /* defined(__TheOtherWorld__GameScene__) */
