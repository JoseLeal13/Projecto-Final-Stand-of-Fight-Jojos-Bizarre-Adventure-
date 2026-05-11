#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
#include <QBrush>

class obstaculo : public QGraphicsRectItem
{
public:
    // Solo declaramos el constructor
    obstaculo(float x, float y, float ancho, float alto);
};

#endif // OBSTACULO_H
