#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <queue>

using namespace std;
using namespace sf;

//Sonic

const float Right_Wall = 4800;
const float Left_Wall = 0;
const float ground = 340;
priority_queue<int> highScoreQ;

struct Player {
    Sprite sonic, spring;
    float currentframe, stand_currentframe, y, lastkey, stand_y;
    float move_x, move_y;

    FloatRect rect;
    bool onground, iswall;

    int state = 0; //default=0 ; play=1; quit=2; game running=3; 4=retry; finish=5;
    int score = 0;
    Text text;

    void declaration(Texture& sonictexture, Texture& springtexture) {
        sonic.setTexture(sonictexture);
        sonic.setScale(1, 1);
        sonic.setTextureRect(IntRect(0, 0, 64, 48));
        spring.setTexture(springtexture);
        spring.setPosition(1468, 345);
        spring.setTextureRect(IntRect(0, 0, 38, 47));
        spring.setScale(1.5, 1);


        move_x = 0;
        move_y = 0;
        currentframe = 0;
        stand_currentframe = 0;
        y = 1;
        stand_y = 1;
        lastkey = 0;
    }

    void update(float time) {
        //Movement
        rect.left += move_x * time;
        rect.top += move_y * time;
        //Gravity
        if (!onground) {
            move_y += (0.001 * time);
        }
        onground = false;

        // Wall
        if (rect.left > Right_Wall)
            rect.left = Right_Wall;
        if (rect.left < Left_Wall)
            rect.left = Left_Wall;

        // Sonic Animation
        //if (state == 3) {
            if (move_x > 0) {
                currentframe += time * 0.01;
                if (currentframe > 4) {
                    currentframe -= 4;
                    if (y < 9)
                        y++;

                }
                if (y == 0 || y == 1 || y == 2)
                    y = 6;
                lastkey = 0;
                sonic.setTextureRect(IntRect(int(currentframe) * 64, y * 48, 64, 48));

            }
            else if (move_x < 0) {
                currentframe += time * 0.01;
                if (currentframe > 4) {
                    currentframe -= 4;
                    if (y < 9)
                        y++;

                }
                if (y == 0 || y == 1 || y == 2)
                    y = 6;
                lastkey = 1;
                sonic.setTextureRect(IntRect(int(currentframe) * 64 + 55, y * 48, -64, 48));

            }
            else  if (move_x == 0) {
                y = 6;
                stand_currentframe += time * 0.003;
                if (stand_currentframe > 7) {
                    stand_currentframe -= 7;
                    if (stand_y < 2)
                        stand_y++;
                    else
                        stand_y -= 1;

                }
                if (lastkey == 0)
                    sonic.setTextureRect(IntRect(int(stand_currentframe) * 48, stand_y * 48, 48, 48));
                else
                    sonic.setTextureRect(IntRect(int(stand_currentframe) * 48 + 40, stand_y * 48, -48, 48));

            }

            
            //retry menu
            /*if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && state == 4) {
                rect.top = 0;
                rect.left = 0;
                lastkey = 0;
                score = 0;
                text.setString("Score : " + to_string(score));

            }*/

            sonic.setPosition(rect.left, rect.top);
            move_x = 0;
        //}


    }

};

Player player;

void Game_Play(RenderWindow& window);
void blocks(Sprite grounds[]);
void coins_pos(Sprite Coin[]);

RenderWindow window(sf::VideoMode(800, 480), "Sonic");
View view(FloatRect(0.f, 0.f, 800.f, 480.f));




//start menue
Sprite startMenu;
Texture startMenuTexture;

void startMenuRender() {
    if (player.state == 0) {
        startMenuTexture.loadFromFile("menuPlay.png");
        player.state = 1;
    }

    startMenu.setPosition(view.getCenter().x, 280);

    if ((Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up)) && player.state!=3) {
        player.state = 1; //play
        startMenuTexture.loadFromFile("menuPlay.png");

    }
    else if ((Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down)) && player.state != 3) {
        player.state = 2; //quit
        startMenuTexture.loadFromFile("menuQuit.png");

    }

    startMenu.setTexture(startMenuTexture);

    if ( Keyboard::isKeyPressed(Keyboard::Key::Enter) && player.state == 1 && player.state != 3) {
        startMenu.setScale(0, 0);
        player.state = 3;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && player.state == 2 && player.state != 3) {
        window.close();
    }

    startMenu.setOrigin(sf::Vector2f(startMenu.getGlobalBounds().width, startMenu.getGlobalBounds().height) / 2.f);
    window.draw(startMenu);
}

Sprite deathMenu;
Sprite Coin[45];
Texture deathMenuTexture;
bool isCollision = 0;

