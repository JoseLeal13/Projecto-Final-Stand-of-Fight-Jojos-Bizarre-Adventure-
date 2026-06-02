#include "steelball.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>

SteelBall::SteelBall(TipoBola tipo, TipoTrayectoria trayectoria, qreal posX, qreal posY, int direccionX)
    : tipoActual(tipo), trayectoriaActual(trayectoria), dirX(direccionX)
{
    setPos(posX, posY);
    posYInicial = posY;
    tiempoInterno = 0.0;

    // Configuración de la física
    velocidadX = 5.0 * dirX;
    velocidadY = 0.0;

    // ── INICIALIZACIÓN DE LA ANIMACIÓN ──
    frameActual = 0;
    contadorFrames = 0;
    retardoFrames = 7;

    cargarGraficos();
}

void SteelBall::cargarGraficos()
{
    QPixmap sprites(":/sprites_juego.png");

    // Dimensiones de un solo frame de la bola
    int anchoBola = 78;
    int altoBola = 37;

    // Coordenadas iniciales en tu sprite sheet
    int xInicio = (tipoActual == VerdeGolpeable) ? 1095 : 1095;
    int yPos = 600;

    // Recortamos los 4 sprites de giro uno al lado del otro
    for (int i = 0; i < 4; ++i) {
        int xActual = xInicio + (i * (anchoBola + 30));
        QPixmap frame = sprites.copy(xActual, yPos, anchoBola, altoBola);

        // ── AQUÍ LE QUITAMOS EL FONDO A CADA FRAME ──
        frame = quitarFondo(frame);

        framesAnimacion.append(frame);
    }

    // Colocamos el primer frame por defecto
    if (!framesAnimacion.isEmpty()) {
        setPixmap(framesAnimacion[0]);
    }
}

void SteelBall::avanzarFisica()
{
    // 1. ── CONTROL DE ANIMACIÓN (Bucle repetitivo de 4 sprites) ──
    if (!framesAnimacion.isEmpty()) {
        contadorFrames++;
        if (contadorFrames >= retardoFrames) {
            contadorFrames = 0;
            frameActual++;

            if (frameActual >= framesAnimacion.size()) {
                frameActual = 0;
            }

            setPixmap(framesAnimacion[frameActual]);
        }
    }

    // 2. ── CÁLCULO DE LAS TRAYECTORIAS MATEMÁTICAS ──
    tiempoInterno += 0.05;

    qreal nuevoX = x() + velocidadX;
    qreal nuevoY = y();

    switch (trayectoriaActual) {
    case Recta:
        break;

    case Parabolica: {
        qreal v0y = -8.0;
        qreal gravedad = 9.8;
        nuevoY = posYInicial + (v0y * tiempoInterno) + (0.5 * gravedad * tiempoInterno * tiempoInterno);
        break;
    }

    case Senoidal: {
        qreal amplitud = 50.0;
        qreal frecuencia = 3.0;
        nuevoY = posYInicial + amplitud * std::sin(frecuencia * tiempoInterno);
        break;
    }
    }

    setPos(nuevoX, nuevoY);
}

void SteelBall::recibirGolpe()
{
    if (tipoActual == VerdeGolpeable) {
        if (scene()) {
            scene()->removeItem(this);
            delete this;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  PROCESAMIENTO DE IMÁGENES: REMOCIÓN DE FONDO TRANSPARENTE
// ═══════════════════════════════════════════════════════════════════════════
QPixmap SteelBall::quitarFondo(const QPixmap &original)
{
    QImage img = original.toImage().convertToFormat(QImage::Format_ARGB32);
    QColor fondoColor(30, 27, 60); // El mismo color de fondo que usa tu sprite sheet

    for (int yPos = 0; yPos < img.height(); yPos++) {
        for (int xPos = 0; xPos < img.width(); xPos++) {
            QColor pixel = img.pixelColor(xPos, yPos);

            // Si el color del píxel es casi idéntico al fondo, lo vuelve transparente (Alfa = 0)
            if (abs(pixel.red() - fondoColor.red()) < 8 &&
                abs(pixel.green() - fondoColor.green()) < 8 &&
                abs(pixel.blue() - fondoColor.blue()) < 8) {
                img.setPixelColor(xPos, yPos, QColor(0, 0, 0, 0));
            }
        }
    }
    return QPixmap::fromImage(img);

}


// ═══════════════════════════════════════════════════════════════════════════
//  SISTEMA DE COLISIONES CORREGIDO (Para que Qt NO ignore los impactos)
// ═══════════════════════════════════════════════════════════════════════════
// 1. EL RECTÁNGULO LÍMITE (Obliga a Qt a saber cuánto mide tu pelota)
QRectF SteelBall::boundingRect() const
{
    // Tus sprites miden 78 de ancho y 37 de alto.
    // Esto le dice al motor el tamaño físico real de la caja.
    return QRectF(0, 0, 78, 37);
}

// 2. EL MÉTODO PAINT (¡Sigue aquí! Dibuja el contorno para que lo veas avanzar)
void SteelBall::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 1. Pinta el sprite animado
    QGraphicsPixmapItem::paint(painter, option, widget);

    // 2. FIANZA DE SEGURIDAD: Si no hay escena todavía, salimos para evitar el crash
    if (!scene() || scene()->views().isEmpty()) {
        return;
    }

    // 3. Comprobar si debemos mostrar la hitbox (Depuración)
    bool mostrarH = false;
    QWidget *topWidget = scene()->views().first()->window();
    QMainWindow *mainWin = qobject_cast<QMainWindow*>(topWidget);
    if (mainWin) {
        mostrarH = mainWin->property("mostrarHitbox").toBool();
    }

    // Dibujamos la hitbox visual si corresponde
    if (mostrarH || true) {
        painter->save();
        if (tipoActual == VerdeGolpeable) {
            painter->setPen(QPen(Qt::green, 2, Qt::DashLine));
        } else {
            painter->setPen(QPen(Qt::red, 2, Qt::SolidLine));
        }
        painter->drawRect(this->boundingRect());
        painter->restore();
    }
}

// 3. LA SINCRONIZACIÓN DE LA COLISIÓN (Muda la hitbox matemática al mismo lugar que el paint)
bool SteelBall::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    return QGraphicsItem::collidesWithItem(other, mode);
}

QPainterPath SteelBall::shape() const
{
    QPainterPath path;
    // Usa el boundingRect completo como hitbox sólida, ignorando transparencia
    path.addRect(boundingRect());
    return path;
}