#ifndef STEELBALL_H
#define STEELBALL_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QList>
#include <QPixmap>
#include <cmath>
#include <QImage>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem> // Añadido para evitar errores con la firma de paint

class SteelBall : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    // Le indicamos a Qt las interfaces que estamos usando para las colisiones
    Q_INTERFACES(QGraphicsItem)

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

    // CORRECCIÓN: Se agrega 'const' para que sea compatible con funciones constantes
    QRectF getHitbox() const { return boundingRect(); }

    void recibirGolpe();
    void avanzarFisica();

    // Métodos gráficos y de colisión nativos de QGraphicsPixmapItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const override;

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

    // ── VARIABLES PARA LA ANIMACIÓN DE LA BOLA ──
    QList<QPixmap> framesAnimacion; // Contenedor para los 4 sprites de giro
    int frameActual;                // Índice del frame que se está mostrando (0 a 3)
    int contadorFrames;             // Tick interno de frames
    int retardoFrames;
    // Velocidad de giro
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

};

#endif // STEELBALL_H