void deathMenuRender(){
    if (player.state == 3) {
        deathMenuTexture.loadFromFile("deadTry.png");
        player.state = 4;
        deathMenu.setScale(1, 1);
    }
    deathMenu.setPosition(view.getCenter().x, 280);

    if ((Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up)) && player.state != 3) {
        player.state = 4; //retry
        deathMenuTexture.loadFromFile("deadTry.png");
    }
    else if ((Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down)) && player.state != 3) {
        player.state = 2; //quit
        deathMenuTexture.loadFromFile("deadQuit.png");
    }

    deathMenu.setTexture(deathMenuTexture);

    if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && player.state == 4) {
        player.sonic.setPosition(10, 0);
        view.setCenter(0, 0);
        player.rect.left = 0;
        player.rect.top= 0;
        deathMenu.setScale(0, 0);
        isCollision = 0;
        player.state = 3;
        player.score = 0;
        player.text.setString("Score : " + to_string(player.score));


        player.text.setPosition(player.sonic.getPosition().x , 10);

        player.lastkey = 0;

        for (int i = 0; i < 45; i++) {
            Coin[i].setScale(0.15, 0.15);
        }
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && player.state == 2) {
        window.close();
    }

    deathMenu.setOrigin(sf::Vector2f(deathMenu.getGlobalBounds().width, deathMenu.getGlobalBounds().height) / 2.f);
    window.draw(deathMenu);
}



Sprite finishMenu;
Texture finishMenuTexture;
Text runScore;
Text HighScore;
int isFinish=0;



void finishMenuRender() {
    Font finishFont;
    finishFont.loadFromFile("scoreFont.TTF");
    highScoreQ.push(player.score);
    if (player.state == 3) {
        finishMenuTexture.loadFromFile("finishRetry.png");
        player.state = 4;
        finishMenu.setScale(1, 1);
    }


    if ((Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up)) && player.state != 3) {
        player.state = 4; //retry
        finishMenuTexture.loadFromFile("finishRetry.png");
    }
    else if ((Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down)) && player.state != 3) {
        player.state = 2; //quit
        finishMenuTexture.loadFromFile("finishQuit.png");
    }

    finishMenu.setTexture(finishMenuTexture);

    if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && player.state == 4) {
        player.sonic.setPosition(10, 0);
        view.setCenter(0, 0);
        player.rect.left = 0;
        player.rect.top = 0;
        finishMenu.setScale(0, 0);
        isFinish = 0;
        player.state = 3;
        player.score = 0;
        player.text.setString("Score : " + to_string(player.score));
        player.lastkey = 0;


        player.text.setPosition(player.sonic.getPosition().x, 10);



        for (int i = 0; i < 45; i++) {
            Coin[i].setScale(0.15, 0.15);
        }
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && player.state == 2) {
        window.close();
    }


    //cout << player.score;
    runScore.setFont(finishFont);
    runScore.setString(to_string(player.score)+" Ponits");
    runScore.setPosition(view.getCenter().x-50, 354);
    runScore.setFillColor(Color(255, 255, 255, 255));
    runScore.setCharacterSize(15);

    HighScore.setFillColor(Color(255, 255, 255, 255));
    HighScore.setCharacterSize(15);
    HighScore.setFont(finishFont);
    HighScore.setString(to_string(highScoreQ.top()) + " Points");
    HighScore.setPosition(view.getCenter().x-50, 389);

    finishMenu.setPosition(view.getCenter().x, 280);
    finishMenu.setOrigin(sf::Vector2f(finishMenu.getGlobalBounds().width, finishMenu.getGlobalBounds().height) / 2.f);
    window.draw(finishMenu);
    window.draw(HighScore);
    window.draw(runScore);
}








int main()
{
    Game_Play(window);
    return 0;
}



