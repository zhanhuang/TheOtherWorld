//
//  GameScene.cpp
//  TheOtherWorld
//
//  Created by Zhan Huang on 11/30/13.
//
//

#include "GameScene.h"
#include "SimpleAudioEngine.h"


using namespace cocos2d;

GameScene::~GameScene()
{
    if(_keyDict){
        _keyDict->release();
        _keyDict = NULL;
    }
	// cpp don't need to call super dealloc
	// virtual destructor will do this
}

GameScene::GameScene()
{
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	bool bRet = false;
	do
	{
		//////////////////////////////////////////////////////////////////////////
		// super init first
		//////////////////////////////////////////////////////////////////////////
        
		CC_BREAK_IF(! Scene::init());
        
        // make game layer
        _gameLayer = Layer::create();
		CC_BREAK_IF(! _gameLayer);
        
        this->addChild(_gameLayer);
        
        // add tiled map as background
        _levelMap = TMXTiledMap::create("final.tmx");
        _levelMap->setScale(2.0);
        _levelMap->getLayer("man")->setVisible(false);
        _levelMap->getLayer("girl")->setVisible(false);
        _levelMap->getLayer("visibility_man")->setVisible(false);
        _levelMap->getLayer("visibility_girl")->setVisible(false);
        _movables = _levelMap->getLayer("move");
        _objects = _levelMap->getObjectGroup("objects");
        _objectTiles = _levelMap->getLayer("objectTiles");
        _platforms = _levelMap->getObjectGroup("platforms");
        _platformTiles = _levelMap->getLayer("platformTiles");
        _gameLayer->addChild(_levelMap, -2);
        
        // add overlay tilemap for trees
        _levelOverlay = TMXTiledMap::create("finalOverlay.tmx");
        _levelOverlay->setScale(2.0);
        _gameLayer->addChild(_levelOverlay, 1);
        
        TILE_SIZE = _levelMap->getTileSize().width;
        
        // add players
        
        Dictionary *spawnPoint0 = _objects->getObject("spawnPoint0");
        int x0 = spawnPoint0->valueForKey("x")->intValue();
        int y0 = spawnPoint0->valueForKey("y")->intValue();
        SpriteBatchNode *batchNodeMan = SpriteBatchNode::create("man.png");
        _gameLayer->addChild(batchNodeMan);
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("man.plist");
        _man = Player::createWithSpriteFrameName("man_front0.png");
        _man->setScale(2.0);
        batchNodeMan->addChild(_man);
        _man->setPosition(Point(x0,y0));
        _man->endCoord = tileCoordForPosition(_man->getPosition());
        
        Dictionary *spawnPoint1 = _objects->getObject("spawnPoint1");
        int x1 = spawnPoint1->valueForKey("x")->intValue();
        int y1 = spawnPoint1->valueForKey("y")->intValue();
        SpriteBatchNode *batchNodeGirl = SpriteBatchNode::create("girl.png");
        _gameLayer->addChild(batchNodeGirl);
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("girl.plist");
        _girl = Player::createWithSpriteFrameName("girl_front0.png");
        _girl->setScale(2.0);
        batchNodeGirl->addChild(_girl);
        _girl->setPosition(Point(x1,y1));
        _girl->endCoord = tileCoordForPosition(_girl->getPosition());
        
        // add HUD
        _hud = HUD::create();
        this->addChild(_hud, 2);
        
        
        // align with the girl first. change when game actually starts
        this->alignViewPosition(Point((x0+x1)/2,(y0+y1)/2));
        
        // init keyDict
        _keyDict = new Dictionary();
        _keyDict->init();
        _keyDict->setObject(Integer::create(1), "placeholder"); //so type is now kdictstr
        
        // don't queue move at first
        nextMoveSent = true;
        
        isFirstLaunch = true;
        showStartGameLayer();
        
        
//		// use updateGame instead of update, otherwise it will conflict with SelectorProtocol::update
//		this->schedule( schedule_selector(GameScene::updateGame));
        
        
//        // add touch listener
//        auto dispatcher = Director::getInstance()->getEventDispatcher();
//        auto listener = EventListenerTouchAllAtOnce::create();
//        listener->onTouchesEnded = CC_CALLBACK_2(GameScene::onTouchesEnded, this);
//        dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        
        
//        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("background-music.wav", true);
        
		bRet = true;
	} while (0);
    
    // If hits any of the CC_BREAK_IF() s, bRet is false.
	return bRet;
}


