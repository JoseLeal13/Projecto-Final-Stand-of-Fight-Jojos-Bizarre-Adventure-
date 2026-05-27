#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib> // Necesario para rand() y srand()
#include <ctime>   // Necesario para time()

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Inicializar la semilla para los números aleatorios de Gyro
    std::srand(std::time(nullptr));

    // ── CONFIGURACIÓN DEL ESCENARIO ──
    int anchoEscena = 800;
    int altoEscena  = 600;

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, anchoEscena, altoEscena);

    // Fondo asignado como un ítem de la escena
    //fondoPixmap = QPixmap(":/fondo_juego.png");
    fondoPixmap = QPixmap(":/zona_entrenamiento.png");
    itemEscenario = new QGraphicsPixmapItem(
        fondoPixmap.scaled(anchoEscena, altoEscena, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    itemEscenario->setZValue(-1); // Se envía al fondo
    scene->addItem(itemEscenario);

    // Configuración de la vista gráfica del UI
    view = new QGraphicsView(scene, this);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFixedSize(anchoEscena, altoEscena);

    setCentralWidget(view); // Hace que la vista ocupe la pantalla automáticamente
    this->setFixedSize(anchoEscena + 2, altoEscena + 2);

    // ── INICIALIZACIÓN DE JOTARO Y ENEMY ──
    jotaro_player = new jotaro();
    jotaro_player->setPos(100, 100); // Tu posición inicial
    scene->addItem(jotaro_player);

    // frameGyro se vuelve un elemento de la escena independiente
    QPixmap sprites(":/sprites_juego.png");
    QPixmap frameGyro = sprites.copy(161, 572, 70, 100);

    gyroItem = new QGraphicsPixmapItem(frameGyro);
    gyroItem->setPos(400, 300);
    scene->addItem(gyroItem);

    // Banderas de estado lógicas
    enMovimiento   = false;
    atacando       = false;
    mostrarHitbox  = false;

    frameActual    = 0;
    contadorFrames = 0;
    retardoFrames  = 8;
    contadorSpawnBolas = 0; // Inicializamos el contador de esferas de forma segura

    // ── TIMER DEL JUEGO SINCRONIZADO A 60 FPS ──
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizar, Qt::UniqueConnection);
    timer->start(1000 / 60);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actualizar()
{
    if (!jotaro_player) return;

    // 1. Detectar si hay movimiento revisando el contenedor de teclas
    enMovimiento = teclasPresionadas.contains(Qt::Key_W) ||
                   teclasPresionadas.contains(Qt::Key_S) ||
                   teclasPresionadas.contains(Qt::Key_A) ||
                   teclasPresionadas.contains(Qt::Key_D);

    jotaro_player->setEnMovimiento(enMovimiento);
    jotaro_player->setAtacando(atacando);

    // 2. Procesar direcciones según los controles activos (Corregido el eje arriba/abajo)
    if (teclasPresionadas.contains(Qt::Key_S)) {
        jotaro_player->moveDown();
        jotaro_player->setDireccion(0); // Abajo según tu Y_DIR
    }
    else if (teclasPresionadas.contains(Qt::Key_W)) {
        jotaro_player->moveUp();
        jotaro_player->setDireccion(1); // Arriba según tu Y_DIR
    }
    else if (teclasPresionadas.contains(Qt::Key_A)) {
        jotaro_player->moveLeft();
        jotaro_player->setDireccion(2); // Izquierda
    }
    else if (teclasPresionadas.contains(Qt::Key_D)) {
        jotaro_player->moveRight();
        jotaro_player->setDireccion(3); // Derecha
    }

    // 3. Control y avance del ciclo de animaciones de Jotaro
    contadorFrames++;
    if (contadorFrames >= retardoFrames) {
        contadorFrames = 0;
        frameActual++;

        int maxFrames = enMovimiento ? 3 : 1;
        if (frameActual >= maxFrames) {
            frameActual = 0;
        }
        jotaro_player->actualizarFrame(frameActual);
    }

    // 4. ACTUALIZAR LAS FÍSICAS DE TODAS LAS BOLAS EN PANTALLA
    for (int i = 0; i < esferasActivas.size(); ++i) {
        SteelBall *ball = esferasActivas[i];
        ball->avanzarFisica();

        // Verificar si la bola salió de los límites para borrarla
        if (ball->x() < -50 || ball->x() > 850 || ball->y() < -50 || ball->y() > 650) {
            scene->removeItem(ball);
            esferasActivas.removeAt(i);
            delete ball;
            --i;
            continue;
        }

        // DETECTAR COLISIONES CONTRA JOTARO
        if (jotaro_player->collidesWithItem(ball)) {
            if (ball->getTipo() == SteelBall::RojaEsquivable) {
                qDebug() << "¡Daño inevitable! Golpeado por bola ROJA.";
            } else {
                qDebug() << "¡No la golpeaste a tiempo! Daño por bola VERDE.";
            }

            scene->removeItem(ball);
            esferasActivas.removeAt(i);
            delete ball;
            --i;
        }
    }

    // 5. DETECTAR SI JOTARO ESTÁ ATACANDO Y GOLPEA UNA BOLA VERDE
    if (atacando) {
        for (int i = 0; i < esferasActivas.size(); ++i) {
            SteelBall *ball = esferasActivas[i];
            if (ball->getTipo() == SteelBall::VerdeGolpeable) {
                if (jotaro_player->getAttackHitbox().intersects(ball->getHitbox())) {
                    qDebug() << "¡ORA! Bola verde destruida.";
                    ball->recibirGolpe();
                    esferasActivas.removeAt(i);
                    --i;
                }
            }
        }
    }

    // 6. LÓGICA DE SPAWN: GYRO SE TELETRANSPORTA Y LANZA LAS ESFERAS
    contadorSpawnBolas++;
    if (contadorSpawnBolas >= 90) { // Cada 1.5 segundos (90 frames / 60 fps) para más dinamismo
        contadorSpawnBolas = 0;

        // Cambiar a Gyro de lugar en posiciones fijas del mapa
        int posicionesY[3] = {150, 300, 450};
        int nuevaY = posicionesY[std::rand() % 3];
        int nuevaX = (std::rand() % 2 == 0) ? 50 : 700;
        gyroItem->setPos(nuevaX, nuevaY);

        // Si aparece a la izquierda (50), dispara a la derecha (1). Si no, a la izquierda (-1)
        int dirDisparo = (nuevaX < 400) ? 1 : -1;

        // Elegir Tipo y Trayectoria Aleatoria Matemáticamente Segura
        SteelBall::TipoBola tipoRandom = (std::rand() % 2 == 0) ? SteelBall::VerdeGolpeable : SteelBall::RojaEsquivable;
        SteelBall::TipoTrayectoria trayectRandom = static_cast<SteelBall::TipoTrayectoria>(std::rand() % 3);

        // Crear la bola e ingresarla al motor físico de la escena
        SteelBall *nuevaBola = new SteelBall(tipoRandom, trayectRandom, nuevaX, nuevaY + 30, dirDisparo);
        scene->addItem(nuevaBola);
        esferasActivas.append(nuevaBola);
    }

    if (mostrarHitbox) {
        scene->update();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!jotaro_player || event->isAutoRepeat()) return;

    teclasPresionadas.insert(event->key());

    if (event->key() == Qt::Key_J) {
        atacando = true;
    }

    if (event->key() == Qt::Key_H) {
        mostrarHitbox = !mostrarHitbox;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    teclasPresionadas.remove(event->key());

    if (event->key() == Qt::Key_J) {
        atacando = false;
    }
}
//gongori