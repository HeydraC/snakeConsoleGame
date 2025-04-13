#include<iostream>
#include<cstdlib>
#include<ctime>
#include<vector>
#include<windows.h>

struct Coordinates{
    int x, y;

    Coordinates(int _x, int _y){
        x = _x;
        y = _y;
    }
    Coordinates(){
        x = y = 0;
    }

    bool operator ==(Coordinates c){
        return x == c.x && y == c.y;
    }
};
class Display{
    public:
    Display(int _height = 20, int _width = 40){
        CONSOLE_CURSOR_INFO cursor;

        stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        
        GetConsoleCursorInfo(stdHandle, &cursor);
        cursor.bVisible = false;
        SetConsoleCursorInfo(stdHandle, &cursor);

        if (_height < 8) _height = 8;
        if (_width < 8)  _width = 8;

        Coordinates start(5, 5), pillStart(random(_width), random(_height));
        
        for (int i = 0; i < 3; ++i) snake.push_back(start);

        dir = '?';
        newDir = dir;

        end = false;
        
        pillExist = true;
        pill = pillStart;

        sleepTime = 200;

        score = 0;

        height  = _height;
        width   = _width;

        boxes = new char*[height];
        for (int i = 0; i < height; ++i) boxes[i] = new char[width];

        int blankY = height / 2, blankX = width / 2;
        
        for (int i = 0; i < height; ++i){
            for (int j = 0; j < width; ++j){
                if (i == blankY || j == blankX){
                    boxes[i][j] = ' ';
                    continue;
                }
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1){
                    boxes[i][j] = 'X';
                    continue;
                }

                boxes[i][j] = ' ';
            }
        }
        
        SetConsoleCursorPosition(stdHandle, {0, 0});
        for (int i = 0; i < height; ++i){
            std::cout.write(boxes[i], width);
            std::cout<<std::endl;
        }

        print();
    }
    ~Display(){
        for (int i = 0; i < height; ++i) delete[] boxes[i];
        delete[] boxes;
    }

    void loop(){

        if      (GetAsyncKeyState(VK_UP))   newDir = 'w';
        else if (GetAsyncKeyState(VK_LEFT)) newDir = 'a';
        else if (GetAsyncKeyState(VK_DOWN)) newDir = 's';
        else if (GetAsyncKeyState(VK_RIGHT))newDir = 'd';

        move();
        print();
    }

    bool gameOver(){
        return end;
    }

    int sleepTime;

    private:
    std::vector<Coordinates> snake;
    char** boxes;
    char dir, newDir;
    int width, height;
    int score;
    Coordinates pill;
    bool end;
    bool pillExist;
    HANDLE stdHandle;

    void print(){
        SetConsoleTextAttribute(stdHandle, 6);
        SetConsoleCursorPosition(stdHandle, {(short)pill.x, (short)pill.y});
        std::cout.put('@');

        SetConsoleTextAttribute(stdHandle, 2);
        auto head = snake.begin();
        SetConsoleCursorPosition(stdHandle, {(short)head->x, (short)head->y});
        std::cout.put('S');

        SetConsoleTextAttribute(stdHandle, 7);
        SetConsoleCursorPosition(stdHandle, {0, (short)height});
        std::cout<<std::endl<<score<<std::endl;
    }

    void move(){
        auto head = snake.begin();
        Coordinates buffer(head->x, head->y);

        switch(newDir){
            case 'w':
                if (dir == 's') newDir = dir;
                break;
            case 'a':
                if (dir == 'd') newDir = dir;
                break;
            case 's':
                if (dir == 'w') newDir = dir;
                break;
            case 'd':
                if (dir == 'a') newDir = dir;
                break;
        }

        switch (newDir){
        case 'w':
            if (head->y - 1 < 0){
                buffer.y = height - 1;
                break;
            }

            if (boxes[head->y - 1][head->x] == 'X'){
                end = true;
                return;
            }

            --buffer.y;

            if (buffer == pill){
                pillExist = false;
            }

            break;
        case 'a':
            if (head->x - 1 < 0){
                buffer.x = width - 1;
                break;
            }

            if (boxes[head->y][head->x - 1] == 'X'){
                end = true;
                return;
            }

            --buffer.x;

            if (buffer == pill){
                pillExist = false;
            }

            break;
        case 's':
            if (head->y + 1 >= height){
                buffer.y = 0;
                break;
            }

            if (boxes[head->y + 1][head->x] == 'X'){
                end = true;
                return;
            }

            ++buffer.y;

            if (buffer == pill){
                pillExist = false;
            }

            break;
        case 'd':
            if (head->x + 1 >= width){
                buffer.x = 0;
                break;
            }
            
            if (boxes[head->y][head->x + 1] == 'X'){
                end = true;
                return;
            }

            ++buffer.x;

            if (buffer == pill){
                pillExist = false;
            }

            break;
        }

        for (int i = 3; i < snake.size(); ++i){
            if (buffer == snake[i]){
                end = true;
                return;
            }
        }

        dir = newDir;

        if (pillExist){
            auto end = snake.end() - 1;

            SetConsoleCursorPosition(stdHandle, {(short)end->x, (short)end->y});

            std::cout.put(' ');
        }else{
            auto end = snake.end() - 1;

            Coordinates newPart(end->x, end->y);

            snake.push_back(newPart);

            pill.x = random(width);
            pill.y = random(height);

            sleepTime = (sleepTime >= 5) ? sleepTime - 5 : 0; 
            score    += 10;

            pillExist = true;
        }

        for (auto part = snake.begin(); part != snake.end(); ++part){
            Coordinates aux(part->x, part->y);

            *part = buffer;

            buffer = aux;
        }
    }

    int random (int range){
        return (int)(rand() % (range - 2)) + 1;
    }
};

int main(){
    int n1, n2;

    std::cout<<"Stage height: "; std::cin>>n1;
    std::cout<<"Stage width: "; std::cin>>n2;

    system("cls");

    srand(time(0));

    Display d(n1, n2);

    do{
        d.loop();
        Sleep(d.sleepTime);
    }while(!GetAsyncKeyState(VK_ESCAPE) && !d.gameOver());

    return 0;
}