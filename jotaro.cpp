#include "jotaro.h"

// ── Constantes del sprite sheet ──────────────────
static const int ANCHO_FRAME = 70;
static const int ALTO_FRAME  = 100;
static const int FRAMES_QUIETO     = 1;
static const int FRAMES_MOVIMIENTO = 3;
static const int FRAMES_ATAQUE = 2;
static const int X_INICIO_ATAQUE = 720;
static const int Y_ATAQUE = 92;
static const int ANCHO_ATAQUE = 80;
static const int ALTO_ATAQUE = 81;
static const int MARGEN_FRAME        = 12;
static const int X_INICIO_MOVIMIENTO = 250;

static const int Y_DIR[4] = { // filas de los sprites (pixeles)
    94,   // 0 = Arriba
    190,  // 1 = Abajo
    290,  // 2 = Izquierda
    390   // 3 = Derecha
};

jotaro::jotaro() : Personaje()
{
    // Atributos iniciales base
    speed = 4;
    direccion = 1; // Mirando abajo por defecto
    enMovimiento = false;
    atacando = false;
    frameActual = 0;

    // Inicialización del estado de salud
    vida = 100;
    invencible = false;
    contadorInvencible = 0;

    setPos(100, 100);

    anchoHitbox = 70;
    altoHitbox  = 100;
    hitbox = QRectF(0, 0, anchoHitbox, altoHitbox);

    // Inicialización de mecánicas especiales
    energiaUlti = 0;
    ultiActiva = false;
    timerUlti = 0;
    velocidadBonus = 0;
    timerVelocidad = 0;

    // Cargar sprites autónomamente
    cargarFrames();
}

void jotaro::moverse() {}
void jotaro::atacar() {}
void jotaro::habilidadEspecial() {}

QRectF jotaro::boundingRect() const
{
    // Cuando Jotaro ataca a la izquierda, el sprite se dibuja en drawX = -100
    // Si el boundingRect no lo cubre, Qt lo recorta y no se ve el golpe.
    // Entonces le doy suficiente margen izquierdo para que quepa el sprite de ataque.
    // En reposo o moviéndose no afecta nada visualmente, solo le damos espacio extra.
    if (atacando && direccion == 2) {
        return QRectF(-110, 0, 180, 100); // margen para el puñazo hacia la izquierda
    }
    if (atacando) {
        return QRectF(0, 0, 160, 100); // margen para el puñazo hacia la derecha
    }
    return QRectF(0, 0, 70, 100);
}

QPainterPath jotaro::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(18, 10, 34, 85));
    return path;
}

void jotaro::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();

    if (invencible && (contadorInvencible % 8 > 4)) {
        // Efecto visual de parpadeo por daño
    } else {
        QPixmap spriteADibujar;

        if(atacando) {
            // Elige el frame de ataque según dirección
            if(framesAtaqueDerecha.isEmpty()) {
                painter->restore();
                return;
            }
            int idx = frameActual % framesAtaqueDerecha.size();
            spriteADibujar = (direccion == 2) ? framesAtaqueIzquierda[idx] : framesAtaqueDerecha[idx];
        } else if(enMovimiento) {
            if(framesMovimiento[direccion].isEmpty()) {
                painter->restore();
                return;
            }
            int idx = frameActual % framesMovimiento[direccion].size();
            spriteADibujar = framesMovimiento[direccion][idx];
        } else {
            if(framesQuieto[direccion].isEmpty()) {
                painter->restore();
                return;
            }
            spriteADibujar = framesQuieto[direccion][0];
        }

        if(!spriteADibujar.isNull()) {
            int drawX = 0;
            if(atacando && direccion == 2) {
                drawX -= 100;
            }
            painter->drawPixmap(drawX, 0, spriteADibujar);
        }
    }

    painter->restore();

    // Las hitboxes de debug van DESPUÉS del restore para que no hereden
    // el clip del sprite. Si van adentro del save/restore anterior, Qt las
    // recorta al tamaño del sprite y no se ve el rectángulo completo.
    if (mostrarHitboxInterna) {
        // Hitbox corporal en rojo punteado
        painter->setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter->setBrush(QBrush(QColor(255, 0, 0, 50)));
        painter->drawRect(QRectF(18, 10, 34, 85));

        // Hitbox de ataque en cian, solo cuando está golpeando
        if (atacando) {
            painter->setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
            painter->setBrush(QBrush(QColor(0, 255, 255, 70)));

            if (direccion == 2) {
                // Puñazo hacia la izquierda: empieza en x negativo
                painter->drawRect(QRectF(-90, 25, 90, 45));
            } else {
                // Puñazo hacia la derecha: empieza donde termina el cuerpo
                painter->drawRect(QRectF(60, 25, 90, 45));
            }
        }
    }
}

// ── MOVIMIENTO CONSIDERANDO EL BONUS DEL ÍTEM CELESTE ──
void jotaro::moveUp() {
    float proximoY = y() - (speed + velocidadBonus);
    if (!verificarColision(x(), proximoY)) setPos(x(), proximoY);
}

void jotaro::moveDown() {
    float proximoY = y() + (speed + velocidadBonus);
    if (!verificarColision(x(), proximoY)) setPos(x(), proximoY);
}

void jotaro::moveLeft() {
    float proximoX = x() - (speed + velocidadBonus);
    if (!verificarColision(proximoX, y())) setPos(proximoX, y());
}

void jotaro::moveRight() {
    float proximoX = x() + (speed + velocidadBonus);
    if (!verificarColision(proximoX, y())) setPos(proximoX, y());
}

