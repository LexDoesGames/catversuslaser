#include "raylib.h"

int randomNumberExclude(int min, int max, int excludeMin, int excludeMax) { // Set a random number. Used so that the gnat doesn't reappear too close to the cursor
    int num;
    do {
        num = GetRandomValue(min, max);
    } while (num >= excludeMin && num <= excludeMax);
    return num;
}

int main(void) {
    // Variable init
    const int screenWidth =  640;
    const int screenHeight = 480; // Ahhh... good ol' fashioned 480p.
    
    Vector2 gnatPos;
    Vector2 mousePos;
    
    double startTime; // Timer variable
    double period; // How long the gnat takes to react to your cursor moving within its radius
    
    int i;
    int speed;
    int score;
    int highscore = 0;
    int misses;
    int radius; // How close your cursor needs to be for the gnat to move
    int xTo; // gnat movement variables
    int yTo;
    
    bool timerRunning = false;
    bool gnatMoving = false;
    
    Rectangle button;
    button.x = 241;
    button.y = 320;
    button.width = 156;
    button.height = 45;
    
    typedef enum GameScreen { TITLE, GAMEPLAY, GAMEOVER, TUTORIAL } GameScreen;
    
    // Window init
    InitWindow(screenWidth, screenHeight, "Cat versus Laser");
    
    // Load the images into RAM...
    // 188 80
    Image bg_i = LoadImage("resources/bg.png");
    Image paw1_i = LoadImage("resources/paw1.png");
    Image x1_i = LoadImage("resources/x1.png");
    Image x2_i = LoadImage("resources/x2.png");
    Image logo_i = LoadImage("resources/logo.png");
    Image gameover_i = LoadImage("resources/gameover.png");
    
    // ...move them into VRAM...
    Texture2D bg = LoadTextureFromImage(bg_i);
    Texture2D paw1 = LoadTextureFromImage(paw1_i);
    Texture2D x1 = LoadTextureFromImage(x1_i);
    Texture2D x2 = LoadTextureFromImage(x2_i);
    Texture2D logo = LoadTextureFromImage(logo_i);
    Texture2D gameover = LoadTextureFromImage(gameover_i);
    
    // and unload the images from RAM.
    UnloadImage(bg_i);
    UnloadImage(paw1_i);
    UnloadImage(x1_i);
    UnloadImage(x2_i);
    UnloadImage(logo_i);
    UnloadImage(gameover_i);
    
    GameScreen currentScreen = TITLE;
    
    InitAudioDevice();
    
    Sound strike = LoadSound("resources/strike.wav");
    Sound success = LoadSound("resources/success.wav");
    Sound fail = LoadSound("resources/fail.wav");
    Sound meow = LoadSound("resources/meow1.wav");
    Sound meow2 = LoadSound("resources/meow2.wav");
    
    Sound loop = LoadSound("resources/loop.ogg");
    
    SetTargetFPS(60);
    
    DisableCursor();
    
    // Main game loop
    while (!WindowShouldClose()) {    // Detect window close button or ESC key
        mousePos = GetMousePosition();
        if (IsMouseButtonPressed(0) == true) {
            if (!IsSoundPlaying(strike)) {
                PlaySound(strike);
                }
            }
        switch(currentScreen) {
            case GAMEPLAY: {
                if (CheckCollisionPointCircle(mousePos, gnatPos, radius) == true && !timerRunning && !gnatMoving) { // Check if the cursor is within radius of the gnat
                    startTime = GetTime(); // Timer script
                    timerRunning = true;
                }
                if (timerRunning == true && GetTime() - startTime >= period && !gnatMoving) { // Detect to see if a certain period of time has passed
                    timerRunning = false;
                    if (misses <= 1) {
                        PlaySound(fail);
                    }
                    misses++;
                    if (misses > 2) {
                        if (score > highscore){
                            highscore = score;
                        }
                        PlaySound(meow2);
                        currentScreen = GAMEOVER;
                    }
                    i = 0;
                    speed = 16;
                    xTo = (randomNumberExclude(16, 600, mousePos.x - 72, mousePos.x + 72) - gnatPos.x) / speed;
                    yTo = (randomNumberExclude(72, 368, mousePos.y - 72, mousePos.y + 72) - gnatPos.y) / speed;
                    gnatMoving = true;
                }
                if ((CheckCollisionPointCircle(mousePos, gnatPos, 32) == true) && (IsMouseButtonPressed(0) == true) && !gnatMoving) { // Detect if the gnat has been clicked
                    timerRunning = false;
                    // Increment everything
                    if (!IsSoundPlaying(success)) {
                        PlaySound(success);
                    }
                    misses = 0; 
                    score++;
                    if (radius < 64) { // Make sure the game doesn't become complete Hell
                        radius += 0.75;
                    }
                    if (period > 0.1) { // Ditto but for the period variable
                        period -= 0.008;
                    }
                    i = 0;
                    speed = 4;
                    xTo = (randomNumberExclude(16, 600, mousePos.x - 72, mousePos.x + 72) - gnatPos.x) / speed;
                    yTo = (randomNumberExclude(72, 368, mousePos.y - 72, mousePos.y + 72) - gnatPos.y) / speed;
                    gnatMoving = true;
                }
                if (!IsSoundPlaying(loop)) { // Loop the music
                    PlaySound(loop);
                }
            } break;
            case TITLE: {
                if (IsMouseButtonReleased(0) == true && CheckCollisionPointRec(mousePos, button)) {
                    gnatPos.x = randomNumberExclude(16, 600, mousePos.x - 72, mousePos.x + 72);
                    gnatPos.y = randomNumberExclude(72, 368, mousePos.y - 72, mousePos.y + 72); // Reset/initialize everything and start the game
                    period = 0.5;
                    score = 0;
                    misses = 0;
                    radius = 36;
                    gnatMoving = false;
                    PlaySound(meow);

                    currentScreen = TUTORIAL;
                }
            } break;
            case GAMEOVER: {
                StopSound(loop);
                if (IsMouseButtonReleased(0) == true && CheckCollisionPointRec(mousePos, button)) {  
                    PlaySound(meow);
                    currentScreen = TITLE;
                }
            } break;
            case TUTORIAL: {
                if (IsMouseButtonReleased(0) == true && CheckCollisionPointRec(mousePos, button)) {
                    PlaySound(meow);
                    PlaySound(loop);
                    currentScreen = GAMEPLAY;
                }
            } break;
            default: break;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(bg, 0, 0, WHITE);
            switch(currentScreen) {
                case GAMEPLAY: {
                    DrawCircle(gnatPos.x + GetRandomValue(-2, 2), gnatPos.y + GetRandomValue(-2, 2), 5, RED);
                    if (gnatMoving == true) { // Movement scripts
                        gnatPos.x += xTo;
                        gnatPos.y += yTo;
                        i++;
                        if (i > speed - 1) {
                            gnatMoving = false;
                        }
                    }
                    DrawText(TextFormat("Score: %i", score), 8, 8, 20, WHITE);
                    if (misses > 0) {
                        DrawTexture(x2, 8, 36, WHITE);
                    } else {
                        DrawTexture(x1, 8, 36, WHITE);
                    }
                    if (misses > 1) {
                        DrawTexture(x2, 48, 36, WHITE);
                    } else {
                        DrawTexture(x1, 48, 36, WHITE);
                    }
                    DrawTexture(x1, 88, 36, WHITE);
                } break;
                case TITLE: {
                    DrawTexture(logo, 188, 80, WHITE);
                    DrawText("(C)2024 lexdoes.neocities.org", 8, 8, 20, WHITE);
                    DrawText("Music by Kevin MacLeod (incompetech.com)", 8, 32, 10, WHITE);
                    DrawText("Licensed under CC BY 3.0 license", 8, 44, 10, WHITE);
                    if (!CheckCollisionPointRec(mousePos, button)) { // Button scripts
                        DrawRectangle(241, 320, 156, 45, BLUE);
                    } else {
                        DrawRectangle(241, 320, 156, 45, SKYBLUE);
                    }
                    DrawRectangleLines(241, 320, 156, 45, BLACK);
                    DrawRectangleLines(242, 321, 154, 43, BLACK);
                    DrawText("Start", 263, 324, 40, WHITE);
                } break;
                case GAMEOVER: {
                    DrawTexture(gameover, 188, 80, WHITE);
                    DrawText(TextFormat("Your Score: %i", score), 8, 8, 20, WHITE);
                    DrawText(TextFormat("High Score: %i", highscore), 8, 32, 20, WHITE);
                    if (!CheckCollisionPointRec(mousePos, button)) { // Button scripts
                        DrawRectangle(241, 320, 156, 45, BLUE);
                    } else {
                        DrawRectangle(241, 320, 156, 45, SKYBLUE);
                    }
                    DrawRectangleLines(241, 320, 156, 45, BLACK);
                    DrawRectangleLines(242, 321, 154, 43, BLACK);
                    DrawText("Back", 270, 324, 40, WHITE);
                } break;
                case TUTORIAL: {
                    DrawText("- Click on the red dot as fast as you can to score points.", 8, 106, 20, WHITE);
                    DrawText("- The red dot is very sensitive to your paw moving near it.", 8, 128, 20, WHITE);
                    DrawText("- It will get away if you get too close.", 8, 152, 20, WHITE);
                    DrawText("- Let it get away three times in a row and it's game over!", 8, 176, 20, WHITE);
                    DrawText("- The red dot gets faster as the game goes on.", 8, 200, 20, WHITE);
                    DrawText("- Go for a high score!", 8, 224, 20, WHITE);
                    if (!CheckCollisionPointRec(mousePos, button)) { // Button scripts
                        DrawRectangle(241, 320, 156, 45, BLUE);
                    } else {
                        DrawRectangle(241, 320, 156, 45, SKYBLUE);
                    }
                    DrawRectangleLines(241, 320, 156, 45, BLACK);
                    DrawRectangleLines(242, 321, 154, 43, BLACK);
                    DrawText("Start", 263, 324, 40, WHITE); 
                } break;
                default: break;
            }
            if (IsCursorOnScreen() == true) {
                HideCursor();
                if (!IsSoundPlaying(strike)) {
                    DrawTexture(paw1, mousePos.x - 24, mousePos.y - 16, WHITE);
                } else {
                    DrawTexture(paw1, mousePos.x - 40, mousePos.y - 32, WHITE);
                }
            }
        EndDrawing();
    }
    UnloadTexture(bg);
    UnloadTexture(paw1);
    UnloadTexture(x1);
    UnloadTexture(x2);
    UnloadTexture(logo);
    UnloadTexture(gameover);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}