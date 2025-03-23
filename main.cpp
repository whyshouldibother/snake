#include "raylib.h"
#include "rlgl.h"
#include <cstdlib>
#include <random>
#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

// Randomizer
random_device rd;
mt19937 mt(rd());
// Return coordinates

int randGen(int max, int blockSize)
{
    uniform_int_distribution<int> dist(0, max / blockSize - 1);
    return (dist(rd) * blockSize);
}

class entity
{
public:
    //(x,y) = coordinates
    int x;
    int y;

    //(directionX, directionY) = direction vector
    int directionX = 0;
    int directionY = 0;

    entity() {};
    entity(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    // Check for walls
    bool check(int screenWidth, int screenHeight)
    {
        if (x >= screenWidth)
        {
            x = 0;
            return false;
        }
        else if (x < 0)
        {
            x = screenWidth;
            return false;
        }
        if (y >= screenHeight)
        {
            y = 0;
            return false;
        }
        else if (y < 0)
        {
            y = screenHeight;
            return false;
        }
        return true;
    }

    // Update entity in refrence to direction
    void update(int blocksize)
    {
        x += blocksize * directionX;
        y += blocksize * directionY;
    }

    // Grow entity if food is eaten
    bool grow(entity *food, int maxX, int maxY, int blockSize, vector<entity> snake, int score)
    {
        if (x == food->x && y == food->y)
        {
            bool maxScore = score >= (maxX * maxY) / (blockSize * blockSize) - 1;
        foodGen: // Regenerate food
            food->x = randGen(maxX, blockSize);
            food->y = randGen(maxY, blockSize);
            if (!maxScore)
                for (int i = 0; i < snake.size(); i++)
                    if (snake[i].x == food->x && snake[i].y == food->y)
                        goto foodGen;
            return true;
        }
        else
            return false;
    }

    // Compare two parts of snake for collision
    bool operator==(entity compare)
    {
        return (x == compare.x && y == compare.y);
    }
};

// Driver code
int main()
{

    // Window Config variables
    int screenWidth = 800, screenHeight = 600, fps = 10, blockSize = 10, boxOffset = blockSize * 15, fontSize = 24;

    // Menu control variables
    int currMode = 0, score = 0, colorSchemeIndex = 0, scoreBoardIndex = 0, menuMode = 6;

    // Game Flags
    bool gameOver = false, walls = false, selfHarm = true, recordScore = false;

    bool devAdv = false; // for dev

    int maxFps = 500;
    // Available Color Schemes
    const char *colorScheme[] = {
        "RIZZLER",
        "DARK1",
        "DARK2",
        "DARK3",
        "DARK4",
        "DARK5",
        "LIGHT",
    };

    // Window Default colors
    Color backgroundColor = Color{255, 252, 239, 255},
          fontColor = Color{92, 137, 157, 255},
          snakeHeadColor = Color{252, 194, 0, 255},
          snakeBodyColor = Color{92, 137, 157, 255},
          foodColor = Color{111, 60, 137, 255},
          gameOverColor = GREEN,
          scoreColor = PINK;

    // Initiliaze window and set default FPS
    InitWindow(screenWidth, screenHeight, TextFormat("Snake\t%dx%d", screenWidth, screenHeight));
    SetTargetFPS(fps);

    // Initialize Audio Devices
    InitAudioDevice();

    // Load Sound Effects
    Sound gameOverSound = LoadSound("assets/gameover.mp3"), biteSound = LoadSound("assets/bite.mp3");

    // Create Food Entity
    entity food(randGen(screenWidth, blockSize), randGen(screenHeight, blockSize));

    // Create Snake vector and add snake head to vector
    vector<entity> snake;
    snake.reserve(1000 * sizeof(entity));
    snake.push_back(entity(screenWidth / 2, screenHeight / 2));

    // Main game loop Run Until Close is triggered
    while (!WindowShouldClose())
    {
        // Show Score board
        if (menuMode == 0)
        {
            // Emtpy static vector to reduce file operations
            static vector<string> lines = vector<string>();
            string line;

            // On first instance while vector is empty push lines to vector
            if (lines.empty())
            {
                ifstream scoreBoard("scoreboard.dat");
                while (getline(scoreBoard, line))
                {
                    lines.push_back(line);
                }
                scoreBoard.close();
            }

            // Calculate Spacing
            int spacing = ((screenHeight - 2 * boxOffset) - 5 * fontSize) / 6;

            // Draw to game and clear background
            BeginDrawing();
            ClearBackground(backgroundColor);

            // Dispaly text
            DrawText("Use SPACE to scroll", (screenWidth - (MeasureText(TextFormat("Use SPACE to scroll"), fontSize * 1.5))) / 2, blockSize, fontSize * 1.5, fontColor);
            DrawText("Use ENTER to return", (screenWidth - (MeasureText(TextFormat("Use ENTER to return"), fontSize * 1.5))) / 2, blockSize + fontSize * 1.5, fontSize * 1.5, fontColor);

            // Show 7 entries at once
            for (int i = scoreBoardIndex; i < scoreBoardIndex + 7; i++)
            {
                DrawText(TextFormat("%s", lines[i % lines.size()].c_str()), (screenWidth - (MeasureText(TextFormat("%s", lines[i % lines.size()].c_str()), fontSize))) / 2, boxOffset + spacing * (i - scoreBoardIndex + 1) + fontSize * (i - scoreBoardIndex), fontSize, fontColor);
            }
            EndDrawing();

            // Check for enter to trigger quit scoreboard
            if (IsKeyPressed(KEY_ENTER))
            {
                lines.clear();
                menuMode = 6;
            }

            // Check for  mouse wheel or space to trigger mouse scroll
            scoreBoardIndex += GetMouseWheelMove();
            if (IsKeyPressed(KEY_SPACE) || IsKeyDown(KEY_SPACE))
            {
                scoreBoardIndex++;
            }
        }

        // Run game for menuModes 1 2 3 can be collapsed to one number
        else if (menuMode >= 1 && menuMode <= 3)
        {

            // While not gameover
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

                if (IsKeyPressed(KEY_K))
                {
                    devAdv = !devAdv;
                }
                // GameLogic

                // Hamilton circuit
                if (devAdv == true)
                {

                    if (snake[0].x >= screenWidth - blockSize && snake[0].directionX == 1 && snake[0].directionY == 0)
                    {
                        snake[0].directionX = 0;
                        snake[0].directionY = 1;
                    }
                    else if (snake[0].x >= screenWidth - blockSize && snake[0].directionX == 0 && snake[0].directionY == 1)
                    {
                        snake[0].directionX = -1;
                        snake[0].directionY = 0;
                    }
                    else if (snake[0].x <= blockSize && snake[0].directionX == -1 && snake[0].directionY == 0 && snake[0].y < screenHeight - blockSize)
                    {
                        snake[0].directionX = 0;
                        snake[0].directionY = 1;
                    }
                    else if (snake[0].x <= blockSize && snake[0].directionX == 0 && snake[0].directionY == 1)
                    {
                        snake[0].directionX = 1;
                        snake[0].directionY = 0;
                    }
                    else if (snake[0].x < blockSize && snake[0].directionX == -1 && snake[0].directionY == 0 && snake[0].y >= screenHeight - blockSize)
                    {
                        snake[0].directionX = 0;
                        snake[0].directionY = -1;
                    }
                    else if (snake[0].x < blockSize && snake[0].directionX == 0 && snake[0].directionY == -1 && snake[0].y < blockSize)
                    {
                        snake[0].directionX = 1;
                        snake[0].directionY = 0;
                    }
                }

                // Check for Walls
                if (!snake[0].check(screenWidth, screenHeight) && walls)
                {
                    PlaySound(gameOverSound);
                    gameOver = true;
                    recordScore = false;
                }

                // Check Growth
                if (snake[0].grow(&food, screenWidth, screenHeight, blockSize, snake, score))
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

                // Check for head and body collision
                if (selfHarm)
                    for (int i = 1; i < snake.size(); i++)
                        if (snake[0] == snake[i])
                        {

                            PlaySound(gameOverSound);
                            gameOver = true;
                            recordScore = false;
                            break;
                        }

                // Draw to game and clear backgrtound
                BeginDrawing();
                ClearBackground(backgroundColor);

                // Draw snake
                for (int i = 1; i < snake.size() - 1; i++)
                {

                    int offset[4] = {0, 0, 0, 0};
                    if (snake[i - 1].y == snake[i + 1].y)
                    {
                        offset[2] = 1;
                        offset[3] = 2;
                    }
                    else if (snake[i - 1].x == snake[i + 1].x)
                    {
                        offset[0] = 1;
                        offset[1] = 2;
                    }
                    else if ((snake[i + 1].directionX == 1 && snake[i].directionY == 1) || (snake[i + 1].directionY == -1 && snake[i].directionX == -1))
                    {
                        offset[1] = 1;
                        offset[2] = 1;
                    }
                    else if ((snake[i + 1].directionX == 1 && snake[i].directionY == -1) || (snake[i + 1].directionY == 1 && snake[i].directionX == -1))
                    {
                        offset[1] = 1;
                        offset[3] = 1;
                    }
                    else if ((snake[i + 1].directionY == 1 && snake[i].directionX == 1) || (snake[i + 1].directionX == -1 && snake[i].directionY == -1))
                    {
                        offset[0] = 1;
                        offset[3] = 1;
                    }
                    else if ((snake[i + 1].directionY == -1 && snake[i].directionX == 1) || (snake[i + 1].directionX == -1 && snake[i].directionY == 1))
                    {
                        offset[0] = 1;
                        offset[2] = 1;
                    }
                    DrawRectangle(snake[i].x + offset[0], snake[i].y + offset[2], blockSize - offset[1], blockSize - offset[3], snakeBodyColor);
                }

                if (snake.size() > 1)
                {

                    int offset[4] = {1, 1, 1, 1};
                    if (snake[snake.size() - 1].directionX == 1)
                    {
                        offset[1] = 0;
                    }
                    else if (snake[snake.size() - 1].directionX == -1)
                    {
                        offset[0] = 0;
                    }
                    else if (snake[snake.size() - 1].directionY == 1)
                    {
                        offset[3] = 0;
                    }
                    else if (snake[snake.size() - 1].directionY == -1)
                    {
                        offset[2] = 0;
                    }
                    DrawRectangle(snake[snake.size() - 1].x + offset[0], snake[snake.size() - 1].y + offset[2], blockSize - 2 * offset[1], blockSize - 2 * offset[3], snakeBodyColor);
                }
                // Display Score
                DrawText(TextFormat("Score:%d", score), screenWidth - MeasureText(TextFormat("Score:%d", score), fontSize), 0, fontSize, fontColor);

                // Draw Food
                DrawRectangle(food.x, food.y, blockSize, blockSize, foodColor);

                // Draw Snake Head
                DrawRectangle(snake[0].x, snake[0].y, blockSize, blockSize, snakeHeadColor);
                EndDrawing();
            }
            else
            {
                // Game Over

                // Record Score once
                if (!recordScore)
                {

                    fstream scoreBoard;
                    scoreBoard.open("scoreboard.dat", ios::app);
                    scoreBoard << "Screen\t" << screenWidth << "X" << screenHeight << "\tFPS\t" << fps << boolalpha << "\tWalls\t" << walls << "\tSelfHarm\t" << selfHarm << "\tScore\t" << score << endl;
                    scoreBoard.close();
                    recordScore = true;
                }
                // Gameover Menu
                const char *options[] = {
                    "PLAY AGAIN",
                    "MAIN MENU",
                    "QUIT"};
                int modes = sizeof(options) / sizeof(options[0]), spacing = ((screenHeight - 2 * boxOffset) - (modes)*fontSize) / (modes + 1);

                // Draw to game and clear background
                BeginDrawing();
                ClearBackground(backgroundColor);

                // Draw Dead Snake Body And Head
                for (int i = 1; i < snake.size() - 1; i++)
                {

                    int offset[4] = {0, 0, 0, 0};
                    if (snake[i - 1].y == snake[i + 1].y)
                    {
                        offset[2] = 1;
                        offset[3] = 2;
                    }
                    else if (snake[i - 1].x == snake[i + 1].x)
                    {
                        offset[0] = 1;
                        offset[1] = 2;
                    }
                    else if ((snake[i + 1].directionX == 1 && snake[i].directionY == 1) || (snake[i + 1].directionY == -1 && snake[i].directionX == -1))
                    {
                        offset[1] = 1;
                        offset[2] = 1;
                    }
                    else if ((snake[i + 1].directionX == 1 && snake[i].directionY == -1) || (snake[i + 1].directionY == 1 && snake[i].directionX == -1))
                    {
                        offset[1] = 1;
                        offset[3] = 1;
                    }
                    else if ((snake[i + 1].directionY == 1 && snake[i].directionX == 1) || (snake[i + 1].directionX == -1 && snake[i].directionY == -1))
                    {
                        offset[0] = 1;
                        offset[3] = 1;
                    }
                    else if ((snake[i + 1].directionY == -1 && snake[i].directionX == 1) || (snake[i + 1].directionX == -1 && snake[i].directionY == 1))
                    {
                        offset[0] = 1;
                        offset[2] = 1;
                    }
                    DrawRectangle(snake[i].x + offset[0], snake[i].y + offset[2], blockSize - offset[1], blockSize - offset[3], GRAY);
                }

                if (snake.size() > 1)
                {

                    int offset[4] = {1, 1, 1, 1};
                    if (snake[snake.size() - 1].directionX == 1)
                    {
                        offset[1] = 0;
                    }
                    else if (snake[snake.size() - 1].directionX == -1)
                    {
                        offset[0] = 0;
                    }
                    else if (snake[snake.size() - 1].directionY == 1)
                    {
                        offset[3] = 0;
                    }
                    else if (snake[snake.size() - 1].directionY == -1)
                    {
                        offset[2] = 0;
                    }
                    DrawRectangle(snake[snake.size()].x + offset[0], snake[snake.size()].y + offset[2], blockSize - 2 * offset[1], blockSize - 2 * offset[3], GRAY);
                }
                DrawRectangle(snake[0].x, snake[0].y, blockSize, blockSize, DARKGRAY);

                // Draw Outline
                DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);

                // Display Text
                DrawText(TextFormat("GAMEOVER"), (screenWidth - MeasureText(TextFormat("GAMEOVER"), fontSize * 3)) / 2, blockSize, fontSize * 3, gameOverColor);
                DrawText(TextFormat("Score:%d", score), (screenWidth - MeasureText(TextFormat("Score:%d", score), fontSize * 2.5)) / 2, blockSize * 2 + fontSize * 3, fontSize * 2.5, scoreColor);

                // Render Menu Items
                for (int i = 0; i < modes; i++)
                {
                    DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, fontColor);
                }

                // Draw Current option line
                DrawLine(
                    (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                    boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                    (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                    boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                    fontColor);

                // Check for  mouse wheel or space or tab or arrow keys to trigger mouse scroll
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_DOWN) || (IsKeyPressed(KEY_TAB) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                    currMode++;
                if (IsKeyPressed(KEY_UP) || (IsKeyPressed(KEY_TAB) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                    currMode--;
                currMode += GetMouseWheelMove();
                if (currMode < 0)
                    currMode = modes - 1;
                if (currMode >= modes)
                    currMode = 0;

                // Check for enter or mouse click to trigger menu select
                if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    switch (currMode)
                    {

                    // Play Again
                    case 0:

                        // Reset game state
                        score = 0;
                        gameOver = false;
                        snake.clear();
                        snake.push_back(entity(screenWidth / 2, screenHeight / 2));
                        break;

                    // Quit to main menu
                    case 1:

                        // Reset game state
                        score = 0;
                        gameOver = false;
                        menuMode = 6;
                        snake.clear();
                        break;

                    // Quit game
                    case 2:
                        CloseWindow();
                        break;
                    }
                }
                EndDrawing();
            }
        }

        // Configure settings for sandbox mode
        else if (menuMode == 4)
        {
            // Draw to game and clear backgrtound
            BeginDrawing();
            ClearBackground(backgroundColor);

            // Display text
            DrawText("Use SPACE for next", (screenWidth - (MeasureText(TextFormat("Use SPACE for next"), fontSize * 1.5))) / 2, blockSize, fontSize * 1.5, fontColor);
            DrawText("Use ENTER to change value", (screenWidth - (MeasureText(TextFormat("Use ENTER to change value"), fontSize * 1.5))) / 2, blockSize + fontSize * 1.5, fontSize * 1.5, fontColor);

            // Draw Outline
            DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);

            // Menu Items
            char currFps[16];
            sprintf(currFps, "FPS\t%d", fps);
            char currWalls[16];
            sprintf(currWalls, "Walls\t%s", walls ? "TRUE" : "FALSE");
            char currSelfHarm[16];
            sprintf(currSelfHarm, "Self Harm\t%s", selfHarm ? "TRUE" : "FALSE");
            const char *options[] = {
                currFps,
                currWalls,
                currSelfHarm,
                "PLAY",
                "MAIN MENU",
                "QUIT"};
            int modes = sizeof(options) / sizeof(options[0]), spacing = ((screenHeight - 2 * boxOffset) - (modes)*fontSize) / (modes + 1);

            // Render Menu Items
            for (int i = 0; i < modes; i++)
            {
                DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, fontColor);
            }

            // Draw Current option line
            DrawLine(
                (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                fontColor);
            EndDrawing();

            // Check for  mouse wheel or space or tab or arrow keys to trigger mouse scroll
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_DOWN) || (IsKeyPressed(KEY_TAB) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode++;
            if (IsKeyPressed(KEY_UP) || (IsKeyPressed(KEY_TAB) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode--;
            currMode += GetMouseWheelMove();
            if (currMode < 0)
                currMode = modes - 1;
            if (currMode >= modes)
                currMode = 0;

            // Check for enter or mouse click to trigger menu select
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                switch (currMode)
                {

                // Change FPS
                case 0:
                    fps %= maxFps;
                    fps += 5;
                    break;

                // Toggle Wall
                case 1:
                    walls = !walls;
                    break;

                // Toggle Self Harm
                case 2:
                    selfHarm = !selfHarm;
                    break;

                // Play
                case 3:
                    SetTargetFPS(fps);
                    menuMode = 2;
                    break;

                // Quit to main menu
                case 4:
                    menuMode = 6;
                    break;

                // Quit Game
                case 5:
                    CloseWindow();
                    break;
                }
            }
        }

        // Configure window options
        else if (menuMode == 5)
        {
            // Draw to game and clear background
            BeginDrawing();
            ClearBackground(backgroundColor);

            // Display text
            DrawText("Use SPACE for next", (screenWidth - (MeasureText(TextFormat("Use SPACE for next"), fontSize * 1.5))) / 2, blockSize, fontSize * 1.5, fontColor);
            DrawText("Use ENTER to change value", (screenWidth - (MeasureText(TextFormat("Use ENTER to change value"), fontSize * 1.5))) / 2, blockSize + fontSize * 1.5, fontSize * 1.5, fontColor);

            // Display outline
            DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);

            // Menu items
            char currColorScheme[32];
            sprintf(currColorScheme, "COLOR SCHEME\t%s", colorScheme[colorSchemeIndex]);
            char currBlockSize[32];
            sprintf(currBlockSize, "BLOCK SIZE\t%d", blockSize);
            char currScreenWidth[32];
            sprintf(currScreenWidth, "SCREEN WIDTH\t%d", screenWidth);
            char currScreenHeight[32];
            sprintf(currScreenHeight, "SCREEN HEIGHT\t%d", screenHeight);
            char currFontSize[32];
            sprintf(currFontSize, "FONT SIZE\t%d", fontSize);
            const char *options[] = {
                currColorScheme,
                currBlockSize,
                currScreenWidth,
                currScreenHeight,
                currFontSize,
                "MAIN MENU",
                "QUIT"};
            int modes = sizeof(options) / sizeof(options[0]), spacing = ((screenHeight - 2 * boxOffset) - (modes)*fontSize) / (modes + 1);

            // Render Menu Items
            for (int i = 0; i < modes; i++)
            {
                DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, fontColor);
            }

            // Draw Current option line
            DrawLine(
                (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                fontColor);
            EndDrawing();

            // Check for  mouse wheel or space or tab or arrow keys to trigger mouse scroll
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_DOWN) || (IsKeyPressed(KEY_TAB) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode++;
            if (IsKeyPressed(KEY_UP) || (IsKeyPressed(KEY_TAB) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode--;
            currMode += GetMouseWheelMove();
            if (currMode < 0)
                currMode = modes - 1;
            if (currMode >= modes)
                currMode = 0;

            // Check for enter or mouse click to trigger menu select
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                switch (currMode)
                {

                // Change Color Schemes
                case 0:
                    colorSchemeIndex++;
                    if (colorSchemeIndex >= sizeof(colorScheme) / sizeof(colorScheme[0]))
                        colorSchemeIndex = 0;

                    switch (colorSchemeIndex)
                    {

                        // Rizzler
                    case 0:
                        backgroundColor = Color{255, 252, 239, 255};
                        fontColor = Color{92, 137, 157, 255};
                        snakeHeadColor = Color{252, 194, 0, 255};
                        snakeBodyColor = Color{92, 137, 157, 255};
                        foodColor = Color{111, 60, 137, 255};
                        gameOverColor = GREEN;
                        scoreColor = PINK;
                        break;
                        // DARK1
                    case 1:
                        backgroundColor = BLACK;
                        fontColor = WHITE;
                        snakeHeadColor = DARKGREEN;
                        snakeBodyColor = GREEN;
                        foodColor = RED;
                        gameOverColor = RED;
                        scoreColor = ORANGE;
                        break;

                    case 2:
                        backgroundColor = BLACK;
                        fontColor = WHITE;
                        snakeHeadColor = DARKBLUE;
                        snakeBodyColor = BLUE;
                        foodColor = RED;
                        gameOverColor = RED;
                        scoreColor = ORANGE;
                        break;

                    // DARK3
                    case 3:
                        backgroundColor = BLACK;
                        fontColor = WHITE;
                        snakeHeadColor = DARKPURPLE;
                        snakeBodyColor = PURPLE;
                        foodColor = LIME;
                        gameOverColor = RED;
                        scoreColor = ORANGE;
                        break;

                    // DARK4
                    case 4:
                        backgroundColor = BLACK;
                        fontColor = WHITE;
                        snakeHeadColor = GREEN;
                        snakeBodyColor = BLUE;
                        foodColor = YELLOW;
                        gameOverColor = RED;
                        scoreColor = ORANGE;
                        break;

                    case 5:
                        // DARK5
                        backgroundColor = BLACK;
                        fontColor = WHITE;
                        snakeHeadColor = LIGHTGRAY;
                        snakeBodyColor = GRAY;
                        foodColor = DARKGRAY;
                        gameOverColor = DARKGRAY;
                        scoreColor = GRAY;
                        break;

                    // Light
                    case 6:
                        backgroundColor = WHITE;
                        fontColor = BLACK;
                        snakeHeadColor = DARKGREEN;
                        snakeBodyColor = GREEN;
                        foodColor = RED;
                        gameOverColor = RED;
                        scoreColor = ORANGE;
                        break;
                    }
                    break;
                // Toggle Block size
                case 1:
                    blockSize = blockSize == 10 ? 20 : 10;

                    // Regenerate food to factor in block size change
                    food.x = randGen(screenWidth, blockSize);
                    food.y = randGen(screenHeight, blockSize);
                    break;

                // Change Screen Width
                case 2:
                    screenWidth += 200;
                    if (screenWidth > 1200)
                        screenWidth = 600;

                    // Regenerate food to factor in screenwidth change
                    food.x = randGen(screenWidth, blockSize);

                    // Reintialize window
                    CloseWindow();
                    InitWindow(screenWidth, screenHeight, TextFormat("Snake\t%dx%d", screenWidth, screenHeight));
                    break;

                // Chnage Screen Height
                case 3:
                    screenHeight += 100;
                    if (screenHeight > 900)
                        screenHeight = 500;

                    // Regenerate food to factor in screenheight change
                    food.y = randGen(screenHeight, blockSize);

                    // Reintialize window
                    CloseWindow();
                    InitWindow(screenWidth, screenHeight, TextFormat("Snake\t%dx%d", screenWidth, screenHeight));
                    break;

                // Change Font Size
                case 4:
                    fontSize += 12;
                    if (fontSize > 36)
                        fontSize = 12;
                    break;

                // Quit to main menu
                case 5:
                    menuMode = 6;
                    break;

                // Quit Game
                case 6:
                    CloseWindow();
                    break;
                }
            }
        }

        // Main Menu
        else if (menuMode == 6)
        {
            // Draw to game and clear backgrtound
            BeginDrawing();
            ClearBackground(backgroundColor);

            // Display Text
            DrawText("Use SPACE for next", (screenWidth - (MeasureText(TextFormat("Use SPACE for next"), fontSize * 1.5))) / 2, blockSize, fontSize * 1.5, fontColor);
            DrawText("Use ENTER to select", (screenWidth - (MeasureText(TextFormat("Use ENTER to select"), fontSize * 1.5))) / 2, blockSize + fontSize * 1.5, fontSize * 1.5, fontColor);

            // Draw Outline
            DrawRectangleLines(boxOffset, boxOffset, screenWidth - boxOffset * 2, screenHeight - boxOffset * 2, fontColor);

            // Menu Items
            const char *options[] = {
                "SCOREBOARD",
                "EASY",
                "MEDIUM",
                "HARD",
                "SANDBOX",
                "CONFIG",
                "QUIT"};
            int modes = sizeof(options) / sizeof(options[0]), spacing = ((screenHeight - 2 * boxOffset) - (modes)*fontSize) / (modes + 1);

            // Render Menu Items
            for (int i = 0; i < modes; i++)
            {
                DrawText(TextFormat("%s", options[i]), (screenWidth - (MeasureText(TextFormat("%s", options[i]), fontSize))) / 2, boxOffset + spacing * (i + 1) + fontSize * (i), fontSize, fontColor);
            }

            // Draw Current option line
            DrawLine(
                (screenWidth - (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 - blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                (screenWidth + (MeasureText(TextFormat("%s", options[currMode]), fontSize))) / 2 + blockSize,
                boxOffset + spacing * (currMode + 1) + fontSize * (currMode) + fontSize,
                fontColor);
            EndDrawing();

            // Check for  mouse wheel or space or tab or arrow keys to trigger mouse scroll
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_DOWN) || (IsKeyPressed(KEY_TAB) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode++;
            if (IsKeyPressed(KEY_UP) || (IsKeyPressed(KEY_TAB) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))
                currMode--;
            currMode += GetMouseWheelMove();
            if (currMode < 0)
                currMode = modes - 1;
            if (currMode >= modes)
                currMode = 0;

            // Check for enter or mouse click to trigger menu select
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                switch (currMode)
                {

                // Show Scoreboard
                case 0:
                    menuMode = 0;
                    break;

                // Play on Easy
                case 1:
                    walls = false;
                    selfHarm = true;
                    menuMode = 1;
                    break;

                // Play on Medium
                case 2:
                    walls = true;
                    selfHarm = true;
                    menuMode = 2;
                    break;

                // Play on hard
                case 3:
                    walls = true;
                    selfHarm = true;
                    SetTargetFPS(20);
                    menuMode = 3;
                    break;

                // Configure Sandbox
                case 4:
                    menuMode = 4;
                    break;

                // Configure window options
                case 5:
                    menuMode = 5;
                    break;

                // Quit Game
                case 6:
                    CloseWindow();
                    break;
                default:
                    break;
                }
            }
        }
    }

    // Unload Assets and close game entirely
    UnloadSound(gameOverSound);
    UnloadSound(biteSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}