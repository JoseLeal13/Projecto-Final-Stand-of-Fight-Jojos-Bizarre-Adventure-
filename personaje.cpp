
#include "personaje.h"


Personaje::Personaje()
{
    x = 600;
    y = 250;

    speed = 4;
    anchoHitbox = 40;
    altoHitbox  = 70;
    direccion = 1;

}

void Personaje::moveUp()
{
    y -= speed;
}

void Personaje::moveDown()
{
    y += speed;
}

void Personaje::moveLeft()
{
    x -= speed;
}

void Personaje::moveRight()
{
    x += speed;
}

int Personaje::getX()
{
    return x;
}

int Personaje::getY()
{
    return y;
}

QRect Personaje::getHitbox()
{
    return QRect(x, y, anchoHitbox, altoHitbox);
}

bool Personaje::colisionaCon(QRect otraHitbox)
{
    return getHitbox().intersects(otraHitbox);
}

void Personaje::setDireccion(int dir)
{
    direccion = dir;
}

int Personaje::getDireccion()
{
    return direccion;
}