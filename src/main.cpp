#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

int cellSize = 30;
int cellCount = 25;
int wind_x = cellSize * cellCount, wind_y = cellSize * cellCount;
Color LGreen = Color{173, 204, 96, 255};
Color DGreen = Color{43, 51, 24, 255};

double lastUpdateTime = 0;

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

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{x * cellSize, y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, DGreen);
        }
    };

    void Update()
    {
        body.pop_back();
        body.push_front(Vector2Add(body[0], direction));
    };
};

class Food
{
private:
    /* private data */
public:
    Vector2 position; /*x & y positions*/
    Texture2D texture;

    Food() /*constructor - used to initialize an object when it is created*/
    {
        Image image = LoadImage("graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRundomPos();
    };
    ~Food() /*destructor - responsible for releasing any resources the object occurred*/
    {
        UnloadTexture(texture);
    };

    void Draw()
    {
        // DrawRectangle(position.x * cellSize, position.y * cellSize, cellSize, cellSize, DGreen);
        DrawTexture(texture, position.x * cellSize, position.y * cellSize, WHITE);
    };

    Vector2 GenerateRundomPos()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    };
};

int main()
{
    cout << "--> Starting the game...\n";
    InitWindow(wind_x, wind_y, "My Snake Game");
    SetTargetFPS(60);

    Food food = Food();
    Snake snake = Snake();

    // Game Loop
    while (!WindowShouldClose())
    {
        // 1. Event Handling
        BeginDrawing();
        ClearBackground(LGreen);

        // 2. Updating positions
        if (EventTriggered(0.2))
        {
            snake.Update();
        }

        // 3. Drawing
        food.Draw();
        snake.Draw();
        DrawGride();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}