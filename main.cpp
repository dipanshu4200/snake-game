#include <raylib.h>
#include <iostream>
#include <deque>
#include <raymath.h>
using namespace std;

Color black = { 50, 50, 50, 255 };
Color darkgreen = { 43,51,24,255 };
Color green = { 150,170,70,255 };

int cellsize = 30;
int cellcount = 25;
int offset = 75;

double lastupdate = 0; // variable to keep track of time at which snake is moving

bool snakeupdate(double interval) { // this func is to detect current fps and slowdown snake speed
    double current = GetTime();
    if (current - lastupdate >= interval) { // to check if a specific time had been passed
        lastupdate = current;
        return true;
    }return false;
}

bool elementinsnake(Vector2 pos,deque<Vector2> deq ) {
    for (auto i : deq) {
        if (i == pos) {
            return true;
        }
    }return false;
}


class Snake {
public :
    deque<Vector2> body = { Vector2{11,12},Vector2{12,12},Vector2{13,12} }; // this to create snake body with 3 box
    Vector2 direction = { 1,0 };
    bool addsegment = false;
    Texture2D text;

    Snake() {
        Image image = LoadImage("res/snake.png");
        text = LoadTextureFromImage(image);
        UnloadImage(image);

    }
    ~Snake() {
        UnloadTexture(text);
    }

    void draw() {  // draw function 

        for (unsigned int i = 0;i<body.size();i++) { // as there there can be n no of snake body
            float x = body[i].x;
            float y = body[i].y;

            if (i == 0) { // this to check head roatation
                float rotation = 0.0f;
                if (direction.x == 1 && direction.y == 0) {
                    rotation = 0.0f;

                }else if (direction.x == 0 && direction.y == 1) {
                    rotation = 90.0f;

                }else if (direction.x == -1 && direction.y == 0) {
                    rotation = 180.0f;

                }else if (direction.x == 0 && direction.y ==-1 ) {
                    rotation = 270.0f;

                }
                Rectangle sourceRec = { 0.0f, 0.0f, (float)text.width, (float)text.height }; // Full texture
                // Correct the destination position to ensure no shift due to rotation
                Rectangle destRec = { offset + x * cellsize + cellsize / 2.0f, offset + y * cellsize + cellsize / 2.0f, (float)cellsize, (float)cellsize };
                Vector2 origin = { cellsize / 2.0f, cellsize / 2.0f }; // Set the origin to the center of the texture for proper rotation
                DrawTexturePro(text, sourceRec, destRec, origin, rotation, WHITE);

            }
            else {
                Rectangle seg = Rectangle{ offset + x * cellsize,offset + y * cellsize ,(float)cellsize ,(float)cellsize  }; // this is a structure to create round rectangle
                DrawRectangleRounded(seg, 0.45, 6, black); // this to create snake body and its positioning
            }


       
        }
    }
    void update() { // to do movement   
        body.push_front(body[0] + direction); // to create a new head to appear it move
        if (addsegment == true) {
            addsegment = false;
        }
        else { // when snake isn't eating food
            body.pop_back();  // to delete an box 
            
              
        }

    }
    void reset() {
        body = { Vector2{11,12},Vector2{12,12} ,Vector2{13,12} };
        direction = { -1,0 };
    }

};

class Food {
public :
    Vector2 pos; // this is 2d space to define the position of food
    Texture2D text;
    Food(deque <Vector2> snakebody ) { // contructor
        Image image = LoadImage("res/apple.png");
        text = LoadTextureFromImage(image);
        UnloadImage(image);
        pos = random(snakebody); // this is to call random inline function
    }
    ~Food() { // deconstructor
        UnloadTexture(text);
    }

    Vector2 randomcell() { // this is to randomy generate food in cordinate structure
        float x = GetRandomValue(0, cellcount - 1);
        float y = GetRandomValue(0, cellcount - 1);
        return Vector2{ x,y };
    }
    Vector2 random(deque<Vector2> snakebody) { // this is to check if food is not inside snake body
        Vector2 position = randomcell();
        elementinsnake(position,snakebody);
        while (elementinsnake(position, snakebody)) {
            position = randomcell(); // if food in snake change it's position to random
        }
        return position;
    }

