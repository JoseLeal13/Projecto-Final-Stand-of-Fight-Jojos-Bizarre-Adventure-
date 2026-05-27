#include "jotaro.h"

// ── Constantes de TU sprite sheet extraídas de tu MainWindow ──────────────────
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

static const int Y_DIR[4] = {
    94,   // 0 = Arriba
    190,  // 1 = Abajo
    290,  // 2 = Izquierda
    390   // 3 = Derecha
};

jotaro::jotaro() : Personaje()
{
    // Atributos iniciales simples
    speed = 4;
    direccion = 1; // Mirando abajo por defecto
    enMovimiento = false;
    atacando = false;
    frameActual = 0;

    setPos(100, 100); // Posición inicial en la escena

    // Tamaño base de tu hitbox (Ancho: 70, Alto: 100)
    anchoHitbox = 70;
    altoHitbox  = 100;
    hitbox = QRectF(0, 0, anchoHitbox, altoHitbox);

    // El objeto se encarga de cargar sus propios recursos al crearse
    cargarFrames();
}

// ═══════════════════════════════════════════════════════════════════════════
//  MÉTODOS OBLIGATORIOS DE LA CLASE BASE
// ═══════════════════════════════════════════════════════════════════════════
void jotaro::moverse() {
    // Tu MainWindow se encarga de llamarlo mediante eventos de teclado individuales
}

void jotaro::atacar() {
    // Aquí puedes meter lógica interna cuando golpeas una Steel Ball si lo deseas
}

void jotaro::habilidadEspecial() {
    // Vacío. No requerido en tu minijuego survival
}

// ═══════════════════════════════════════════════════════════════════════════
//  MÉTODOS OBLIGATORIOS DE DIBUJO DE QT (Reemplazan tu paintEvent manual)
// ═══════════════════════════════════════════════════════════════════════════
QRectF jotaro::boundingRect() const
{
    // Define el área rectangular máxima que puede ocupar el sprite al redibujarse
    // Agregamos un margen extra hacia la izquierda por si ataca mirando a la izq (-100)
    return QRectF(-100, 0, 300, 120);
}

void jotaro::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPixmap spriteADibujar;

    // 1. Seleccionar el Frame correcto según el estado interno
    if(atacando) {
        int idx = frameActual % framesAtaqueDerecha.size();
        spriteADibujar = (direccion == 2) ? framesAtaqueIzquierda[idx] : framesAtaqueDerecha[idx];
    } else if(enMovimiento) {
        int idx = frameActual % framesMovimiento[direccion].size();
        spriteADibujar = framesMovimiento[direccion][idx];
    } else {
        spriteADibujar = framesQuieto[direccion][0];
    }

    // 2. Calcular coordenada de dibujo en X (Tu corrección por desajuste de ataque izquierdo)
    int drawX = 0;
    if(atacando && direccion == 2) {
        drawX -= 100;
    }

    // 3. Dibujar el sprite de forma nativa en sus coordenadas locales (0,0 de su propio eje)
    painter->drawPixmap(drawX, 0, spriteADibujar);
}

// ═══════════════════════════════════════════════════════════════════════════
//  MÉTODOS DE MOVIMIENTO CON VALIDACIÓN
// ═══════════════════════════════════════════════════════════════════════════
void jotaro::moveUp() {
    float proximoY = y() - speed;
    if (!verificarColision(x(), proximoY)) setPos(x(), proximoY);
}

void jotaro::moveDown() {
    float proximoY = y() + speed;
    if (!verificarColision(x(), proximoY)) setPos(x(), proximoY);
}

void jotaro::moveLeft() {
    float proximoX = x() - speed;
    if (!verificarColision(proximoX, y())) setPos(proximoX, y());
}

void jotaro::moveRight() {
    float proximoX = x() + speed;
    if (!verificarColision(proximoX, y())) setPos(proximoX, y());
}

// ═══════════════════════════════════════════════════════════════════════════
//  SETTERS DE CONTROL DE ESTADO
// ═══════════════════════════════════════════════════════════════════════════
void jotaro::setDireccion(int dir)   { direccion = dir; update(); }
void jotaro::setAtacando(bool status) { atacando = status; update(); }
void jotaro::setEnMovimiento(bool status) { enMovimiento = status; update(); }
void jotaro::actualizarFrame(int frameActualIndex) { frameActual = frameActualIndex; update(); }

// ═══════════════════════════════════════════════════════════════════════════
//  HITBOXES LÓGICAS (Mapeadas globalmente en la escena)
// ═══════════════════════════════════════════════════════════════════════════
QRectF jotaro::getHitbox()
{
    return QRectF(x(), y(), anchoHitbox, altoHitbox);
}

QRectF jotaro::getAttackHitbox()
{
    if (direccion == 2) { // Izquierda
        return QRectF(x() - 90, y() + 25, 90, 45);
    }
    return QRectF(x() + 60, y() + 25, 90, 45); // Derecha
}

// ═══════════════════════════════════════════════════════════════════════════
//  PROCESAMIENTO INTERNO DE IMÁGENES
// ═══════════════════════════════════════════════════════════════════════════
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
        QPixmap frame = sprites.copy(xPos, Y_ATAQUE, (f > 0) ? ANCHO_ATAQUE + 115 : ANCHO_ATAQUE, ALTO_ATAQUE);
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