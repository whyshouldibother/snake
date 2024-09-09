#include "raylib.hpp"
#include <cstdlib>
#include <random>
#include <iostream>
#include <vector>
#include <string>
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

int main()
{

    int screenWidth = 800, screenHeight = 600, fps = 10, blockSize = 10, boxOffset = blockSize * 15, fontSize = 24, currMode = 0, score = 0, difficulty;
    InitWindow(screenWidth, screenHeight, "Snake");
    InitAudioDevice();
    Sound gameOverSound = LoadSound("assets/gameover.mp3"), biteSound = LoadSound("assets/bite.mp3");
    SetTargetFPS(fps);
    entity food(randGen(screenWidth, blockSize), randGen(screenHeight, blockSize));
    vector<entity> snake;
    snake.push_back(entity(randGen(screenWidth, blockSize), randGen(screenHeight, blockSize)));
    bool gameOver = false, start = false;
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
                if (snake[0].check(screenWidth, screenHeight) == false && difficulty > 0)
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
                    DrawRectangle(snake[i].x, snake[i].y, blockSize, blockSize, GREEN);
                }

                ClearBackground(BLACK);
                DrawText(TextFormat("Score:%d", score), screenWidth - MeasureText(TextFormat("Score:%d", score), fontSize), 0, fontSize, WHITE);
                DrawRectangle(food.x, food.y, 10, 10, RED);
                DrawRectangle(snake[0].x, snake[0].y, 10, 10, DARKGREEN);

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
                ClearBackground(BLACK);
                for (int i = 1; i < snake.size(); i++)
                {
                    DrawRectangle(snake[i].x, snake[i].y, blockSize, blockSize, GRAY);
                }
                DrawRectangle(snake[0].x, snake[0].y, 10, 10, DARKGRAY);
                DrawText(TextFormat("GAMEOVER"), (screenWidth - MeasureText(TextFormat("GAMEOVER"), fontSize * 3)) / 2, blockSize, fontSize * 3, RED);
                DrawText(TextFormat("Score:%d", score), (screenWidth - MeasureText(TextFormat("Score:%d", score), fontSize * 2.5)) / 2, blockSize * 2 + fontSize * 3, fontSize * 2.5, ORANGE);
                DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, WHITE);
                for (int i = 0; i < modes; i++)
                {
                    DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, WHITE);
                }

                // Select option
                DrawLine(
                    (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                    boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                    (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                    boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                    WHITE);

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
        else
        {
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("Use SPACE for next", (screenWidth - (MeasureText(TextFormat("Use SPACE for next"), fontSize * 1.5))) / 2, blockSize, fontSize * 1.5, WHITE);
            DrawText("Use ENTER to select", (screenWidth - (MeasureText(TextFormat("Use ENTER to select"), fontSize * 1.5))) / 2, blockSize + fontSize * 1.5, fontSize * 1.5, WHITE);
            DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, WHITE);

            // Draw Menu
            const char *options[] = {
                "Tutorial",
                "EASY",
                "MEDIUM",
                "HARD",
                "CUSTOM",
                "CONFIG",
                "QUIT"};
            int modes = sizeof(options) / sizeof(options[0]), spacing = ((screenHeight - 2 * boxOffset) - (modes)*fontSize) / (modes + 1);
            for (int i = 0; i < modes; i++)
            {
                DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, WHITE);
            }

            // Select menu
            DrawLine(
                (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                WHITE);
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
                    difficulty = 0;
                    break;
                case 2:
                    start = true;
                    difficulty = 1;
                    break;
                case 3:
                    start = true;
                    difficulty = 2;
                    SetTargetFPS(20);
                    break;
                case 4:
                    start = true;
                    break;
                case 5:
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