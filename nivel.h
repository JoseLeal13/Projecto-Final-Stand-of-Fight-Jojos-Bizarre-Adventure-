#ifndef NIVEL_H
#define NIVEL_H

#include <QGraphicsScene>
#include <QTimer>
#include <QObject>
#include "jojo.h"
#include "efectovisual.h"

class Nivel : public QObject {
    Q_OBJECT

protected:
    QGraphicsScene* escena;   // Escena física compartida de la ventana principal
    QTimer* gameLoopTimer;    // El reloj que maneja los ticks de 16ms
    Jojo* jojo;          // El protagonista persistente

public:
    // El constructor recibe la escena y el puntero al personaje principal
    Nivel(QGraphicsScene* escenaCompartida, Jojo* personajeJojo, QObject* parent = nullptr)
        : QObject(parent), escena(escenaCompartida), jojo(personajeJojo) {

        gameLoopTimer = new QTimer(this);
    }

    // Destructor virtual para asegurar la correcta liberación de memoria en las clases hijas
    virtual ~Nivel() {
        if (gameLoopTimer->isActive()) {
            gameLoopTimer->stop();
        }
    }

    Jojo* getJugador() const { return jojo; }

    // ==========================================
    // INTERFAZ POLIMÓRFICA (Métodos Virtuales Puros)
    // ==========================================
    virtual void iniciarNivel() = 0;
    virtual bool verificarCondicionVictoria() = 0;
    virtual bool verificarCondicionDerrota() = 0;
    virtual void limpiarNivel() = 0;

protected slots:
    // Slot obligatorio que procesará el Game Loop de cada nivel de manera independiente
    virtual void actualizarLoop() = 0;
};

#endif // NIVEL_H
