#include "item.h"
#include <cmath>
/*
 * QPixmap sprites("C:\\Users\\Emmanuel\\Documents\\DESAFIOIII\\items.png");
*/
// ── CONSTANTES DEL SPRITE SHEET DE ITEMS ──
static const int X_INICIO_VIDA      = 30;   // X inicial corazón
static const int X_INICIO_VELOCIDAD = 46;   // X inicial sombrero
static const int Y_VIDA             = 132;  // Y fila corazón
static const int Y_VELOCIDAD        = 678;  // Y fila sombrero ← nuevo
static const int ANCHO_FRAME_VIDA   = 220;  // ancho frame corazón
static const int ANCHO_FRAME_VEL    = 207;  // ancho frame sombrero ← nuevo
static const int ALTO_FRAME_VIDA    = 254;  // alto corazón
static const int ALTO_FRAME_VEL     = 169;  // alto sombrero ← nuevo
static const int GAP_VIDA           = 35;   // gap corazón
static const int GAP_VELOCIDAD      = 40;   // gap sombrero ← nuevo
static const int TOTAL_FRAMES       = 6;

ItemJuego::ItemJuego(TipoItem tipo, qreal posX, qreal posY)
    : tipoActual(tipo), frameActual(0), contadorFrames(0)
{
    setPos(posX, posY);
    cargarGrafico();
}

void ItemJuego::cargarGrafico()
{
    QPixmap sprites("C:\\Users\\Emmanuel\\Documents\\DESAFIOIII\\items.png");

    int yPos, xInicio, anchoFrame, altoFrame, gap;

    if (tipoActual == Vida) {
        yPos       = Y_VIDA;
        xInicio    = X_INICIO_VIDA;
        anchoFrame = ANCHO_FRAME_VIDA;
        altoFrame  = ALTO_FRAME_VIDA;
        gap        = GAP_VIDA;
    } else {
        yPos       = Y_VELOCIDAD;
        xInicio    = X_INICIO_VELOCIDAD;
        anchoFrame = ANCHO_FRAME_VEL;
        altoFrame  = ALTO_FRAME_VEL;
        gap        = GAP_VELOCIDAD;
    }

    // Debug: imprime el color del fondo para ajustar quitarFondo()
    QImage imgDebug = sprites.toImage();
    QColor colorFondo = imgDebug.pixelColor(xInicio, yPos);
    qDebug() << "Color fondo tipo" << tipoActual << ":"
             << colorFondo.red() << colorFondo.green() << colorFondo.blue();

    for (int i = 0; i < TOTAL_FRAMES; i++) {
        int xActual = xInicio + i * (anchoFrame + gap);
        QPixmap frame = sprites.copy(xActual, yPos, anchoFrame, altoFrame);
        frame = quitarFondo(frame);
        frame = frame.scaled(50, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        framesAnimacion.append(frame);
    }

    if (!framesAnimacion.isEmpty())
        setPixmap(framesAnimacion[0]);
}

QPixmap ItemJuego::quitarFondo(const QPixmap &original)
{
    QImage img = original.toImage().convertToFormat(QImage::Format_ARGB32);
    QColor fondoColor(174, 157, 137);//RGB: (174, 157, 137)

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
    return QRectF(0, 0, 40, 40); // mismo tamaño que el scaled()
}

// paint() cerrado correctamente, dibuja el frame actual
void ItemJuego::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!framesAnimacion.isEmpty()) {
        painter->drawPixmap(0, 0, framesAnimacion[frameActual]);
    }

    // Debug temporal: muestra el rectángulo del item en amarillo
    /*
    painter->setPen(QPen(Qt::yellow, 1, Qt::DotLine));
    painter->drawRect(boundingRect());
*/
}

// actualizarAnimacion() es una función SEPARADA, fuera de paint()
void ItemJuego::actualizarAnimacion()
{
    if (framesAnimacion.isEmpty()) return;

    contadorFrames++;
    if (contadorFrames >= 8) {
        contadorFrames = 0;
        frameActual = (frameActual + 1) % framesAnimacion.size();
        setPixmap(framesAnimacion[frameActual]);
    }
}