void Game_Play(RenderWindow& window) {

    window.setFramerateLimit(60);

    bool turn = true, ishidden = false;
    float currentframecoin = 0;
    int hide_time = 0;
    player.score = 0;

    Clock clock;
    Font font;
    font.loadFromFile("scoreFont.TTF");
 

    player.text.setFont(font);
    player.text.setString("Score : " + to_string(player.score));
    player.text.setPosition(10, 10);
    player.text.setFillColor(Color(255, 255, 255, 255));
    player.text.setCharacterSize(25);

    view.zoom(1);

    Texture background, map, blocktexture, traptexture, cointexture;
    cointexture.loadFromFile("SonicCoinsAnimation.png");

    Texture sonictexture;
    Texture springtexture;
    sonictexture.loadFromFile("SonicNewAnimation.png");
    springtexture.loadFromFile("SonicSpring.png");
    player.declaration(sonictexture, springtexture);

    background.loadFromFile("map.png");
    map.loadFromFile("map.png");
    blocktexture.loadFromFile("block.png");
    traptexture.loadFromFile("sonictrap.png");

    Sprite back, grounds[17];
    back.setTexture(background);
    for (int i = 0; i < 17; i++) {
        grounds[i].setTexture(map);
        if (i == 14 || i == 16)
            grounds[i].setTexture(blocktexture);
        if (i == 15)
            grounds[i].setTexture(traptexture);
    }
    for (int i = 0; i < 45; i++)
    {
        Coin[i].setTexture(cointexture);
        coins_pos(Coin);
        Coin[i].setScale(0.15, 0.15);
        Coin[i].setTextureRect(IntRect(0, 0, 135, 134));
    }



    blocks(grounds);

    //render a motoEnemy sprite and add a texture to it and set a starting position + start a clock for the animation
    Sprite motoEnemy[3];
    Texture motoEnemyTexture;
    int motoAnim = 0;
    Clock motoClock;

    vector<pair<pair<int, int>, pair<int, int>>> motoData = { {{300, 370}, {200, 450}}, {{1250, 370}, {1100, 1400}}, {{3500, 370}, {3400, 3550}} };
    bool motoLeft[3] = {1,1,1};

    motoEnemyTexture.loadFromFile("motoEnemy.png");
    for (int i = 0; i < 3; i++)
    {
        motoEnemy[i].setTexture(motoEnemyTexture);
        motoEnemy[i].setPosition(Vector2f(motoData[i].first.first, motoData[i].first.second));
    }

    //render a fisnish line and set its position
    Sprite finishLine;
    Texture finishLineTexture;
    finishLineTexture.loadFromFile("finishLine.png");
    finishLine.setTexture(finishLineTexture);
    finishLine.setScale(Vector2f(1.5, 1.5));
    finishLine.setPosition(4600, 310);

    //load a bgMusic and play it on repeat
    Music bgMusic;
    bgMusic.openFromFile("bgMusic.wav");
    bgMusic.setLoop(1);
    bgMusic.play();
     

    while (window.isOpen())
    {
        //Timer
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time /= 650;
        if (time > 20)
            time = 20;

        //Close Event
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

        }

        //Ground_Collision
        for (int i = 0; i < 17; i++)
        {

            if (player.sonic.getGlobalBounds().intersects(grounds[i].getGlobalBounds())) {
                //game over
                if (i == 1 || i == 3 || i == 6 || i == 8 || i == 15) {
                    isCollision = 1;
                }
                // Left_Collision
                if (player.rect.left + 30 <= grounds[i].getPosition().x || player.rect.left + 30.5 <= grounds[i].getPosition().x) {

                    player.iswall = true;
                }
                else {
                    player.iswall = false;
                }
                //Upper_Collision 
                if (player.rect.top + 48 >= grounds[i].getPosition().y && !player.iswall) {
                    player.move_y = 0;
                    player.onground = true;
                }
                else {
                    player.onground = false;
                }

                if (player.onground && player.rect.top + 49 <= grounds[i].getPosition().y) {
                    player.rect.top = grounds[i].getPosition().y - 49;
                }
                if (player.iswall) {
                    player.rect.left = grounds[i].getPosition().x - 67;
                }

            }

        }

        //Spring Collision
        if (player.sonic.getGlobalBounds().intersects(player.spring.getGlobalBounds())) {
            player.move_y = -0.7;
            player.currentframe = 0;
            player.currentframe += 4;


            player.spring.setTextureRect(IntRect(int(player.currentframe) * 38, 0, 38, 47));

        }
        else {
            player.spring.setTextureRect(IntRect(0, 0, 38, 47));
        }

        //Coins Animation
        currentframecoin += 0.03 * time;
        if (currentframecoin > 10)
            currentframecoin -= 10;

        for (int i = 0; i < 45; i++)
        {
            Coin[i].setTextureRect(IntRect(int(currentframecoin) * 135, 0, 135, 134));
        }

        //Coins collision
        for (int i = 0; i < 45; i++)
        {
            if (player.sonic.getGlobalBounds().intersects(Coin[i].getGlobalBounds()))
            {
                player.score++;
                Coin[i].setScale(0, 0);
                player.text.setString("Score : " + to_string(player.score));
            }
        }


        //Movement + checks if the game is in player.state 3 (the game is running and not on screen)
        if (player.state == 3) {

            if ((Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up)) && player.onground) {
                player.move_y = -0.4;
                player.onground = false;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                player.move_x = 0.3;

            }
            else if (Keyboard::isKeyPressed(Keyboard::Key::A) || Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                player.move_x = -0.3;

            }
            else
            {
                player.move_x = 0;
            }
        }

        //Moving_block
        if (grounds[14].getPosition().x + 96 <= 2394 && turn)
            grounds[14].move(2, 0);
        else if (grounds[14].getPosition().x > 1691) {
            turn = false;
            grounds[14].move(-2, 0);
            if (grounds[14].getPosition().x <= 1691)
                turn = true;
        }


        //Hidden Trap

        if (hide_time == 100 || ishidden) {
            ishidden = true;
            grounds[15].setScale(0, 0);
            hide_time--;
        }
        if (hide_time == 0 || !ishidden) {
            grounds[15].setScale(1, 1);
            ishidden = false;
            hide_time++;
        }

        //Update

        player.update(time);

        //Camera
        if (player.sonic.getPosition().x >= 4400)
            view.setCenter(4400, 240);
        else if (player.sonic.getPosition().x <= 400)
            view.setCenter(400, 240);
        else {
            //view.setCenter(player.sonic.getPosition().x, 240);
            view.setCenter(player.rect.left, 240);
            player.text.setPosition(player.sonic.getPosition().x - 390, 10);
        }


        //mototEnemy
        //animation of motoEnemy
        if (motoClock.getElapsedTime().asSeconds() >= 0.1) {
            for (int i = 0; i < 3; i++)
            {
                motoEnemy[i].setTextureRect(IntRect(47 * motoAnim, 0, 47, 40));
                motoEnemy[i].setOrigin(sf::Vector2f(motoEnemy[i].getGlobalBounds().width, motoEnemy[i].getGlobalBounds().height) / 2.f);
            }
            motoAnim++;
            motoAnim = motoAnim % 6;
            motoClock.restart();
        }
        
        for (int i = 0; i < 3; i++) {
            if (motoEnemy[i].getPosition().x == motoData[i].second.second) {
                motoLeft[i] = 1;
                motoEnemy[i].setScale(1.f, 1.f);
            }
            if (motoEnemy[i].getPosition().x == motoData[i].second.first) {
                motoLeft[i] = 0;
                motoEnemy[i].setScale(-1.f, 1.f);
            }

            if (motoLeft[i]) {
                motoEnemy[i].move(-2.5, 0);
            }
            else {
                motoEnemy[i].move(2.5, 0);
            }
        
        }

        //colission with motoEnemy
        for (int i = 0; i < 3; i++) {
            if (player.sonic.getGlobalBounds().intersects(motoEnemy[i].getGlobalBounds())) {
                isCollision = 1;

            }
        }

       

        //finish line detection
        if (player.sonic.getGlobalBounds().intersects(finishLine.getGlobalBounds())) {
            isFinish = 1;
        }

        





        //Draw
        window.clear();
        window.draw(back);
        for (int i = 0; i < 45; i++)
        {
            window.draw(Coin[i]);
        }
        for (int i = 0; i < 17; i++)
        {
            window.draw(grounds[i]);
        }
        window.setView(view);
        window.draw(finishLine);
        for (int i = 0; i < 3; i++) {
            window.draw(motoEnemy[i]);
        }
        window.draw(player.spring);
        window.draw(player.text);
        window.draw(player.sonic); 


        startMenuRender();
        if (isCollision) {
            deathMenuRender();
        }
        if (isFinish) {
            finishMenuRender();
        }
        
        window.display();
    }

}
void blocks(Sprite grounds[]) {
    grounds[0].setTextureRect(IntRect(0, 384, 513, 96));
    grounds[0].setPosition(0, 384);
    grounds[1].setTextureRect(IntRect(513, 414, 190, 66)); //trap
    grounds[1].setPosition(513, 414);
    grounds[2].setTextureRect(IntRect(703, 384, 160, 96));
    grounds[2].setPosition(703, 384);
    grounds[3].setTextureRect(IntRect(863, 414, 190, 66)); //trap
    grounds[3].setPosition(863, 414);
    grounds[4].setTextureRect(IntRect(1053, 384, 478, 96));
    grounds[4].setPosition(1053, 384);
    grounds[5].setTextureRect(IntRect(1531, 224, 160, 256));
    grounds[5].setPosition(1531, 224);
    grounds[6].setTextureRect(IntRect(1691, 253, 703, 227)); //trap
    grounds[6].setPosition(1691, 253);
    grounds[7].setTextureRect(IntRect(2394, 224, 321, 256));
    grounds[7].setPosition(2394, 224);
    grounds[8].setTextureRect(IntRect(2715, 445, 670, 35)); //trap
    grounds[8].setPosition(2715, 445);
    grounds[9].setTextureRect(IntRect(2816, 159, 96, 33));
    grounds[9].setPosition(2816, 159);
    grounds[10].setTextureRect(IntRect(3008, 224, 96, 33));
    grounds[10].setPosition(3008, 224);
    grounds[11].setTextureRect(IntRect(3167, 352, 96, 33));
    grounds[11].setPosition(3167, 352);
    grounds[12].setTextureRect(IntRect(3385, 384, 1375, 96));
    grounds[12].setPosition(3385, 384);
    grounds[13].setTextureRect(IntRect(4760, 0, 33, 480));
    grounds[13].setPosition(4760, 0);
    grounds[14].setTextureRect(IntRect(0, 0, 96, 33));      //Moving_block
    grounds[14].setPosition(1691, 224);
    grounds[15].setTextureRect(IntRect(0, 0, 640, 33));      //Hidden_trap
    grounds[15].setPosition(3600, 351);
    grounds[16].setTextureRect(IntRect(0, 0, 96, 25));
    grounds[16].setPosition(3872, 310);
}
void coins_pos(Sprite Coin[]) {
    for (int i = 0; i < 45; i++)
    {
        if (i == 0 || i == 1 || i == 2 || i == 3)
            Coin[i].setPosition(350 + (i * 35), 360);
        if (i == 4 || i == 5 || i == 6)
            Coin[i].setPosition(420 + (i * 35), 300);
        if (i == 7 || i == 8 || i == 9)
            Coin[i].setPosition(490 + (i * 35), 360);
        if (i == 10 || i == 11 || i == 12)
            Coin[i].setPosition(560 + (i * 35), 300);
        if (i == 13 || i == 14 || i == 15 || i == 16 || i == 17 || i == 18 || i == 19 || i == 20)
            Coin[i].setPosition(650 + (i * 35), 360);
        if (i == 21 || i == 22 || i == 23)
            Coin[i].setPosition(830 + (i * 35), 202);
        if (i == 24 || i == 25 || i == 26)
            Coin[i].setPosition(1030 + (i * 35), 142);
        if (i == 27 || i == 28 || i == 29)
            Coin[i].setPosition(1200 + (i * 35), 142);
        if (i == 30 || i == 31 || i == 32 || i == 33 || i == 34 || i == 35 || i == 36)
            Coin[i].setPosition(1400 + (i * 35), 202);
        if (i == 37 || i == 38)
            Coin[i].setPosition(1700 + (i * 35), 160);
        if (i == 39 || i == 40)
            Coin[i].setPosition(1820 + (i * 35), 280);
        if (i == 41 || i == 42)
            Coin[i].setPosition(2300 + (i * 35), 360);
        if (i == 43 || i == 44)
            Coin[i].setPosition(2550 + (i * 35), 360);


    }
}








