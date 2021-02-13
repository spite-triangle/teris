#ifndef __TERIS_H__
#define __TERIS_H__
#include <Windows.h>
#include "SFML/Graphics.hpp"
#include <iostream>
#include <random>
#include <vector>
#include <ctime>
using namespace sf;
using namespace std;

struct configures
{   
    // 屏幕分辨率 缩放 
    float windowScale;
    // 屏幕分辨率
    float windowX;
    // 屏幕分辨率
    float windowY;

    // 方块显示区域
    float fieldBricksScale = 0.65;
    // 砖块的大小
    int brickSize = 512;

    configures(){}

    configures(float wScale){
        windowX = 997 * wScale;
        windowY = 1050 * wScale;
        windowScale = wScale;
    }

};

struct keybroadControl
{   
    // 选中图形
    bool isRote = false;
    // 移动图形
    int move = 0;
    // 下降延迟
    float delay = 0.5;
};

struct mColor{
    int r;
    int g;
    int b;

    mColor(){}

    mColor(int red,int green,int blue){
        r = red;
        g = green;
        b = blue;
    }
};

struct brickPoints
{
    int x = 0;
    int y = 0;
};



class Teris
{
private:
    mColor* colors;

    // 当前状态
    enum STATES{
        STATE_BEGAIN,
        STATE_ACCEL,
        STATE_OVER,
        STATE_RUN,
        STATE_STOP
    } currentState; 

    // 上分加速
    float dropDelay;

    // 记分
    int scores = 0;
    // 当前的形状
    int currentShap;

    // 配置信息
    configures config;
    // 控制器
    keybroadControl controller;

    brickPoints shapePoint[4];
    brickPoints cachePoint[4];

    // 计时器
    float timer = 0;
    float bottomTimer = 0;
    float secondTimer = 0;
    float accelTimer = 0;

    // 方块的缩放
    float brickScale;
    // 方块区的行
    int numberX = 10;
    // 方块区的列
    int numberY = 20;
    // 方块显示区域
    int bricksBlock[20][10] = {0};
    // 所有方块的图形
    int brickShapes[7][4] = {
        1,3,5,7, // I
        0,2,3,5, // z
        1,3,2,4, // S
        1,3,2,5, // T
        0,1,3,5, // L
        1,3,5,4, // J
        0,1,2,3, // 0
    };

    // 汉字
    vector<wstring> chinese;
    sf::Text scoreNumberText;
    sf::Text timeNumberText;
    
    /* 
     * 描述: 检测键盘
     */
    void eventManager(RenderWindow &window,Event &e);

    /* 
     * 描述: 边缘检测；已经占用的位置检测
     */
    bool gamePoll();

    /* 
     * 描述: 设置形状
     */
    void setShape();

    /* 
     * 描述: 移动
     */
    void move();

    /* 
     * 描述: 旋转
     */
    void rotate();

    /* 
     * 描述: 下落;
     */
    void drop();

    /* 
     * 描述: 是否结束游戏；是否生成新的图形
     */
    void rounte();

    /* 
     * 描述: 检测是否可以消除行
     */
    void removeLines();

    /* 
     * 描述: 恢复控制器
     */
    void resetController();

    /* 
     * 描述: 重启游戏
     */
    void restartGame();

    /* 
     * 描述: 加载字体
     */
    sf::Font loadFont(const char * fontPath);

    /* 
     * 描述: 生成文本
     */
    sf::Text getText(const std::wstring &str,const sf::Font & font,sf::Color color,int fontSize,Vector2f position);

    /* 
     * 描述: 转换坐标
     */
    Vector2f convertV2f(int x,int y);

    /* 
     * 描述: 将分数转为汉字
     */
    std::wstring convertScore();

    /* 
     * 描述: 将时间转字符串
     */
    std::wstring convertTime();

    /* 
     * 描述: 将时间转字符串
     */
    std::wstring convertTime(int number);

public:
    Teris(configures config);
    ~Teris();

    void run();
};


#endif // __TERIS_H__