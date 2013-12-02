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
        this->addChild(_levelMap, -1);
        
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

void GameScene::updateGame(float dt)
{
    if (!_playerIsMoving) {
        int direction = _dpad->getDirection();
        log("direction:%d", direction);
        
        if (direction != 0) {
            auto moveVector = Point::ZERO;
            switch (direction) {
                case 1:
                    moveVector = Point(0,32);
                    break;
                case 2:
                    moveVector = Point(0,-32);
                    break;
                case 3:
                    moveVector = Point(-32,0);
                    break;
                case 4:
                    moveVector = Point(32,0);
                    break;
                default:
                    break;
            }
            // Create the action
            FiniteTimeAction* actionMove = MoveBy::create(0.5, moveVector);
            FiniteTimeAction* actionMoveDone = CallFunc::create(CC_CALLBACK_0(GameScene::playerMoveFinished, this));
            _player1->runAction( Sequence::create(actionMove, actionMoveDone, NULL) );
            _playerIsMoving = true;
        }
    }
}

void GameScene::playerMoveFinished(){
    _playerIsMoving = false;
}

