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
        _levelMap = TMXTiledMap::create("level.tmx");
        _levelMap->setScale(2.0);
        _meta = _levelMap->getLayer("Meta");
        _movables = _levelMap->getLayer("Movables");
        _meta->setVisible(false);
        _gameLayer->addChild(_levelMap, -2);
        
        // add overlay tilemap for trees
        _levelOverlay = TMXTiledMap::create("levelOverlay.tmx");
        _levelOverlay->setScale(2.0);
        _gameLayer->addChild(_levelOverlay, 1);
        
        TILE_SIZE = _levelMap->getTileSize().width;
        
        // add players
        TMXObjectGroup *mapObjectGroup = _levelMap->getObjectGroup("Objects");
        
        Dictionary *spawnPoint0 = mapObjectGroup->getObject("SpawnPoint0");
        int x0 = spawnPoint0->valueForKey("x")->intValue();
        int y0 = spawnPoint0->valueForKey("y")->intValue();
        SpriteBatchNode *batchNodeMan = SpriteBatchNode::create("man.png");
        _gameLayer->addChild(batchNodeMan);
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("man.plist");
        _man = Player::createWithSpriteFrameName("man_front0.png");
        _man->setScale(2.0);
        batchNodeMan->addChild(_man);
        _man->setPosition(Point(x0,y0));
        
        Dictionary *spawnPoint1 = mapObjectGroup->getObject("SpawnPoint1");
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
        this->alignViewPosition(Point(x1,y1));
        
        // add objects
        Array *allObjects = mapObjectGroup->getObjects();
        for(int i = 0; i < allObjects->count(); i++){
            Dictionary *nextObject = (Dictionary *)allObjects->getObjectAtIndex(i);
            if (nextObject->valueForKey("name")->compare("Block") == 0){
                // TODO: make block
            }
        }
        
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
    if (!_controlledPlayer->isMoving) {
        int action = _hud->getActionPressed();
        int direction = _hud->getDirection();
        if (action || direction){
            sendData(action, direction, _controlledPlayer->getPosition().x, _controlledPlayer->getPosition().y);
        }
    }
}

