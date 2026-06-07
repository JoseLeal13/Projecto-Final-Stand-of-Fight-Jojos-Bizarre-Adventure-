#ifndef JOJOBRO_H
#define JOJOBRO_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <QPixmap>
#include <QImage>
#include <QTransform>
#include "steelball.h"



class Jojobro : public QGraphicsPixmapItem
{
public:
    // Definimos los estados para la exposición de POO
    enum EstadoIA {
        Tranquilo,  // Lanza una sola bola de vez en cuando
        Rafaga,     // Se tepea y tira de a 3 ráfagas
        Lluvia      // Modo presión, bolas seguidas pero manejables
    };

    Jojobro();
    EstadoIA getEstado() const { return estadoActual; }
    void setDificultad(bool esDificil);
    // Método clave que decide qué hacer según el tiempo restante
    void actualizarComportamiento(int tiempoRestante, qreal jotaroY, bool camaraLenta);

    // Getter para saber si Jojobro generó esferas en este frame
    QList<SteelBall*> tomarNuevasBolas();

private:
    EstadoIA estadoActual;
    int contadorAtaque; // Reemplaza al contador de la ventana principal

    // Lista temporal para pasarle las bolas creadas a la escena principal
    QList<SteelBall*> bolasGeneradas;


    // Frames de animación de ataque: 2 frames (apuntando + lanzando)
    // Los sprites del sheet apuntan a la derecha, así que uso el reflejado siempre
    // porque Jojobro en el juego siempre lanza hacia la izquierda
    QList<QPixmap> framesAtaqueIzquierda;
    int frameAtaqueActual;
    float contadorFrameAtaque;
    bool modoDificil = false;

    void cargarFramesAtaque();
    void actualizarAnimacion();
    QPixmap quitarFondo(const QPixmap &original);
    QPixmap reflejarHorizontal(const QPixmap &original);
    void dispararBola(SteelBall::TipoBola tipo, SteelBall::TipoTrayectoria trayec, int offsetY = 30);
};

#endif // JOJOBRO_H