    void draw() {// to draw food
        DrawTexture(text,offset+pos.x* cellsize, offset + pos.y* cellsize,WHITE); // this is the func used to draw shapes ex: food in this case
    }

};

class Game {   // this is to keep game readibility easy ans accessible
public:
    Food food = Food(snake.body);
    Snake snake = Snake();
    bool running = true;
    int score = 0;
    int maxscore = 0;
    Sound eatsound;
    Sound wallsound;
    Sound self;

    Game() {
        InitAudioDevice();
        eatsound = LoadSound("sounds/eat2.mp3");
        wallsound = LoadSound("sounds/wall2.mp3");
        self = LoadSound("sounds/wall.mp3");


    }
    ~Game() {
        UnloadSound(eatsound);
        UnloadSound(wallsound);
        UnloadSound(self);
        CloseAudioDevice();
    }

    void draw() {
        food.draw();
        snake.draw();
    }
    void update() {
        if (running) { // to check if game is running after touching the border an resetting   (R)
            snake.update();
            checkcollisonfood();
            checkcollisionwithedges();
            checkcollisionwithtail();
        }
    }

    void checkcollisonfood() {
        if (Vector2Equals(snake.body[0] ,food.pos)) {
            food.pos = food.random(snake.body);
            snake.addsegment = true;
            score++;
            PlaySound(eatsound);

        }
    }

    void checkcollisionwithedges() {
        if (snake.body[0].x == cellcount || snake.body[0].x == -1) {
            PlaySound(wallsound);
            Gameover();
        }
        if (snake.body[0].y == cellcount || snake.body[0].y == -1) {
            PlaySound(wallsound);
            Gameover();
        }   
    }

    void Gameover() {
        snake.reset();
        food.pos = {6,7};
        running = false; // to stop the game after going off border  (R)
        maxscore=max(maxscore,score);
        score = 0;

    }

    void checkcollisionwithtail() {
        deque<Vector2> snakebody1 = snake.body;
        snakebody1.pop_front();
        if (elementinsnake(snake.body[0], snakebody1)) {
            PlaySound(self);
            Gameover();
        }
    }

};

int main(void)
{

    InitWindow(2*offset + cellsize*cellcount, 2 * offset + cellsize * cellcount, "snake game");
    SetTargetFPS(60);               

    //Food food = Food(); // this is an object to call its method
    //Snake snake = Snake();
    Game game = Game();
    Image image = LoadImage("res/background1.png");
    Texture2D text = LoadTextureFromImage(image);
    UnloadImage(image);

    while (!WindowShouldClose())    
    {
        BeginDrawing();

        if (snakeupdate(0.2)) { // this is to make snake movement slow to 
            game.update(); // imp as it changes all in game class and update it and evrything that comes under game             
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) { // this is to prevent it movement from left to right
            game.snake.direction = { -1,0 }; // to  make snake go up
            game.running = true;
        }

        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1 ) {
            game.snake.direction = { 1,0 }; // to  make snake go up
            game.running = true;
        }

        if (IsKeyPressed(KEY_UP) && !IsKeyPressed(KEY_LEFT) && game.snake.direction.y != 1 ) {
            game.snake.direction = { 0,-1 }; // to  make snake go up
            game.running = true;
        }

        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1 ) {
            game.snake.direction = { 0,1 }; // to  make snake go up
            game.running = true;
        }
                

     
        ClearBackground(green);
        DrawTexture(text, (2*offset+cellsize * cellcount) / 2 - text.width / 2, (2 * offset+cellsize * cellcount) / 2 - text.height / 2, green);

        //food.draw();  // this is to call the draw inline funct from Food class
        //snake.draw();

        // this to draw border around game
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)cellsize*cellcount+10,(float)cellsize * cellcount + 10 },5,darkgreen);
        // to create text in game
        DrawText("SCORE : ", offset, 20, 40, BLACK);
        DrawText(TextFormat("%i", game.score), offset+200 , 20, 40, BLACK); // to print score in game
        DrawText("HIGHSCORE : ", offset + 400, 20, 40, BLACK);
        DrawText(TextFormat("%i", game.maxscore), offset + 700, 20, 40, BLACK);
        game.draw(); //instead of calling it seprately we call game class as 

        EndDrawing();
        
    }


    CloseWindow();        

    return 0;
}