void GameScene::alignViewPosition(Point playerPosition){
    auto visibleSize = Director::getInstance()->getVisibleSize();
    _gameLayer->setPosition(Point(visibleSize.width/2 - playerPosition.x, visibleSize.height/2 - playerPosition.y));
}

void GameScene::startGame(){
    this->alignViewPosition(_controlledPlayer->getPosition());
    // use updateGame instead of update, otherwise it will conflict with SelectorProtocol::update
    this->schedule( schedule_selector(GameScene::updateGame));
}

// cpp with cocos2d-x
void GameScene::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
	// Choose one of the touches to work with
	Touch* touch = touches[0];
	Point location = touch->getLocation();
    auto tileCoord = tileCoordForPosition(location);
    if(tileCoord.x >= _levelMap->getMapSize().width || tileCoord.y >= _levelMap->getMapSize().height || tileCoord.x < 0 || tileCoord.y < 0){
        return;
    }
    
	// CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("effect.wav");
}

Point GameScene::tileCoordForPosition(Point position) {
    int x = position.x/TILE_SIZE - 0.5;
    int y = _levelMap->getMapSize().height - position.y/TILE_SIZE - 0.5;
    return Point(x,y);
}

void GameScene::updateGame(float dt)
{
    if (!_controlledPlayer->isMoving || !nextMoveSent) {
        int action = _hud->getActionPressed();
        int direction = _hud->getDirection();
        if (action || direction){
            float xPos = _controlledPlayer->getPosition().x;
            float yPos = _controlledPlayer->getPosition().y;
            if (_controlledPlayer->isMoving) {
                // send a move to be queued only once.
                // Set position to negative so no position correction done.
                nextMoveSent = true;
                xPos = -1.0;
                yPos = -1.0;
            } else{
                nextMoveSent = false;
            }
            sendData(action, direction, xPos, yPos);
        }
    }
}

