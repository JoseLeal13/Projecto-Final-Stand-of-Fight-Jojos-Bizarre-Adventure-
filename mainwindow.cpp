#include "mainwindow.h"

// ── Constantes del sprite sheet ──────────────────
static const int ANCHO_FRAME = 70;
static const int ALTO_FRAME  = 100;

static const int FRAMES_QUIETO     = 1;
static const int FRAMES_MOVIMIENTO = 3;

static const int FRAMES_ATAQUE = 2;
static const int X_INICIO_ATAQUE = 720; // coordenada de pixel en el sprite
static const int Y_ATAQUE = 92;


static const int ANCHO_ATAQUE = 80;
static const int ALTO_ATAQUE = 81;
static const int MARGEN_ATAQUE = 24;


static const int X_INICIO_QUIETO     = 0;
static const int X_INICIO_MOVIMIENTO = 250;
static const int MARGEN_FRAME        = 12;

static const int Y_DIR[4] = {
    94,   // 0 = Arriba
    190,  // 1 = Abajo
    290,  // 2 = Izquierda
    390   // 3 = Derecha (reflejo, pero igual necesita Y) filas en la imagen
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(800, 600);
    fondo = QPixmap(":/fondo_juego.png");// fondo
    sprites = QPixmap(":/sprites_juego.png");

    frameGyro = sprites.copy(
        161,
        572,
        70,
        100
        );
    frameGyro = quitarFondo(frameGyro);
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout,
            this, &MainWindow::updateGame);

    timer->start(16);
    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;
    // ── Inicializar variables de animación ──
    frameActual    = 0;
    contadorFrames = 0;
    retardoFrames  = 8;   // cambia el frame cada 8 ticks
    direccion      = 1;   // empieza mirando abajo
    enMovimiento   = false;

    //ataques
    atacando = false;
    mostrarHitbox = false;


    cargarFrames();

}
void MainWindow::paintEvent(QPaintEvent *)//luego con drawPixmap para png
{
    /*QPainter painter(this);

    painter.drawPixmap(0, 0, this->width(), this->height(), fondo);

    painter.drawPixmap(jotaro.getX(), jotaro.getY(), frameJotaro);
    */
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), fondo);

    int drawX = jotaro.getX();

    if(atacando && direccion == 2)
    {
        drawX -= 100;
    }

    painter.drawPixmap(drawX, jotaro.getY(), getFrameActual());

    if(mostrarHitbox)
    {
        painter.setPen(Qt::red);

        // hitbox personaje
        painter.drawRect(jotaro.getHitbox());

        // hitbox ataque
        if(atacando)
        {
            painter.setPen(Qt::green);
            painter.drawRect(jotaro.getAttackHitbox());
        }
    }

    painter.drawPixmap(
        400,
        300,
        frameGyro
        );
    // Muestra el color exacto del fondo en consola
    /*painter.drawPixmap(0, 0, sprites);
    QImage img = sprites.toImage();
    QColor color = img.pixelColor(0, 0); // esquina del sprite sheet completo
    qDebug() << "Color fondo R:" << color.red()
             << "G:" << color.green()
             << "B:" << color.blue();
    */
}
QPixmap MainWindow::getFrameActual()
{


        // ATAQUE
        if(atacando)
        {
            int idx = frameActual % framesAtaqueDerecha.size();

            // izquierda
            if(direccion == 2)
            {
                return framesAtaqueIzquierda[idx];
            }

            // derecha
            return framesAtaqueDerecha[idx];
        }

        // MOVIMIENTO
        if(enMovimiento)
        {
            int idx = frameActual % framesMovimiento[direccion].size();

            return framesMovimiento[direccion][idx];
        }

        // QUIETO
        return framesQuieto[direccion][0];


}

QPixmap MainWindow::reflejarHorizontal(const QPixmap &original)
{
    // QTransform aplica una transformación de espejo en X
    QTransform espejo;
    espejo.scale(-1, 1); // -1 en X = voltear horizontalmente
    return original.transformed(espejo);
}