//int x = 0, y = 0, x2 = 0, y2 = 0, score = 0;
//bool isground = false;
//bool iswall = false;

//void ground(Sprite grounds[] , Sprite &sonic);

//void Player(RenderWindow& window , Sprite &sonic, float& timer ,Sprite grounds[] , Sprite &back , View &view);

//void MainWindow(RenderWindow& window);
//void MainWindow(RenderWindow& window) {
//    
//    window.setFramerateLimit(30);
//
//    Clock clock; 
//
//  
//    
//
//    View view(FloatRect(0.f, 0.f, 800.f, 480.f));
//   // view.zoom(0.7);
//
//
//    Texture background, map , sonictexture;
//    sonictexture.loadFromFile("SonicAnimation copy.png");
//    background.loadFromFile("map.png");
//    map.loadFromFile("map.png");
//
//    Sprite back, grounds[14] , sonic;
//    back.setTexture(background);
//    sonic.setTexture(sonictexture);
//    sonic.setPosition(200, 250);
//    sonic.setScale(0.5, 0.5);
//    sonic.setTextureRect(IntRect(x * 46, y * 63, 46, 63));
//    sonic.setOrigin(42, 51);
//    for (int i = 0; i < 14; i++) {
//        grounds[i].setTexture(map);
//    }
//
//    ground(grounds , sonic);
//
//    while (window.isOpen())
//    {
//        float timer = clock.getElapsedTime().asMicroseconds();
//        clock.restart();
//        timer /= 650;
//        if (timer > 20) {
//            timer = 20;
//        }
//
//        Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == Event::Closed)
//                window.close();
//
//        }
//
//
//        for (int i = 0; i < 14; i++)
//        {
//            if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
//                grounds[i].move(-10, 0);              
//            }
//            else if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
//                grounds[i].move(10, 0);           
//            }
//
//        }
//
//        Player(window, sonic , timer , grounds ,back ,view);
//
//      //  view.setCenter(sonic.getPosition());
//        
//
//        window.clear(Color::Blue);
//       // window.setView(view);
//         // window.draw(back);
//        for (int i = 0; i < 14; i++)
//        {
//            window.draw(grounds[i]);
//        }
//        window.draw(sonic);
//        window.display();
//    }
//
//}
//
////void Player(RenderWindow& window, Sprite& sonic, float& timer, Sprite grounds[], Sprite& back ,View &view) {
////
////   
////
////
////    for (int i = 0; i < 14; i++)
////    {
////        if (sonic.getGlobalBounds().intersects(grounds[i].getGlobalBounds())) {
////            isground = true;
////            
////           /* if (i == 1 || i == 3 || i == 6 || i == 8) {
////                sonic.setPosition(20, 350);
////                ground(grounds, sonic);
////                back.setPosition(0, 0);
////            }*/
////        }
////        
////    }
////    if (!isground) {
////        sonic.move(0, 0.5 * timer);
////    }
////    
////
////
////
////    if (Keyboard::isKeyPressed(Keyboard::Key::W) && isground)
////    {
////        sonic.move(0, -150);
////        y = 2;
////        x = 0;
////        x++;
////        
////        sonic.setTextureRect(IntRect(x * 84, y * 103, 84, 103));
////
////    }
////    else if (Keyboard::isKeyPressed(Keyboard::Key::S))
////    {
////        y = 2;
////        x = 0;
////        sonic.setTextureRect(IntRect(x * 84, y * 103, 84, 103));
////
////    }
////    else if (Keyboard::isKeyPressed(Keyboard::Key::D))
////    {
////        sonic.move(0.1* timer, 0);
////        sonic.setScale(0.5, 0.5);
////        x++;
////        x %= 9;
////        if (x % 9 == 0) {
////
////            y = 1;
////        }
////        if (x == 3 && y == 1) {
////            y = 0;
////        }
////
////        back.move(-10, 0);
////
////        sonic.setTextureRect(IntRect(x * 84, y * 103, 84, 103));
////    }
////    else if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
////        sonic.move(-0.1 * timer, 0);
////        sonic.setScale(-0.5, 0.5);
////        x++;
////        x %= 9;
////        if (x % 9 == 0) {
////
////            y = 1;
////        }
////        if (x == 3 && y == 1) {
////            y = 0;
////        }
////
////        back.move(10, 0);
////
////        sonic.setTextureRect(IntRect(x * 84, y * 103, 84, 103));
////    }
////    else if (isground) {
////        y = 0;
////        x = 0;
////        sonic.setTextureRect(IntRect(x * 84, y * 103, 84, 103));
////
////    }
////    
////    isground = false;
////
////}
////
////void ground(Sprite grounds[] , Sprite &sonic) {
////    grounds[0].setTextureRect(IntRect(0, 384, 513, 96));
////    grounds[0].setPosition(0, 384);
////    grounds[1].setTextureRect(IntRect(513, 414, 190, 66)); //trap
////    grounds[1].setPosition(513, 414);
////    grounds[2].setTextureRect(IntRect(703, 384, 160, 96));
////    grounds[2].setPosition(703, 384);
////    grounds[3].setTextureRect(IntRect(863, 414, 190, 66)); //trap
////    grounds[3].setPosition(863, 414);
////    grounds[4].setTextureRect(IntRect(1053, 384, 478, 96));
////    grounds[4].setPosition(1053, 384);
////    grounds[5].setTextureRect(IntRect(1531, 224, 160, 256));
////    grounds[5].setPosition(1531, 224);
////    grounds[6].setTextureRect(IntRect(1691, 253, 703, 227)); //trap
////    grounds[6].setPosition(1691, 253);
////    grounds[7].setTextureRect(IntRect(2394, 224, 321, 256));
////    grounds[7].setPosition(2394, 224);
////    grounds[8].setTextureRect(IntRect(2715, 445, 670, 35)); //trap
////    grounds[8].setPosition(2715, 445);
////    grounds[9].setTextureRect(IntRect(2816, 159, 96, 33));
////    grounds[9].setPosition(2816, 159);
////    grounds[10].setTextureRect(IntRect(3008, 224, 96, 33));
////    grounds[10].setPosition(3008, 224);
////    grounds[11].setTextureRect(IntRect(3167, 352, 96, 33));
////    grounds[11].setPosition(3167, 352);
////    grounds[12].setTextureRect(IntRect(3385, 384, 1375, 96));
////    grounds[12].setPosition(3385, 384);
////    grounds[13].setTextureRect(IntRect(4760, 0, 33, 480));
////    grounds[13].setPosition(4760, 0);
////
////
////
////    
////}
////

















