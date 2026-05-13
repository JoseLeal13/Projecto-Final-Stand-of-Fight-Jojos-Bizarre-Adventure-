#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
#include <QBrush>

class obstaculo : public QGraphicsRectItem
{
public:
    obstaculo(float x, float y, float ancho, float alto, QGraphicsItem *parent = nullptr);
    enum { Type = UserType + 1 };
    int type() const override { return Type; }
};

#endif // OBSTACULO_H
