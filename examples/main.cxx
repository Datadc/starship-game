#include "starship/game.hxx"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <sstream>
#include <iomanip>

// Helper function to render text
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, 
                int x, int y, SDL_Color color = {255, 255, 255, 255}) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect rect{x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Starship Game",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Try to load system font (common paths on macOS)
    TTF_Font* font = nullptr;
    const char* fontPaths[] = {
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "Arial.ttf"
    };
    
    for (const char* path : fontPaths) {
        font = TTF_OpenFont(path, 24);
        if (font) break;
    }
    
    if (!font) {
        std::cerr << "Warning: Could not load font, using default rendering" << std::endl;
    }

    starship::Game game(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT));

    bool running = true;
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        lastTime = currentTime;
        float deltaTime = elapsed.count();
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode kc = event.key.keysym.sym;
                if (kc == SDLK_q || kc == SDLK_ESCAPE) {
                    running = false;
                }
                // Fire on space
                if (kc == SDLK_SPACE) {
                    game.handleInput(' ', deltaTime);
                }
            }
        }

        // Poll keyboard state for continuous actions
        const Uint8* state = SDL_GetKeyboardState(NULL);
        bool movingLeft = state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT];
        bool movingRight = state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT];
        
        if (movingLeft) {
            game.handleInput('a', deltaTime);
        } else if (movingRight) {
            game.handleInput('d', deltaTime);
        } else {
            // Stop moving when no keys pressed
            game.getPlayer().stopMoving();
        }
        
        if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
            game.handleInput('w', deltaTime);
        }

        // Update game logic
        game.update(deltaTime);

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw player as detailed rocket
        const auto& player = game.getPlayer();
        float px = player.getPosition().x;
        float py = player.getPosition().y;
        
        // Rocket: nose cone, body, fins, and flame
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White body
        
        // Nose cone (pointed tip)
        SDL_FPoint noseCone[3] = {
            {px, py - 18.0f},                   // sharp tip
            {px - 3.0f, py - 10.0f},            // left
            {px + 3.0f, py - 10.0f}             // right
        };
        SDL_RenderDrawLineF(renderer, noseCone[0].x, noseCone[0].y, noseCone[1].x, noseCone[1].y);
        SDL_RenderDrawLineF(renderer, noseCone[1].x, noseCone[1].y, noseCone[2].x, noseCone[2].y);
        SDL_RenderDrawLineF(renderer, noseCone[2].x, noseCone[2].y, noseCone[0].x, noseCone[0].y);
        
        // Main body (cylinder)
        SDL_RenderDrawLineF(renderer, px - 3.0f, py - 10.0f, px - 3.0f, py + 8.0f);
        SDL_RenderDrawLineF(renderer, px + 3.0f, py - 10.0f, px + 3.0f, py + 8.0f);
        SDL_RenderDrawLineF(renderer, px - 3.0f, py + 8.0f, px + 3.0f, py + 8.0f);
        
        // Left fin
        SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);  // Light blue fins
        SDL_FPoint leftFin[3] = {
            {px - 3.0f, py + 4.0f},
            {px - 10.0f, py + 10.0f},
            {px - 3.0f, py + 8.0f}
        };
        SDL_RenderDrawLineF(renderer, leftFin[0].x, leftFin[0].y, leftFin[1].x, leftFin[1].y);
        SDL_RenderDrawLineF(renderer, leftFin[1].x, leftFin[1].y, leftFin[2].x, leftFin[2].y);
        SDL_RenderDrawLineF(renderer, leftFin[2].x, leftFin[2].y, leftFin[0].x, leftFin[0].y);
        
        // Right fin
        SDL_FPoint rightFin[3] = {
            {px + 3.0f, py + 4.0f},
            {px + 10.0f, py + 10.0f},
            {px + 3.0f, py + 8.0f}
        };
        SDL_RenderDrawLineF(renderer, rightFin[0].x, rightFin[0].y, rightFin[1].x, rightFin[1].y);
        SDL_RenderDrawLineF(renderer, rightFin[1].x, rightFin[1].y, rightFin[2].x, rightFin[2].y);
        SDL_RenderDrawLineF(renderer, rightFin[2].x, rightFin[2].y, rightFin[0].x, rightFin[0].y);
        
        // Flame effect at base
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);    // Yellow flame
        SDL_FPoint flameYellow[3] = {
            {px - 2.0f, py + 8.0f},
            {px + 2.0f, py + 8.0f},
            {px, py + 15.0f}
        };
        SDL_RenderDrawLineF(renderer, flameYellow[0].x, flameYellow[0].y, flameYellow[1].x, flameYellow[1].y);
        SDL_RenderDrawLineF(renderer, flameYellow[1].x, flameYellow[1].y, flameYellow[2].x, flameYellow[2].y);
        SDL_RenderDrawLineF(renderer, flameYellow[2].x, flameYellow[2].y, flameYellow[0].x, flameYellow[0].y);
        
        // Inner orange flame
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);    // Orange
        SDL_FPoint flameOrange[3] = {
            {px - 1.0f, py + 9.0f},
            {px + 1.0f, py + 9.0f},
            {px, py + 12.0f}
        };
        SDL_RenderDrawLineF(renderer, flameOrange[0].x, flameOrange[0].y, flameOrange[1].x, flameOrange[1].y);
        SDL_RenderDrawLineF(renderer, flameOrange[1].x, flameOrange[1].y, flameOrange[2].x, flameOrange[2].y);
        SDL_RenderDrawLineF(renderer, flameOrange[2].x, flameOrange[2].y, flameOrange[0].x, flameOrange[0].y);

        // Draw asteroids as gray circles (approximated with lines as polygons)
        SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        for (const auto& asteroid : game.getAsteroids()) {
            float r = asteroid.getRadius();
            float ax = asteroid.getPosition().x;
            float ay = asteroid.getPosition().y;
            // Draw as circle using multiple line segments
            int segments = 12;
            for (int i = 0; i < segments; i++) {
                float angle1 = 2.0f * 3.14159f * i / segments;
                float angle2 = 2.0f * 3.14159f * (i + 1) / segments;
                float x1 = ax + r * std::cos(angle1);
                float y1 = ay + r * std::sin(angle1);
                float x2 = ax + r * std::cos(angle2);
                float y2 = ay + r * std::sin(angle2);
                SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
            }
        }

        // Draw projectiles as fire (small flame shapes - orange to yellow gradient) - LARGER
        for (const auto& projectile : game.getProjectiles()) {
            float prx = projectile.getPosition().x;
            float pry = projectile.getPosition().y;
            
            // Yellow flame tip (larger)
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_FPoint flame_tip[3] = {
                {prx, pry - 7.0f},               // tip (increased)
                {prx - 3.5f, pry + 3.0f},        // left (increased)
                {prx + 3.5f, pry + 3.0f}         // right (increased)
            };
            SDL_RenderDrawLineF(renderer, flame_tip[0].x, flame_tip[0].y, flame_tip[1].x, flame_tip[1].y);
            SDL_RenderDrawLineF(renderer, flame_tip[1].x, flame_tip[1].y, flame_tip[2].x, flame_tip[2].y);
            SDL_RenderDrawLineF(renderer, flame_tip[2].x, flame_tip[2].y, flame_tip[0].x, flame_tip[0].y);
            
            // Orange flame base
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
            SDL_FPoint flame_base[3] = {
                {prx - 2.5f, pry + 3.0f},
                {prx - 2.0f, pry + 6.0f},
                {prx + 2.0f, pry + 6.0f}
            };
            SDL_RenderDrawLineF(renderer, flame_base[0].x, flame_base[0].y, flame_base[1].x, flame_base[1].y);
            SDL_RenderDrawLineF(renderer, flame_base[1].x, flame_base[1].y, flame_base[2].x, flame_base[2].y);
            SDL_RenderDrawLineF(renderer, flame_base[2].x, flame_base[2].y, flame_base[0].x, flame_base[0].y);
        }

        // Draw screen boundaries (left and right)
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderDrawLineF(renderer, 0.0f, 0.0f, 0.0f, static_cast<float>(SCREEN_HEIGHT));
        SDL_RenderDrawLineF(renderer, static_cast<float>(SCREEN_WIDTH), 0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT));

        // Render score and lives at top center
        if (font) {
            std::ostringstream scoreStream;
            scoreStream << "Score: " << game.getScore();
            renderText(renderer, font, scoreStream.str(), SCREEN_WIDTH / 2 - 40, 10);
            
            std::ostringstream livesStream;
            livesStream << "Lives: " << game.getPlayer().getHealth();
            renderText(renderer, font, livesStream.str(), SCREEN_WIDTH / 2 - 40, 40);
            
            // Display game over message when lives exhausted
            if (game.isGameOver()) {
                SDL_Color redColor = {255, 0, 0, 255};
                std::string gameOverMsg = "GAME OVER";
                renderText(renderer, font, gameOverMsg, SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 40, redColor);
                
                std::ostringstream finalScoreStream;
                finalScoreStream << "Final Score: " << game.getScore();
                renderText(renderer, font, finalScoreStream.str(), SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2, redColor);
                
                std::ostringstream levelStream;
                levelStream << "Level Reached: " << game.getLevel();
                renderText(renderer, font, levelStream.str(), SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 40, redColor);
            }
        }

        SDL_RenderPresent(renderer);
        
        // Exit game when lives exhausted
        if (game.isGameOver()) {
            std::this_thread::sleep_for(std::chrono::seconds(3));  // Show final stats for 3 seconds
            running = false;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    // Print final game stats
    std::cout << std::endl;
    std::cout << "=== GAME OVER ===" << std::endl;
    std::cout << "Final Score: " << game.getScore() << std::endl;
    std::cout << "Level Reached: " << game.getLevel() << std::endl;
    std::cout << "Thank you for playing!" << std::endl;

    return 0;
}
