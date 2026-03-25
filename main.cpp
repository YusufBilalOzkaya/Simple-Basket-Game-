#include "myinclude.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

using namespace std;

// ==========================================
// CLASS
// ==========================================
class Obstacle
{
public:
    Rectangle rect;
    Color color;

    Obstacle(float x, float y, float w, float h, Color c)
    {
        rect = {x, y, w, h};
        color = c;
    }

    void UpdateCollision(Vector2 &ballPos, Vector2 &ballVel, float radius)
    {
        float collisionRadius = radius * 0.75f;
        if (CheckCollisionCircleRec(ballPos, collisionRadius, rect))
        {
            if (ballPos.y < rect.y || ballPos.y > rect.y + rect.height)
            {
                ballVel.y *= -0.4f;
                if (ballPos.y < rect.y)
                    ballPos.y = rect.y - collisionRadius;
                else
                    ballPos.y = rect.y + rect.height + collisionRadius;
            }
            else
            {
                ballVel.x *= -0.4f;
                if (ballPos.x < rect.x)
                    ballPos.x = rect.x - collisionRadius;
                else
                    ballPos.x = rect.x + rect.width + collisionRadius;
            }
        }
    }

    void Draw()
    {
        DrawRectangleRec(rect, color);
    }
};

// ==========================================
// GLOBAL DEĞİŞKENLER
// ==========================================
vector<Obstacle> potaParcalari;
Rectangle fileGorselAlani = {585, 116, 80, 65};
Rectangle basketAlani = {614, 130, 30, 10};
int skor = 0;
float basketYazisiZamanlayici = 0.0f;
Vector2 mousePosition = {0, 0};
Vector2 PrvMousePosition = {0, 0};
int activeball = -1;
bool ActiveWalls = 0;
int i = 0;
int j = 0;
Vector2 potaPos = {550, 90};
float potaScale = 0.3f;
Texture2D potaResmi;
int daireAdet = 3;
vector<Vector2> positions;
vector<Vector2> velocity;
float rad = 30;
float gravity = 0.7;

// ==========================================
// GAME LOOP
// ==========================================
void GameLoop()
{
    // --- INPUT & UPDATE ---
    if (IsKeyPressed(KEY_W))
    {
        ActiveWalls = !ActiveWalls;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mousePosition = GetMousePosition();
        for (int i = 0; i < daireAdet; i++)
        {
            if (CheckCollisionPointCircle(mousePosition, positions[i], rad))
            {
                activeball = i;
                break;
            }
        }
    }
    mousePosition = GetMousePosition();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && activeball != -1)
    {
        Vector2 MouseVelocity = {mousePosition.x - PrvMousePosition.x, mousePosition.y - PrvMousePosition.y};
        positions[activeball] = mousePosition;
        velocity[activeball] = MouseVelocity;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        activeball = -1;
    }
    PrvMousePosition = mousePosition;

    for (i = 0; i < daireAdet; i++)
    {
        for (j = i + 1; j < daireAdet; j++)
        {
            float mesafe = Vector2Distance(positions[i], positions[j]);
            if (mesafe < rad * 2)
            {
                Vector2 temp = velocity[i];
                velocity[i] = velocity[j];
                velocity[j] = temp;
                float overlap = (rad * 2) - mesafe;
                Vector2 itmeYonu = {
                    (positions[i].x - positions[j].x) / mesafe,
                    (positions[i].y - positions[j].y) / mesafe
                };
                positions[i].x += itmeYonu.x * overlap / 2.0f;
                positions[i].y += itmeYonu.y * overlap / 2.0f;
                positions[j].x -= itmeYonu.x * overlap / 2.0f;
                positions[j].y -= itmeYonu.y * overlap / 2.0f;
            }
        }
    }

    for (int i = 0; i < daireAdet; i++)
    {
        if (CheckCollisionCircleRec(positions[i], rad, basketAlani))
        {
            if (velocity[i].y > 0 && (positions[i].y < basketAlani.y) && basketYazisiZamanlayici <= 0)
            {
                skor++;
                basketYazisiZamanlayici = 1.5f;
            }
        }
    }

    if (basketYazisiZamanlayici > 0)
        basketYazisiZamanlayici -= GetFrameTime();

    for (i = 0; i < daireAdet; i++)
    {
        velocity[i].y += gravity;
        positions[i].y += velocity[i].y;
        positions[i].x += velocity[i].x;

        if (positions[i].y + rad >= 450)
        {
            positions[i].y = 450 - rad;
            velocity[i].y *= -0.4;
        }

        if (ActiveWalls)
        {
            if (positions[i].x + rad >= 800)
            {
                positions[i].x = 800 - rad;
                velocity[i].x *= -0.4f;
            }
            if (positions[i].x - rad <= 0)
            {
                positions[i].x = rad;
                velocity[i].x *= -0.4f;
            }
            if (positions[i].y - rad <= 0)
            {
                positions[i].y = rad;
                velocity[i].y *= -0.4f;
            }
        }
    }

    for (int i = 0; i < daireAdet; i++)
    {
        for (auto &obs : potaParcalari)
        {
            obs.UpdateCollision(positions[i], velocity[i], rad);
        }
    }

    // --- DRAWING ---
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, GREEN);
    if (ActiveWalls)
    {
        DrawText("WALLS ACTIVATED", 40, 50, 20, GREEN);
    }
    for (auto &obs : potaParcalari)
    {
        if (obs.rect.height > 100)
        {
            obs.Draw();
        }
    }
    for (i = 0; i < daireAdet; i++)
    {
        DrawCircleV(positions[i], rad, ORANGE);
    }
    DrawText(TextFormat("SKOR: %i", skor), 350, 20, 30, BLACK);
    if (basketYazisiZamanlayici > 0)
    {
        DrawText("BASKET!", 320, 150, 50, ORANGE);
    }
    DrawTextureEx(potaResmi, potaPos, 0.0f, potaScale, WHITE);
    //DrawRectangleRec(basketAlani, Fade(GREEN, 0.3f)); yeşil alan
    //DrawRectangleRec(fileGorselAlani, Fade(BLUE, 0.3f)); mavi alan
    EndDrawing();
}

// ==========================================
// MAIN
// ==========================================
int main()
{
    potaParcalari.push_back(Obstacle(700, 50, 24, 400, DARKGRAY));
    potaParcalari.push_back(Obstacle(585, 99, 15, 10, RED));
    potaParcalari.push_back(Obstacle(680, 99, 20, 10, RED));

    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "TRYİNG TO UNDERSTAND");
    potaResmi = LoadTexture("pota.png");
    positions.resize(daireAdet);
    velocity.resize(daireAdet);
    SetTargetFPS(60);

    for (i = 0; i < daireAdet; i++)
    {
        positions[i].y = 100;
        positions[i].x = 120 + (i * 60);
        velocity[i].y = 0;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(GameLoop, 0, 1);
#else
    while (!WindowShouldClose()) { GameLoop(); }
    UnloadTexture(potaResmi);
    CloseWindow();
#endif

    return 0;
}
