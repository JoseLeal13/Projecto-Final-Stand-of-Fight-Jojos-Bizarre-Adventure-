#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1000, 600);
    ui->graphicsView->setScene(scene);

    obstaculo *suelo = new obstaculo(0, 550, 1000, 50);
    scene->addItem(suelo);

    jojo = new Jojo();

    jojo->setPos(100, 480);
    scene->addItem(jojo);

    Jojo *dummy = new Jojo();
    dummy->setEsDummy(true);
    dummy->setPos(500, 480);
    dummy->setMirandoDerecha(false);
    scene->addItem(dummy);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizar);
    timer->start(1000/60);
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
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (!jojo || event->isAutoRepeat()) return;

    // Guardamos la tecla actual
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
        // Verificamos si W o S están en el set ANTES de disparar
        if (teclasPresionadas.contains(Qt::Key_W)) {
            jojo->atacarFuerte(1); // W + J
        }
        else if (teclasPresionadas.contains(Qt::Key_S)) {
            jojo->atacarFuerte(2); // S + J
        }
        else {
            jojo->atacar(); // J normal
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
    }
    else if ((key == Qt::Key_D || key == Qt::Key_Right) && jojo->getVelocidadX() > 0) {
        jojo->setVelocidadX(0);
    }

    if (key == Qt::Key_K) {
        jojo->setDefensa(false);
    }
}
