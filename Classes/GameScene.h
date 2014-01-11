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
#include "Player.h"
#include "appwarp.h"

#define APPWARP_APP_KEY     "de80eeab97716fd56519adb1190a6823384636c0ca1cebb32c2a06d0da692754"
#define APPWARP_SECRET_KEY  "8d614c7559468c004a0257fb37d7346311a17e51814a499877c4e65768b044bc"

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
    
    void GameStart();
    void alignViewPosition(cocos2d::Point playerPosition);
	void updateGame(float dt);
    
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    
    bool isSinglePlayer;
    
    // TODO: move over to menuscene
    void multiplayerConnect();
    
protected:
    int TILE_SIZE;
    int MOVE_;
    cocos2d::TMXTiledMap *_levelMap;
    cocos2d::TMXTiledMap *_levelOverlay;
    cocos2d::TMXObjectGroup *_objects;
    cocos2d::TMXLayer *_objectTiles;
    cocos2d::TMXObjectGroup *_platforms;
    cocos2d::TMXLayer *_platformTiles;
    cocos2d::TMXLayer *_movables;
    cocos2d::Layer *_gameLayer;
    HUD *_hud;
    
    cocos2d::Dictionary *_keyDict;
    
    // player info
    Player *_girl;
    Player *_man;
    Player *_controlledPlayer;
    Player *_otherPlayer;
    bool isMan;
    
    // queue for all actions
    cocos2d::Array *_chatQueue;
    
    cocos2d::Point moveVectorFromDirection(int direction);
    void processPlayerChat(Player* player, std::string chat);
    void playerMoveFinished(Player* player);
    void collide(Player* player, int direction);
    void tileMoveFinished(cocos2d::TMXLayer *layer, cocos2d::Point fromCoord, cocos2d::Point toCoord);
    bool failBoundsCheck(cocos2d::TMXTiledMap *map, cocos2d::Point coord);
    bool collisionCheck(cocos2d::Point coord);
    cocos2d::Point tileCoordForPosition(cocos2d::Point position);
    
    // game mechanisms
    cocos2d::Dictionary* objDictFromCoord(cocos2d::TMXLayer* objLayer, cocos2d::TMXObjectGroup* objGroup, cocos2d::Point coord);
    void removeObjectsWithName(const char *switchTarget, bool temporary);
    void removeKeysWithKeyname(const char *keyName);
    void removeKeysWithKeynameInGroup(const char *keyName, cocos2d::TMXObjectGroup *group, cocos2d::TMXLayer *layer);
    void unlockKeyWithName(const char *keyName, int toggle);
    void showDismissableMessageLayer(std::string message);
    void platformTrigger(cocos2d::Point coord, const char *property);
    void platformToggleOff(cocos2d::Point coord, const char *property);
    cocos2d::Point tileCoordFromObjDict(cocos2d::Dictionary *objDict);
    
    // appwarp
    void connectToAppWarp(Object* pSender);
    
    bool isConnected;
    std::string userName;
    bool isFirstLaunch;
    cocos2d::LayerColor *_gameOverlay;
    
    void showOverlay();
    void removeOverlay();
    
    void onConnectDone(int res);
    void onJoinRoomDone(AppWarp::room revent);
    void onSubscribeRoomDone(AppWarp::room revent);
    void sendData(int action, int direction, float xpos, float ypos);
    void onChatReceived(AppWarp::chat chatevent);
    void onUserPaused(std::string user,std::string locId,bool isLobby);
    void onUserResumed(std::string user,std::string locId,bool isLobby);
    
    void scheduleRecover();
    void unscheduleRecover();
    void recover(float dt);
    void showReconnectingLayer(std::string message);
};

#endif /* defined(__TheOtherWorld__GameScene__) */
