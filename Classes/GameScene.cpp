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

Scene* GameScene::scene()
{
	Scene * scene = NULL;
	do
	{
		// 'scene' is an autorelease object
		scene = Scene::create();
		CC_BREAK_IF(! scene);
        
		// 'layer' is an autorelease object
		GameScene *layer = GameScene::create();
		CC_BREAK_IF(! layer);
        
		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);
    
	// return the scene
	return scene;
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
        
		CC_BREAK_IF(! Layer::init());
        
        // add tiled map as background
        _levelMap = TMXTiledMap::create("testLevel.tmx");
        _levelMap->setScale(2.00);
        _background = _levelMap->getLayer("Background");
        _meta = _levelMap->getLayer("Meta");
        _meta->setVisible(false);
        this->addChild(_levelMap, -1);
        
        TILE_SIZE = _levelMap->getTileSize().width;
        
        // add player
        TMXObjectGroup *mapObjectGroup = _levelMap->getObjectGroup("Objects");
        Dictionary *spawnPoint = mapObjectGroup->getObject("Spawn");
        int x = spawnPoint->valueForKey("x")->intValue();
        int y = spawnPoint->valueForKey("y")->intValue();
        
        _player1 = Sprite::create("Player.png");
        _player1->setPosition(Point(x,y));
        this->addChild(_player1);
        
        _playerIsMoving = false;
        
        // add dpad
        _dpad = DPad::create();
        _dpad->setPosition(Point::ZERO);
        this->addChild(_dpad, 1);
        
        
        // add touch listener
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        auto listener = EventListenerTouchAllAtOnce::create();
        listener->onTouchesEnded = CC_CALLBACK_2(GameScene::onTouchesEnded, this);
        dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        
		// use updateGame instead of update, otherwise it will conflict with SelectorProtocol::update
		this->schedule( schedule_selector(GameScene::updateGame));
        
        
		// CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("background-music.wav", true);
        
		bRet = true;
	} while (0);
    
    // If hits any of the CC_BREAK_IF() s, bRet is false.
	return bRet;
}

// cpp with cocos2d-x
void GameScene::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
//	// Choose one of the touches to work with
//	Touch* touch = touches[0];
//	Point location = touch->getLocation();
//    
//	log("++++++++after  x:%f, y:%f", location.x, location.y);
    
	// CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("effect.wav");
}

Point GameScene::tileCoordForPosition(Point position) {
    int x = position.x/TILE_SIZE;
    int y = _levelMap->getMapSize().height - position.y/TILE_SIZE;
    return Point(x,y);
}

void GameScene::updateGame(float dt)
{
    if (!_playerIsMoving) {
        int direction = _dpad->getDirection();
        
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
            
            // Create the action
            FiniteTimeAction* actionMove = MoveTo::create(0.5, moveTarget);
            FiniteTimeAction* actionMoveDone = CallFunc::create(CC_CALLBACK_0(GameScene::playerMoveFinished, this));
            Sequence* actionSeq = Sequence::create(actionMove, actionMoveDone, NULL);
            
            // detect collision & alter action if collided
            auto tileCoord = tileCoordForPosition(moveTarget);
            log("coord: %f, %f", tileCoord.x, tileCoord.y);
            int tileGID = _meta->getTileGIDAt(tileCoord);
            if (tileGID) {
                auto properties = _levelMap->getPropertiesForGID(tileGID);
                if (properties) {
                    auto collision = properties->valueForKey("Collidable");
                    if (collision && collision->compare("True") == 0){
                        actionMove->release();
                        actionSeq->release();
                        actionMove =MoveTo::create(0.2, _player1->getPosition() + moveVector/4);
                        FiniteTimeAction* actionMove2 =MoveTo::create(0.2, _player1->getPosition());
                        actionSeq = Sequence::create(actionMove, actionMove2, actionMoveDone, NULL);
                    }
                }
            }
            _player1->runAction(actionSeq);
            _playerIsMoving = true;
        }
    }
}

void GameScene::playerMoveFinished(){
    _playerIsMoving = false;
}

