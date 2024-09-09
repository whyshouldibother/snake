#include "raylib.h"
#include <cstdlib>
#include <random>
#include <iostream>
#include <vector>
#include <cstring>
using namespace std;

int randGen(int max, int blockSize)
{
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(5, max / blockSize - 5);
    return dist(mt) * blockSize;
}

class entity
{
public:
    int x;
    int y;
    int directionX = 0;
    int directionY = 0;
    entity() {};
    entity(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    bool check(int screenWidth, int screenHeight)
    {
        if (x >= screenWidth)
        {
            x = 0;
            return false;
        }
        if (x < 0)
        {
            x = screenWidth;
            return false;
        }
        if (y >= screenHeight)
        {
            y = 0;
            return false;
        }
        if (y < 0)
        {
            y = screenHeight;
            return false;
        }
        return true;
    }
    void update(int blocksize)
    {
        x += blocksize * directionX;
        y += blocksize * directionY;
    }
    bool grow(entity *food, int maxX, int maxY, int blockSize)
    {
        if (x == food->x && y == food->y)
        {
            food->x = randGen(maxX, blockSize);
            food->y = randGen(maxY, blockSize);
            return true;
        }
        else
            return false;
    }

    bool operator==(entity compare)
    {
        return (x == compare.x && y == compare.y);
    }
};
const char *getColor(Color x){
    if(x.a=255 && x.r==0 && x.g ==0 && x.b==0) return "BLACK";
    if(x.a=255 && x.r==255 && x.g ==255 && x.b==255) return "WHITE";
    if(x.a=255 && x.r==0 && x.g ==117 && x.b==44) return "DARKGREEN";
    if(x.a=255 && x.r==0 && x.g ==288 && x.b==48) return "GREEN";
    if(x.a=255 && x.r==230 && x.g ==41 && x.b==55) return "RED";
    if(x.a=255 && x.r==255 && x.g ==161 && x.b==0) return "ORANGE";
    return "UNKWON";
}
int main()
{

    int screenWidth = 800, screenHeight = 600, fps = 10, blockSize = 10, boxOffset = blockSize * 15, fontSize = 24, currMode = 0, score = 0;
    InitWindow(screenWidth, screenHeight, "Snake");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    InitAudioDevice();
    Sound gameOverSound = LoadSound("assets/gameover.mp3"), biteSound = LoadSound("assets/bite.mp3");
    SetTargetFPS(fps);
    entity food(randGen(screenWidth, blockSize), randGen(screenHeight, blockSize));
    vector<entity> snake;
    snake.push_back(entity(randGen(screenWidth, blockSize), randGen(screenHeight, blockSize)));
    bool gameOver = false, start = false, custom = false, walls = false, selfHarm = true, configure = false;
    Color backgroundColor = BLACK, fontColor = WHITE, snakeHeadColor = DARKGREEN,snakeBodyColor = GREEN, foodColor=RED, gameOverColor = RED, scoreColor=ORANGE;
    while (!WindowShouldClose())
    {
        if (start)
        {

            if (!gameOver)
            {
                currMode = 0;
                // Controls

                // Directions
                if (IsKeyPressed(KEY_LEFT) && snake[0].directionX == 0)
                {
                    snake[0].directionX = -1;
                    snake[0].directionY = 0;
                };
                if (IsKeyPressed(KEY_RIGHT) && snake[0].directionX == 0)
                {
                    snake[0].directionX = 1;
                    snake[0].directionY = 0;
                };
                if (IsKeyPressed(KEY_UP) && snake[0].directionY == 0)
                {
                    snake[0].directionX = 0;
                    snake[0].directionY = -1;
                };
                if (IsKeyPressed(KEY_DOWN) && snake[0].directionY == 0)
                {
                    snake[0].directionX = 0;
                    snake[0].directionY = 1;
                };
                // GameLogic

                // Check for Walls
                if (!snake[0].check(screenWidth, screenHeight) && walls)
                {
                    PlaySound(gameOverSound);
                    gameOver = true;
                }

                // Check Growth
                if (snake[0].grow(&food, screenWidth, screenHeight, blockSize))
                {
                    // Grow snake
                    PlaySound(biteSound);
                    snake.push_back(entity(snake[snake.size() - 1].x, snake[snake.size() - 1].y));
                    score++;
                };

                // Update snake body
                for (int i = snake.size() - 1; i > 0; i--)
                {
                    snake[i] = snake[i - 1];
                }

                // Move Head
                snake[0].update(blockSize);

                // Check Game Over
                if(selfHarm)
                for (int i = 1; i < snake.size(); i++)
                {
                    if (snake[0] == snake[i])
                    {
                        PlaySound(gameOverSound);
                        gameOver = true;
                        break;
                    }
                }
                BeginDrawing();
                for (int i = 1; i < snake.size(); i++)
                {
                    DrawRectangle(snake[i].x, snake[i].y, blockSize, blockSize, snakeBodyColor);
                }

                ClearBackground(backgroundColor);
                DrawText(TextFormat("Score:%d", score), screenWidth - MeasureText(TextFormat("Score:%d", score), fontSize), 0, fontSize, fontColor);
                DrawRectangle(food.x, food.y, 10, 10, foodColor);
                DrawRectangle(snake[0].x, snake[0].y, 10, 10, snakeHeadColor);

                EndDrawing();
            }
            else
            {
                // Gameover
                const char *options[] = {
                    "PLAY AGAIN",
                    "MAIN MENU",
                    "QUIT"};
                int modes = sizeof(options) / sizeof(options[0]), spacing = ((screenHeight - 2 * boxOffset) - (modes)*fontSize) / (modes + 1);
                BeginDrawing();
                ClearBackground(backgroundColor);
                for (int i = 1; i < snake.size(); i++)
                {
                    DrawRectangle(snake[i].x, snake[i].y, blockSize, blockSize, GRAY);
                }
                DrawRectangle(snake[0].x, snake[0].y, 10, 10, DARKGRAY);
                DrawText(TextFormat("GAMEOVER"), (screenWidth - MeasureText(TextFormat("GAMEOVER"), fontSize * 3)) / 2, blockSize, fontSize * 3, gameOverColor);
                DrawText(TextFormat("Score:%d", score), (screenWidth - MeasureText(TextFormat("Score:%d", score), fontSize * 2.5)) / 2, blockSize * 2 + fontSize * 3, fontSize * 2.5, scoreColor);
                DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);
                for (int i = 0; i < modes; i++)
                {
                    DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, fontColor);
                }

                // Select option
                DrawLine(
                    (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                    boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                    (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                    boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                    fontColor);

                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_DOWN) || (IsKeyPressed(KEY_TAB) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                    currMode++;
                if (IsKeyPressed(KEY_UP) || (IsKeyPressed(KEY_TAB) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                    currMode--;
                currMode += GetMouseWheelMove();
                if (currMode < 0)
                    currMode = modes - 1;
                if (currMode >= modes)
                    currMode = 0;
                if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    switch (currMode)
                    {
                    case 0:
                        score = 0;
                        gameOver = false;
                        snake.clear();
                        break;
                    case 1:
                        score = 0;
                        gameOver = false;
                        start = false;
                        snake.clear();
                        break;
                    case 2:
                        CloseWindow();
                        break;
                    }
                }
                EndDrawing();
            }
        }
        else if (custom)
        {
            BeginDrawing();
            ClearBackground(backgroundColor);
            DrawText("Use SPACE for next", (screenWidth - (MeasureText(TextFormat("Use SPACE for next"), fontSize * 1.5))) / 2, blockSize, fontSize * 1.5, fontColor);
            DrawText("Use ENTER to change value", (screenWidth - (MeasureText(TextFormat("Use ENTER to value"), fontSize * 1.5))) / 2, blockSize + fontSize * 1.5, fontSize * 1.5, fontColor);
            DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);
            DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);
            char currFps[16];
            sprintf(currFps, "FPS\t%d", fps);
            char currWalls[16];
            sprintf(currWalls, "Walls\t%s", walls?"TRUE":"FALSE");
            char currSelfHarm[16];
            sprintf(currSelfHarm, "Self Harm\t%s", selfHarm?"TRUE":"FALSE");
            const char *options[] = {
                currFps,
                currWalls,
                currSelfHarm,
                "PLAY",
                "MAIN MENU",
                "QUIT"
            };
            int modes = sizeof(options) / sizeof(options[0]), spacing = ((screenHeight - 2 * boxOffset) - (modes)*fontSize) / (modes + 1);
            for (int i = 0; i < modes; i++)
            {
                DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, fontColor);
            }

            // Select menu
            DrawLine(
                (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                fontColor);
            EndDrawing();

            // Change mode
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_DOWN) || (IsKeyPressed(KEY_TAB) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode++;
            if (IsKeyPressed(KEY_UP) || (IsKeyPressed(KEY_TAB) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode--;
            currMode += GetMouseWheelMove();
            if (currMode < 0)
                currMode = modes - 1;
            if (currMode >= modes)
                currMode = 0;
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                switch (currMode)
                {
                case 0:
                    fps+=5;
                    if(fps>40) fps =5;
                    if(fps < 5) fps = 40;
                    break;
                case 1:
                    walls=!walls;
                    break;
                case 2:
                    selfHarm=!selfHarm;
                    break;
                case 3:
                    SetTargetFPS(fps);
                    custom = false;
                    start = true;
                case 4:
                    custom = false;
                    break;
                case 5:
                    CloseWindow();
                    break;
                default:
                    break;
                }
            }
        }else if (configure)
        {
            BeginDrawing();
            ClearBackground(backgroundColor);
            DrawText("Use SPACE for next", (screenWidth - (MeasureText(TextFormat("Use SPACE for next"), fontSize * 1.5))) / 2, blockSize, fontSize * 1.5, fontColor);
            DrawText("Use ENTER to change value", (screenWidth - (MeasureText(TextFormat("Use ENTER to value"), fontSize * 1.5))) / 2, blockSize + fontSize * 1.5, fontSize * 1.5, fontColor);
            DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);
            DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);
            char currBackgroundColor[32];
            sprintf(currBackgroundColor, "BACKGROUND COLOR\t%s", getColor(backgroundColor));
            char currfontColor[32];
            sprintf(currfontColor, "FONT COLOR\t%s", getColor(fontColor));
            char currSnakeHeadColor[32];
            sprintf(currSnakeHeadColor, "SNAKE HEAD COLOR \t%s",getColor(snakeHeadColor));
            char currSnakeBodyColor[32];
            sprintf(currSnakeBodyColor, "SNAKE BODY COLOR\t%s", getColor(snakeBodyColor));
            char currFoodColor[32];
            sprintf(currFoodColor, "FOODCOLOR\t%s", getColor(foodColor));
            char currGameOverColor[32];
            sprintf(currGameOverColor, "GAMEOVER COLOR \t%s",getColor(gameOverColor));
            char currScoreColor[32];
            sprintf(currScoreColor, "SCORE COLOR \t%s",getColor(scoreColor));
            const char *options[] = {
                currBackgroundColor,
                currfontColor,
                currSnakeHeadColor,
                currSnakeBodyColor,
                currFoodColor,
                currGameOverColor,
                currScoreColor,
                "MAIN MENU",
                "QUIT"
            };
            int modes = sizeof(options) / sizeof(options[0]), spacing = ((screenHeight - 2 * boxOffset) - (modes)*fontSize) / (modes + 1);
            for (int i = 0; i < modes; i++)
            {
                DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, fontColor);
            }

