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

    // ── INICIALIZACIÓN DE SALUD Y CONTROL DEL SURVIVAL ──
    vida = 100;
    invencible = false;
    contadorInvencible = 0;

    setPos(100, 100); // Posición inicial en la escena

    // Tamaño base de tu hitbox (Ancho: 70, Alto: 100)
    anchoHitbox = 70;
    altoHitbox  = 100;
    hitbox = QRectF(0, 0, anchoHitbox, altoHitbox);

    // El objeto se encarga de cargar sus propios recursos al crearse
    cargarFrames();

    vida = 100;
    invencible = false;
    contadorInvencible = 0;

}

// ═══════════════════════════════════════════════════════════════════════════
//  MÉTODOS OBLIGATORIOS DE LA CLASE BASE
// ═══════════════════════════════════════════════════════════════════════════
void jotaro::moverse() {
    // Tu MainWindow se encarga de llamarlo mediante eventos de teclado individuales
}

void jotaro::atacar() {
    // Lógica de ataque activa
}

void jotaro::habilidadEspecial() {
    // Vacío. No requerido en tu minijuego survival
}

// ═══════════════════════════════════════════════════════════════════════════
//  MÉTODOS DE SALUD Y CONTROL DE DAÑO
// ═══════════════════════════════════════════════════════════════════════════


// ═══════════════════════════════════════════════════════════════════════════
//  MÉTODOS OBLIGATORIOS DE DIBUJO DE QT (Con soporte para parpadeo de daño)
// ═══════════════════════════════════════════════════════════════════════════
QRectF jotaro::boundingRect() const
{
    // Define el área rectangular máxima que puede ocupar el sprite al redibujarse
    return QRectF(0, 0, 70, 100);
}

QPainterPath jotaro::shape() const
{
    QPainterPath path;
    // Hitbox corporal real: offset 18 en X, 10 en Y, 34 ancho, 85 alto
    path.addRect(QRectF(18, 10, 34, 85));
    return path;
}

void jotaro::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // ── IMPORTANTE: guardamos el estado del painter antes de tocar nada ──
    // Esto evita que configuraciones previas de Qt (del QGraphicsPixmapItem base)
    // causen ese bug visual donde el sprite se ve raro cuando la hitbox está oculta.
    // Básicamente "limpiamos la pizarra" antes de dibujar encima.
    painter->save();

    // 1. CONTROL DEL EFECTO VISUAL DE DAÑO (Parpadeo)
    if (invencible && (contadorInvencible % 8 > 4)) {
        // En estos frames del parpadeo no dibujamos el sprite (efecto de daño)
        // pero igual llegamos al restore() de abajo para no dejar el painter sucio
    } else {
        QPixmap spriteADibujar;

        // Seleccionar el Frame correcto según el estado interno
        if(atacando) {
            // Si los frames de ataque están vacíos por algún error de carga, no crashear
            if(framesAtaqueDerecha.isEmpty()) {
                painter->restore();
                return;
            }
            int idx = frameActual % framesAtaqueDerecha.size();
            spriteADibujar = (direccion == 2) ? framesAtaqueIzquierda[idx] : framesAtaqueDerecha[idx];
        } else if(enMovimiento) {
            // Misma protección para frames de movimiento
            if(framesMovimiento[direccion].isEmpty()) {
                painter->restore();
                return;
            }
            int idx = frameActual % framesMovimiento[direccion].size();
            spriteADibujar = framesMovimiento[direccion][idx];
        } else {
            // Frame quieto, el más básico
            if(framesQuieto[direccion].isEmpty()) {
                painter->restore();
                return;
            }
            spriteADibujar = framesQuieto[direccion][0];
        }

        // Solo dibujar si el pixmap es válido (no está nulo/vacío)
        if(!spriteADibujar.isNull()) {
            int drawX = 0;
            if(atacando && direccion == 2) {
                drawX -= 100; // corrimiento para el ataque izquierda
            }
            // Dibujamos el sprite manualmente, ignorando el pixmap del item base
            painter->drawPixmap(drawX, 0, spriteADibujar);
        }
    }

    // Restauramos el painter a como estaba antes de que llegáramos
    // Esto es lo que faltaba y causaba el bug visual
    painter->restore();

    // ── 2. DIBUJAR HITBOXES VISUALES CUANDO SE PRESIONA LA 'H' ──
    if (mostrarHitboxInterna) {
        // Recordatorio: En Qt, las hitboxes se dibujan en COORDENADAS LOCALES del objeto (sin x() ni y())

        // Hitbox Corporal (Color Rojo)
        painter->setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter->setBrush(QBrush(QColor(255, 0, 0, 50))); // Rojo semi-transparente
        painter->drawRect(QRectF(18, 10, 34, 85)); // Mismas medidas que tu getHitbox()

        // Hitbox de Ataque (Color Azul) si Jotaro está golpeando
        if (atacando) {
            painter->setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
            painter->setBrush(QBrush(QColor(0, 255, 255, 70))); // Azul/Cian traslúcido

            if (direccion == 2) { // Izquierda
                painter->drawRect(QRectF(-90, 25, 90, 45));
            } else { // Derecha (o cualquier otra dirección por defecto)
                painter->drawRect(QRectF(60, 25, 90, 45));
            }
        }
    }
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
//  HITBOXES LÓGICAS OPTIMIZADAS (Mapeadas globalmente en la escena)
// ═══════════════════════════════════════════════════════════════════════════
QRectF jotaro::getHitbox()
{
    // Reajustada para el cuerpo real: recortamos márgenes para evitar el aire transparente lateral
    // x_offset = +18, y_offset = +10, ancho = 34, alto = 85 (Ajustado a tus constantes 70x100)
    return QRectF(x() + 18, y() + 10, 34, 85);
}

QRectF jotaro::getAttackHitbox()
{
    if (!atacando) return QRectF(0, 0, 0, 0);

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


void jotaro::recibirDanio(int cantidad)
{
    if (invencible) return; // Si parpadea por un golpe reciente, ignora el daño

    vida -= cantidad;
    if (vida < 0) vida = 0;

    // Activar invulnerabilidad por 60 frames (1 segundo a 60 FPS)
    invencible = true;
    contadorInvencible = 60;

    qDebug() << "¡JOTARO RECIBIÓ DAÑO! Vida restante:" << vida;
}

void jotaro::actualizarInvulnerabilidad()
{
    if (invencible) {
        contadorInvencible--;
        if (contadorInvencible <= 0) {
            invencible = false;
        }
        update(); // Forza a Qt a redibujar (útil para efectos visuales de parpadeo)
    }
}