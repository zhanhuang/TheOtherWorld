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
        _meta = _levelMap->getLayer("Meta");
        _movables = _levelMap->getLayer("Movables");
        _meta->setVisible(false);
        _gameLayer->addChild(_levelMap, -2);
        
        // add overlay tilemap for trees
        _levelOverlay = TMXTiledMap::create("levelOverlay.tmx");
        _levelOverlay->setScale(2.0);
        _gameLayer->addChild(_levelOverlay, 1);
        
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
        this->addChild(_hud, 2);
        
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
            auto tileCoordWalk = tileCoordForPosition(moveTarget);
            
            // Check for out of border and collision
            if(failBoundsCheck(_levelMap, tileCoordWalk) || passTilePropertyCheck(_levelMap, _meta, tileCoordWalk, "collidable")){
                collide(moveVector);
                return;
            }
            if (_movables->getTileGIDAt(tileCoordWalk)){
                if (!passTilePropertyCheck(_levelMap, _movables, tileCoordWalk, "push") || _hud->getActionPressed()) {
                    collide(moveVector);
                    return;
                }
            }
            
            
            // handle pushing / pulling object
            Point itemTarget;
            Point tileCoordItem;
            Point tileCoordItemTarget;
            if (_hud->getActionPressed()) {
                // pull
                itemTarget = _player1->getPosition() - moveVector;
            } else{
                // push
                itemTarget = _player1->getPosition() + moveVector;
            }
            tileCoordItem = tileCoordForPosition(itemTarget);
            tileCoordItemTarget = tileCoordForPosition(itemTarget + moveVector);
            
            if (passTilePropertyCheck(_levelMap, _movables, tileCoordItem, "push")) {
                if(!_hud->getActionPressed()
                   && (failBoundsCheck(_levelMap, tileCoordItemTarget)
                    || passTilePropertyCheck(_levelMap, _meta, tileCoordItemTarget, "collidable")
                    || _movables->getTileGIDAt(tileCoordItemTarget))){
                    
                    // item pushed collides
                    collide(moveVector);
                    return;
                } else{
                    FiniteTimeAction* actionPushed = MoveBy::create(0.5, moveVector/2);
                    FiniteTimeAction* actionPushDone = CallFunc::create(CC_CALLBACK_0(GameScene::tileMoveFinished, this, _movables, tileCoordItem, tileCoordItemTarget));
                    Sequence* pushSeq = Sequence::create(actionPushed, actionPushDone, NULL);
                    _movables->getTileAt(tileCoordItem)->runAction(pushSeq);
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

void GameScene::playerMoveFinished(){
    _playerIsMoving = false;
}

void GameScene::tileMoveFinished(cocos2d::TMXLayer *layer, cocos2d::Point fromCoord, cocos2d::Point toCoord){
    int tileGID = layer->getTileGIDAt(fromCoord);
    layer->setTileGID(tileGID, toCoord);
    layer->removeTileAt(fromCoord);
}

