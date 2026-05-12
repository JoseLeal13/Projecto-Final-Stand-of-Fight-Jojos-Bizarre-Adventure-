#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Configurar la escena
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1000, 600); // Un mundo ancho para caminar
    ui->graphicsView->setScene(scene);

    // 2. CREAR EL SUELO (Obstáculo)
    // Lo ponemos en la parte inferior (y=550) de extremo a extremo
    obstaculo *suelo = new obstaculo(0, 550, 1000, 50);
    scene->addItem(suelo);

    // 3. CREAR A JOTARO
    jojo = new Jojo();
    jojo->setPos(100, 400); // Posición inicial sobre el suelo
    scene->addItem(jojo);

    // 4. EL TIMER (El latido del juego)
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizar);
    timer->start(1000/60); // 60 FPS
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actualizar() {
    if (jojo) jojo->moverse();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (!jojo || event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        // Solo reinicia el frame si estaba quieto, para que la caminata sea fluida
        if (jojo->getVelocidadX() == 0) jojo->setFrameActual(0);
        jojo->setVelocidadX(-7);
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        if (jojo->getVelocidadX() == 0) jojo->setFrameActual(0);
        jojo->setVelocidadX(7);
        break;
    case Qt::Key_W:
    case Qt::Key_Space:
        jojo->saltar();
        break;
    case Qt::Key_K:
        jojo->setDefensa(true);
        break;
    case Qt::Key_J:
        jojo->atacar();
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (!jojo || event->isAutoRepeat()) return;

    int key = event->key();

    // LÓGICA DE MOVIMIENTO INDEFINIDO:
    // Solo frenamos si la tecla que soltamos es la que nos estaba moviendo.
    if ((key == Qt::Key_A || key == Qt::Key_Left) && jojo->getVelocidadX() < 0) {
        jojo->setVelocidadX(0);
    }
    else if ((key == Qt::Key_D || key == Qt::Key_Right) && jojo->getVelocidadX() > 0) {
        jojo->setVelocidadX(0);
    }

    if (key == Qt::Key_K) {
        jojo->setDefensa(false);
    }
}
