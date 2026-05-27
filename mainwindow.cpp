#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nivel2.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , nivelActual(nullptr) // Inicializa tu puntero de nivel en el .h si no lo tienes
{
    ui->setupUi(this);

    int anchoEscena = 1200;
    int altoEscena  = 500;

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, anchoEscena, altoEscena);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setFixedSize(anchoEscena, altoEscena);
    this->setFixedSize(anchoEscena + 2, altoEscena + 2);

    // 1. Creamos ÚNICAMENTE el objeto global de Jojo (Nivel 2 usa el por defecto)
    jojo = new Jojo();

    // 2. Cargamos el Nivel 2 directamente pasando la escena y el puntero de jojo
    nivelActual = new Nivel2(scene, jojo, this);
    nivelActual->iniciarNivel(); // Esto activará el fondo de Egipto, a DIO y los 60FPS internos del nivel
}

MainWindow::~MainWindow() {
    if (nivelActual) delete nivelActual;
    delete ui;
}

// =======================================================================
// ¡EL INPUT AHORA SE GESTIONA DIRECTAMENTE DESDE LA VENTANA HACIA EL NIVEL!
// =======================================================================
void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    // Registramos la tecla en el set global si lo usas
    teclasPresionadas.insert(event->key());

    // --- LÓGICA DE MOVIMIENTO DE JOTARO ---
    if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) {
        if (jojo->getVelocidadX() == 0) jojo->setFrameActual(0);
        jojo->setVelocidadX(-7);
    }
    else if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) {
        if (jojo->getVelocidadX() == 0) jojo->setFrameActual(0);
        jojo->setVelocidadX(7);
    }
    else if (event->key() == Qt::Key_Space) {
        jojo->saltar();
    }
    else if (event->key() == Qt::Key_K) {
        jojo->setDefensa(true);
    }
    // --- ATAQUES (Enviados al Jotaro del nivel actual) ---
    else if (event->key() == Qt::Key_J) {
        if (teclasPresionadas.contains(Qt::Key_W))      jojo->atacarFuerte(1);
        else if (teclasPresionadas.contains(Qt::Key_S)) jojo->atacarFuerte(2);
        else                                            jojo->atacar();
    }
    else if (event->key() == Qt::Key_L) {
        jojo->habilidadEspecial();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
    teclasPresionadas.remove(event->key());

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
