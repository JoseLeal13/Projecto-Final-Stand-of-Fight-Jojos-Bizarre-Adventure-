#include "personaje.h"

// Inicialización de variables estáticas obligatorias de vuestra arquitectura
bool Personaje::tiempoDetenido = false;
Personaje* Personaje::electorDelTiempo = nullptr;
bool Personaje::modoDebug = false;

// Constructor corregido para que coincida con la declaración del .h
Personaje::Personaje(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    puntosdevida = 100;
    barradeCarga = 0;
    vx = 0;
    vy = 0;
    aceleracion_y = 0;
    enSuelo = true;

    speed = 4;
    direccion = 1;

    // Inicializamos una hitbox por defecto
    hitbox = QRectF(0, 0, 70, 100);
}

// Implementación del método de colisión lógica que declaraste en tu .h
// personaje.cpp — SOLO AQUÍ, no en jotaro.cpp
bool Personaje::verificarColision(float proximoX, float proximoY)
{
    if (proximoX < 0)   return true;  // borde izquierdo
    if (proximoX > 730) return true;  // borde derecho
    if (proximoY < 0)   return true;  // borde arriba
    if (proximoY > 500) return true;  // borde abajo
    return false;
}

// Implementación obligatoria del método paint que declaraste en tu .h
void Personaje::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Este método lo dejas llamando al paint base o vacío si tus hijos (jotaro)
    // se encargan de su propio dibujado.
    QGraphicsPixmapItem::paint(painter, option, widget);
}