void GameScene::processPlayerMove(Player* player, int action, int direction){
    if (player->isMoving) {
        return;
    }
    
    if (direction != 0) {
        player->faceDirection(direction);
        Point moveVector = moveVectorFromDirection(direction);
        auto moveTarget = player->getPosition() + moveVector;
        auto tileCoordWalk = tileCoordForPosition(moveTarget);
        
        // Check for out of border and collision
        if(failBoundsCheck(_levelMap, tileCoordWalk) || passTilePropertyCheck(_levelMap, _meta, tileCoordWalk, "collidable")){
            collide(player, direction);
            return;
        }
        if (_movables->getTileGIDAt(tileCoordWalk)){
            if (!passTilePropertyCheck(_levelMap, _movables, tileCoordWalk, "push") || _hud->getActionPressed()) {
                collide(player, direction);
                return;
            }
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
        if (passTilePropertyCheck(_levelMap, _movables, tileCoordItem, "push")) {
            if(!_hud->getActionPressed()
               && (failBoundsCheck(_levelMap, tileCoordItemTarget)
                   || passTilePropertyCheck(_levelMap, _meta, tileCoordItemTarget, "collidable")
                   || _movables->getTileGIDAt(tileCoordItemTarget))){
                   
                   // item pushed collides
                   collide(player, direction);
                   return;
               } else{
                   FiniteTimeAction* actionPushed = MoveBy::create(MOVE_TIME-0.1, moveVector/2);
                   FiniteTimeAction* actionPushDone = CallFunc::create(CC_CALLBACK_0(GameScene::tileMoveFinished, this, _movables, tileCoordItem, tileCoordItemTarget));
                   Sequence* pushSeq = Sequence::create(actionPushed, actionPushDone, NULL);
                   _movables->getTileAt(tileCoordItem)->runAction(pushSeq);
               }
        }
        
        // Create player movement action
        FiniteTimeAction* actionMove = MoveBy::create(MOVE_TIME, moveVector);
        FiniteTimeAction* actionMoveScreen = MoveBy::create(MOVE_TIME, -moveVector);
        FiniteTimeAction* actionMoveDone = CallFunc::create(CC_CALLBACK_0(GameScene::playerMoveFinished, this, player));
        Sequence* actionSeq = Sequence::create(actionMove, actionMoveDone, NULL);
        
        // move player, move camera & HUD if it's controlled player
        if(player == _controlledPlayer){
            _gameLayer->runAction(actionMoveScreen);
        }
        player->runAction(actionSeq);
        player->animateMove(direction);
        player->isMoving = true;
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

void GameScene::collide(Player* player, int direction){
    Point moveVector = moveVectorFromDirection(direction);
    player->isMoving = true;
    FiniteTimeAction* actionMove = MoveBy::create((MOVE_TIME-0.1)/2, moveVector/4);
    FiniteTimeAction* actionMove2 = MoveBy::create((MOVE_TIME-0.1)/2, -moveVector/4);
    FiniteTimeAction* actionMoveDone = CallFunc::create(CC_CALLBACK_0(GameScene::playerMoveFinished, this, player));
    Sequence* actionSeq = Sequence::create(actionMove, actionMove2, actionMoveDone, NULL);
    player->runAction(actionSeq);
}

bool GameScene::passTilePropertyCheck(cocos2d::TMXTiledMap *map ,cocos2d::TMXLayer *layer, cocos2d::Point coord, const char *property){
    int tileGID = layer->getTileGIDAt(coord);
    if (tileGID) {
        auto properties = map->getPropertiesForGID(tileGID);
        if (properties) {
            auto val = properties->valueForKey(property);
            if (val && val->compare("true") == 0){
                return true;
            }
        }
    }
    return false;
}

bool GameScene::failBoundsCheck(cocos2d::TMXTiledMap *map, cocos2d::Point coord){
    if(coord.x >= map->getMapSize().width || coord.y >= map->getMapSize().height || coord.x < 0 || coord.y < 0){
        return true;
    }
    return false;
}

void GameScene::playerMoveFinished(Player* player){
    player->isMoving = false;
}

void GameScene::tileMoveFinished(cocos2d::TMXLayer *layer, cocos2d::Point fromCoord, cocos2d::Point toCoord){
    int tileGID = layer->getTileGIDAt(fromCoord);
    layer->setTileGID(tileGID, toCoord);
    layer->removeTileAt(fromCoord);
}

void GameScene::correctPlayerState(Player* player, float xpos, float ypos){
    player->setPosition(Point(xpos, ypos));
    player->isMoving = false;
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
    
    std::size_t locd = chatevent.chat.find('d');
    std::size_t locx = chatevent.chat.find('x');
    std::size_t locy = chatevent.chat.find('y');
    std::string str1 = chatevent.chat.substr(0,locd);
    std::string str2 = chatevent.chat.substr(locd+1,locx);
    std::string str3 = chatevent.chat.substr(locx+1,locy);
    std::string str4 = chatevent.chat.substr(locy+1,chatevent.chat.length());
    int action = std::atoi(str1.c_str());
    int direction = std::atoi(str2.c_str());
    float xpos = std::atoi(str3.c_str());
    float ypos = std::atoi(str4.c_str());
    printf("chat is : action:%d,direction:%d,x:%f,y:%f\n",action,direction,xpos,ypos);
    if (_otherPlayer->isMoving) {
        // Lag. Correct location of other player first.
        this->correctPlayerState(player, xpos,ypos);
    }
    this->processPlayerMove(player, action, direction);
}

void GameScene::onUserPaused(std::string user,std::string locId,bool isLobby)
{
    //    printf("\nonUserPaused...user=%s",user.c_str());
    //    printf("\nonUserPaused...locId=%s",locId.c_str());
    std::string message = "Other user connection lost";
    this->unschedule( schedule_selector(GameScene::updateGame));
    showReconnectingLayer(message);
}

void GameScene::onUserResumed(std::string user,std::string locId,bool isLobby)
{
    //    printf("\nonUserResumed...user=%s",user.c_str());
    //    printf("\nonUserResumed...locId=%s",locId.c_str());
    removeStartGameLayer();
    this->schedule( schedule_selector(GameScene::updateGame));
}