//void sonicmovement(sprite &sonic,int &x,int &y);
//void sonicjump(sprite& sonic, int& x, int& y , rectangleshape &ground);
//void collision_Coin(Sprite& sonic, Sprite Coin[10], int &score , Text &text);
//void Coin_Animation(Sprite Coin[10], int& x2, int& y2);
//
//
//int main() {
//
//   
//    RenderWindow window(VideoMode(1500, 1000), "SFML works!");
//    window.setFramerateLimit(30);
//
//    int x = 0, y = 0, x2 = 0, y2 = 0, score = 0;
//
//   
//    RectangleShape rect, ground,JumpGround;
//    ground.setSize(Vector2f(280, 50));
//    ground.setPosition(0, 960);
//    rect.setSize(Vector2f(1022, 70));
//    JumpGround.setSize(Vector2f(50,50));
//    JumpGround.setPosition(230, 960);
//
//    Texture sonictexture, backgroundtexture, cointexture;
//    cointexture.loadFromFile("SonicCoinsAnimation.png");
//    sonictexture.loadFromFile("SonicAnimation copy.png");
//    backgroundtexture.loadFromFile("SonicLevel1.png");
//
//
//    Sprite sonic, Back, Coin[10];
//    Back.setTexture(backgroundtexture);
//    Back.setTextureRect(IntRect(0, 0, 11250, 1000));
//    sonic.setTexture(sonictexture);
//    sonic.setPosition(42, 930);
//    sonic.setTextureRect(IntRect(x * 84, y * 90, 84, 90));
//    sonic.setOrigin(42, 51);
//
//
//
//
//    for (int i = 0; i < 10; i++)
//    {
//        Coin[i].setTexture(cointexture);
//        Coin[i].setPosition(200 + (i * 50), 340);
//        Coin[i].setScale(0.3, 0.3);
//        Coin[i].setTextureRect(IntRect(x2 * 135, y2 * 134, 135, 134));
//    }
//
//    Font font;
//    font.loadFromFile("Asman.ttf");
//
//
//    Text text;
//    text.setFont(font);
//    text.setString("Score : " + to_string(score));
//    text.setPosition(10, 10);
//    text.setFillColor(Color(50, 200, 300, 200));
//    text.setCharacterSize(50);
//
//
//    
//
//    while (window.isOpen())
//    {
//
//        Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == Event::Closed)
//                window.close();
//        }
//
//        SonicJump(sonic, x, y, ground);
//
//        SonicMovement(sonic , x, y );
//
//        collision_Coin(sonic, Coin, score, text);
//
//        Coin_Animation(Coin, x2, y2);
//        
//         
//
//        window.clear();
//        window.draw(Back);
//        window.draw(rect);
//        window.draw(text);
//        window.draw(sonic);
//        for (size_t i = 0; i < 10; i++)
//        {
//            window.draw(Coin[i]);
//
//        }
//
//        
//        window.display();
//    }
//  
//
//    return 0;
//}
//
//
//
//
//
//
//void SonicMovement(Sprite &sonic , int &x ,int &y) {
//    if (Keyboard::isKeyPressed(Keyboard::Key::S))
//    {
//        y = 2;
//        x = 0;
//        sonic.setTextureRect(IntRect(x * 84, y * 90, 84, 90));
//
//    }
//    else if (Keyboard::isKeyPressed(Keyboard::Key::D))
//    {
//        sonic.setScale(1, 1);
//        x++;
//        if (x % 9 == 0) {
//
//            y = 1;
//        }
//        if (x == 3 && y == 1) {
//            y = 0;
//        }
//        sonic.move(15, 0);
//        
//       
//        x %= 9;
//
//        sonic.setTextureRect(IntRect(x * 84, y * 90, 84, 90));
//    }
//    else if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
//        sonic.setScale(-1, 1);
//        x++;
//        if (x % 9 == 0) {
//
//            y = 1;
//        }
//        if (x == 3 && y == 1) {
//            y = 0;
//        }
//        sonic.move(-15, 0);
//        x %= 9;
//        sonic.setTextureRect(IntRect(x * 84, y * 90, 84, 90));
//    }
//    else {
//        x = 0;
//        sonic.setTextureRect(IntRect(x * 84, y * 90, 84, 90));
//
//    }
//    
//}
//
//void SonicJump(Sprite& sonic, int& x, int& y ,RectangleShape &ground) {
//    if (sonic.getGlobalBounds().intersects(ground.getGlobalBounds())) {
//
//        if (Keyboard::isKeyPressed(Keyboard::Key::W))
//        {
//            for (int i = 1; i < 15; i++)
//            {
//                sonic.move(0, -i);
//            }
//
//        }
//    }
//    else {
//
//        sonic.move(0, 7);
//        y = 2;
//        x = 0;
//        sonic.setTextureRect(IntRect(x * 84, y * 90, 84, 90));
//    }
//}
//
//void collision_Coin(Sprite& sonic, Sprite Coin[10], int &score , Text &text) {
//    for (int i = 0; i < 10; i++)
//    {
//        if (sonic.getGlobalBounds().intersects(Coin[i].getGlobalBounds()))
//        {
//            score++;
//            Coin[i].setScale(0, 0);
//            text.setString("Score : " + to_string(score));
//        }
//    }
//}
//
//void Coin_Animation(Sprite Coin[10], int& x2, int& y2) {
//
//    x2++;
//    x2 %= 10;
//    for (int i = 0; i < 10; i++)
//    {
//
//        Coin[i].setTextureRect(IntRect(x2 * 135, y2 * 134, 135, 134));
//    }
//
//}