void GameScene::processPlayerChat(Player* player, std::string chat)
{
    std::size_t locd = chat.find('d');
    std::size_t locx = chat.find('x');
    std::size_t locy = chat.find('y');
    std::string str1 = chat.substr(0,locd);
    std::string str2 = chat.substr(locd+1,locx);
    std::string str3 = chat.substr(locx+1,locy);
    std::string str4 = chat.substr(locy+1,chat.length());
    int action = std::atoi(str1.c_str());
    int direction = std::atoi(str2.c_str());
    float xpos = std::atoi(str3.c_str());
    float ypos = std::atoi(str4.c_str());
//    printf("chat is : action:%d,direction:%d,x:%f,y:%f\n",action,direction,xpos,ypos);
    
    // queue the action is player is moving
    if (player->isMoving) {
        player->queuedChat = chat;
        return;
    }
    
    // correct player position & state
    if (xpos > 0) {
        player->setPosition(Point(xpos, ypos));
        if (player == _controlledPlayer) {
            this->alignViewPosition(_controlledPlayer->getPosition());
        }
    }
    
    if (direction != 0) {
        // movement in a direction
        player->faceDirection(direction);
        Point moveVector = moveVectorFromDirection(direction);
        auto moveTarget = player->getPosition() + moveVector;
        auto tileCoordWalk = tileCoordForPosition(moveTarget);
        
        // Check for out of border and collision
        if(collisionCheck(tileCoordWalk)){
            collide(player, direction, chat);
            return;
        }
        
        // handle pushing / pulling object
        Point itemTarget;
        Point tileCoordItem;
        Point tileCoordItemTarget;
        if (_hud->getActionPressed()) {
            // pull
            itemTarget = player->getPosition() - moveVector;
        } else{
            // push
            itemTarget = player->getPosition() + moveVector;
        }
        tileCoordItem = tileCoordForPosition(itemTarget);
        tileCoordItemTarget = tileCoordForPosition(itemTarget + moveVector);
        
        // check for item pushed collision
        if (_movables->getTileGIDAt(tileCoordItem)) {
            // check if item collides when pushed
            if(!_hud->getActionPressed()){
                if(collisionCheck(tileCoordItemTarget)||_movables->getTileGIDAt(tileCoordItemTarget)){
                   collide(player, direction, chat);
                   return;
                }
            }
            FiniteTimeAction* actionPushed = MoveBy::create(MOVE_TIME-0.1, moveVector/2);
            FiniteTimeAction* actionPushDone = CallFunc::create(CC_CALLBACK_0(GameScene::tileMoveFinished, this, _movables, tileCoordItem, tileCoordItemTarget));
            Sequence* pushSeq = Sequence::create(actionPushed, actionPushDone, NULL);
            _movables->getTileAt(tileCoordItem)->runAction(pushSeq);
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("tileMove.wav");
        }
        
        // Create player movement action
        FiniteTimeAction* actionMove = MoveBy::create(MOVE_TIME, moveVector);
        FiniteTimeAction* actionMoveScreen = MoveBy::create(MOVE_TIME, -moveVector);
        FiniteTimeAction* actionMoveDone = CallFunc::create(CC_CALLBACK_0(GameScene::playerMoveFinished, this, player, chat));
        Sequence* actionSeq = Sequence::create(actionMove, actionMoveDone, NULL);
        
        // move player, move camera & HUD if it's controlled player
        if(player == _controlledPlayer){
            _gameLayer->runAction(actionMoveScreen);
        }
        player->isMoving = true;
        player->runAction(actionSeq);
        player->animateMove(direction);
        player->startCoord = tileCoordForPosition(player->getPosition());
        player->endCoord = tileCoordWalk;
    }else{
        // standing action
        if (chat.compare(_lastActionChat) == 0) {
            return;
        } else{
            _lastActionChat = chat;
        }
        Point moveVector = moveVectorFromDirection(player->facingDirection);
        auto actionTarget = player->getPosition() + moveVector;
        auto tileCoord = tileCoordForPosition(actionTarget);
        Dictionary* objectDict = objDictFromCoord(_objectTiles, _objects, tileCoord);
        if (objectDict) {
            // handle switch & key
            const String* switchTarget = objectDict->valueForKey("switch");
            const String* keyName = objectDict->valueForKey("key");
            if (switchTarget->length()) {
                removeObjectsWithName(switchTarget->getCString(), false);
                _objectTiles->removeTileAt(tileCoord);
            } else if (keyName->length()) {
                unlockKeyWithName(keyName->getCString(), 0);
                _objectTiles->removeTileAt(tileCoord);
            }
            
            // handle read
            const String* text = objectDict->valueForKey("text");
            if (text->length() && player == _controlledPlayer) {
                this->showDismissableMessageLayer(text->_string);
            }
            
            // play get key sound
            const String* type = objectDict->valueForKey("type");
            if (!type->compare("key")) {
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("getKey.wav");
            }
        }
    }
}

void GameScene::playerMoveFinished(Player* player, std::string lastChat){
    player->isMoving = false;
    platformTrigger(player->endCoord, "player");
    platformToggleOff(player->startCoord, "player");
    if (!player->queuedChat.empty()) {
        if (player == _controlledPlayer) {
            nextMoveSent = false;
        }
        // check not the same first move
        if (player->queuedChat.compare(lastChat) != 0 || player->queuedChat.find("-") != std::string::npos) {
            processPlayerChat(player, player->queuedChat);
        }
        player->queuedChat = "";
    }
}

