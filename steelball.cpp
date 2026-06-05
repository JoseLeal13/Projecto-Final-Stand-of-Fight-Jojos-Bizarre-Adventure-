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

    // La bola empieza en modo normal, no cayendo
    cayendo = false;
    velocidadCaidaY = 0.0;

    // El efecto morado empieza desactivado, se activa al recibir golpe
    mostrandoEfecto = false;
    frameEfecto     = 0;
    contadorEfecto  = 0;

    // ── INICIALIZACIÓN DE LA ANIMACIÓN ──
    frameActual = 0;
    contadorFrames = 0;
    retardoFrames = 7;

    cargarGraficos();
    cargarEfectoGolpe(); // Cargamos los frames del destello morado también
}

void SteelBall::cargarGraficos()
{
    QPixmap sprites(":/sprites_juego.png");

    // Dimensiones de un solo frame de la bola
    int anchoBola = 78;
    int altoBola = 37;

    // Coordenadas iniciales en el sprite sheet
    // IMPORTANTE: si verde y roja están en filas distintas del sprite sheet,
    // cambia el yPos de cada una. Por ahora ambas usan y=600 porque el sheet
    // tiene los frames uno al lado del otro en la misma fila.
    // Si quieres separarlas visualmente, ajusta estos valores midiendo el PNG.
    int xInicio = 1095; // Mismo punto de inicio para las dos (ajustar si cambia el sheet)
    int yPos = (tipoActual == VerdeGolpeable) ? 600 : 600; // <- cambiar el segundo 600 si roja está en otra fila

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
        if (contadorFrames >= retardoFrames) { // retardoFrames = 7
            contadorFrames = 0;
            frameActual++;

            if (frameActual >= framesAnimacion.size()) { // size = 4 brou
                frameActual = 0;
            }

            setPixmap(framesAnimacion[frameActual]); // dibuja el nuevo frame
        }
    }

    // 2. ── MODO CAÍDA (cuando Jotaro golpea la bola) ──
    // Si estamos cayendo ignoramos la trayectoria normal y aplicamos gravedad simple
    if (cayendo) {
        // La gravedad la acelera hacia abajo cada frame
        // El 0.6 es la aceleracion de caida, si se ve muy lento subir ese valor
        velocidadCaidaY += 0.6;

        qreal nuevoX = x() + velocidadX * 0.4; // Va perdiendo velocidad horizontal
        qreal nuevoY = y() + velocidadCaidaY;
        setPos(nuevoX, nuevoY);

        // Avanzar el destello morado mientras este activo
        // Cada 6 ticks salta al siguiente frame; cuando se acaban se desactiva solo
        if (mostrandoEfecto) {
            contadorEfecto++;
            if (contadorEfecto >= 6) {
                contadorEfecto = 0;
                frameEfecto++;
                if (frameEfecto >= framesEfectoGolpe.size()) {
                    mostrandoEfecto = false; // Ya termino, dejamos de dibujarlo
                }
            }
            update(); // Pedir redibujado para que se vea el cambio de frame
        }
        return; // No ejecutar la fisica normal
    }

    // 3. ── CÁLCULO DE LAS TRAYECTORIAS MATEMÁTICAS (modo normal) ──
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
        // Activar fisica de caida: sale hacia arriba primero, luego cae
        cayendo = true;
        velocidadCaidaY = -5.0;
        velocidadX = (dirX > 0) ? 2.0 : -2.0;

        // Activar el destello morado que se dibuja encima de la bola
        // (los frames ya estan cargados desde el constructor)
        mostrandoEfecto = true;
        frameEfecto    = 0;
        contadorEfecto = 0;

        qDebug() << " Bola verde golpeada, iniciando caida + efecto morado";
    }
}

// Cargamos los 6 frames del destello morado del sprite sheet
// Coordenadas sacadas midiendo el PNG con Python
void SteelBall::cargarEfectoGolpe()
{
    QPixmap sprites(":/sprites_juego.png");

    // Fila de estrellas moradas: y=97 a y=191, 6 frames encogiendo
    struct { int x, w; } data[] = {
        {1108, 92},  // frame 0: estrella grande
        {1217, 80},  // frame 1: un poco mas chica
        {1313, 68},  // frame 2: mediana
        {1397, 51},  // frame 3: más pequeña
        {1461, 33},  // frame 4: pequeña
        {1506,  8},  // frame 5: casi nada
    };

    for (auto &d : data) {
        QPixmap frame = sprites.copy(d.x, 97, d.w, 94);
        framesEfectoGolpe.append(quitarFondo(frame));
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
// 1. EL RECTÁNGULO LÍMITE
// Cuando hay efecto activo lo hacemos más grande para que Qt redibuje bien el destello
// que sobresale del tamaño normal de la bola (92x94 vs 78x37)
QRectF SteelBall::boundingRect() const
{
    // Siempre devuelvo el bounding rect grande para que Qt no se confunda
    // cuando el efecto morado aparece y desaparece (eso rompía las hitboxes antes).
    // El efecto más grande mide 92x94, lo centro sobre la bola de 78x37.
    return QRectF(-7, -28, 92, 94);
}

// 2. EL MÉTODO PAINT

void SteelBall::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // ── AURA ÉPICA ──
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    QRadialGradient aura(39, 18, 35);

    if (tipoActual == VerdeGolpeable) {
        aura.setColorAt(0.0, QColor(0,   255, 80,  255));
        aura.setColorAt(0.5, QColor(0,   200, 50,  180));
        aura.setColorAt(1.0, QColor(0,   150, 20,    0));
    } else {
        aura.setColorAt(0.0, QColor(255,  20,  0,  255));
        aura.setColorAt(0.5, QColor(200,   5,  0,  180));
        aura.setColorAt(1.0, QColor(120,   0,  0,    0));
    }

    painter->setBrush(QBrush(aura));
    painter->drawEllipse(QRectF(4, -10, 70, 57));
    painter->restore();

    // ── SPRITE DE LA BOLA (encima del aura) ──
    if (!framesAnimacion.isEmpty() && frameActual < framesAnimacion.size()) {
        painter->drawPixmap(0, 0, framesAnimacion[frameActual]);
    }

    // ── EFECTO MORADO DEL GOLPE (encima de todo) ──
    if (mostrandoEfecto && frameEfecto < framesEfectoGolpe.size()) {
        QPixmap &efecto = framesEfectoGolpe[frameEfecto];
        int offsetX = (78 - efecto.width())  / 2;
        int offsetY = (37 - efecto.height()) / 2;
        painter->drawPixmap(offsetX, offsetY, efecto);
    }

    // ── HITBOX DEBUG ──
    bool mostrarH = false;
    if (scene() && !scene()->views().isEmpty()) {
        QWidget *topWidget = scene()->views().first()->window();
        QMainWindow *mainWin = qobject_cast<QMainWindow*>(topWidget);
        if (mainWin) {
            mostrarH = mainWin->property("mostrarHitbox").toBool();
        }
    }
    if (mostrarH) {
        painter->save();
        if (tipoActual == VerdeGolpeable) {
            painter->setPen(QPen(Qt::green, 2, Qt::DashLine));
        } else {
            painter->setPen(QPen(Qt::red, 2, Qt::SolidLine));
        }
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(QRectF(0, 0, 78, 37));
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
    // La hitbox de COLISIÓN es solo el área de la bola (78x37), en posición (0,0)
    // NO incluyo el área del efecto morado porque sino Jotaro recibe daño en el aire
    // y las colisiones se sienten raras. El bounding rect grande es solo para el dibujo.
    path.addRect(QRectF(0, 0, 78, 37));
    return path;
}