//
//int main() {
//
//    RenderWindow window(VideoMode(1022, 498), "SFML works!");
//    window.setFramerateLimit(30);
//
//    int x = 0, y = 0, x2 = 0, y2 = 0, score = 0;
//    bool grounded = false;
//
//    RectangleShape rect, ground[10];
//    rect.setSize(Vector2f(1022, 70));
//    ground[0].setSize(Vector2f(378, 10));
//    ground[0].setPosition(0, 400);
//    ground[1].setSize(Vector2f(100, 10));
//    ground[1].setPosition(378, 400);
//    ground[1].setRotation(-20);
//    ground[2].setSize(Vector2f(650, 10));
//    ground[2].setPosition(462, 370);
//    
//
//    Texture sonictexture, backgroundtexture, cointexture ,groundtexture;
//    cointexture.loadFromFile("SonicCoinsAnimation.png");
//    sonictexture.loadFromFile("SonicAnimation copy.png");
//    backgroundtexture.loadFromFile("SonicLevel2.png");
//    groundtexture.loadFromFile("Backgroung_ground.png");
//
//    Sprite sonic, Back, Coin[10], Ground;
//    Back.setTexture(backgroundtexture);
//    sonic.setTexture(sonictexture);
//    sonic.setPosition(42, 200);
//    sonic.setTextureRect(IntRect(x * 46, y * 63, 46, 63));
//    sonic.setOrigin(42, 51);
//     Ground.setTexture(groundtexture);
//     Ground.setPosition(0, 240);
//    Ground.setTextureRect(IntRect(0,140,1000,115));
//
//
//    for (int i = 0; i < 10; i++)
//    {
//        Coin[i].setTexture(cointexture);
//        Coin[i].setPosition(200 + (i * 50), 340);
//        Coin[i].setScale(0.3, 0.3);
//        Coin[i].setTextureRect(IntRect(x2 * 135, y2 * 134, 135, 134));
//    }
//
//    
//
//    Font font;
//    font.loadFromFile("Asman.ttf");
//
//
//    Text text;
//    text.setFont(font);
//    text.setString("Score : " + to_string(score));
//    text.setPosition(10, 10);
//    text.setFillColor(Color(50, 200, 300, 200));
//    text.setCharacterSize(50);
//
//
//    while (window.isOpen())
//    {
//        Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == Event::Closed)
//                window.close();
//        }
//        
//        
//            if (sonic.getGlobalBounds().intersects(ground[0].getGlobalBounds())) {
//
//                if (Keyboard::isKeyPressed(Keyboard::Key::W))
//                {
//                    for (int j = 1; j < 15; j++)
//                    {
//                        sonic.move(0, -j);
//                    }
//
//                }
//            }
//            else {
//
//                sonic.move(0, 7);
//            }
//        
//           
//
//            
//
//
//       for (int i = 0; i < 10; i++)
//        {
//            if (sonic.getGlobalBounds().intersects(Coin[i].getGlobalBounds()))
//            {
//                score++;
//                Coin[i].setScale(0, 0);
//                text.setString("Score : " + to_string(score));
//            }
//        }
//
//        if (Keyboard::isKeyPressed(Keyboard::Key::S))
//        {
//            y = 2;
//            x = 0;
//            sonic.setTextureRect(IntRect(x * 84, y * 103, 84, 103));
//
//        }
//        else if (Keyboard::isKeyPressed(Keyboard::Key::D))
//        {
//            sonic.setScale(1, 1);
//            x++;
//            if (x % 9 == 0) {
//
//                y = 1;
//            }
//            if (x == 3 && y == 1) {
//                y = 0;
//            }
//           
//            for (int i = 0; i < 3; i++)
//            {
//                ground[i].move(-15,0);
//            }
//            Back.move(-15, 0);
//            x %= 9;
//
//            sonic.setTextureRect(IntRect(x * 84, y * 103, 84, 103));
//        }
//        else if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
//            sonic.setScale(-1, 1);
//            x++;
//            if (x % 9 == 0) {
//
//                y = 1;
//            }
//            if (x == 3 && y == 1) {
//                y = 0;
//            }
//            for (int i = 0; i < 3; i++)
//            {
//                ground[i].move(15, 0);
//            }
//            Back.move(15, 0);
//            x %= 9;
//            sonic.setTextureRect(IntRect(x * 84, y * 103, 84, 103));
//        }
//        else {
//            y = 0;
//            x = 0;
//            sonic.setTextureRect(IntRect(x * 84, y * 103, 84, 103));
//
//        }
//       
//        x2++;
//        x2 %= 10;
//        for (int i = 0; i < 10; i++)
//        {
//
//            Coin[i].setTextureRect(IntRect(x2 * 135, y2 * 134, 135, 134));
//        }
//        
//
//
//
//      
//
//        window.clear();
//        window.draw(Back);
//       // window.draw(Ground);
//      
//        window.draw(rect);
//        window.draw(text);
//        window.draw(sonic);
//        for (int i = 0; i < 3; i++)
//        {
//            window.draw(ground[i]);
//        }
//
//       /* for (size_t i = 0; i < 10; i++)
//        {
//            window.draw(Coin[i]);
//
//        }*/
//       
//
//        window.display();
//    }
//
//
//    return 0;
//}


  //Fish Frenzy