Point GameScene::moveVectorFromDirection(int direction){
    switch (direction) {
        case 1:
            return Point(0,TILE_SIZE);
            break;
        case 2:
            return Point(0,-TILE_SIZE);
            break;
        case 3:
            return Point(-TILE_SIZE,0);
            break;
        case 4:
            return Point(TILE_SIZE,0);
            break;
        default:
            break;
    }
    return Point::ZERO;
}

Dictionary* GameScene::objDictFromCoord(TMXLayer* objLayer, TMXObjectGroup* objGroup, Point coord){
    if (!objLayer->getTileGIDAt(coord)) {
        return NULL;
    }
    Object *nextObj;
    CCARRAY_FOREACH(objGroup->getObjects(), nextObj){
        Dictionary *nextObjDict = dynamic_cast<Dictionary*>(nextObj);
        if(tileCoordFromObjDict(nextObjDict) == coord){
            return nextObjDict;
        }
    }
    return NULL;
}

void GameScene::removeObjectsWithName(const char *switchTarget, bool temporary){
    Object* nextObj;
    CCARRAY_FOREACH(_objects->getObjects(), nextObj){
        Dictionary *nextObjDict = dynamic_cast<Dictionary*>(nextObj);
        if (!nextObjDict->valueForKey("name")->compare(switchTarget)) {
            if (temporary) {
                auto toggleTarget = _objectTiles->getTileAt(tileCoordFromObjDict(nextObjDict));
                toggleTarget->setVisible(!toggleTarget->isVisible());
            } else{
                _objectTiles->removeTileAt(tileCoordFromObjDict(nextObjDict));
            }
        }
    }
    if (_levelMap->getLayer(switchTarget)) {
        if (temporary) {
            auto toggleTarget = _levelMap->getLayer(switchTarget);
            toggleTarget->setVisible(!toggleTarget->isVisible());
        } else{
            _levelMap->removeChild(_levelMap->getLayer(switchTarget));
        }
    }
}

void GameScene::unlockKeyWithName(const char *keyName, int toggle){
    // increase key count in keyDict
    int keyVal = _keyDict->valueForKey(keyName)->intValue();
    printf("keyVal: %d, toggle:%d", keyVal, toggle);
    if (toggle != 0) {
        keyVal += toggle;
        if (keyVal < 0){
            keyVal = 0;
        }
    } else{
        keyVal++;
    }
    std::stringstream ss;
    ss << keyVal;
    _keyDict->setObject(String::create(ss.str()), keyName);
    
    
    // remove corresponding locks
    bool hasLocked = false;
    
    int keyCount;
    Object* nextObj;
    CCARRAY_FOREACH(_objects->getObjects(), nextObj){
        Dictionary *nextObjDict = dynamic_cast<Dictionary*>(nextObj);
        if (!nextObjDict->valueForKey("name")->compare(keyName)) {
            keyCount = nextObjDict->valueForKey("keyCount")->intValue();
            Point targetLoc = tileCoordFromObjDict(nextObjDict);
            if (keyCount <= keyVal) {
                if (toggle == 0) {
                    _objectTiles->removeTileAt(targetLoc);
                } else if(toggle == 1){
                    _objectTiles->getTileAt(targetLoc)->setVisible(false);
                }
            } else{
                if(toggle == -1){
                    _objectTiles->getTileAt(targetLoc)->setVisible(true);
                }
                hasLocked = true;
            }
        }
    }
    CCARRAY_FOREACH(_levelMap->getChildren(), nextObj){
        TMXLayer *nextLayer = dynamic_cast<TMXLayer*>(nextObj);
        auto properties = nextLayer->getProperties();
        if (properties) {
            keyCount = nextLayer->getProperties()->valueForKey(keyName)->intValue();
            if (keyCount) {
                if (keyCount <= keyVal) {
                    if (toggle == 0) {
                        _levelMap->removeChild(nextLayer);
                    } else if(toggle == 1){
                        nextLayer->setVisible(false);
                    }
                } else{
                    if(toggle == -1){
                        nextLayer->setVisible(true);
                    }
                    hasLocked = true;
                }
            }
        }
    }
    
    if (!hasLocked) {
        keyVal += 10;
        std::stringstream ss2;
        ss2 << keyVal;
        _keyDict->setObject(String::create(ss2.str()), keyName);
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("unlocked.wav");
    }
}

