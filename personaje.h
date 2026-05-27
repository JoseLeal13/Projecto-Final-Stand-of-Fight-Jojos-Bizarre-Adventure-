/*
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
*/
#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QList>
//#include "obstaculo.h"
#include <QTimer>
#include <QPainter>

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

    //nivel 1
    int speed; //velocidad de movimiento
    int direccion;//direccion  del movimiento

    QRectF hitbox;


public:
    Personaje(QGraphicsItem *parent = nullptr);

    static bool tiempoDetenido;
    static Personaje* electorDelTiempo;

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

    static bool modoDebug; // Variable para activar/desactivar hitboxes
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

};

#endif // PERSONAJE_H
