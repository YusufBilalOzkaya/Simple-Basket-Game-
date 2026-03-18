#include "myinclude.h"

using namespace std;
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

    // Bu fonksiyon top ile bu engelin çarpışmasını kontrol eder
    void UpdateCollision(Vector2 &ballPos, Vector2 &ballVel, float radius)
    {
        float collisionRadius = radius * 0.75f;
        if (CheckCollisionCircleRec(ballPos, collisionRadius, rect))
        {
            // Hangi yönden çarptığını anlamak için merkeze bakıyoruz
            if (ballPos.y < rect.y || ballPos.y > rect.y + rect.height)
            {
                ballVel.y *= -0.7f;
                // Topu dışarı it (Takılmayı önler)
                if (ballPos.y < rect.y)
                    ballPos.y = rect.y - collisionRadius;
                else
                    ballPos.y = rect.y + rect.height + collisionRadius;
            }
            else
            {
                ballVel.x *= -0.7f;
                // Topu dışarı it
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
int main()
{
    // --- 1. INITIALIZATION (BAŞLATMA) ---
    // Pencereyi açar ve grafik motorunu hazırlar.
    // Pota koordinatları (Örnek: Sağ yukarıda)
    vector<Obstacle> potaParcalari;
    // Panya ve Çember uçlarını ekleyelim
    // x    y    w    h
    potaParcalari.push_back(Obstacle(700, 50, 24, 400, DARKGRAY)); // Panya
    potaParcalari.push_back(Obstacle(585, 99, 15, 10, RED));       // Çember ucu 1
    potaParcalari.push_back(Obstacle(680, 99, 20, 10, RED));       // Çember ucu 2

    Rectangle fileGorselAlani = {585, 116, 80, 65}; // Resmindeki filenin üzerine denk gelecek şekilde ayarla

    Rectangle basketAlani = {614, 130, 30, 10}; // Çemberin ortasına hizala
    int skor = 0;
    float basketYazisiZamanlayici = 0.0f;
    Vector2 mousePosition = {0, 0};
    Vector2 PrvMousePosition = {0, 0};
    const int screenWidth = 800;
    int activeball = -1;
    bool ActiveWalls = 0;
    int i = 0;
    int j = 0;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "TRYİNG TO UNDERSTAND");
    Vector2 potaPos = {550, 90};                   // Resmin sol üst köşesi (Panya ve çemberle hizala)
    float potaScale = 0.3f;                        // 1.0 tam boyut, 0.5 yarı boyut, 0.1 on kat küçük demektir
    Texture2D potaResmi = LoadTexture("pota.png"); // Dosya adın neyse o (örn: image_2e1a0c.jpg)
    int daireAdet = 3;
    vector<Vector2> positions(daireAdet); // vector liste demek VE Vector2 türü ve positions adında daireAdet kadarlık bir liste olustur
    vector<Vector2> velocity(daireAdet);
    float rad = 30;
    float gravity = 0.7;

    // Oyunun hızını saniyede 60 kareye sabitler (Bilgisayarın fan açmasını önler).
    SetTargetFPS(60);
    for (i = 0; i < daireAdet; i++)
    {
        positions[i].y = 100;
        positions[i].x = 120 + (i * 60);
        velocity[i].y = 0;
    }

    // --- OYUN DÖNGÜSÜ (GAME LOOP) ---
    // Pencere kapatılana kadar (X tuşu veya ESC) bu döngü döner.
    while (!WindowShouldClose())
    {
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
                    activeball = i; // Sadece bu daireyi seç!
                    break;          // Birini seçtik, diğerlerine bakmaya gerek yok
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
        // Update döngüsünde:
        for (int i = 0; i < daireAdet; i++)
        {
            if (CheckCollisionCircleRec(positions[i], rad, basketAlani))
            {
                // Top yukarıdan aşağıya düşerken basket sayılması için:
                if (velocity[i].y > 0 && (positions[i].y < basketAlani.y) && basketYazisiZamanlayici <= 0)
                {
                    skor++;

                    basketYazisiZamanlayici = 1.5f;
                    // Burada bir efekt veya ses tetikleyebilirsin
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
            // zemin
            if (positions[i].y + rad >= 450)
            {
                positions[i].y = 450 - rad;
                velocity[i].y *= -0.7;
            }

            if (ActiveWalls)
            {
                // 1. SAĞ DUVAR
                if (positions[i].x + rad >= 800)
                {
                    positions[i].x = 800 - rad;
                    velocity[i].x *= -0.7f;
                }

                // 2. SOL DUVAR
                if (positions[i].x - rad <= 0)
                {
                    positions[i].x = rad;
                    velocity[i].x *= -0.7f;
                }

                // 3. ÜST DUVAR (Tavan)
                if (positions[i].y - rad <= 0)
                {
                    positions[i].y = rad;
                    velocity[i].y *= -0.7f;
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

        // --- 2. INPUT & UPDATE (GİRDİ VE GÜNCELLEME) ---
        // Burada hesaplamalar yapılır. Örneğin: "Eğer sağa basıldıysa X koordinatını artır."
        // Henüz ekrana bir şey çizilmez, sadece veriler güncellenir.

        // --- 3. DRAWING (ÇİZİM) ---
        // Ekran kartına "şimdi çizmeye başla" emri verilir.

        // Çizim fonksiyonları her zaman Begin ve End arasında olmalıdır.

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, GREEN);
        if (ActiveWalls)
        {
            DrawText("WALLS ACTIVATED", 40, 50, 20, GREEN);
        }

        for (auto &obs : potaParcalari) // pota panya uclarını gosterme
        {
            if (obs.rect.height > 100) // sadece potayı cizmek icin
            {
                obs.Draw();
            }
        }

        for (i = 0; i < daireAdet; i++)
        {
            DrawCircleV(positions[i], rad, ORANGE);
        }
        // TextFormat fonksiyonu sayıları metne çevirmene yarar
        DrawText(TextFormat("SKOR: %i", skor), 350, 20, 30, BLACK);

        // --- BASKET YAZISI ---
        if (basketYazisiZamanlayici > 0)
        {
            DrawText("BASKET!", 320, 150, 50, ORANGE);
        }
        DrawTextureEx(potaResmi, potaPos, 0.0f, potaScale, WHITE);
        // Çizim kısmında:
        DrawRectangleRec(basketAlani, Fade(GREEN, 0.3f)); // %30 görünür yeşil
        DrawRectangleRec(fileGorselAlani, Fade(BLUE, 0.3f));
        EndDrawing(); // Çizilenleri ekrana yansıtır.
    }
    // --- 4. DE-INITIALIZATION (KAPATMA) ---
    // Kullanılan belleği temizler ve pencereyi düzgünce kapatır.
    // main sonunda, CloseWindow'dan hemen önce:
    UnloadTexture(potaResmi);
    CloseWindow();

    return 0;
}
