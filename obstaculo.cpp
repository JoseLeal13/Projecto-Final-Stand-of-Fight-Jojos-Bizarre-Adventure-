#include "obstaculo.h"

obstaculo::obstaculo(float x, float y, float ancho, float alto, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setRect(0, 0, ancho, alto);
    setPos(x, y);
    setBrush(QBrush(Qt::darkGray));
}
