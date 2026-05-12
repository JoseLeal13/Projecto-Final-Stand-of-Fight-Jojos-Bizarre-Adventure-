#include "personaje.h"

Personaje::Personaje(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    puntosdevida = 100;
    barradeCarga = 0;
    vx = 0;
    vy = 0;
    aceleracion_y = 0.8;
    enSuelo = false;
    hitbox = QRectF(8, 5, 30, 60);
}
bool Personaje::verificarColision(float proximoX, float proximoY)
{
    if (!scene()) {
        return false;
    }

    // 2. Creamos la hitbox en la posición futura
    QRectF rectFuturo = hitbox;
    rectFuturo.translate(proximoX, proximoY);

    // 3. Preguntamos a la escena
    QList<QGraphicsItem *> objetosEnRect = scene()->items(rectFuturo);

    for (QGraphicsItem *item : objetosEnRect) {
        if (item == this) continue;

        if (dynamic_cast<obstaculo*>(item)) {
            return true;
        }
    }

    return false;
}
