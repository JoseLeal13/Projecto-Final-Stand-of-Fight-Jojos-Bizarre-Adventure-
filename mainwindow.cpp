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

    // ── INICIALIZACIÓN DEL CONTROL SURVIVAL POR TIEMPO ──
    tiempoSurvival = 60;     // Sobrevive 30 segundos para ganar
    framesParaSegundo = 0;

    // ── CONFIGURACIÓN DEL ESCENARIO ──
    int anchoEscena = 800;
    int altoEscena  = 600;

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, anchoEscena, altoEscena);

    // Fondo asignado como un ítem de la escena
    fondoPixmap = QPixmap(":/fondo_juego.png");
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

    // ── A. PROCESAR ESTADOS DE INMUNIDAD Y TIEMPOS SURVIVAL ──
    jotaro_player->actualizarInvulnerabilidad();

    framesParaSegundo++;
    if (framesParaSegundo >= 60) { // Si el temporizador contó 60 ticks, pasó 1 segundo real
        framesParaSegundo = 0;
        if (tiempoSurvival > 0) {
            tiempoSurvival--;
            qDebug() << "TIEMPO RESTANTE:" << tiempoSurvival << "s | VIDA DE JOTARO:" << jotaro_player->getVida();
        } else {
            qDebug() << "¡¡VICTORIA!! Jotaro sobrevivió al combate de Steel Balls.";
            timer->stop(); // Congela el juego al ganar
            return;
        }
    }

    // Terminar juego inmediatamente si Jotaro se queda sin vida
    if (jotaro_player->getVida() <= 0) {
        qDebug() << "── GAME OVER ── Jotaro se quedó sin energía.";
        timer->stop(); // Congela el juego
        return;
    }

    // 1. Detectar si hay movimiento revisando el contenedor de teclas
    enMovimiento = teclasPresionadas.contains(Qt::Key_W) ||
                   teclasPresionadas.contains(Qt::Key_S) ||
                   teclasPresionadas.contains(Qt::Key_A) ||
                   teclasPresionadas.contains(Qt::Key_D);

    jotaro_player->setEnMovimiento(enMovimiento);
    jotaro_player->setAtacando(atacando);

    // 2. Procesar direcciones según los controles activos
    if (teclasPresionadas.contains(Qt::Key_S)) {
        jotaro_player->moveDown();
        jotaro_player->setDireccion(0); // Modificado a 1 según tu Y_DIR real (Abajo)
    }
    else if (teclasPresionadas.contains(Qt::Key_W)) {
        jotaro_player->moveUp();
        jotaro_player->setDireccion(1); // Modificado a 0 según tu Y_DIR real (Arriba)
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

    // 4. ACTUALIZAR LAS FÍSICAS DE TODAS LAS BOLAS EN PANTALLA Y VERIFICAR DAÑO
    // 4. ACTUALIZAR LAS FÍSICAS DE TODAS LAS BOLAS Y VERIFICAR DAÑO (VERSIÓN ESCENA)
    for (int i = 0; i < esferasActivas.size(); ++i) {
        SteelBall *ball = esferasActivas[i];
        ball->avanzarFisica();

        // Verificar si la bola salió de los límites para borrarla de forma segura
        if (ball->x() < -100 || ball->x() > 900 || ball->y() < -100 || ball->y() > 700) {
            scene->removeItem(ball);
            esferasActivas.removeAt(i);
            delete ball;
            --i;
            continue;
        }

        // ── DETECCIÓN TOTALMENTE INMUNE A ERRORES DE TRASLACIÓN ──
        // Le preguntamos a la escena qué ítems están colisionando directamente con Jotaro
        QList<QGraphicsItem*> itemsColisionando = scene->collidingItems(ball);

        if (itemsColisionando.contains(jotaro_player)) {

            if (ball->getTipo() == SteelBall::RojaEsquivable) {
                qDebug() << "💥 ¡IMPACTO REAL! Bola ROJA inflige daño (-20).";
                jotaro_player->recibirDanio(20);
            } else {
                qDebug() << "🟢 ¡IMPACTO REAL! Bola VERDE inflige daño (-10).";
                jotaro_player->recibirDanio(10);
            }

            // Borrar la bola inmediatamente para que no deje "zonas fantasmas" de daño
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

                //  CORRECCIÓN DE HITBOX DE ATAQUE: Pasar a coordenadas globales
                QRectF attackGlobal = jotaro_player->getAttackHitbox().translated(jotaro_player->pos());
                QRectF hitboxBolaGlobal = ball->getHitbox().translated(ball->pos());

                if (attackGlobal.intersects(hitboxBolaGlobal)) {
                    qDebug() << " ¡ORA! Bola verde destruida con éxito.";
                    ball->recibirGolpe();

                    // TODO: Aquí es donde pintaremos el "EFECTO DE IMPACTO" antes de borrar la bola

                    esferasActivas.removeAt(i);
                    --i;
                }
            }
        }
    }

    // 6. LÓGICA DE SPAWN: GYRO SE TELETRANSPORTA Y LANZA LAS ESFERAS
    contadorSpawnBolas++;
    if (contadorSpawnBolas >= 45) { // (90) Cada 1.5 segundos
        contadorSpawnBolas = 0;

        int posicionesY[3] = {150, 300, 450};
        int nuevaY = posicionesY[std::rand() % 3];
        int nuevaX = (std::rand() % 2 == 0) ? 700 : 700;//50
        gyroItem->setPos(nuevaX, nuevaY);

        int dirDisparo = (nuevaX < 400) ? 1 : -1;

        SteelBall::TipoBola tipoRandom = (std::rand() % 2 == 0) ? SteelBall::VerdeGolpeable : SteelBall::RojaEsquivable;
        SteelBall::TipoTrayectoria trayectRandom = static_cast<SteelBall::TipoTrayectoria>(std::rand() % 3);

        SteelBall *nuevaBola = new SteelBall(tipoRandom, trayectRandom, nuevaX, nuevaY + 30, dirDisparo);
        scene->addItem(nuevaBola);
        esferasActivas.append(nuevaBola);
    }

    // Asegurar que se le avise a Jotaro si dibuja o no sus cajas al presionar 'H'
    if (mostrarHitbox) {
        jotaro_player->setMostrarHitbox(true);
        scene->update();
    } else {
        jotaro_player->setMostrarHitbox(false);
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