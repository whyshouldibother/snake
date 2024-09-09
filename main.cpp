#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

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
    void update()
    {
        x += 10 * directionX;
        y += 10 * directionY;
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

int main()
{
    InitWindow(800, 600, "Snake");
    SetTargetFPS(10);
    srand(time(0));
    entity food((rand() % (800 / 10) * 10), (rand() % (600 / 10) * 10));
    vector<entity> snake;
    snake.push_back(entity(400, 300));
    int score = 0;
    bool gameOver = false;
    while (!WindowShouldClose())
    {

        if (!gameOver)
        {
            // Controls
            if (IsKeyPressed(KEY_LEFT))
            {
                snake[0].directionX = -1;
                snake[0].directionY = 0;
            };
            if (IsKeyPressed(KEY_RIGHT))
            {
                snake[0].directionX = 1;
                snake[0].directionY = 0;
            };
            if (IsKeyPressed(KEY_UP))
            {
                snake[0].directionX = 0;
                snake[0].directionY = -1;
            };
            if (IsKeyPressed(KEY_DOWN))
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
            snake[0].update();

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
                DrawRectangle(snake[i].x, snake[i].y, 10, 10, GREEN);
            }

            ClearBackground(BLACK);
            DrawText(TextFormat("Head\t%d%d", snake[0].x, snake[0].y), 800 - MeasureText(TextFormat("Head\t%d%d", snake[0].x, snake[0].y), 24), 24, 24, WHITE);
            DrawText(TextFormat("Score\t%d", score), 800 - MeasureText(TextFormat("Score\t%d", score), 24), 0, 24, WHITE);
            DrawRectangle(food.x, food.y, 10, 10, RED);
            DrawRectangle(snake[0].x, snake[0].y, 10, 10, DARKGREEN);

            EndDrawing();
        }
        else
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText(TextFormat("GameOver"), 400 - MeasureText(TextFormat("GameOver"), 24 / 2), (300 - 24), 24, WHITE);
            EndDrawing();
        }
    }
    return 0;
}