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
    if (!scene()) return false;

    QRectF rectFuturo = hitbox;
    rectFuturo.translate(proximoX, proximoY);

    QList<QGraphicsItem *> objetosEnRect = scene()->items(rectFuturo);

    for (QGraphicsItem *item : objetosEnRect) {
        if (item == this) continue;

        // ESTO ES MUCHO MÁS SEGURO QUE DYNAMIC_CAST
        if (item->type() == UserType + 1) {
            return true;
        }
    }
    return false;
}