            // Select menu
            DrawLine(
                (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                fontColor);
            EndDrawing();

            // Change mode
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_DOWN) || (IsKeyPressed(KEY_TAB) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode++;
            if (IsKeyPressed(KEY_UP) || (IsKeyPressed(KEY_TAB) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode--;
            currMode += GetMouseWheelMove();
            if (currMode < 0)
                currMode = modes - 1;
            if (currMode >= modes)
                currMode = 0;
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                switch (currMode)
                {
                case 0:
                    fps+=5;
                    if(fps>40) fps =5;
                    if(fps < 5) fps = 40;
                    break;
                case 1:
                    walls=!walls;
                    break;
                case 2:
                    selfHarm=!selfHarm;
                    break;
                case 3:
                    SetTargetFPS(fps);
                    custom = false;
                    start = true;
                case 7:
                    configure = false;
                    break;
                case 8:
                    CloseWindow();
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            BeginDrawing();
            ClearBackground(backgroundColor);

            DrawText("Use SPACE for next", (screenWidth - (MeasureText(TextFormat("Use SPACE for next"), fontSize * 1.5))) / 2, blockSize, fontSize * 1.5, fontColor);
            DrawText("Use ENTER to select", (screenWidth - (MeasureText(TextFormat("Use ENTER to select"), fontSize * 1.5))) / 2, blockSize + fontSize * 1.5, fontSize * 1.5, fontColor);
            DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);

            // Draw Menu
            const char *options[] = {
                "Tutorial",
                "EASY",
                "MEDIUM",
                "HARD",
                "SANDBOX",
                "CONFIG",
                "QUIT"};
            int modes = sizeof(options) / sizeof(options[0]), spacing = ((screenHeight - 2 * boxOffset) - (modes)*fontSize) / (modes + 1);
            for (int i = 0; i < modes; i++)
            {
                DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, fontColor);
            }

            // Select menu
            DrawLine(
                (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                fontColor);
            EndDrawing();

            // Change mode
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_DOWN) || (IsKeyPressed(KEY_TAB) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode++;
            if (IsKeyPressed(KEY_UP) || (IsKeyPressed(KEY_TAB) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode--;
            currMode += GetMouseWheelMove();
            if (currMode < 0)
                currMode = modes - 1;
            if (currMode >= modes)
                currMode = 0;
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                switch (currMode)
                {
                case 0:
                    break;
                case 1:
                    start = true;
                    walls = false;
                    selfHarm = true;
                    break;
                case 2:
                    start = true;
                    walls = true;
                    selfHarm = true;
                    break;
                case 3:
                    start = true;
                    walls = true;
                    selfHarm = true;
                    SetTargetFPS(20);
                    break;
                case 4:
                    custom = true;
                    break;
                case 5:
                    configure=true;
                    break;
                case 6:
                    CloseWindow();
                    break;
                default:
                    break;
                }
            }
        }
    }
    UnloadSound(gameOverSound);
    UnloadSound(biteSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}