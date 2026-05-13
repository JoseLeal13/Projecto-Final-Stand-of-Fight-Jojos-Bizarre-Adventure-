#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QList>
#include "obstaculo.h"
#include <QTimer>

class Personaje : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
protected:

    int puntosdevida;
    int barradeCarga;

    float vx;
    float vy;
    float aceleracion_y;
    bool enSuelo;

    QRectF hitbox;


public:
    Personaje(QGraphicsItem *parent = nullptr);

    // Métodos de lógica de colisión
    bool verificarColision(float proximoX, float proximoY);

    virtual void atacar() = 0;
    virtual void moverse() = 0;
    virtual void habilidadEspecial() = 0;
    void aumentarCarga(int cantidad) {
        barradeCarga += cantidad;
        if (barradeCarga > 100) barradeCarga = 100; // Límite máximo
    }

    void setVida(int v) { puntosdevida = v; }
    int getVida() { return puntosdevida; }
    int getCarga() { return barradeCarga; }

};

#endif // PERSONAJE_H
