#ifndef STEELBALL_H
#define STEELBALL_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QList>
#include <QPixmap>
#include <cmath>
#include <QImage>

class SteelBall : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    enum TipoBola {
        VerdeGolpeable,
        RojaEsquivable
    };

    enum TipoTrayectoria {
        Recta,
        Parabolica,
        Senoidal
    };

    SteelBall(TipoBola tipo, TipoTrayectoria trayectoria, qreal posX, qreal posY, int direccionX);

    TipoBola getTipo() const { return tipoActual; }
    QRectF getHitbox() { return boundingRect(); }

    void recibirGolpe();
    void avanzarFisica();

private:
    TipoBola tipoActual;
    TipoTrayectoria trayectoriaActual;

    void cargarGraficos();
    QPixmap quitarFondo(const QPixmap &original);

    // Variables de movimiento y física
    qreal velocidadX;
    qreal velocidadY;
    int dirX;

    qreal tiempoInterno;
    qreal posYInicial;

    // ── NUEVAS VARIABLES PARA LA ANIMACIÓN DE LA BOLA ──
    QList<QPixmap> framesAnimacion; // Contenedor para los 4 sprites de giro
    int frameActual;                // Índice del frame que se está mostrando (0 a 3)
    int contadorFrames;             // Tick interno de frames
    int retardoFrames;              // Velocidad de giro

};

#endif // STEELBALL_H