void jotaro::setDireccion(int dir)        { direccion = dir; update(); }
void jotaro::setAtacando(bool status) {
    if (atacando != status) {
        // prepareGeometryChange le avisa a Qt que el boundingRect va a cambiar.
        // Sin esto, Qt no redibuja el área extra del ataque y la hitbox aparece cortada.
        prepareGeometryChange();
        atacando = status;
    }
    update();
}
void jotaro::setEnMovimiento(bool status) { enMovimiento = status; update(); }
void jotaro::actualizarFrame(int frameActualIndex) { frameActual = frameActualIndex; update(); }

QRectF jotaro::getHitbox()
{
    return QRectF(x() + 18, y() + 10, 34, 85);
}

QRectF jotaro::getAttackHitbox()
{
    if (!atacando) return QRectF(0, 0, 0, 0);

    if (direccion == 2) {
        return QRectF(x() - 90, y() + 25, 90, 45);
    }
    return QRectF(x() + 60, y() + 25, 90, 45);
}

void jotaro::cargarFrames()
{
    QPixmap sprites(":/sprites_juego.png");

    for (int dir = 0; dir < 3; dir++)
    {
        for (int f = 0; f < FRAMES_QUIETO; f++) {
            int xPos = X_INICIO_MOVIMIENTO + f * (ANCHO_FRAME + MARGEN_FRAME);
            QPixmap frame = sprites.copy(xPos, Y_DIR[dir], ANCHO_FRAME, ALTO_FRAME);
            framesQuieto[dir].append(quitarFondo(frame));
        }

        for (int f = 0; f < FRAMES_MOVIMIENTO; f++) {
            int xPos = X_INICIO_MOVIMIENTO + f * ANCHO_FRAME;
            QPixmap frame = sprites.copy(xPos, Y_DIR[dir], ANCHO_FRAME, ALTO_FRAME);
            framesMovimiento[dir].append(quitarFondo(frame));
        }
    }

    for (int f = 0; f < FRAMES_QUIETO; f++) {
        framesQuieto[3].append(reflejarHorizontal(framesQuieto[2][f]));
    }
    for (int f = 0; f < FRAMES_MOVIMIENTO; f++) {
        framesMovimiento[3].append(reflejarHorizontal(framesMovimiento[2][f]));
    }

    for(int f = 0; f < FRAMES_ATAQUE; f++) {

        int xPos = X_INICIO_ATAQUE + f * ANCHO_ATAQUE;
        QPixmap frame = sprites.copy(xPos, Y_ATAQUE, (f > 0) ? ANCHO_ATAQUE + 124 : ANCHO_ATAQUE, ALTO_ATAQUE);
        framesAtaqueDerecha.append(quitarFondo(frame));

    }

    for(int f = 0; f < FRAMES_ATAQUE; f++) {
        framesAtaqueIzquierda.append(reflejarHorizontal(framesAtaqueDerecha[f]));
    }
}

QPixmap jotaro::quitarFondo(const QPixmap &original)
{
    QImage img = original.toImage().convertToFormat(QImage::Format_ARGB32);
    QColor fondoColor(30,27,60);

    for (int yPos = 0; yPos < img.height(); yPos++) {
        for (int xPos = 0; xPos < img.width(); xPos++) {
            QColor pixel = img.pixelColor(xPos, yPos);
            if (abs(pixel.red() - fondoColor.red()) < 8 && abs(pixel.green() - fondoColor.green()) < 8 && abs(pixel.blue() - fondoColor.blue()) < 8) {
                img.setPixelColor(xPos, yPos, QColor(0, 0, 0, 0));
            }
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap jotaro::reflejarHorizontal(const QPixmap &original)
{
    QTransform espejo;
    espejo.scale(-1, 1);
    return original.transformed(espejo);
}

void jotaro::recibirDanio(int cantidad)
{
    if (invencible) return;

    vida -= cantidad;
    if (vida < 0) vida = 0;

    invencible = true;
    contadorInvencible = 60; // 1 segundo de invulnerabilidad
}

void jotaro::actualizarInvulnerabilidad()
{
    if (invencible) {
        contadorInvencible--;
        if (contadorInvencible <= 0) {
            invencible = false;
        }
        update();
    }
}

// ── MÉTODOS DE LA INTERFAZ DE ITEMS Y PODERES ──
void jotaro::curar(int cantidad) {
    vida += cantidad;
    if (vida > 100) vida = 100; // No sobrepasar la vida máxima
}

void jotaro::aumentarVelocidad() {
    velocidadBonus = 1;    // Sumamos +3 puntos de velocidad
    timerVelocidad = 180;  // Dura 2 segundos activos (180 frames / 60fps)
}

void jotaro::cargarEnergia(int cantidad) {
    energiaUlti += cantidad;
    if (energiaUlti > 100) energiaUlti = 100; // Límite de la barra
}

void jotaro::actualizarEfectosItems() {
    if (timerVelocidad > 0) {
        timerVelocidad--;
        if (timerVelocidad == 0) {
            velocidadBonus = 0;
        }
    }

    if (ultiActiva) {
        timerUlti--;
        if (timerUlti <= 0) {
            ultiActiva = false;
        }
    }
}

void jotaro::usarUlti() {
    if (energiaUlti >= 100) {
        energiaUlti = 0;
        ultiActiva = true;
        timerUlti = 240;     // 4 segundos de congelamiento
        qDebug() << " ¡¡ZA WARUDO!! El tiempo se ha ralentizado.";
    }
}
