#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <algorithm>  // for std::remove

struct Bullet {
    SDL_FRect rect;
    float speed = 5.0f;
};

bool isColliding(const SDL_FRect& a, const SDL_FRect& b) {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

std::string callPythonAI(float px, float py, float ex, float ey, const std::vector<Bullet>& bullets) {
    std::ostringstream oss;
    // Build escaped JSON string
    oss << "\"{ ";
    oss << "\\\"player\\\": [" << px << ", " << py << "], ";
    oss << "\\\"enemy\\\": [" << ex << ", " << ey << "], ";
    oss << "\\\"bullets\\\": [";

    for (size_t i = 0; i < bullets.size(); ++i) {
        oss << "[" << bullets[i].rect.x << ", " << bullets[i].rect.y << "]";
        if (i != bullets.size() - 1) oss << ", ";
    }

    oss << "] }\"";

    std::string input = oss.str();
    std::string command = "python D:/SmartBotArena/PythonAI/bot_model.py " + input;

    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return "stay";

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);

    // ✂️ Trim trailing whitespace/newline
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    return result;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    const int screenWidth = 800;
    const int screenHeight = 600;

    SDL_Window* window = SDL_CreateWindow("SmartBot Arena", screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_FRect player = {100, 100, 50, 50};
    SDL_FRect enemy  = {600, 300, 50, 50};
    const float playerSize = 50.0f;
    const float playerSpeed = 5.0f;
    bool running = true;
    int frameCount = 0;

    std::vector<Bullet> bullets;
    int bulletCooldown = 0;

    bool spacePressedLastFrame = false; // For key debounce
    int frameCounter = 0;

    while (running) {
        SDL_Event event;
        SDL_PumpEvents(); // Required before GetKeyboardState in SDL3

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        const bool* keyboard = SDL_GetKeyboardState(nullptr); // SDL3 returns bool*, not Uint8*
        if (keyboard[SDL_SCANCODE_UP]) player.y -= playerSpeed;
        if (keyboard[SDL_SCANCODE_DOWN]) player.y += playerSpeed;
        if (keyboard[SDL_SCANCODE_LEFT]) player.x -= playerSpeed;
        if (keyboard[SDL_SCANCODE_RIGHT]) player.x += playerSpeed;

        // 🌀 Wraparound logic
        if (player.x + playerSize < 0) player.x = screenWidth;
        if (player.x > screenWidth)    player.x = -playerSize;
        if (player.y + playerSize < 0) player.y = screenHeight;
        if (player.y > screenHeight)   player.y = -playerSize;

        // 🧠 Enemy AI movement (basic)
        float baseSpeed = 20.0f;
        float enemySpeed = baseSpeed + (frameCount / 1000.0f);  // slowly ramps up
        if (frameCounter++ % 10 == 0) {
            std::string action = callPythonAI(player.x, player.y, enemy.x, enemy.y, bullets);
            std::cout << "[AI Action] " << action << std::endl;
            if (action == "left")  enemy.x -= enemySpeed;
            if (action == "right") enemy.x += enemySpeed;
            if (action == "up")    enemy.y -= enemySpeed;
            if (action == "down")  enemy.y += enemySpeed;
        }

        // 🎯 Bullet shooting
        bool spacePressed = keyboard[SDL_SCANCODE_SPACE];
        if (bulletCooldown == 0 && spacePressed && !spacePressedLastFrame) {
            Bullet b;
            b.rect = {
                player.x + player.w / 2.0f - 5,
                player.y + player.h / 2.0f - 5,
                10.0f,
                10.0f
            };
            bullets.push_back(b);
            bulletCooldown = 20; // 20 frames = ~0.33s
        }
        if (bulletCooldown > 0) bulletCooldown--;
        spacePressedLastFrame = spacePressed;

        // 🔄 Update bullets
        for (auto it = bullets.begin(); it != bullets.end();) {
            it->rect.x += it->speed;

            if (it->rect.x > screenWidth) {
                it = bullets.erase(it);
                continue;
            }

            // 💥 Check collision with enemy
            if (isColliding(it->rect, enemy)) {
                std::cout << "Player Wins! Bullet hit enemy.\n";
                running = false;
                break;
            }

            ++it;
        }

        // 🔥 Collision check
        if (isColliding(player, enemy)) {
            std::cout << "Game Over! Player collided with enemy." << std::endl;
            running = false;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // Draw enemy (red)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &enemy);

        // Draw player
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &player);

        // Yellow bullets
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (const auto& b : bullets) {
            SDL_RenderFillRect(renderer, &b.rect);
        }

        // Present frame
        SDL_RenderPresent(renderer);
        frameCount++;
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}