#ifndef STANDUSERSTATS_H
#define STANDUSERSTATS_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QBrush>
#include <QPen>
#include <QFont>
#include "personaje.h"

class StandUserStats {
private:
    // Componentes para el Jugador 1 (Jotaro - Esquina Inferior Izquierda)
    QGraphicsRectItem* jojoFondoVida;
    QGraphicsRectItem* jojoBarraVida;
    QGraphicsRectItem* jojoFondoCarga;
    QGraphicsRectItem* jojoBarraCarga;
    QGraphicsTextItem* jojoTexto;

    // Componentes para el Jugador 2 (DIO - Esquina Inferior Derecha)
    QGraphicsRectItem* dioFondoVida;
    QGraphicsRectItem* dioBarraVida;
    QGraphicsRectItem* dioFondoCarga;
    QGraphicsRectItem* dioBarraCarga;
    QGraphicsTextItem* dioTexto;

    // Dimensiones de las barras en píxeles
    const int ANCHO_BARRA_VIDA = 250;
    const int ALTO_BARRA_VIDA = 20;
    const int ANCHO_BARRA_CARGA = 200;
    const int ALTO_BARRA_CARGA = 12;

public:
    StandUserStats(QGraphicsScene* scene);
    void actualizarEstados(Personaje* jojo, Personaje* dio);
};

#endif // STANDUSERSTATS_H
