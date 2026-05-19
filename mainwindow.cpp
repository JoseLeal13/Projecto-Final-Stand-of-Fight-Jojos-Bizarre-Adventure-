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

    // Fondo escalado al tamaño de la escena
    QPixmap fondo(":/sprites/SpritesJojoChampionship/JoestarChampionship Jaula MMA.png");
    QGraphicsPixmapItem* itemFondo = new QGraphicsPixmapItem(
        fondo.scaled(anchoEscena, altoEscena, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    itemFondo->setZValue(-1);
    scene->addItem(itemFondo);

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
    scene->addItem(dummy);

    dio = new DIO(jojo);
    dio->setPos(950, yPersonaje);
    scene->addItem(dio);

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

        if (dummy) {
            dummy->moverse();
        }

        if (dio) {
            dio->moverse();
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
