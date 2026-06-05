#ifndef ITEM_H
#define ITEM_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QPainter>
#include <QImage>

class ItemJuego : public QGraphicsPixmapItem
{
public:
    enum TipoItem {
        Vida,       // Paquete de salud (Rojo/Rosa)
        Velocidad   // Botas de velocidad (Celeste/Azul)
    };

    ItemJuego(TipoItem tipo, qreal posX, qreal posY);

    // Identificador único de tipo para Qt Graphics View
    enum { Type = UserType + 5 };
    int type() const override { return Type; }

    TipoItem getTipo() const { return tipoActual; }

    // Métodos obligatorios de Qt para el dibujado y colisiones fijas
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void actualizarAnimacion(); // ← AGREGAR ESTA LÍNEA

private:


    TipoItem tipoActual;
    QPixmap spriteItem; // puedes dejarlo o quitarlo, ya no lo usaremos

    // ── ANIMACIÓN DE 6 FRAMES ──
    QList<QPixmap> framesAnimacion; // los 6 sprites del item
    int frameActual;
    int contadorFrames;

    void cargarGrafico();
    QPixmap quitarFondo(const QPixmap &original);

};

#endif // ITEM_H