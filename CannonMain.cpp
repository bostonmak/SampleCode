#include "pch.h"

// Event
SDL_Event event;

Cannon* cannon;
Projectile* projectile;
Background* background;

bool load_files()
{
    // Load the Background
    background = new Background();
    if (background == NULL)
    {
        return false;
    }

    // Load the Cannon
    cannon = new Cannon();
    if (cannon == NULL)
    {
        return false;
    }

    // Load the Object
    projectile = new Projectile();
    if (projectile == NULL)
    {
        return false;
    }

    return true;
}

void exit()
{
    // Free the Images
    background->clear_surface();
    cannon->clear_surface();
    projectile->clear_surface();
    Interface::sInstance->clear_surface();
    TTF_CloseFont(GraphicsDevice::sInstance->getFont());

    // Quit True Type Font
    TTF_Quit();

    // Quit SDL
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    bool quit = false;

    // Run Initialize Function
    if (GraphicsDevice::sInstance->init() == false)
    {
        std::cout << "ERROR: Could not Initialize." << std::endl;
    }

    if (FPSCounter::sInstance->init() == false)
    {
        std::cout << "Error: Could not FPS." << std::endl;
    }

    if (Interface::sInstance->init() == false)
    {
        std::cout << "ERROR: Could not Initialize." << std::endl;
    }

    // Load all Files
    if (load_files() == false)
    {
        std::cout << "ERROR: Could not Load Files." << std::endl;
    }

    // Event Handling
    while (quit == false)
    {
        // Apply Background to Game Screen
        if (background)
        {
            background->show_object(projectile, cannon);
        }

        if (Interface::sInstance)
        {
            Interface::sInstance->calculateMeter(cannon, projectile);

            // User Toggled Cannon Info Meters
            if (Interface::sInstance->interfaceIsShown())
            {
                Interface::sInstance->show_object(cannon);
            }
        }

        // User Toggled FPS Counter
        if (FPSCounter::sInstance->isShown())
        {
            GraphicsDevice::sInstance->apply_surface(GraphicsDevice::sInstance->getScreenWidth() / 1.25, 0, 0, 0, FPSCounter::sInstance->getFpsCounter(), GraphicsDevice::sInstance->getGameScreen());
        }

        if (GameManager::sInstance)
        {
            if (GameManager::sInstance->enemyList.size() < 4)
            {
                Enemy* enemy = new Enemy();
                enemy->setX(GameManager::sInstance->enemyList.back()->getX() + (rand() % 100) + 25);
                enemy->setY(projectile->getTrueY() * -1 + (rand() % 50));
                enemy->setImage("images/public enemy number 1.jpeg");
                enemy->setBounceAmount(500);
                GameManager::sInstance->enemyList.push_back(enemy);
            }
            for (int x = 0; x < GameManager::sInstance->enemyList.size(); x++)
            {
                GameManager::sInstance->enemyList[x]->show_object();
                GameManager::sInstance->enemyList[x]->move_object(projectile);
                if (GameManager::sInstance->enemyList[x]->getX() < -25)
                {
                    Enemy* e = GameManager::sInstance->enemyList[x];
                    GameManager::sInstance->enemyList.erase(GameManager::sInstance->enemyList.begin() + x);
                    e->destroy();
                    free(e);
                    break;
                }
            }
        }

        // Apply Cannon to Game Screen
        if (cannon)
        {
            cannon->show_object(projectile);
            // Check Launch State of Cannon
            switch(cannon->getLaunchState())
            {
                case 1: cannon->setPower();
            }

        }

        // Draw Projectile to Game Screen
        if (projectile->isShot())
        {
            projectile->show_object(cannon);
        }

        // Update the Screen
        if (SDL_Flip(GraphicsDevice::sInstance->getGameScreen()) == -1)
        {
            return 1;
        }

        // Timer Functions
        FPSCounter::sInstance->updateFPS();
        FPSCounter::sInstance->delayFrame();

        // Event Polling
        while (SDL_PollEvent(&event))
        {
            // When Window is Closed
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            // When a Key is Pressed
            if (event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:   quit = true; break;
                    case SDLK_SPACE:    cannon->runAction(projectile);
                                        break;
                    case SDLK_f:        FPSCounter::sInstance->toggleFps();
                                        break;
                    case SDLK_h:        Interface::sInstance->toggleInterface();
                                        break;
                    case SDLK_r:        cannon->resetGame();
                                        projectile->resetGame();
                                        background->resetGame();
                                        break;
                }
            }
        }

    }
    if (quit == true)
    {
        exit();
    }

    return 0;
}