Point GameScene::tileCoordFromObjDict(Dictionary *objDict){
    int x = objDict->valueForKey("x")->intValue() + TILE_SIZE/2;
    int y = objDict->valueForKey("y")->intValue() + TILE_SIZE/2;
    return tileCoordForPosition(Point(x,y));
}

void GameScene::collide(Player* player, int direction, std::string chat){
    Point moveVector = moveVectorFromDirection(direction);
    player->isMoving = true;
    FiniteTimeAction* actionMove = MoveBy::create((MOVE_TIME-0.1)/2, moveVector/4);
    FiniteTimeAction* actionMove2 = MoveBy::create((MOVE_TIME-0.1)/2, -moveVector/4);
    FiniteTimeAction* actionMoveDone = CallFunc::create(CC_CALLBACK_0(GameScene::playerMoveFinished, this, player, chat));
    Sequence* actionSeq = Sequence::create(actionMove, actionMove2, actionMoveDone, NULL);
    player->runAction(actionSeq);
}

bool GameScene::failBoundsCheck(cocos2d::TMXTiledMap *map, cocos2d::Point coord){
    if(coord.x >= map->getMapSize().width || coord.y >= map->getMapSize().height || coord.x < 0 || coord.y < 0){
        return true;
    }
    return false;
}

bool GameScene::collisionCheck(cocos2d::Point coord){
    // out of bounds
    if(coord.x >= _levelMap->getMapSize().width || coord.y >= _levelMap->getMapSize().height || coord.x < 0 || coord.y < 0){
        return true;
    }
    // collide with collidable layer
    Object* nextObj;
    CCARRAY_FOREACH(_levelMap->getChildren(), nextObj){
        TMXLayer *nextLayer = dynamic_cast<TMXLayer*>(nextObj);
        if (nextLayer->isVisible()) {
            if (nextLayer->getProperties()->valueForKey("collision")->length()) {
                if (nextLayer->getTileGIDAt(coord)) {
                    if (nextLayer->getTileAt(coord)->isVisible()) {
                        return true;
                    }
                }
            }
        }
    }
    // collide with player
    Point coord0 = tileCoordForPosition(_controlledPlayer->getPosition());
    Point coord1 = tileCoordForPosition(_otherPlayer->getPosition());
    if (coord == coord0 || coord == coord1) {
        return true;
    }
    
//    // includes walking positions
//    if (coord == _controlledPlayer->endCoord || coord == _otherPlayer->endCoord) {
//        return true;
//    }
//    if (_controlledPlayer->isMoving && coord == _controlledPlayer->startCoord) {
//        return true;
//    }
//    if (_otherPlayer->isMoving && coord == _otherPlayer->startCoord) {
//        return true;
//    }
    return false;
}

void GameScene::tileMoveFinished(cocos2d::TMXLayer *layer, cocos2d::Point fromCoord, cocos2d::Point toCoord){
    int tileGID = layer->getTileGIDAt(fromCoord);
    layer->setTileGID(tileGID, toCoord);
    layer->removeTileAt(fromCoord);
    
    auto properties = _levelMap->getPropertiesForGID(tileGID);
    if (properties) {
        const String* blockProperty = properties->valueForKey("block");
        if (blockProperty->length()) {
            platformTrigger(toCoord, blockProperty->getCString());
            platformToggleOff(fromCoord, blockProperty->getCString());
            return;
        }
    }
    platformTrigger(toCoord, "blocks");
    platformToggleOff(fromCoord, "blocks");
}

