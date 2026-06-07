#include "jojobro.h"
#include <cstdlib>
#include <cmath>

// ── CONSTANTES DEL SPRITE SHEET DE GYRO ──────────────────────────────────
// Modifica estos valores si el sprite sheet cambia, sin tocar el resto del código
static const int X_FRAME0_ATAQUE  = 703;  // X donde empieza el frame 0 (apuntando)
static const int X_FRAME1_ATAQUE  = 806;  // X donde empieza el frame 1 (lanzando) = 703+88+29
static const int Y_ATAQUE_GYRO    = 674;  // Y de la fila de ataque en el sheet
static const int ANCHO_FRAME0     = 74;   // Ancho del frame 0
static const int ANCHO_FRAME1     = 83;  // Ancho del frame 1 (más ancho por el brazo + bola)
static const int ALTO_FRAME_GYRO  = 68;   // Alto de ambos frames (misma fila)
static const int GAP_ENTRE_FRAMES = 29;   // Espacio vacío entre frame 0 y frame 1

Jojobro::Jojobro() {
    estadoActual = Tranquilo;
    contadorAtaque = 0.0f;
    frameAtaqueActual = 0;
    contadorFrameAtaque = 0;
    cargarFramesAtaque(); // Cargo los 2 frames de lanzamiento animados
    setPos(700, 300);
    // Posición inicial fija en la derecha
}

// Cargo los 2 frames de ataque de Jojobro desde el sprite sheet.
// Para ajustar posiciones solo toca las constantes arriba, no este código.
// Los sprites del sheet apuntan a la DERECHA, pero Jojobro siempre lanza
// hacia la izquierda en el juego, así que los reflejo igual que Jotaro
// hace con sus frames en jotaro.cpp con reflejarHorizontal().
void Jojobro::cargarFramesAtaque()
{
    QPixmap sprites(":/sprites/SpritesJojoChampionship/sprites_juego.png");
    //QPixmap sprites("C:\\Users\\Emmanuel\\Documents\\DESAFIOIII\\sprites_juego.png");

    struct { int x, ancho; } data[] = {
        {X_FRAME0_ATAQUE, ANCHO_FRAME0},  // frame 0: Jojobro apuntando con el dedo
        {X_FRAME1_ATAQUE, ANCHO_FRAME1},  // frame 1: Jojobro con la bola ya lanzada
    };

    for (auto &d : data) {
        QPixmap frame = sprites.copy(d.x, Y_ATAQUE_GYRO, d.ancho, ALTO_FRAME_GYRO);
        frame = quitarFondo(frame);
        // Reflejo para que mire a la izquierda (dirección real del lanzamiento)
        framesAtaqueIzquierda.append(reflejarHorizontal(frame));
    }

    // Arrancamos en el primer frame
    if (!framesAtaqueIzquierda.isEmpty()) {
        setPixmap(framesAtaqueIzquierda[0]);
    }
}

// Quita el fondo oscuro (30, 27, 60) igual que en item.cpp y jotaro.cpp.
// Tolerancia de 12 para cubrir los píxeles de borde con compresión del PNG.
QPixmap Jojobro::quitarFondo(const QPixmap &original)
{
    QImage img = original.toImage().convertToFormat(QImage::Format_ARGB32);
    QColor fondoColor(30, 27, 60);

    for (int y = 0; y < img.height(); y++) {
        for (int x = 0; x < img.width(); x++) {
            QColor px = img.pixelColor(x, y);
            if (std::abs(px.red()   - fondoColor.red())   < 12 &&
                std::abs(px.green() - fondoColor.green()) < 12 &&
                std::abs(px.blue()  - fondoColor.blue())  < 12)
            {
                img.setPixelColor(x, y, QColor(0, 0, 0, 0));
            }
        }
    }
    return QPixmap::fromImage(img);
}

