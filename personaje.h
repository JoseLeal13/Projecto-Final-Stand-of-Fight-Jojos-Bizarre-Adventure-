#ifndef PERSONAJE_H
#define PERSONAJE_H
#include <QRect>

class Personaje
{
protected:
    int x;
    int y;


    int speed;
    int anchoHitbox;
    int altoHitbox;
    int direccion;

public:
    Personaje();

    // movimiento
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    // getters
    int getX();
    int getY();
    //hitbox
    QRect getHitbox();

    bool colisionaCon(QRect otraHitbox);

    void setDireccion(int dir);
    int getDireccion();
};


#endif // PERSONAJE_H
