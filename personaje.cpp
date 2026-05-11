#include "personaje.h"

Personaje::Personaje(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    puntosdevida = 100;
    barradeCarga = 0;
    vx = 0;
    vy = 0;
    aceleracion_y = 0.8;
    enSuelo = false;
}

bool Personaje::verificarColision(float proximoX, float proximoY)
{
    float xActual = x();
    float yActual = y();

    setPos(proximoX, proximoY);

    // Revisamos con qué está chocando en esa nueva posición
    QList<QGraphicsItem *> objetosChocando = collidingItems();

    for (QGraphicsItem *item : objetosChocando) {
        // Si el objeto con el que choca es de la clase obstaculo
        if (dynamic_cast<obstaculo*>(item)) {
            setPos(xActual, yActual); // Volvemos a la posición segura
            return true; // Confirmamos que hay un choque
        }
    }

    setPos(xActual, yActual);
    return false;
}
