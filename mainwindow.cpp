#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>
#include <ctime>
#include "item.h" // Incluimos nuestro archivo de powerups

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Inicializar la semilla para los números aleatorios de Gyro
    std::srand(std::time(nullptr));

    // ── INICIALIZACIÓN DEL CONTROL SURVIVAL POR TIEMPO ──
    tiempoSurvival = 60;     // Sobrevive 60 segundos para ganar
    framesParaSegundo = 0;

    // ── CONFIGURACIÓN DEL ESCENARIO ──
    int anchoEscena = 800;
    int altoEscena  = 600;

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, anchoEscena, altoEscena);

    // Fondo asignado como un ítem de la escena
    //fondoPixmap = QPixmap(":/fondo_juego.png");
    //C:\Users\Emmanuel\Documents\DESAFIOIII
    fondoPixmap = QPixmap("C:\\Users\\Emmanuel\\Documents\\DESAFIOIII\\zona_entrenamiento.png");
    itemEscenario = new QGraphicsPixmapItem(
        fondoPixmap.scaled(anchoEscena, altoEscena, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    itemEscenario->setZValue(-1); // Se envía al fondo
    scene->addItem(itemEscenario);



    // ── HUD INFERIOR IZQUIERDO ──
    int hudX = 10;
    int hudY = 540; // cerca del borde inferior (escena mide 600 de alto)

    // Fondo barra vida
    hudBarraVidaFondo = new QGraphicsRectItem(hudX, hudY, 150, 18);
    hudBarraVidaFondo->setBrush(QBrush(QColor(50, 50, 50)));
    hudBarraVidaFondo->setPen(Qt::NoPen);
    hudBarraVidaFondo->setZValue(20);
    scene->addItem(hudBarraVidaFondo);

    // Barra vida verde
    hudBarraVida = new QGraphicsRectItem(hudX, hudY, 150, 18);
    hudBarraVida->setBrush(QBrush(QColor(100, 220, 100))); // verde claro
    hudBarraVida->setPen(Qt::NoPen);
    hudBarraVida->setZValue(21);
    scene->addItem(hudBarraVida);

    // Texto vida
    hudTextoVida = new QGraphicsTextItem("VIDA");
    hudTextoVida->setPos(hudX, hudY - 18);
    hudTextoVida->setDefaultTextColor(Qt::white);
    hudTextoVida->setFont(QFont("Arial", 8, QFont::Bold));
    hudTextoVida->setZValue(22);
    scene->addItem(hudTextoVida);

    // Fondo barra ulti
    hudBarraUltiFondo = new QGraphicsRectItem(hudX, hudY + 28, 150, 18);
    hudBarraUltiFondo->setBrush(QBrush(QColor(50, 50, 50)));
    hudBarraUltiFondo->setPen(Qt::NoPen);
    hudBarraUltiFondo->setZValue(20);
    scene->addItem(hudBarraUltiFondo);

    // Barra ulti azul celeste
    hudBarraUlti = new QGraphicsRectItem(hudX, hudY + 28, 0, 18);
    hudBarraUlti->setBrush(QBrush(QColor(80, 180, 255))); // azul celeste
    hudBarraUlti->setPen(Qt::NoPen);
    hudBarraUlti->setZValue(21);
    scene->addItem(hudBarraUlti);

    // Texto ulti
    hudTextoUlti = new QGraphicsTextItem("ULTI");
    hudTextoUlti->setPos(hudX, hudY + 10);
    hudTextoUlti->setDefaultTextColor(Qt::white);
    hudTextoUlti->setFont(QFont("Arial", 8, QFont::Bold));
    hudTextoUlti->setZValue(22);
    scene->addItem(hudTextoUlti);




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

    //Ahora inicializamos a Gyro como un objeto inteligente autónomo
    gyroJefe = new Gyro();
    scene->addItem(gyroJefe);

    // Banderas de estado lógicas
    enMovimiento   = false;
    atacando       = false;
    mostrarHitbox  = false;

    frameActual    = 0;
    contadorFrames = 0;
    retardoFrames  = 8;
    contadorSpawnItems = 0; // Inicializamos reloj de items

    // Cargar los frames de la explosion dorada una sola vez al inicio
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
    jotaro_player->actualizarEfectosItems(); // 🔌 Update de los items y poderes de Jotaro

    // ── B. ACTUALIZAR EXPLOSIONES DORADAS ACTIVAS ──
    for (int i = explosionesActivas.size() - 1; i >= 0; --i) {
        EfectoExplosion &ef = explosionesActivas[i];
        ef.contadorFrames++;

        if (ef.contadorFrames >= 9) {
            ef.contadorFrames = 0;
            ef.frameActual++;

            if (ef.frameActual < ef.frames.size()) {
                ef.item->setPixmap(ef.frames[ef.frameActual]);
            } else {
                scene->removeItem(ef.item);
                delete ef.item;
                explosionesActivas.removeAt(i);
            }
        }
    }

    framesParaSegundo++;
    if (framesParaSegundo >= 60) { // 60 ticks = 1 segundo real
        framesParaSegundo = 0;
        if (tiempoSurvival > 0) {
            tiempoSurvival--;
            qDebug() << "TIEMPO RESTANTE:" << tiempoSurvival << "s | VIDA:" << jotaro_player->getVida() << " | ENERGÍA L:" << jotaro_player->getEnergia() << "%";
        } else {
            qDebug() << "¡¡VICTORIA!! Jotaro sobrevivió al combate de Steel Balls.";
            timer->stop();
            return;
        }
    }

    if (jotaro_player->getVida() <= 0) {
        qDebug() << "── GAME OVER ── Jotaro se quedó sin energía.";
        timer->stop();
        return;
    }

    // 1. Detectar si hay movimiento
    enMovimiento = teclasPresionadas.contains(Qt::Key_W) ||
                   teclasPresionadas.contains(Qt::Key_S) ||
                   teclasPresionadas.contains(Qt::Key_A) ||
                   teclasPresionadas.contains(Qt::Key_D);

    jotaro_player->setEnMovimiento(enMovimiento);
    jotaro_player->setAtacando(atacando);

    // 2. Procesar direcciones
    if (teclasPresionadas.contains(Qt::Key_S)) {
        jotaro_player->moveDown();
        jotaro_player->setDireccion(0);
    }
    else if (teclasPresionadas.contains(Qt::Key_W)) {
        jotaro_player->moveUp();
        jotaro_player->setDireccion(1);
    }
    else if (teclasPresionadas.contains(Qt::Key_A)) {
        jotaro_player->moveLeft();
        jotaro_player->setDireccion(2);
    }
    else if (teclasPresionadas.contains(Qt::Key_D)) {
        jotaro_player->moveRight();
        jotaro_player->setDireccion(3);
    }

    // 3. Control de animaciones de Jotaro
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

    // Ralentización del tiempo por la Ulti "The World"
    bool camaraLenta = jotaro_player->estaUltiActiva();

    // 4. ACTUALIZAR LAS FÍSICAS DE TODAS LAS BOLAS
    for (int i = 0; i < esferasActivas.size(); ++i) {
        SteelBall *ball = esferasActivas[i];

        if (!camaraLenta || (contadorFrames % 3 == 0)) {
            ball->avanzarFisica();
        }

        if (ball->x() < -100 || ball->x() > 900 || ball->y() < -100 || ball->y() > 700) {
            scene->removeItem(ball);
            esferasActivas.removeAt(i);
            delete ball;
            --i;
            continue;
        }

        QList<QGraphicsItem*> itemsColisionando = scene->collidingItems(ball);

        if (itemsColisionando.contains(jotaro_player)) {
            if (ball->getTipo() == SteelBall::RojaEsquivable) {
                qDebug() << " ¡IMPACTO REAL! Bola ROJA inflige daño (-20).";
                jotaro_player->recibirDanio(20);
            } else {
                qDebug() << " ¡IMPACTO REAL! Bola VERDE inflige daño (-10).";
                jotaro_player->recibirDanio(10);
            }

            if (!framesExplosion.isEmpty()) {
                EfectoExplosion ef;
                ef.frames        = framesExplosion;
                ef.frameActual   = 0;
                ef.contadorFrames = 0;

                ef.item = new QGraphicsPixmapItem(ef.frames[0]);
                ef.item->setPos(jotaro_player->x() + 35 - ef.frames[0].width()  / 2.0,
                                jotaro_player->y() + 50 - ef.frames[0].height() / 2.0);
                ef.item->setZValue(10);
                scene->addItem(ef.item);
                explosionesActivas.append(ef);
            }

            scene->removeItem(ball);
            esferasActivas.removeAt(i);
            delete ball;
            --i;
        }
    }

    // 5. DETECTAR SI JOTARO GOLPEA UNA BOLA VERDE
    if (atacando) {
        for (int i = 0; i < esferasActivas.size(); ++i) {
            SteelBall *ball = esferasActivas[i];

            if (ball->getTipo() == SteelBall::VerdeGolpeable && !ball->estaCayendo()) {
                QRectF attackGlobal = jotaro_player->getAttackHitbox();
                QRectF hitboxBolaGlobal = ball->getHitbox().translated(ball->pos());

                if (attackGlobal.intersects(hitboxBolaGlobal)) {
                    qDebug() << " ¡ORA! Bola verde golpeada, inicia caida.";
                    ball->recibirGolpe();
                    jotaro_player->cargarEnergia(25); // Carga la barra del Ulti
                }
            }
        }
    }

    // 6. LÓGICA DE SPAWN CENTRALIZADA EN GYRO
    gyroJefe->actualizarComportamiento(tiempoSurvival, jotaro_player->y());

    QList<SteelBall*> nuevasBolas = gyroJefe->tomarNuevasBolas();
    for (SteelBall* b : nuevasBolas) {
        scene->addItem(b);
        esferasActivas.append(b);
    }

    // ── GESTIÓN DE ITEMS EN EL ESCENARIO ──
    contadorSpawnItems++;
    if (contadorSpawnItems >= 900) { // Cada 15 segundos
        contadorSpawnItems = 0;

        // Zona segura: solo aparecen en la mitad izquierda (lejos de Gyro que está en x=700)
        qreal posX = (std::rand() % 400) + 50;  // X entre 50 y 450
        qreal posY = (std::rand() % 400) + 100; // Y entre 100 y 500

        ItemJuego::TipoItem tipoRnd = (std::rand() % 2 == 0)
                                          ? ItemJuego::Vida
                                          : ItemJuego::Velocidad;

        ItemJuego *nuevoItem = new ItemJuego(tipoRnd, posX, posY);
        nuevoItem->setZValue(1);
        scene->addItem(nuevoItem);
        itemsActivos.append(nuevoItem);

        qDebug() << "Item spawneado en x=" << posX << "y=" << posY;
    }

    // ── ANIMAR Y COLISIONAR ITEMS ──
    for (int i = itemsActivos.size() - 1; i >= 0; --i) {
        ItemJuego *it = itemsActivos[i];

        it->actualizarAnimacion(); // anima los 6 frames cada tick

        if (jotaro_player->collidesWithItem(it)) {
            if (it->getTipo() == ItemJuego::Vida) {
                jotaro_player->curar(25);
                qDebug() << "¡Jotaro recogió Vida! +25 HP";
            }
            else if (it->getTipo() == ItemJuego::Velocidad) {
                jotaro_player->aumentarVelocidad();
                qDebug() << "¡Jotaro recogió Velocidad!";
            }

            scene->removeItem(it);
            itemsActivos.removeAt(i);
            delete it;
        }
    }

    // Sincronizar hitboxes globales por si se presiona la tecla de debug 'H'
    // Esto es importante: SteelBall busca esta propiedad en la ventana para saber si dibujar
    // la hitbox. Si no la seteamos aquí, nunca se activa aunque presionemos H.
    setProperty("mostrarHitbox", mostrarHitbox);
    if (mostrarHitbox) {
        jotaro_player->setMostrarHitbox(true);
    } else {
        jotaro_player->setMostrarHitbox(false);
    }


    // ── ACTUALIZAR HUD ──
    int vida  = jotaro_player->getVida();    // 0 a 100
    int ulti  = jotaro_player->getEnergia(); // 0 a 100

    // Ancho proporcional: 150px = 100%, entonces vida*1.5 = ancho actual
    hudBarraVida->setRect(10, 540, vida * 1.5, 18);
    hudBarraUlti->setRect(10, 568, ulti * 1.5, 18);

    // Actualizar texto
    hudTextoVida->setPlainText(QString("VIDA: %1/100").arg(vida));
    hudTextoUlti->setPlainText(QString("ULTI: %1%").arg(ulti));

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

    if (event->key() == Qt::Key_L) {
        jotaro_player->usarUlti(); // Activa Za Warudo si está llena al 100%
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

void MainWindow::cargarFramesExplosion()
{
    QPixmap sprites(":/sprites_juego.png");

    struct { int x, w; } data[] = {
                {1099,  53},
                {1170,  81},
                {1266, 102},
                {1370, 126},
                };

    QColor fondoColor(30, 27, 60);
    for (auto &d : data) {
        QPixmap frame = sprites.copy(d.x, 811, d.w, 88);

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
//gongori