void GameScene::platformTrigger(cocos2d::Point coord, const char *property){
    Dictionary *objectDict = objDictFromCoord(_platformTiles, _platforms, coord);
    if (objectDict) {
        const String* blockRequired = objectDict->valueForKey("block");
        if (!blockRequired->compare(property) || !blockRequired->compare("any")) {
            // handle switch, toggle & key
            const String* switchTarget = objectDict->valueForKey("switch");
            const String* toggleTarget = objectDict->valueForKey("toggle");
            const String* keyName = objectDict->valueForKey("key");
            if (switchTarget->length()) {
                removeObjectsWithName(switchTarget->getCString(), false);
                _platformTiles->removeTileAt(coord);
            } else if (toggleTarget->length()) {
                if (!toggleTarget->compare("key")) {
                    unlockKeyWithName(keyName->getCString(), 1);
                }
                removeObjectsWithName(toggleTarget->getCString(), true);
                auto toggleTarget = _platformTiles->getTileAt(coord);
                toggleTarget->setVisible(!toggleTarget->isVisible());
            } else if (keyName->length()) {
                unlockKeyWithName(keyName->getCString(), 0);
                _platformTiles->removeTileAt(coord);
            }
            
            const String* text = objectDict->valueForKey("text");
            if (text->length()) {
                this->showDismissableMessageLayer(text->getCString());
            }
            
            const String* win = objectDict->valueForKey("win");
            if (win->length()) {
                this->unschedule(schedule_selector(GameScene::updateGame));
            }
        }
    }
}

void GameScene::platformToggleOff(cocos2d::Point coord, const char *property){
    Dictionary *objectDict = objDictFromCoord(_platformTiles, _platforms, coord);
    if (objectDict) {
        const String* blockRequired = objectDict->valueForKey("block");
        if (!blockRequired->compare(property) || !blockRequired->compare("any")) {
            // handle toggle off when it was previously on
            const String* toggleTarget = objectDict->valueForKey("toggle");
            if (toggleTarget->length()) {
                if (!toggleTarget->compare("key")) {
                    const String* keyName = objectDict->valueForKey("key");
                    unlockKeyWithName(keyName->getCString(), -1);
                }
                removeObjectsWithName(toggleTarget->getCString(), true);
                _platformTiles->getTileAt(coord)->setVisible(true);
            }
        }
    }
}

/***
 * AppWarp Helper Methods
 */

void GameScene::showStartGameLayer()
{
    // Get the dimensions of the window for calculation purposes
    Size winSize = Director::getInstance()->getWinSize();
    
    startGameLayer = StartGameLayer::create();
    addChild(startGameLayer, 100);
    
    LabelTTF *buttonTitle = LabelTTF::create("Start Game", "Marker Felt", 30);
    buttonTitle->setColor(Color3B::WHITE);
    
    MenuItemLabel *startGameButton = MenuItemLabel::create(buttonTitle, CC_CALLBACK_1(GameScene::connectToAppWarp, this));
    startGameButton->setPosition(Point(winSize.width/2,winSize.height/2));
    Menu *pMenu = Menu::create(startGameButton,NULL);
    pMenu->setPosition(Point::ZERO);
    startGameLayer->addChild(pMenu, 1);
}

void GameScene::removeStartGameLayer()
{
    removeChild(startGameLayer,false);
    if(_controlledPlayer){
        _controlledPlayer->isMoving = false;
    }
}

std::string genRandom()
{
	std::string charStr;
	srand (time(NULL));
    
	for (int i = 0; i < 10; ++i) {
		charStr += (char)(65+(rand() % (26)));
	}
    
	return charStr;
}

