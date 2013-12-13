//
//  MenuScene.cpp
//  TheOtherWorld
//
//  Created by Zhan Huang on 11/28/13.
//
//

#include "MenuScene.h"
#include "AppMacros.h"
#include "GameScene.h"

USING_NS_CC;

Scene* MenuScene::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    MenuScene *layer = MenuScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    
    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(MenuScene::menuCloseCallback,this));
    
    closeItem->setPosition(origin + Point(visibleSize) - Point(closeItem->getContentSize() / 2));
    
    // create menu, it's an autorelease object
    auto closeButton = Menu::create(closeItem, NULL);
    closeButton->setPosition(Point::ZERO);
    this->addChild(closeButton, 1);
    
    /////////////////////////////
    // 3. add your codes below...
    
    // Tutorial Start Button
    auto startLabel = LabelTTF::create("Start Tutorial", "Arial", TITLE_FONT_SIZE);
    startLabel->setColor(Color3B(255, 255, 255));
    auto startItem = MenuItemLabel::create(
                                           startLabel,
                                           CC_CALLBACK_1(MenuScene::menuStartCallback, this));
    startItem->setPosition(Point(origin.x + visibleSize.width/2,
                                 origin.y + visibleSize.height/2 +
                                 startLabel->getContentSize().height));
    
    auto connectLabel = LabelTTF::create("Start Game", "Arial", TITLE_FONT_SIZE);
    connectLabel->setColor(Color3B(255, 255, 255));
    auto connectItem = MenuItemLabel::create(
                                           connectLabel,
                                           CC_CALLBACK_1(MenuScene::menuConnectCallback, this));
    connectItem->setPosition(Point(origin.x + visibleSize.width/2,
                                 origin.y + visibleSize.height/2 -
                                 startLabel->getContentSize().height));
    
    // position the label on the center of the screen
    auto startMenu = Menu::create(startItem, connectItem, NULL);
    startMenu->setPosition(Point::ZERO);
    
    // add the label as a child to this layer
    this->addChild(startMenu, 1);
    
    return true;
}

void MenuScene::menuCloseCallback(Object* sender)
{
    Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void MenuScene::menuStartCallback(Object* sender)
{
    auto newGameScene = GameScene::create();
    Director::getInstance()->replaceScene(newGameScene);
}


void MenuScene::menuConnectCallback(Object* sender)
{
    auto newGameScene = GameScene::create();
    Director::getInstance()->replaceScene(newGameScene);
}
