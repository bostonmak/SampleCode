#include "pch.h"

/// --------------
/// From vAlpha0.1
/// --------------
/// Known Bugs:
///
/// Bug Fixes:
///
/// * When Y Velocity reaches >350 it resets to 0
///

Projectile::Projectile()
{
    x = 0;
    y = GraphicsDevice::sInstance->getScreenHeight();
    w = 0;
    h = 0;

    trueX = 0;
    trueY = GraphicsDevice::sInstance->getScreenHeight();

    xVel = 0;
    yVel = 0;

    shot = false;
    falling = false;

    rotation = 0;
    gravity = 2.5;
    angle = 0;

    firstProjectile = GraphicsDevice::sInstance->load_image("images/projectilesmall.png");
    rotatingProjectile = NULL;

}

Projectile::~Projectile()
{
    //dtor
}

void Projectile::show_object(Cannon* cannon)
{
    // Center Projectile on Screen
    if (trueY < (GraphicsDevice::sInstance->getScreenHeight() / 1.5))
    {
        y = GraphicsDevice::sInstance->getScreenHeight() / 3;
    }
    if (x > (GraphicsDevice::sInstance->getScreenWidth() / 2))
    {
        x = GraphicsDevice::sInstance->getScreenWidth() / 2;
    }

    // Initialize Rotated Surface of Projectile
    rotatingProjectile = firstProjectile;
    rotatingProjectile = rotozoomSurfaceXY(rotatingProjectile, rotation, 1, 1, SMOOTHING_ON);
    GraphicsDevice::sInstance->apply_surface(x, y, w, h, rotatingProjectile, GraphicsDevice::sInstance->getGameScreen());

    /*
    The height of the grass so that after the projectile lands,
    it lands on the ground. Also, after landing on the ground
    the projectile stops rotating.
    */

    if (y >= GraphicsDevice::sInstance->getScreenHeight() - cannon->getHeight())
    {
        y = GraphicsDevice::sInstance->getScreenHeight() - cannon->getHeight();
        yVel = 0;
        state = 1;

    }
    else
    {
        x += xVel;
        rotation += -10 * (FPSCounter::sInstance->update.getTicks() / 1000.f) - 10;
    }

    if (state == 1)
    {
        if (yVel == 0 && trueY >= GraphicsDevice::sInstance->getScreenHeight() - cannon->getHeight())
        {
            // bounceProjectile();
            cannonPower = cannonPower / 2;
            xVel = xVel / 2;
            if (cannonPower <= 1)
            {
                xVel = 0;
                yVel = 0;
            }
            else
            {
                yVel = cannonPower * -1;
            }
        }
    }

    move_object(cannon);
}

void Projectile::move_object(Cannon *cannon)
{
    //time = (FPSCounter::sInstance->update.getTicks() / 1000.f);

    // Make the projectile fly
    trueX += xVel * (FPSCounter::sInstance->update.getTicks() / 1000.f);
    y += yVel * (FPSCounter::sInstance->update.getTicks() / 1000.f);
    trueY += yVel * (FPSCounter::sInstance->update.getTicks() / 1000.f);

    // Terminal Velocity & Applying Gravity
    if (yVel > 350)
    {
        yVel = 350;
    }
    else
    {
        if (y < GraphicsDevice::sInstance->getScreenHeight() - cannon->getHeight())
        {
            yVel = yVel + (gravity) * (FPSCounter::sInstance->update.getTicks() / 1000.f);
        }
    }

    // Falling Boolean
    if (yVel > 0)
    {
        falling = true;
    }
    else
    {
        falling = false;
    }
}

void Projectile::clear_surface()
{
    SDL_FreeSurface(firstProjectile);
    SDL_FreeSurface(rotatingProjectile);
}

void Projectile::shootProjectile(float rot, Cannon* cannon)
{
    shot = true;
    angle = rot + 45;
    cannonPower = cannon->getPower();
    float hypotenuse = sqrtf((cannon->getWidth()*cannon->getWidth()) + (cannon->getHeight()*cannon->getHeight()));
    float radians = (rot + 45.f) * 3.14159 / 180;
    float newX = hypotenuse*cosf(radians);
    float newY = hypotenuse*sinf(radians) * (-1);

    newX += 33;
    newY += GraphicsDevice::sInstance->getScreenHeight() - cannon->getHeight();

    trueY = GraphicsDevice::sInstance->getScreenHeight() - cannon->getHeight();
    x = newX;
    y = newY;
    xVel = (cannonPower*cosf(radians));
    yVel = (cannonPower*sinf(radians))*(-1);

    cannonPower = (cannonPower*sinf(radians));
}

bool Projectile::isShot()
{
    return shot;
}

void Projectile::resetGame()
{
    x = 0;
    y = GraphicsDevice::sInstance->getScreenHeight();
    w = 0;
    h = 0;

    trueX = 0;
    trueY = GraphicsDevice::sInstance->getScreenHeight();

    xVel = 0;
    yVel = 0;

    shot = false;
    falling = false;

    rotation = 0;
    gravity = 2.5;
    angle = 0;
}