void GameScene::connectToAppWarp(Object* pSender)
{
    isConnected = false;
    AppWarp::Client *warpClientRef;
    if (isFirstLaunch)
    {
        isFirstLaunch = !isFirstLaunch;
        AppWarp::Client::initialize(APPWARP_APP_KEY,APPWARP_SECRET_KEY);
        warpClientRef = AppWarp::Client::getInstance();
        warpClientRef->setRecoveryAllowance(60);
        warpClientRef->setConnectionRequestListener(this);
        warpClientRef->setNotificationListener(this);
        warpClientRef->setRoomRequestListener(this);
        warpClientRef->setZoneRequestListener(this);
        userName = genRandom();
        warpClientRef->connect(userName);
    }
    else
    {
        AppWarp::Client::getInstance()->connect(userName);
    }
}

void GameScene::onConnectDone(int res)
{
    if (res==AppWarp::ResultCode::success)
    {
        unscheduleRecover();
        printf("\nonConnectDone .. SUCCESS..session=%d\n",AppWarp::AppWarpSessionID);
        AppWarp::Client *warpClientRef;
        warpClientRef = AppWarp::Client::getInstance();
        // join room with 1 player. Second player to join a room is man
        isMan = true;
        warpClientRef->joinRoomInUserRange(1, 1, false);
        
    }
    else if (res==AppWarp::ResultCode::success_recovered)
    {
        unscheduleRecover();
        removeStartGameLayer();
        printf("\nonConnectDone .. SUCCESS with success_recovered..session=%d\n",AppWarp::AppWarpSessionID);
    }
    else if (res==AppWarp::ResultCode::connection_error_recoverable)
    {
        scheduleRecover();
        printf("\nonConnectDone .. FAILED..connection_error_recoverable..session=%d\n",AppWarp::AppWarpSessionID);
    }
    else if (res==AppWarp::ResultCode::bad_request)
    {
        unscheduleRecover();
        printf("\nonConnectDone .. FAILED with bad request..session=%d\n",AppWarp::AppWarpSessionID);
    }
    else
    {
        unscheduleRecover();
        printf("\nonConnectDone .. FAILED with unknown reason..session=%d\n",AppWarp::AppWarpSessionID);
    }
}

void GameScene::scheduleRecover()
{
    printf("\nHelloWorld::scheduleRecover");
    this->schedule(schedule_selector(GameScene::recover), 5.0f);
    
    showReconnectingLayer("Reconnecting ...");
}

void GameScene::unscheduleRecover()
{
    printf("\nHelloWorld::unscheduleRecover");
    unschedule(schedule_selector(GameScene::recover));
}

void GameScene::recover(float dt)
{
    printf("\nHelloWorld::recover");
    AppWarp::Client::getInstance()->recoverConnection();
}

void GameScene::showReconnectingLayer(std::string message)
{
    
    // Get the dimensions of the window for calculation purposes
    Size winSize = Director::getInstance()->getWinSize();
    
    startGameLayer = StartGameLayer::create();
    startGameLayer->setColor(Color3B(0, 0, 0));
    startGameLayer->setOpacity(50);
    addChild(startGameLayer);
    
    LabelTTF *buttonTitle = LabelTTF::create(message.c_str(), "Marker Felt", 30);
    buttonTitle->setColor(Color3B::WHITE);
    startGameLayer->addChild(buttonTitle);
    buttonTitle->setPosition(Point(winSize.width/2,winSize.height/2));
    
    
}

