#include "Teris.hpp"


Teris::Teris(configures config) 
{
    this->config = config;
    this->brickScale = (config.windowY - 10)/numberY/config.brickSize;

    // 颜色
    colors = new mColor[7];
    colors[0] = mColor(85,88,105);
    colors[1] = mColor(162,192,228);
    colors[2] = mColor(247,186,43);
    colors[3] = mColor(51,77,157);
    colors[4] = mColor(142,207,101);
    colors[5] = mColor(219,50,50);
    colors[6] = mColor(151,110,191);
    
    chinese.push_back(L"零");
    chinese.push_back(L"壹");
    chinese.push_back(L"貳");
    chinese.push_back(L"三");
    chinese.push_back(L"肆");
    chinese.push_back(L"伍");
    chinese.push_back(L"陸");
    chinese.push_back(L"七");
    chinese.push_back(L"捌");
    chinese.push_back(L"玖");

    this->scores = 0;
    this->dropDelay = 1;


    this->currentState = STATE_BEGAIN;
}

Teris::~Teris() 
{
    if (colors)
    {
        delete [] colors;
    }
    
}

void Teris::run() 
{
    // 生成窗口
    RenderWindow window(VideoMode(config.windowX,config.windowY), "Teris",sf::Style::None);
    
    sf::Image icon;
    icon.loadFromFile("image/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // 限制帧率
    window.setFramerateLimit(60);
    // 垂直同步
    // window.setVerticalSyncEnabled(true);

    // 自身窗口句柄
    HWND hWnd = FindWindow(NULL,"Teris");

    // 透明背景
    LONG t = GetWindowLong(hWnd, GWL_EXSTYLE);
    t |= WS_EX_LAYERED;
    SetWindowLong(hWnd, GWL_EXSTYLE, t);  
    SetLayeredWindowAttributes(hWnd,RGB(228,244,243), 0, LWA_COLORKEY);

    // 窗口置底
    SetWindowPos(hWnd,HWND_BOTTOM,0,0,0,0,SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
    // 保存底部的句柄，用于之后的判断
    HWND lastHWND = GetNextWindow(hWnd,GW_HWNDNEXT);

    // 时钟
    Clock clock;

    // 砖块贴图
    Texture brickTexture;
    brickTexture.loadFromFile("image/brick.png");
    Sprite brick(brickTexture);
    brick.setScale(Vector2f(brickScale,brickScale));

    // 背景贴图
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("image/background.png");
    Sprite background(backgroundTexture);
    background.setScale(Vector2f(config.windowScale,config.windowScale));
    background.setPosition(0,590*config.windowScale);

    // 结束贴图
    Texture overTexture;
    overTexture.loadFromFile("image/over.png");
    Sprite overTex(overTexture);
    overTex.setScale(Vector2f(config.windowScale * 0.5,config.windowScale * 0.5));
    overTex.setPosition(convertV2f(4,269));

    // 出击贴图
    Texture begainTexture;
    begainTexture.loadFromFile("image/begain.png");
    Sprite begainTex(begainTexture);
    begainTex.setScale(Vector2f(config.windowScale * 0.5,config.windowScale * 0.5));
    begainTex.setPosition(convertV2f(4,269));

    // 暴走贴图
    Texture accelTexture;
    accelTexture.loadFromFile("image/accelerate.png");
    Sprite accelTex(accelTexture);
    accelTex.setScale(Vector2f(config.windowScale * 0.5,config.windowScale * 0.5));
    accelTex.setPosition(convertV2f(4,269));
    
    Texture logBackTexture;
    logBackTexture.loadFromFile("image/logBackground.jpg");
    
    Sprite logBack(logBackTexture);
    logBack.setColor(Color(100,0,16,200));
    logBack.setScale(Vector2f(config.windowScale * 0.5,config.windowScale * 0.5));
    logBack.setPosition(convertV2f(4,269));

    time_t now = time(0);
    tm *ltm = localtime(&now);

    // 加载字体
    sf::Font mfont = loadFont("fonts/FOT-MatissePro-B.otf");
    // 初始化文本
    sf::Text scoreText = getText(L"分\n數",mfont,sf::Color::White,55*config.windowScale,convertV2f(557,605));
    sf::Text dateText = getText(L"日\n期",mfont,sf::Color::White,55*config.windowScale,convertV2f(557,605));
    
    string mon = to_string(ltm->tm_mon + 1);
    string mday = to_string(ltm->tm_mday);

    if(ltm->tm_mon  < 9){
        mon = "0" + mon;
    }

    if (ltm->tm_mday < 10)
    {
        mday = "0" + to_string(ltm->tm_mday);            
    }
    string str = to_string(ltm->tm_year + 1900) + "/" + mon + "/" + mday;
    
    sf::Text dateNumberText = getText(L"",mfont,sf::Color::Black,48*config.windowScale,convertV2f(675,642));
    dateNumberText.setString(str);
    dateNumberText.setScale(Vector2f(1,2));

    scoreNumberText = getText(L"零.零.零",mfont,sf::Color::Black,60*config.windowScale,convertV2f(702,653));
    scoreNumberText.setScale(Vector2f(1,1.4));

    sf::Text timeText = getText(L"時\n間",mfont,sf::Color::White,55*config.windowScale,convertV2f(557,845));
    timeNumberText = getText(L"零零：零零：零零",mfont,sf::Color::Black,40*config.windowScale,convertV2f(662,885));
    timeNumberText.setScale(Vector2f(1,2));

    // 初始化图形
    srand(now);
    this->setShape();

    while (window.isOpen())
    {
        // 计时器
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        
        bottomTimer = bottomTimer + time;
        secondTimer = secondTimer + time;

        // 暴走 显示时间
        if (currentState == STATE_ACCEL)
        {
            accelTimer = accelTimer + time;
            if (accelTimer > 1)
            {
                accelTimer = 0;
                currentState = STATE_RUN;
            }
        }
        
        // 修改时间
        if(secondTimer > 0.8){
            secondTimer = 0;
            timeNumberText.setString(convertTime());
        }

        // 将窗口置底
        if(bottomTimer > 1){
            HWND temp = GetNextWindow(hWnd,GW_HWNDNEXT);
            if(lastHWND && lastHWND != temp){
                SetWindowPos(hWnd,HWND_BOTTOM,0,0,0,0,SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
            }
            bottomTimer = 0;
        }

        // 事件
        Event e;

        eventManager(window,e);

        if (currentState == STATE_RUN)
        {
            timer = timer + time;
            // 左右移动
            move();
            // 旋转
            rotate();
            if (timer > controller.delay * dropDelay){
                // 下落
                drop();
                timer = 0;
                // 是否结束游戏；是否生成新的图形
                rounte();
            }
            removeLines();
            // 控制器复原
            resetController();
        }
        
        //显示
        window.clear(Color(228,244,243));
        window.draw(background);

        // 文字显示
        if (currentState != STATE_RUN)
        {
            window.draw(dateText);
            window.draw(dateNumberText);
        }else{
            window.draw(scoreText);
            window.draw(scoreNumberText);
        }
        window.draw(timeText);
        window.draw(timeNumberText);

        // 绘制保留的图形
        for (size_t y = 0; y < numberY; y++)
        {
            for (size_t x = 0; x < numberX; x++)
            {
                if (bricksBlock[y][x] == 0)
                {
                    continue;
                }
            brick.setPosition(x * config.brickSize * brickScale,
                                y * config.brickSize * brickScale);
            mColor temp = colors[bricksBlock[y][x] - 1];
            brick.setColor(Color(temp.r,temp.g,temp.b));
            window.draw(brick);
            }
            
        }

        // 绘制移动的图形
        for (size_t i = 0; i < 4; i++)
        {
            brick.setPosition(shapePoint[i].x * config.brickSize * brickScale,
                                shapePoint[i].y * config.brickSize * brickScale);
            mColor temp = colors[currentShap];
            brick.setColor(Color(temp.r,temp.g,temp.b));
            window.draw(brick);
        }

        switch (currentState)
        {
        case STATE_OVER:
            window.draw(logBack);
            window.draw(overTex);            
            break;
        case STATE_BEGAIN:
            window.draw(logBack);
            window.draw(begainTex);            
            break;
        case STATE_ACCEL:
            window.draw(logBack);
            window.draw(accelTex);
            break;
        }
        
        window.display();
    }

}


void Teris::eventManager(RenderWindow &window, Event &e) 
{
     while (window.pollEvent(e)){
            if (e.type == Event::Closed || e.key.code == Keyboard::Escape){
                window.close();
            }
            
            if(e.type == Event::KeyPressed){

                // 暂停游戏;重启游戏
                if (e.key.code == Keyboard::Space)
                {
                    switch (currentState)
                    {
                    case STATE_STOP:
                        currentState = STATE_RUN;
                        break;
                    case STATE_OVER:
                        restartGame();
                        break;
                    case STATE_RUN:
                        currentState = STATE_STOP;
                        break;

                    case STATE_BEGAIN:
                        currentState = STATE_RUN;                        
                        break;
                    }
                }
                
                // 切换移动窗口和移动形状的指令
                if (currentState == STATE_STOP || currentState == STATE_BEGAIN)
                {
                    int dx = 0;
                    int dy = 0;
                    if (e.key.code == Keyboard::Up)
                    {
                       dy = -1;
                    }
                    if (e.key.code == Keyboard::Left)
                    {
                        dx = -1;
                    }
                    if (e.key.code == Keyboard::Right)
                    {
                        dx = 1;
                    }
                    if (e.key.code == Keyboard::Down)
                    {
                        dy = 1;
                    }

                    window.setPosition(Vector2i(window.getPosition().x + dx*10,
                                        window.getPosition().y + dy*10));
                }else{

                    if (e.key.code == Keyboard::Up)
                    {
                        // 方形不用旋转
                        if (currentShap != 6)
                        {
                            controller.isRote = true;
                        }
                        
                    }
                    if (e.key.code == Keyboard::Left)
                    {
                        controller.move = -1;
                    }
                    if (e.key.code == Keyboard::Right)
                    {
                        controller.move = 1;
                    }
                    if (e.key.code == Keyboard::Down)
                    {
                        controller.delay = 0.03;
                    }
   
                }
            }
    }
}

bool Teris::gamePoll() 
{
    for (size_t i = 0; i < 4; i++)
    {
        if (shapePoint[i].x >= numberX || shapePoint[i].x < 0 
                || shapePoint[i].y >= numberY || shapePoint[i].y < 0)
        {
            return true;

        }else if(bricksBlock[shapePoint[i].y][shapePoint[i].x] > 0){
            return true;
        }
    }
    return false;
}

void Teris::setShape() 
{
    // 生成图形
    currentShap = rand() % 7;

    // 进行形状初始化
    for (size_t i = 0; i < 4; i++)
    {
        // 图形由两列组成。% 确实当前方块在左 还是右
        shapePoint[i].x = brickShapes[currentShap][i] % 2 + numberX / 2;
        // 确实当前方块的  竖向位置
        shapePoint[i].y = brickShapes[currentShap][i] / 2;
    }
}

void Teris::move() 
{
    // 左右移动
    for (size_t i = 0; i < 4; i++)
    {
        cachePoint[i] = shapePoint[i];
        shapePoint[i].x = shapePoint[i].x + controller.move;
    }

    // 移动检测
    if (gamePoll())
    {
        for (size_t i = 0; i < 4; i++)
        {
            shapePoint[i] = cachePoint[i];
        }
    }
}

void Teris::rotate() 
{
     // 旋转：以中心点建立局部坐标，然后 x，y坐标反向（转了90）
    if (controller.isRote)
    {
        brickPoints center;
        center = shapePoint[1];
        for (size_t i = 0; i < 4; i++)
        {
            cachePoint[i] = shapePoint[i];
            int x = shapePoint[i].y - center.y;
            int y = shapePoint[i].x - center.x;
            shapePoint[i].x = center.x - x;
            shapePoint[i].y = center.y + y;
        }

        // 旋转检测
        if (gamePoll())
        {
            for (size_t i = 0; i < 4; i++)
            {
                shapePoint[i] = cachePoint[i];
            }
        }
    }
}

void Teris::drop() 
{
    // 下落
    for (size_t i = 0; i < 4; i++)
    {
        cachePoint[i] = shapePoint[i];
        shapePoint[i].y = shapePoint[i].y + 1;
    }
}

void Teris::rounte() 
{
    // y到底，就开始下一个图形的生成
    if (gamePoll())
    {
        for (size_t i = 0; i < 4; i++)
        {
            // 将图形位置保存
            bricksBlock[cachePoint[i].y][cachePoint[i].x] = currentShap + 1; 
        }

        // 生成形状

        setShape();

        // 结束
        if(gamePoll()){
            currentState = STATE_OVER;
        }
    }
}

void Teris::removeLines() 
{
    // 消除行: 利用 k 与 y 递减快慢不同，更新 场景中的方块
    int k = numberY - 1;
    for (size_t y = k; y > 0; y--)
    {
        int count = 0;
        for (size_t x = 0; x < numberX; x++)
        {
            if (bricksBlock[y][x] > 0)
            {
                count ++ ;
            }
            bricksBlock[k][x] = bricksBlock[y][x];
        }
        if (count < numberX)
        {
            k --;
        }else{
            // 记录得分
            this->scores = this->scores + 1;
            scoreNumberText.setString(convertScore());
            
            if(this->scores > 9){
                // 加速
                 this->dropDelay = max(0.16,exp(-this->scores* 0.03));
                if (this->scores == 10)
                {
                    this->currentState = STATE_ACCEL;
                }
            }
        }
    }
}

void Teris::resetController() 
{
    // 将控制器复原
    controller.isRote = false;
    controller.move = 0;
    controller.delay = 0.5; 
}

void Teris::restartGame() 
{
    // 分归零
    this->scores = 0;
    // 场景清空
    memset(this->bricksBlock,0,sizeof(this->bricksBlock));
    // 恢复
    currentState = STATE_BEGAIN;
}

sf::Font Teris::loadFont(const char * fontPath) 
{
    sf::Font font;
    font.loadFromFile(fontPath);
    return font;
}

sf::Text Teris::getText(const std::wstring & str,const sf::Font & font,sf::Color color,int fontSize,Vector2f position) 
{
    sf::Text text;
    text.setString(str); //设置字符串
    text.setFont(font); //设置字体
    text.setCharacterSize(fontSize); //文字大小
    text.setFillColor(color); //颜色
    text.setPosition(position);
    // text.setStyle(sf::Text::Bold | sf::Text::Underlined | sf::Text::Italic | sf::Text::StrikeThrough);
    //属性
    return text;
}

Vector2f Teris::convertV2f(int x,int y) 
{
    return Vector2f(x*config.windowScale,y*config.windowScale);
}

std::wstring Teris::convertScore() 
{
    int unit = this->scores % 10;
    int decade = (this->scores % 100  - unit) / 10;
    int hundred = (this->scores - decade * 10  - unit) / 100;
    wstring str = chinese[hundred] + "." + chinese[decade] + "." + chinese[unit];
    return str;

}

std::wstring Teris::convertTime() 
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    
    wstring str = convertTime(ltm->tm_hour) + L"：" + convertTime(ltm->tm_min) + L"：" + convertTime(ltm->tm_sec);
    return str;
}

std::wstring Teris::convertTime(int number) 
{
    int unit = number % 10;
    int decade = (number  - unit) / 10;

    wstring str =  chinese[decade] + chinese[unit];
    return str;
}








