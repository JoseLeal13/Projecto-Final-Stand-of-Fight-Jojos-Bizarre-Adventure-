#include "steelball.h"

SteelBall::SteelBall(int x, int y)
{
    this->x = x;
    this->y = y;

    destruida = false;
}

QRect SteelBall::getHitbox()
{
    return QRect(x, y, 40, 40);
}

void SteelBall::recibirGolpe()
{
    destruida = true;
}