void GameScene::showDismissableMessageLayer(std::string message)
{
    this->removeStartGameLayer();
    if (_controlledPlayer) {
        _controlledPlayer->isMoving = true;
    }
    
    // Get the dimensions of the window for calculation purposes
    Size winSize = Director::getInstance()->getWinSize();
    
    startGameLayer = StartGameLayer::create();
    startGameLayer->setColor(Color3B(0, 0, 0));
    startGameLayer->setOpacity(180);
    addChild(startGameLayer, 3);
    
    LabelTTF *buttonTitle = LabelTTF::create(message.c_str(), "Marker Felt", 18, Size(300, 0), TextHAlignment::CENTER);
    buttonTitle->setColor(Color3B::WHITE);
    startGameLayer->addChild(buttonTitle);
    buttonTitle->setPosition(Point(winSize.width/2,winSize.height/2));
    
    LabelTTF *dismissButtonTitle = LabelTTF::create("BACK", "Marker Felt", 30);
    dismissButtonTitle->setColor(Color3B::GRAY);
    MenuItemLabel *dismissButton = MenuItemLabel::create(dismissButtonTitle, CC_CALLBACK_0(GameScene::removeStartGameLayer, this));
    dismissButton->setPosition(Point(winSize.width*7/8,winSize.height/5));
    Menu *pMenu = Menu::create(dismissButton,NULL);
    pMenu->setPosition(Point::ZERO);
    startGameLayer->addChild(pMenu, 1);
}

void GameScene::onJoinRoomDone(AppWarp::room revent)
{
    if (revent.result==0)
    {
        printf("\nonJoinRoomDone .. SUCCESS\n");
        AppWarp::Client *warpClientRef;
        warpClientRef = AppWarp::Client::getInstance();
        warpClientRef->subscribeRoom(revent.roomId);
        if (isMan) {
            _controlledPlayer = _man;
            _otherPlayer = _girl;
            _levelMap->getLayer("man")->setVisible(true);
            _levelMap->getLayer("visibility_man")->setVisible(true);
        }else{
            _controlledPlayer = _girl;
            _otherPlayer = _man;
            _levelMap->getLayer("girl")->setVisible(true);
            _levelMap->getLayer("visibility_girl")->setVisible(true);
        }
        this->startGame();
        removeStartGameLayer();
    }
    else{
        printf("\nonJoinRoomDone .. FAILED\n");
        if (isMan) {
            printf("\nNo Player found in rooms. Joining new room...\n");
            isMan = false;
            AppWarp::Client *warpClientRef;
            warpClientRef = AppWarp::Client::getInstance();
            // join room with no players. First player to join a room is girl
            warpClientRef->joinRoomInUserRange(0, 0, false);
        }else{
            printf("\nNo Empty Room. Joining room with player...\n");
            isMan = true;
            AppWarp::Client *warpClientRef;
            warpClientRef = AppWarp::Client::getInstance();
            // join room with 1 player. Second player to join a room is man
            warpClientRef->joinRoomInUserRange(1, 1, false);
        }
    }
}

void GameScene::onSubscribeRoomDone(AppWarp::room revent)
{
    if (revent.result==0)
    {
        printf("\nonSubscribeRoomDone .. SUCCESS\n");
    }
    else
        printf("\nonSubscribeRoomDone .. FAILED\n");
}

void GameScene::sendData(int action, int direction, float xpos, float ypos)
{
    AppWarp::Client *warpClientRef;
	warpClientRef = AppWarp::Client::getInstance();
    
    std::stringstream str;
    str << action << "d" << direction << "x" << xpos << "y" << ypos;
    warpClientRef->sendChat(str.str());
}

void GameScene::onChatReceived(AppWarp::chat chatevent)
{
    printf("onChatReceived..");
    Player* player;
    if(chatevent.sender != userName){
        player = _otherPlayer;
    }else{
        player = _controlledPlayer;
    }
    
    this->processPlayerChat(player, chatevent.chat);
}

void GameScene::onUserPaused(std::string user,std::string locId,bool isLobby)
{
    //    printf("\nonUserPaused...user=%s",user.c_str());
    //    printf("\nonUserPaused...locId=%s",locId.c_str());
    std::string message = "Other user connection lost";
    showDismissableMessageLayer(message);
}

void GameScene::onUserResumed(std::string user,std::string locId,bool isLobby)
{
    //    printf("\nonUserResumed...user=%s",user.c_str());
    //    printf("\nonUserResumed...locId=%s",locId.c_str());
    // removeStartGameLayer();
}
