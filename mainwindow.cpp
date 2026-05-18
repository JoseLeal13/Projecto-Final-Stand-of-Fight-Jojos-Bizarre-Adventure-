#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Definimos un tamaño fijo para la escena (ejemplo: 800 de ancho x 600 de alto)
    int anchoEscena = 800;
    int altoEscena = 600;

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, anchoEscena, altoEscena);
    ui->graphicsView->setScene(scene);

    // 2. Ajustamos el QGraphicsView para que no cree barras de scroll ni bordes raros
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setFixedSize(anchoEscena, altoEscena);
    this->setFixedSize(anchoEscena + 20, altoEscena + 20); // Ventana contenedora ligeramente más grande

    // 3. Colocamos el SUELO (Grosor: 50px).
    // Debe estar al fondo de la escena: Y = AltoEscena - Grosor (600 - 50 = 550)
    int grosorSuelo = 50;
    obstaculo *suelo = new obstaculo(0, altoEscena - grosorSuelo, anchoEscena, grosorSuelo);
    scene->addItem(suelo);

    // 4. Colocamos las PAREDES (Grosor: 40px)
    int grosorPared = 40;
    // Pared Izquierda: desde X = -40 hasta X = 0 (así no se ve en pantalla, pero frena)
    obstaculo *paredIzquierda = new obstaculo(-grosorPared, 0, grosorPared, altoEscena);
    // Pared Derecha: empieza justo donde termina la escena (X = 800)
    obstaculo *paredDerecha = new obstaculo(anchoEscena, 0, grosorPared, altoEscena);

    scene->addItem(paredIzquierda);
    scene->addItem(paredDerecha);

    // 5. Inicializamos y posicionamos a los personajes de forma segura dentro del escenario
    jojo = new Jojo();
    // Y = 550 (suelo) - 90 (alto del sprite de Jojo) = 460 para que pise el suelo perfectamente
    jojo->setPos(150, 460);
    scene->addItem(jojo);

    // El puntero dummy debe ser miembro de la clase (en mainwindow.h) para poder llamarlo en actualizar()
    dummy = new Jojo();
    dummy->setEsDummy(true);
    dummy->setPos(200, 460); // Centrado en la mitad derecha
    dummy->setMirandoDerecha(false);
    scene->addItem(dummy);

    // 6. Configuración del Timer del juego
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizar, Qt::UniqueConnection);
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
        if (dummy) {
            dummy->moverse(); // Procesa su gravedad, sus estados de daño y animaciones de golpe
        }
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