//int main()
//{
//    RenderWindow window(sf::VideoMode(981, 580), "Fish Frenzy");
//    window.setFramerateLimit(20);
//    
//   
//    int y = 0 , x=0;
//    bool right = false;
//    
//    Texture  background, mainfish ;
//    background.loadFromFile("FrenzyFishBackground.jpg");
//    mainfish.loadFromFile("MainFish.png");
//   
//
//    Sprite MainFish , back ;
//    
//    back.setTexture(background);
//    MainFish.setTexture(mainfish);
//    MainFish.setTextureRect(IntRect(x * 127, y * 107, 127, 107));
//    MainFish.setPosition(450, 225);
//    MainFish.setScale(0.5, 0.5);
//    MainFish.setOrigin(63, 53);
//
//    while (window.isOpen())
//    {
//       
//        Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == Event::Closed)
//                window.close();
//
//        }
//        
//        if (Keyboard::isKeyPressed(Keyboard::Key::W))
//        {
//
//            MainFish.move(0, -15);
//        }else if (Keyboard::isKeyPressed(Keyboard::Key::S))
//        {
//            MainFish.move(0, 15);
//
//        }else if (Keyboard::isKeyPressed(Keyboard::Key::D))
//        {
//            
//            
//                if (right == false) {
//                    y = 3; x = 0;
//                   
//                    MainFish.setTextureRect(IntRect(x * 127, y * 107, 127, 107));
//                    x++;
//                    x %= 4;
//                }
//           
//        
//            MainFish.setScale(-0.5, 0.5);
//            MainFish.move(15, 0);
//            y = 2;
//            x++;
//            x %= 15;
//            MainFish.setTextureRect(IntRect(x * 127, y * 107, 127, 107));
//
//        }else if (Keyboard::isKeyPressed(Keyboard::Key::A))
//        {
//            right = false;
//            MainFish.setScale(0.5, 0.5);
//            MainFish.move(-15, 0);
//            y = 2;
//            x++;
//            x %= 15;
//            MainFish.setTextureRect(IntRect(x * 127, y * 107, 127, 107));
//           
//        }
//        else {
//            y = 1;
//            x++;
//            x %= 6;
//            MainFish.setTextureRect(IntRect(x * 127, y * 107, 127, 107));
//
//        }
//        
//        
//            
//
//        window.clear();
//        window.draw(back);
//        window.draw(MainFish);
//        window.display();
//    }
//
//
//    return 0;
//}
