#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

int cellSize = 30;
int cellCount = 25;
int offset = 60;
int wind_x = cellSize * cellCount + offset * 2, wind_y = cellSize * cellCount + offset * 2;
Color LGreen = Color{173, 204, 96, 255};
Color DGreen = Color{43, 51, 24, 255};
double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element_position, deque<Vector2> snakeBody_deque) // return true if given element is in given deque
{
    for (unsigned i = 0; i < snakeBody_deque.size(); i++)
    {
        if (Vector2Equals(snakeBody_deque[i], element_position))
        {
            return true;
        }
    }
    return false;
};

bool EventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
};

void DrawGride()
{
    for (int i = 1; i < cellCount; i++)
    {
        DrawLine(i * cellSize, 0, i * cellSize, cellSize * cellCount, WHITE);
    }
    for (int i = 1; i < cellCount; i++)
    {
        DrawLine(0, i * cellSize, cellCount * cellSize, i * cellSize, WHITE);
    }
};

class Snake
{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;
    Texture2D bodyTexture;
    Texture2D headTexture;
    Texture2D tailTexture;

    Snake() /*constructor - used to initialize an object when it is created*/
    {
        Image bodyImage = LoadImage("graphics/snake-body.png");
        bodyTexture = LoadTextureFromImage(bodyImage);
        UnloadImage(bodyImage);
        Image headImage = LoadImage("graphics/snake-head.png");
        headTexture = LoadTextureFromImage(headImage);
        UnloadImage(headImage);
        Image tailImage = LoadImage("graphics/food.png");
        tailTexture = LoadTextureFromImage(tailImage);
        UnloadImage(bodyImage);
    };
    ~Snake() /*destructor - responsible for releasing any resources the object occurred*/
    {
        UnloadTexture(headTexture);
        UnloadTexture(bodyTexture);
        UnloadTexture(tailTexture);
    };

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    };

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            if (i == 0)
            {
                DrawTexture(headTexture, x * cellSize + offset, y * cellSize + offset, WHITE);
            }
            else if (i == body.size() - 1)
            {
                DrawTexture(tailTexture, x * cellSize + offset, y * cellSize + offset, WHITE);
            }
            else
                DrawTexture(bodyTexture, x * cellSize + offset, y * cellSize + offset, WHITE);
        }
    };

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    };
};

class Food
{
private:
    /* private data */
public:
    Vector2 position; /*x & y positions*/
    Texture2D texture;

    Food(deque<Vector2> snakeBody) /*constructor - used to initialize an object when it is created*/
    {
        Image image = LoadImage("graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    };
    ~Food() /*destructor - responsible for releasing any resources the object occurred*/
    {
        UnloadTexture(texture);
    };

    void Draw()
    {
        // DrawRectangle(position.x * cellSize, position.y * cellSize, cellSize, cellSize, DGreen);
        DrawTexture(texture, position.x * cellSize + offset, position.y * cellSize + offset, WHITE);
    };

    Vector2 GenerateRundomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    };

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRundomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRundomCell();
        }
        return position;
    };
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool gameRunning = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if (gameRunning)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        gameRunning = false;
        score = 0;
        PlaySound(wallSound);
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};


int main()
{
    // cout << "--> Starting the game...\n";
    InitWindow(wind_x, wind_y, "My Snake Game");
    SetTargetFPS(60);

    Game game = Game();

    // Game Loop
    while (!WindowShouldClose())
    {
        // 1. Event Handling
        BeginDrawing();

        // 2. Updating positions
        if (EventTriggered(0.2))
        {
            game.Update();
        }
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0, -1};
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1, 0};
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1, 0};
        }
        if (!game.gameRunning && IsKeyPressed(KEY_ENTER))
        {
            game.gameRunning = true;
        }

        if (!game.gameRunning)
        {
            DrawText("Game Over", 100, 100, 42, DGreen);
            DrawText("To Play Again Press ENTER", 100, 200, 42, DGreen);
        }

        // 3. Drawing
        ClearBackground(LGreen);
        game.Draw();
        DrawText(TextFormat("Score: %i", game.score), 60, 10, 32, DGreen);
        // DrawGride();
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellCount * cellSize + 10, (float)cellCount * cellSize + 10}, 5, DGreen);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}