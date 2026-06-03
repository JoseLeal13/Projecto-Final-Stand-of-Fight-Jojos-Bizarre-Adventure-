#include "item.h"
#include <cmath>

ItemJuego::ItemJuego(TipoItem tipo, qreal posX, qreal posY)
    : tipoActual(tipo)
{
    setPos(posX, posY);
    cargarGrafico();
}

void ItemJuego::cargarGrafico()
{
    QPixmap sprites(":/sprites_juego.png");
    QPixmap frameRecortado;

    // Coordenadas calculadas y estandarizadas del Sprite Sheet
    if (tipoActual == Vida) {
        // Recorte del ítem de corazón / poción de salud
        frameRecortado = sprites.copy(1015, 60, 45, 45);
    } else {
        // Recorte del ítem de botas / aura celeste de velocidad
        frameRecortado = sprites.copy(1075, 60, 45, 45);
    }

    // Guardamos el sprite final ya procesado sin el fondo oscuro (30, 27, 60)
    spriteItem = quitarFondo(frameRecortado);
}

QPixmap ItemJuego::quitarFondo(const QPixmap &original)
{
    QImage img = original.toImage().convertToFormat(QImage::Format_ARGB32);
    QColor fondoColor(30, 27, 60);

    for (int y = 0; y < img.height(); y++) {
        for (int x = 0; x < img.width(); x++) {
            QColor px = img.pixelColor(x, y);
            // Tolerancia de color para remover imperfecciones de compresión del sprite
            if (std::abs(px.red()   - fondoColor.red())   < 12 &&
                std::abs(px.green() - fondoColor.green()) < 12 &&
                std::abs(px.blue()  - fondoColor.blue())  < 12)
            {
                img.setPixelColor(x, y, QColor(0, 0, 0, 0)); // Transparencia pura
            }
        }
    }
    return QPixmap::fromImage(img);
}

QRectF ItemJuego::boundingRect() const
{
    // Tamaño estándar para la caja de colisión del ítem en el mapa (45x45 px)
    return QRectF(0, 0, 45, 45);
}

void ItemJuego::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!spriteItem.isNull()) {
        painter->drawPixmap(0, 0, spriteItem);
    }

    // Dibujado opcional de Hitbox en modo Debug si fuese necesario
    // Para ver si Jotaro los pisa bien, descomenta las líneas de abajo si quieres:
    /*
    painter->save();
    painter->setPen(QPen(Qt::yellow, 1, Qt::DotLine));
    painter->drawRect(boundingRect());
    painter->restore();
    */
}