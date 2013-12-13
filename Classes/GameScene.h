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
#include "StartGameLayer.h"
#include "appwarp.h"

#define APPWARP_APP_KEY     "de80eeab97716fd56519adb1190a6823384636c0ca1cebb32c2a06d0da692754"
#define APPWARP_SECRET_KEY  "8d614c7559468c004a0257fb37d7346311a17e51814a499877c4e65768b044bc"
#define ROOM_ID             "60606240"

//#include "SimpleAudioEngine.h"

class GameScene : public cocos2d::Scene, public AppWarp::ConnectionRequestListener,public AppWarp::RoomRequestListener,public AppWarp::NotificationListener,public AppWarp::ZoneRequestListener
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
    int MOVE_;
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
    
    
    
    // appwarp
    void connectToAppWarp(Object* pSender);
    
    bool isConnected;
    std::string userName;
    bool isFirstLaunch;
    StartGameLayer *startGameLayer;
    
    void showStartGameLayer();
    void removeStartGameLayer();
    
    void onConnectDone(int res);
    void onJoinRoomDone(AppWarp::room revent);
    void onSubscribeRoomDone(AppWarp::room revent);
    void sendData(int action, int direction);
    void onChatReceived(AppWarp::chat chatevent);
    void onUserPaused(std::string user,std::string locId,bool isLobby);
    void onUserResumed(std::string user,std::string locId,bool isLobby);
    
    void scheduleRecover();
    void unscheduleRecover();
    void recover(float dt);
    void showReconnectingLayer(std::string message);
};

#endif /* defined(__TheOtherWorld__GameScene__) */
