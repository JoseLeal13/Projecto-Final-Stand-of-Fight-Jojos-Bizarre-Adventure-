#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>
#include <ctime>

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
    contadorSpawnBolas = 0;

    // Cargar los frames de la explosion dorada una sola vez al inicio
    // (los reutilizamos para cada explosion que aparezca en pantalla)
    cargarFramesExplosion();

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

    // ── B. ACTUALIZAR EXPLOSIONES DORADAS ACTIVAS ──
    // Recorremos al revés para poder borrar sin problema de índices
    for (int i = explosionesActivas.size() - 1; i >= 0; --i) {
        EfectoExplosion &ef = explosionesActivas[i];
        ef.contadorFrames++;

        // Cada 9 ticks avanzamos un frame de la explosion
        if (ef.contadorFrames >= 9) {
            ef.contadorFrames = 0;
            ef.frameActual++;

            if (ef.frameActual < ef.frames.size()) {
                // Actualizar el pixmap del item en la escena con el nuevo frame
                ef.item->setPixmap(ef.frames[ef.frameActual]);
            } else {
                // Ya terminó la animacion: sacamos el item de la escena y lo borramos
                scene->removeItem(ef.item);
                delete ef.item;
                explosionesActivas.removeAt(i);
            }
        }
    }

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

            // Crear la explosion dorada como un QGraphicsPixmapItem simple en la escena
            // No necesitamos una clase, solo un item que cambia de pixmap cada N ticks
            if (!framesExplosion.isEmpty()) {
                EfectoExplosion ef;
                ef.frames        = framesExplosion; // copia de los frames ya cargados
                ef.frameActual   = 0;
                ef.contadorFrames = 0;

                // Crear el item y centrarlo en Jotaro (el offset 35,50 es el centro aprox)
                ef.item = new QGraphicsPixmapItem(ef.frames[0]);
                ef.item->setPos(jotaro_player->x() + 35 - ef.frames[0].width()  / 2.0,
                                jotaro_player->y() + 50 - ef.frames[0].height() / 2.0);
                ef.item->setZValue(10); // Que aparezca encima de todo
                scene->addItem(ef.item);
                explosionesActivas.append(ef);
            }

            // Borrar la bola para que no siga dando daño
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

            // Solo golpear bolas que NO esten ya cayendo (para no golpearla dos veces)
            if (ball->getTipo() == SteelBall::VerdeGolpeable && !ball->estaCayendo()) {

                // getAttackHitbox() ya viene en coordenadas globales, sin translated()
                QRectF attackGlobal = jotaro_player->getAttackHitbox();
                QRectF hitboxBolaGlobal = ball->getHitbox().translated(ball->pos());

                if (attackGlobal.intersects(hitboxBolaGlobal)) {
                    qDebug() << "👊 ¡ORA! Bola verde golpeada, inicia caida.";

                    // El destello morado lo maneja la propia SteelBall en su paint()
                    // cuando activamos la caida con recibirGolpe()
                    ball->recibirGolpe();
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

// Carga los 4 frames de la explosion dorada una sola vez al iniciar el juego.
// Los guardamos en framesExplosion y los copiamos cada vez que haya un impacto.
// Coordenadas sacadas midiendo el sprite sheet con Python.
void MainWindow::cargarFramesExplosion()
{
    QPixmap sprites(":/sprites_juego.png");

    // Fila de explosion dorada: y=811 a y=899, 4 frames de humo y destello
    struct { int x, w; } data[] = {
        {1099,  53},   // frame 0: humo pequeño
        {1170,  81},   // frame 1: humo creciendo
        {1266, 102},   // frame 2: explosion grande
        {1370, 126},   // frame 3: destello final dorado
    };

    QColor fondoColor(30, 27, 60);
    for (auto &d : data) {
        QPixmap frame = sprites.copy(d.x, 811, d.w, 88);

        // Quitar el fondo oscuro del sprite sheet
        QImage img = frame.toImage().convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; y < img.height(); y++) {
            for (int x = 0; x < img.width(); x++) {
                QColor px = img.pixelColor(x, y);
                if (abs(px.red()   - fondoColor.red())   < 10 &&
                    abs(px.green() - fondoColor.green()) < 10 &&
                    abs(px.blue()  - fondoColor.blue())  < 10) {
                    img.setPixelColor(x, y, QColor(0,0,0,0));
                }
            }
        }
        framesExplosion.append(QPixmap::fromImage(img));
    }
}
//gongori