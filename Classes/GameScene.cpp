//
//  GameScene.cpp
//  TheOtherWorld
//
//  Created by Zhan Huang on 11/30/13.
//
//

#include "GameScene.h"
#include "GameOverScene.h"
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
        
        _background = _levelMap->getLayer("background");
        _meta = _levelMap->getLayer("Meta");
        _meta->setVisible(false);
        _gameLayer->addChild(_levelMap, -1);
        
        TILE_SIZE = _levelMap->getTileSize().width;
        
        // add player
        TMXObjectGroup *mapObjectGroup = _levelMap->getObjectGroup("Objects");
        Dictionary *spawnPoint = mapObjectGroup->getObject("SpawnPoint");
        int x = spawnPoint->valueForKey("x")->intValue();
        int y = spawnPoint->valueForKey("y")->intValue();
        
        _player1 = Sprite::create("Player.png");
        _player1->setPosition(Point(x,y));
        _player1->setScale(2.0);
        _playerIsMoving = false;
        _gameLayer->addChild(_player1);
        
        // add HUD
        _hud = HUD::create();
//        _hud->setScale(2.0);
        this->addChild(_hud, 1);
        
        this->alignViewPosition(Point(x,y));
        
        // add objects
        Array *allObjects = mapObjectGroup->getObjects();
        for(int i = 0; i < allObjects->count(); i++){
            Dictionary *nextObject = (Dictionary *)allObjects->getObjectAtIndex(i);
            if (nextObject->valueForKey("name")->compare("Block") == 0){
                // TODO: make block
            }
        }
        
//        // add touch listener
//        auto dispatcher = Director::getInstance()->getEventDispatcher();
//        auto listener = EventListenerTouchAllAtOnce::create();
//        listener->onTouchesEnded = CC_CALLBACK_2(GameScene::onTouchesEnded, this);
//        dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        
		// use updateGame instead of update, otherwise it will conflict with SelectorProtocol::update
		this->schedule( schedule_selector(GameScene::updateGame));
        
        
		// CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("background-music.wav", true);
        
		bRet = true;
	} while (0);
    
    // If hits any of the CC_BREAK_IF() s, bRet is false.
	return bRet;
}

void GameScene::alignViewPosition(Point playerPosition){
    auto visibleSize = Director::getInstance()->getVisibleSize();
    _gameLayer->setPosition(Point(visibleSize.width/2 - playerPosition.x, visibleSize.height/2 - playerPosition.y));
}

// cpp with cocos2d-x
void GameScene::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
	// Choose one of the touches to work with
	Touch* touch = touches[0];
	Point location = touch->getLocation();
    auto tileCoord = tileCoordForPosition(location);
    log("coord touched: %f, %f", tileCoord.x, tileCoord.y);
    FiniteTimeAction* tintRed = TintTo::create(0, 255, 0, 0);
    _background->getTileAt(tileCoord)->runAction(tintRed);
    
	// CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("effect.wav");
}

Point GameScene::tileCoordForPosition(Point position) {
    int x = position.x/TILE_SIZE - 0.5;
    int y = _levelMap->getMapSize().height - position.y/TILE_SIZE - 0.5;
    return Point(x,y);
}

void GameScene::updateGame(float dt)
{
    if (!_playerIsMoving) {
        int direction = _hud->getDirection();
        
        if (direction != 0) {
            auto moveVector = Point::ZERO;
            switch (direction) {
                case 1:
                    moveVector = Point(0,TILE_SIZE);
                    break;
                case 2:
                    moveVector = Point(0,-TILE_SIZE);
                    break;
                case 3:
                    moveVector = Point(-TILE_SIZE,0);
                    break;
                case 4:
                    moveVector = Point(TILE_SIZE,0);
                    break;
                default:
                    break;
            }
            auto moveTarget = _player1->getPosition() + moveVector;
            auto tileCoord = tileCoordForPosition(moveTarget);
            
            // Check for out of border
            if(tileCoord.x >= _levelMap->getMapSize().width || tileCoord.y >= _levelMap->getMapSize().height || tileCoord.x < 0 || tileCoord.y < 0){
                collide(moveVector);
                return;
            }
            
            // detect collision & alter action if collided
            log("coord: %f, %f", tileCoord.x, tileCoord.y);
            int tileGID = _meta->getTileGIDAt(tileCoord);
            if (tileGID) {
                auto properties = _levelMap->getPropertiesForGID(tileGID);
                if (properties) {
                    auto collision = properties->valueForKey("collidable");
                    if (collision && collision->compare("true") == 0){
                        collide(moveVector);
                        return;
                    }
                }
            }
            
            // Create the action
            FiniteTimeAction* actionMove = MoveBy::create(0.5, moveVector);
            FiniteTimeAction* actionMoveScreen = MoveBy::create(0.5, -moveVector);
            FiniteTimeAction* actionMoveDone = CallFunc::create(CC_CALLBACK_0(GameScene::playerMoveFinished, this));
            Sequence* actionSeq = Sequence::create(actionMove, actionMoveDone, NULL);
            
            // Run the action, adjust camera & HUD
            _gameLayer->runAction(actionMoveScreen);
            _player1->runAction(actionSeq);
            _playerIsMoving = true;
        }
    }
}

void GameScene::collide(Point moveVector){
    _playerIsMoving = true;
    FiniteTimeAction* actionMove =MoveBy::create(0.2, moveVector/4);
    FiniteTimeAction* actionMove2 =MoveBy::create(0.2, -moveVector/4);
    FiniteTimeAction* actionMoveDone = CallFunc::create(CC_CALLBACK_0(GameScene::playerMoveFinished, this));
    Sequence* actionSeq = Sequence::create(actionMove, actionMove2, actionMoveDone, NULL);
    _player1->runAction(actionSeq);
}

void GameScene::playerMoveFinished(){
    _playerIsMoving = false;
}

