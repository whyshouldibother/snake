#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
using namespace std;
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
    bool check()
    {
        if (x > 800)
            x = 0;
        if (x < 0)
            x = 800;
        if (y > 600)
            y = 0;
        if (y < 0)
            y = 600;
        return true;
    }
    void update(int blocksize)
    {
        x += blocksize * directionX;
        y += blocksize * directionY;
    }
    bool grow(entity *food)
    {
        if (x == food->x && y == food->y)
        {
            food->x = rand() % 80 * 10;
            food->y = rand() % 60 * 10;
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

int randGen(int max, int blockSize)
{
    srand(time(0));
    return rand() % (max / blockSize) * blockSize;
}

int main()
{

    int screenWidth = 800, screenHeight = 600, fps = 10, blockSize = 10, fontSize = 24;
    InitWindow(screenWidth, screenHeight, "Snake");
    SetTargetFPS(fps);
    entity food(randGen(screenWidth, blockSize), randGen(screenHeight, blockSize));
    vector<entity> snake;
    snake.push_back(entity(randGen(screenWidth, blockSize), randGen(screenHeight, blockSize)));
    int score = 0;
    bool gameOver = false, start = false;
    while (!WindowShouldClose())
    {
        if (start)
        {

            if (!gameOver)
            {
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
                snake[0].check();

                // Check Growth
                if (snake[0].grow(&food))
                {
                    // Grow snake
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
                DrawText(TextFormat("Score\t%d", score), screenWidth - MeasureText(TextFormat("Score\t%d", score), fontSize), 0, fontSize, WHITE);
                DrawRectangle(food.x, food.y, 10, 10, RED);
                DrawRectangle(snake[0].x, snake[0].y, 10, 10, DARKGREEN);

                EndDrawing();
            }
            else
            {
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText(TextFormat("GameOver"), screenWidth / 2 - MeasureText(TextFormat("GameOver"), fontSize) / 2, (screenHeight / 2 - fontSize), fontSize, WHITE);
                EndDrawing();
            }
        }
        BeginDrawing();
        int boxOffset = blockSize * 15;
        DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, WHITE);

        string optins[] = {
            "EASY",
            "MEDIUM",
            "HARD",
            "CUSTOM",
            "CONFIG",
            "QUIT"
        };
        DrawText("EASY", screenWidth / 2 - MeasureText(TextFormat("EASY"), fontSize) / 2, boxOffset + blockSize + fontSize, fontSize, WHITE);
        DrawText("MEDIUM", screenWidth / 2 - MeasureText(TextFormat("MEDIUM"), fontSize) / 2, boxOffset + blockSize * 2 + fontSize, fontSize, WHITE);
        DrawText("HARD", screenWidth / 2 - MeasureText(TextFormat("HARD"), fontSize) / 2, boxOffset + blockSize * 3 + fontSize * 2, fontSize, WHITE);
        DrawText("CUSTOM", screenWidth / 2 - MeasureText(TextFormat("CUSTOM"), fontSize) / 2, boxOffset + blockSize * 4 + fontSize * 3, fontSize, WHITE);
        DrawText("CONFIG", screenWidth / 2 - MeasureText(TextFormat("CONFIG"), fontSize) / 2, boxOffset + blockSize * 5 + fontSize * 4, fontSize, WHITE);
        DrawText("QUIT", screenWidth / 2 - MeasureText(TextFormat("QUIT"), fontSize) / 2, boxOffset + blockSize * 6 + fontSize * 6, fontSize, WHITE);
        EndDrawing();
    }
    return 0;
}