// Reflejo horizontal idéntico al de jotaro.cpp, reutilizo la misma lógica
QPixmap Jojobro::reflejarHorizontal(const QPixmap &original)
{
    QTransform espejo;
    espejo.scale(-1, 1);
    return original.transformed(espejo);
}

// Avanza el frame de animación cada 10 ticks (~6 fps de animación a 60fps de juego).
// Frame 0 = apuntando, Frame 1 = lanzando. Se alternan continuamente.
void Jojobro::actualizarAnimacion()
{
    if (framesAtaqueIzquierda.isEmpty()) return;

    contadorFrameAtaque++;
    if (contadorFrameAtaque >= 10) {
        contadorFrameAtaque = 0;
        frameAtaqueActual = (frameAtaqueActual + 1) % framesAtaqueIzquierda.size();
        setPixmap(framesAtaqueIzquierda[frameAtaqueActual]);
    }
}

// Lógica centralizada de la inteligencia artificial de Jojobro
void Jojobro::actualizarComportamiento(int tiempoRestante, qreal jotaroY, bool camaraLenta) {

    contadorAtaque++;
    actualizarAnimacion();

    if (tiempoRestante > 40)      estadoActual = Tranquilo;
    else if (tiempoRestante > 15) estadoActual = Rafaga;
    else                          estadoActual = Lluvia;

    // Intervalos según dificultad: difícil dispara más seguido
    int intervalTranquilo = modoDificil ? 45 : 80;
    int intervalRafaga    = modoDificil ? 40 : 65;
    int intervalLluvia    = modoDificil ? 35 : 65;

    if (estadoActual == Tranquilo) {
        if (contadorAtaque >= intervalTranquilo) {
            contadorAtaque = 0;
            int posicionesY[4] = {100, 200, 300, 400};
            setPos(800, posicionesY[std::rand() % 4]);
            SteelBall::TipoBola t = (std::rand() % 2 == 0) ? SteelBall::VerdeGolpeable : SteelBall::RojaEsquivable;
            SteelBall::TipoTrayectoria tray = static_cast<SteelBall::TipoTrayectoria>(std::rand() % 3);
            dispararBola(t, tray);
        }
    }
    else if (estadoActual == Rafaga) {
        if (contadorAtaque >= intervalRafaga) {
            contadorAtaque = 0;
            setPos(1000, (std::rand() % 300) + 150);
            SteelBall::TipoBola t = (std::rand() % 2 == 0) ? SteelBall::VerdeGolpeable : SteelBall::RojaEsquivable;
            dispararBola(t, SteelBall::Recta, -20);
            dispararBola(t, SteelBall::Recta, 10);
            dispararBola(t, SteelBall::Recta, 40);
        }
    }
    else if (estadoActual == Lluvia) {
        if (contadorAtaque >= intervalLluvia) {
            contadorAtaque = 0;
            setY(jotaroY);
            SteelBall::TipoBola t = (std::rand() % 3 == 0) ? SteelBall::VerdeGolpeable : SteelBall::RojaEsquivable;
            dispararBola(t, SteelBall::Senoidal);
            if (std::rand() % 10 < 4) {
                SteelBall::TipoBola t2 = (std::rand() % 2 == 0) ? SteelBall::VerdeGolpeable : SteelBall::RojaEsquivable;
                dispararBola(t2, SteelBall::Recta, -30);
            }
        }
    }
}
void Jojobro::dispararBola(SteelBall::TipoBola tipo, SteelBall::TipoTrayectoria trayec, int offsetY) {
    // Creamos la instancia pasándole la info necesaria
    SteelBall *nueva = new SteelBall(tipo, trayec, x(), y() + offsetY, -1); // -1 va a la izquierda
    bolasGeneradas.append(nueva);
}

QList<SteelBall*> Jojobro::tomarNuevasBolas() {
    QList<SteelBall*> lista = bolasGeneradas;
    bolasGeneradas.clear(); // Limpiamos la lista interna para el próximo frame
    return lista;
}

void Jojobro::setDificultad(bool esDificil) {
    modoDificil = esDificil;
}