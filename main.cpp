/**
 * C++. Match 3(Marvel Style) game
 *
 * @author Maruf Asatullaev
 * @version dated Jan 28, 2020

 */

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>

using namespace sf;

int ts = 56;
Vector2i offset(33, 28);

//// структурка аттрибутов каждого гема ////
struct piece{
    int x, y, col, row, kind, match, alpha;
    piece(){
        match = 0; alpha = 255;
    }
} grid[10][10];

//// функция замены двух гемов местами ////
void swap(piece p1, piece p2){
    std::swap(p1.col, p2.col);
    std::swap(p1.row, p2.row);

    grid[p1.row][p1.col] = p1;
    grid[p2.row][p2.col] = p2;

}

int main(){
    srand(time(0));

    RenderWindow rw(VideoMode(800, 500), "Match-3. Marvel-Style!");
    rw.setFramerateLimit(60);

//// Загрузка картинок, шрифтов, музыки ////
    Texture t1, t2;
    t1.loadFromFile("images/image.jpg");                // задний фон
    t2.loadFromFile("images/icons.png");                // иконки гемов

    Sprite background(t1), gems(t2);

    Font font;
    font.loadFromFile("fonts/OstrichSans-Bold.otf");
    Text text("", font, 40);                            // текст очков
    text.setColor(Color::Yellow);
    text.setStyle(Text::Bold);
    Text text2("", font, 40);                           // текст очков на гемах
    text2.setColor(Color::Green);
    text2.setStyle(Text::Bold);

    Music music;
    music.openFromFile("music/Track 1.ogg");            // Музыка для заднего фона
    music.setVolume(5);
    music.play();

    SoundBuffer matchBuffer;
    matchBuffer.loadFromFile("music/Match.ogg");        // Звук соответствия
    Sound Match(matchBuffer);

//// Расположение рандомных гемов по сетке////
    for(int i = 1 ; i <= 8; ++i)
        for(int j = 1 ; j <= 8; ++j){
            grid[i][j].kind = rand()%7;
            grid[i][j].col = j;
            grid[i][j].row = i;
            grid[i][j].x = j*ts;
            grid[i][j].y = i*ts;
        }

    bool GemScore = false;                  // чекер очков на гемах
    int xg, yg, vert = 0, hor = 0;          // координаты и направленя вывода очков на гемах
    int x0, y0, x, y;                       // координаты двух выбранных гемов
    int click = 0, score = 0;               // клик-счётчик, cчётчик очков
    Vector2i pos;                           // координаты в данный момент выбранного гема
    bool isSwap = false, isMoving = false;  // чекер "стоит ли менять местами?" и "меняется ли?"

////// Основной цикл открытия окна //////
     while(rw.isOpen()){
        Event e;
        while(rw.pollEvent(e)){
            if(e.type == Event::Closed)
                rw.close();
        //// координата клика мышью ////
            if(e.type == Event::MouseButtonPressed)
                if(e.key.code == Mouse::Left){
                    if(!isSwap && !isMoving) ++click;
                    pos = Mouse::getPosition(rw)-offset;
                }
        }

//// Виды кликов мышью ////
        if(click == 1){
            x0 = pos.x/ts+1;
            y0 = pos.y/ts+1;
        }
        if(click == 2){
            x = pos.x/ts+1;
            y = pos.y/ts+1;
            if(abs(x-x0)+abs(y-y0) == 1){
                swap(grid[y0][x0], grid[y][x]);
                isSwap = true;
                click = 0;
            }else
                click = 1;
        }

//// Нахождение соответствий////
        for(int i = 1 ; i <= 8; ++i)
            for(int j = 1 ; j <= 8; ++j){
                if(grid[i][j].kind == grid[i+1][j].kind)
                    if(grid[i][j].kind == grid[i-1][j].kind){
                        for(int n = -1; n <= 1; ++n)
                            grid[i+n][j].match++;
                        vert = 1;
                        xg = grid[i-1][j].x;
                        yg = grid[i-1][j].y;
                    }


                if(grid[i][j].kind == grid[i][j+1].kind)
                    if(grid[i][j].kind == grid[i][j-1].kind){
                        for(int n = -1; n <= 1; ++n)
                            grid[i][j+n].match++;
                        hor = 1;
                        xg = grid[i][j-1].x;
                        yg = grid[i][j-1].y;
                    }

            }

//// Анимация движения 2ух гемов ////
        isMoving = false;
        for(int i = 1 ; i <= 8; ++i)
            for(int j = 1 ; j <= 8; ++j){
                piece &p = grid[i][j];          // гем рассматриваемый в данный момент
                int dx, dy;                     // координаты-чекеры происходит ли замена соседей
                for(int n = 0; n < 3; ++n){     // 3 - скорость замены
                    dx = p.x - p.col*ts;
                    dy = p.y - p.row*ts;
                    if(dx) p.x-= dx/abs(dx);
                    if(dy) p.y-= dy/abs(dy);
                    if(dx || dy) isMoving = true;
                }
            }

//// Анимация удаления////
        if(!isMoving)
            for(int i = 1; i <= 8; ++i)
                for(int j = 1 ; j <= 8; ++j)
                    if(grid[i][j].match)
                        if(grid[i][j].alpha>10){
                            grid[i][j].alpha-=10;
                            isMoving = true;
                            GemScore = true;
                        }

//// Заработок очков////
        int smth = 0;
        for(int i = 1 ; i <= 8; ++i)
            for(int j = 1 ; j <= 8; ++j){
                smth+= grid[i][j].match;
            }

//// Обратный свап если нет соответствий////
        if(isSwap && !isMoving){
            swap(grid[y0][x0], grid[y][x]);
            isSwap = 0;
        }

//// Обновление сетки////
        if(!isMoving){
            for(int i = 8 ; i > 0; --i)
                for(int j = 1 ; j <= 8; ++j)
                    if(grid[i][j].match)
                        for(int n = i; n > 0; --n)
                            if(!grid[n][j].match){
                                swap(grid[n][j], grid[i][j]);
                                break;
                            }

             for(int j = 1 ; j <= 8; ++j)
                for(int i = 8, n = 0 ; i > 0; --i)
                    if(grid[i][j].match){
                        grid[i][j].kind = rand()%7;
                        grid[i][j].y = -ts*n++;
                        grid[i][j].match = 0;
                        grid[i][j].alpha = 255;
                    }
        }

//// Отрисовка////
        rw.draw(background);

        text.setString("Score:");
        text.setPosition(510, 50);
        rw.draw(text);

        std::ostringstream CurrentScore;
        score+= smth;
        CurrentScore << score;
        text2.setString(CurrentScore.str());
        text2.setPosition(610, 50);
        rw.draw(text2);

        for(int i = 1 ; i <= 8; ++i)
            for(int j = 1 ; j <= 8; ++j)
            {
                    piece p = grid[i][j];
                    gems.setTextureRect(IntRect(p.kind*50, 0, 49, 50));
                    gems.setPosition(p.x, p.y);
                    gems.setColor(Color(255, 255, 255, p.alpha));
                    gems.move(offset.x-ts, offset.y-ts);
                    rw.draw(gems);
            }
        if(GemScore){
            GemScore = false;
            Match.play();
            text2.setString("999");
            for(int n = 0; n < 3; ++n){
                text2.setPosition(xg-20+hor*ts*n, yg-20+vert*ts*n);
                rw.draw(text2);
            }
            vert = 0; hor = 0;
        }
        rw.display();
     }

    return 0;
}
