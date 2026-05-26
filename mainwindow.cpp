#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int anchoEscena = 1200;
    int altoEscena  = 500;

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, anchoEscena, altoEscena);

    // Fondo asignado al atributo global de la clase
    QPixmap fondo(":/sprites/SpritesJojoChampionship/JoestarChampionship Jaula MMA.png");
    itemEscenario = new QGraphicsPixmapItem(
        fondo.scaled(anchoEscena, altoEscena, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    itemEscenario->setZValue(-1);
    scene->addItem(itemEscenario);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setFixedSize(anchoEscena, altoEscena);
    this->setFixedSize(anchoEscena + 2, altoEscena + 2);


    int ySuperficieSuelo = 400;
    int grosorSuelo = 20;
    int altoSprite  = 80*1.8;  // altura aproximada del sprite de Jotaro

    // Suelo invisible
    obstaculo *suelo = new obstaculo(0, ySuperficieSuelo, anchoEscena, grosorSuelo);
    scene->addItem(suelo);

    // Paredes invisibles fuera de pantalla
    short int grosorPared = 40;
    short int limiteIzquierdoX = 110;
    int limiteDerechoX = anchoEscena - 110;
    obstaculo *paredIzquierda = new obstaculo(limiteIzquierdoX - grosorPared, 0, grosorPared, altoEscena);
    obstaculo *paredDerecha   = new obstaculo(limiteDerechoX, 0, grosorPared, altoEscena);

    scene->addItem(paredIzquierda);
    scene->addItem(paredDerecha);
    // Personajes posicionados encima del suelo
    int yPersonaje = ySuperficieSuelo - altoSprite;

    jojo = new Jojo();
    jojo->setPos(150, yPersonaje);
    scene->addItem(jojo);

    dummy = new Jojo();
    dummy->setEsDummy(true);
    dummy->setPos(500, yPersonaje);
    dummy->setMirandoDerecha(false);
    //scene->addItem(dummy);

    dio = new DIO(jojo);
    dio->setPos(950, yPersonaje);
    scene->addItem(dio);

    usuario = new StandUserStats(scene);

    // Timer del juego a 60fps
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizar, Qt::UniqueConnection);
    timer->start(1000 / 60);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actualizar() {
    if (jojo) {
        // ── SI EL TIEMPO ESTÁ DETENIDO, JOTARO SE QUEDA TOTALMENTE CONGELADO ──
        if (Personaje::tiempoDetenido) {
            jojo->setVelocidadX(0); // Evita que acumule velocidad inercial
            // NOTA: No llamamos a moverse() ni a sus ataques. Queda estático.
        }
        else {
            // Movimiento normal solo si el tiempo fluye
            if (teclasPresionadas.contains(Qt::Key_A) || teclasPresionadas.contains(Qt::Key_Left)) {
                jojo->setVelocidadX(-7);
            } else if (teclasPresionadas.contains(Qt::Key_D) || teclasPresionadas.contains(Qt::Key_Right)) {
                jojo->setVelocidadX(7);
            } else {
                jojo->setVelocidadX(0);
            }

            jojo->moverse();
            jojo->actualizarAtaque();
            jojo->actualizarAtaquesFuertes();
            jojo->actualizarEspecial();
        }

        // El dummy también se congela si el tiempo se detiene
        if (dummy && !Personaje::tiempoDetenido) {
            dummy->moverse();
        }

        // DIO SIEMPRE SE MUEVE (Él es el dueño del TimeStop)
        if (dio) {
            dio->moverse();
        }

        // ── CONTROL VISUAL DE ZA WARUDO ──
        if (Personaje::tiempoDetenido) {
            if (!efectoGrisActivo) {
                efectoGrisActivo = true;
                aplicarEfectoZaWarudo(true);
            }
        } else {
            if (efectoGrisActivo) {
                efectoGrisActivo = false;
                aplicarEfectoZaWarudo(false);
            }
        }
    }
    if (usuario) {
        usuario->actualizarEstados(jojo, dio);
    }
}

void MainWindow::aplicarEfectoZaWarudo(bool activar) {
    if (!itemEscenario) return;

    if (activar) {
        // 1. Efecto Negativo en el Escenario
        QPixmap fondoActual = itemEscenario->pixmap();
        QImage img = fondoActual.toImage();
        img.invertPixels(QImage::InvertRgb); // Invierte los colores (Mítico en el anime)
        itemEscenario->setPixmap(QPixmap::fromImage(img));

        // 2. Apagar los colores de Jotaro (Gris/Azulado oscuro)
        if (jojo) {
            efectoJotaro = new QGraphicsColorizeEffect(this);
            efectoJotaro->setColor(QColor(70, 80, 95)); // Tono frío desaturado
            efectoJotaro->setStrength(0.85);
            jojo->setGraphicsEffect(efectoJotaro);
        }
    } else {
        // 1. Restaurar el escenario original recargando la imagen limpia
        int anchoEscena = 1200;
        int altoEscena  = 500;
        QPixmap fondoOriginal(":/sprites/SpritesJojoChampionship/JoestarChampionship Jaula MMA.png");
        itemEscenario->setPixmap(fondoOriginal.scaled(anchoEscena, altoEscena, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

        // 2. Quitar el efecto de Jotaro para devolverle sus colores vivos
        if (jojo && jojo->graphicsEffect()) {
            jojo->setGraphicsEffect(nullptr);
            efectoJotaro = nullptr;
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (!jojo || event->isAutoRepeat()) return;

    teclasPresionadas.insert(event->key());

    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        if (jojo->getVelocidadX() == 0) jojo->setFrameActual(0);
        jojo->setVelocidadX(-7);
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        if (jojo->getVelocidadX() == 0) jojo->setFrameActual(0);
        jojo->setVelocidadX(7);
        break;
    case Qt::Key_Space:
        jojo->saltar();
        break;
    case Qt::Key_K:
        jojo->setDefensa(true);
        break;
    case Qt::Key_J:
        if (teclasPresionadas.contains(Qt::Key_W)) {
            jojo->atacarFuerte(1);
        } else if (teclasPresionadas.contains(Qt::Key_S)) {
            jojo->atacarFuerte(2);
        } else {
            jojo->atacar();
        }
        break;
    case Qt::Key_L:
        jojo->habilidadEspecial();
        break;
    case Qt::Key_F1:
        Personaje::modoDebug = !Personaje::modoDebug;
        scene->update();
        qDebug() << "Modo Debug:" << Personaje::modoDebug;
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    teclasPresionadas.remove(event->key());

    if (!jojo) return;

    int key = event->key();

    if ((key == Qt::Key_A || key == Qt::Key_Left) && jojo->getVelocidadX() < 0) {
        jojo->setVelocidadX(0);
    } else if ((key == Qt::Key_D || key == Qt::Key_Right) && jojo->getVelocidadX() > 0) {
        jojo->setVelocidadX(0);
    }

    if (key == Qt::Key_K) {
        jojo->setDefensa(false);
    }
}
