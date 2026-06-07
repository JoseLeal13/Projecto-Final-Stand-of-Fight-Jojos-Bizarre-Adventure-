#include "steelball.h"
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

    QPixmap sprites(":/sprites/SpritesJojoChampionship/sprites_juego.png");

    // Dimensiones de un solo frame de la bola
    int anchoBola = 78;
    int altoBola = 37;

    int xInicio = 1095;
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

    // 2. ── MODO CAÍDA (cuando Jotaro golpea la bola) ──
    if (cayendo) {
        velocidadCaidaY += 0.6;

        qreal factor = ralentizada ? 0.3f : 1.0f;
        qreal nuevoX = x() + velocidadX * 0.4 * factor;
        qreal nuevoY = y() + velocidadCaidaY * factor;
        setPos(nuevoX, nuevoY);

        if (mostrandoEfecto) {
            contadorEfecto++;
            if (contadorEfecto >= 6) {
                contadorEfecto = 0;
                frameEfecto++;
                if (frameEfecto >= framesEfectoGolpe.size()) {
                    mostrandoEfecto = false;
                }
            }
            update();
        }
        return;
    }

    // 3. ── CÁLCULO DE LAS TRAYECTORIAS MATEMÁTICAS (modo normal) ──
    tiempoInterno += ralentizada ? 0.015 : 0.05;

    qreal factor = ralentizada ? 0.3f : 1.0f;
    qreal nuevoX = x() + velocidadX * factor;
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
        cayendo = true;
        velocidadCaidaY = -5.0;
        velocidadX = (dirX > 0) ? 2.0 : -2.0;

        mostrandoEfecto = true;
        frameEfecto    = 0;
        contadorEfecto = 0;

        qDebug() << " Bola verde golpeada, iniciando caida + efecto morado";
    }
}

void SteelBall::cargarEfectoGolpe()
{
    QPixmap sprites(":/sprites/SpritesJojoChampionship/sprites_juego.png");

    struct { int x, w; } data[] = {
                {1108, 92},
                {1217, 80},
                {1313, 68},
                {1397, 51},
                {1461, 33},
                {1506,  8},
    };

    for (auto &d : data) {
        QPixmap frame = sprites.copy(d.x, 97, d.w, 94);
        framesEfectoGolpe.append(quitarFondo(frame));
    }
}

QPixmap SteelBall::quitarFondo(const QPixmap &original)
{
    QImage img = original.toImage().convertToFormat(QImage::Format_ARGB32);
    QColor fondoColor(30, 27, 60);

    for (int yPos = 0; yPos < img.height(); yPos++) {
        for (int xPos = 0; xPos < img.width(); xPos++) {
            QColor pixel = img.pixelColor(xPos, yPos);

            if (abs(pixel.red() - fondoColor.red()) < 20 &&
                abs(pixel.green() - fondoColor.green()) < 20 &&
                abs(pixel.blue() - fondoColor.blue()) < 20) {
                img.setPixelColor(xPos, yPos, QColor(0, 0, 0, 0));
            }
        }
    }
    return QPixmap::fromImage(img);
}

QRectF SteelBall::boundingRect() const
{
    return QRectF(-7, -28, 92, 94);
}

void SteelBall::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

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

    if (!framesAnimacion.isEmpty() && frameActual < framesAnimacion.size()) {
        painter->drawPixmap(0, 0, framesAnimacion[frameActual]);
    }

    if (mostrandoEfecto && frameEfecto < framesEfectoGolpe.size()) {
        QPixmap &efecto = framesEfectoGolpe[frameEfecto];
        int offsetX = (78 - efecto.width())  / 2;
        int offsetY = (37 - efecto.height()) / 2;
        painter->drawPixmap(offsetX, offsetY, efecto);
    }

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

bool SteelBall::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    return QGraphicsItem::collidesWithItem(other, mode);
}

QPainterPath SteelBall::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(0, 0, 78, 37));
    return path;
}