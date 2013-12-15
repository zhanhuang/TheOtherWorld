//
//  GameScene.cpp
//  TheOtherWorld
//
//  Created by Zhan Huang on 11/30/13.
//
//

#include "GameScene.h"
//#include "SimpleAudioEngine.h"


using namespace cocos2d;

GameScene::~GameScene()
{
    
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
        _levelMap->getLayer("collision")->setVisible(false);
        _movables = _levelMap->getLayer("move");
        _objectTiles = _levelMap->getLayer("objectTiles");
        _platforms = _levelMap->getObjectGroup("platforms");
        _gameLayer->addChild(_levelMap, -2);
        
        // add overlay tilemap for trees
        _levelOverlay = TMXTiledMap::create("finalOverlay.tmx");
        _levelOverlay->setScale(2.0);
        _gameLayer->addChild(_levelOverlay, 1);
        
        TILE_SIZE = _levelMap->getTileSize().width;
        
        // add players
        _objects = _levelMap->getObjectGroup("objects");
        
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
        
        
        // add HUD
        _hud = HUD::create();
        this->addChild(_hud, 2);
        
        
        // align with the girl first. change when game actually starts
        this->alignViewPosition(Point((x0+x1)/2,(y0+y1)/2));
        
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
                // Set position to negative so correctState() is not called.
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
    printf("chat is : action:%d,direction:%d,x:%f,y:%f\n",action,direction,xpos,ypos);
    
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
        player->runAction(actionSeq);
        player->animateMove(direction);
        player->isMoving = true;
    }else{
        // standing action
        Point moveVector = moveVectorFromDirection(player->facingDirection);
        auto actionTarget = player->getPosition() + moveVector;
        auto tileCoord = tileCoordForPosition(actionTarget);
        Dictionary* objectDict = objectDictByCoord(_objects, tileCoord);
        if (objectDict) {
            // check if it's a paper
            const String* type = objectDict->valueForKey("type");
            if (type->length() && type->compare("paper") == 0) {
                const String* text = objectDict->valueForKey("text");
                if (text->length()) {
                    showDismissableMessageLayer(text->_string);
                } else{
                    printf("PAPER WITH NO TEXT");
                }
            }
        }
    }
}

void GameScene::playerMoveFinished(Player* player, std::string lastChat){
    player->isMoving = false;
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

Dictionary* GameScene::objectDictByCoord(cocos2d::TMXObjectGroup* objGroup, cocos2d::Point coord){
    if (!_objectTiles->getTileGIDAt(coord)) {
        return NULL;
    }
    Object *nextObj;
    CCARRAY_FOREACH(objGroup->getObjects(), nextObj){
        Dictionary *nextObjDict = dynamic_cast<Dictionary*>(nextObj);
        int x = nextObjDict->valueForKey("x")->intValue() + TILE_SIZE/2;
        int y = nextObjDict->valueForKey("y")->intValue() + TILE_SIZE/2;
        if(tileCoordForPosition(Point(x,y)) == coord){
            return nextObjDict;
        }
    }
    return NULL;
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
        if (nextLayer->getProperties()->valueForKey("collision")->length()) {
            if (nextLayer->getTileGIDAt(coord)) {
                return true;
            }
        }
    }
    // collide with player
    Point coord0 = tileCoordForPosition(_controlledPlayer->getPosition());
    Point coord1 = tileCoordForPosition(_otherPlayer->getPosition());
    if (coord == coord0 || coord == coord1) {
        return true;
    }
    return false;
}

void GameScene::tileMoveFinished(cocos2d::TMXLayer *layer, cocos2d::Point fromCoord, cocos2d::Point toCoord){
    int tileGID = layer->getTileGIDAt(fromCoord);
    layer->setTileGID(tileGID, toCoord);
    layer->removeTileAt(fromCoord);
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
    removeChild(startGameLayer,true);
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
        }else{
            _controlledPlayer = _girl;
            _otherPlayer = _man;
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