void MainWindow::cargarFrames()
{
    for (int dir = 0; dir < 3; dir++)
    {
        for (int f = 0; f < FRAMES_QUIETO; f++)
        {
            int x = X_INICIO_MOVIMIENTO + f * (ANCHO_FRAME + MARGEN_FRAME);
            QPixmap frame = sprites.copy(x, Y_DIR[dir], ANCHO_FRAME, ALTO_FRAME);
            frame = quitarFondo(frame);
            framesQuieto[dir].append(frame);
        }

        for (int f = 0; f < FRAMES_MOVIMIENTO; f++)
        {
            int x = X_INICIO_MOVIMIENTO + f * ANCHO_FRAME;
            QPixmap frame = sprites.copy(x, Y_DIR[dir], ANCHO_FRAME, ALTO_FRAME);
            frame = quitarFondo(frame);
            framesMovimiento[dir].append(frame);
        }
    }

    // Derecha = reflejo de Izquierda

    for (int f = 0; f < FRAMES_QUIETO; f++)
    {
        framesQuieto[3].append(
            reflejarHorizontal(framesQuieto[2][f])
            );
    }
    for (int f = 0; f < FRAMES_MOVIMIENTO; f++)
    {
        framesMovimiento[3].append(
            reflejarHorizontal(framesMovimiento[2][f])
            );
    }

    // ataque
    for(int f = 0; f < FRAMES_ATAQUE; f++)
    {
        if(f > 0){
            int x = X_INICIO_ATAQUE + f * ANCHO_ATAQUE;

            QPixmap frame = sprites.copy(
                x,
                Y_ATAQUE,
                ANCHO_ATAQUE + 115,
                ALTO_ATAQUE
                );

            frame = quitarFondo(frame);

            framesAtaqueDerecha.append(frame);

        }
        else{
            int x = X_INICIO_ATAQUE + f * (ANCHO_ATAQUE);

        QPixmap frame = sprites.copy(
            x,
            Y_ATAQUE,
            ANCHO_ATAQUE,
            ALTO_ATAQUE
            );

        frame = quitarFondo(frame);

        framesAtaqueDerecha.append(frame);
        }
    }
    // ataque izquierda
    for(int f = 0; f < FRAMES_ATAQUE; f++)
    {
        framesAtaqueIzquierda.append(
            reflejarHorizontal(framesAtaqueDerecha[f])
            );
    }
}
QPixmap MainWindow::quitarFondo(const QPixmap &original)
{
    QImage img = original.toImage().convertToFormat(QImage::Format_ARGB32);
    QColor fondoColor(30,27,60); // toma el color azul de la esquina

    for (int y = 0; y < img.height(); y++) {
        for (int x = 0; x < img.width(); x++) {
            QColor pixel = img.pixelColor(x, y);
            // Tolerancia de ±8 para no afectar el sombrero de jotaro
            if (abs(pixel.red()   - fondoColor.red())   < 8 &&
                abs(pixel.green() - fondoColor.green()) < 8 &&
                abs(pixel.blue()  - fondoColor.blue())  < 8)
            {
                img.setPixelColor(x, y, QColor(0, 0, 0, 0));
            }
        }
    }
    return QPixmap::fromImage(img);
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        upPressed = true;
    }

    if(event->key() == Qt::Key_S)
    {
        downPressed = true;
    }

    if(event->key() == Qt::Key_A)
    {
        leftPressed = true;
    }

    if(event->key() == Qt::Key_D)
    {
        rightPressed = true;
    }
    if(event ->key() == Qt::Key_J)
    {
        atacando = true;
    }
    if(event->key() == Qt::Key_H)
    {
        mostrarHitbox = !mostrarHitbox;
    }
}
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        upPressed = false;
    }

    if(event->key() == Qt::Key_S)
    {
        downPressed = false;
    }

    if(event->key() == Qt::Key_A)
    {
        leftPressed = false;
    }

    if(event->key() == Qt::Key_D)
    {
        rightPressed = false;
    }
    if(event->key() == Qt::Key_J)
    {
        atacando = false;
    }
}

void MainWindow::updateGame()
{
    enMovimiento = upPressed || downPressed || leftPressed || rightPressed;

    if (downPressed)
    {
        jotaro.moveDown();
        jotaro.setDireccion(0);
    }
    if (upPressed)    {
    jotaro.moveUp();    direccion = 1;
    jotaro.setDireccion(1);
    }
    if (leftPressed)  {
    jotaro.moveLeft();  direccion = 2;
    jotaro.setDireccion(2);
    }
    if (rightPressed) {
    jotaro.moveRight(); direccion = 3;
    jotaro.setDireccion(3);
    }

    contadorFrames++;
    if (contadorFrames >= retardoFrames)
    {
        contadorFrames = 0;
        frameActual++;

        int maxFrames = enMovimiento ? FRAMES_MOVIMIENTO : FRAMES_QUIETO;
        if (frameActual >= maxFrames)
            frameActual = 0;
    }
    /*if(atacando)
    {
        if(jotaro.getAttackHitbox().intersects(steelBall.getHitbox()))
        {
            steelBall.recibirGolpe();
        }
    }
    */
    if(atacando)
    {
        jotaro.atacar();
    }

    update();
}

//gongori