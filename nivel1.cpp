#include "nivel1.h"
#include <QPixmap>
#include <QImage>
#include <cstdlib>

// ==========================================
// CONSTRUCTOR
// ==========================================
Nivel1::Nivel1(QGraphicsScene* escenaCompartida, Jojo* personajeJojo, const QString& dificultad, QObject* parent)
    : Nivel(escenaCompartida, personajeJojo, parent)
{
    gyroJefe           = nullptr;
    interfazHUD        = nullptr;
    fondoMapa          = nullptr;

    this->dificultad   = dificultad;
    contadorSpawnItems = 0;
    mostrarHitbox      = false;
    nivelFinalizado    = false;

    tiempoRestante     = 60;

    timerUnSegundo = new QTimer(this);
    cargarFramesExplosion();
}

// ==========================================
// DESTRUCTOR
// ==========================================
Nivel1::~Nivel1() {
    limpiarNivel();
}

// ==========================================
// INICIAR NIVEL
// ==========================================
void Nivel1::iniciarNivel() {
    // ── Fondo ──
    //QPixmap fondo("C:\\Users\\Emmanuel\\Documents\\DESAFIOIII\\zona_entrenamiento.png");
    QPixmap fondo(":/sprites/SpritesJojoChampionship/zona_entrenamiento.png");
    fondoMapa = new QGraphicsPixmapItem(
        fondo.scaled(1200, 550, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    fondoMapa->setPos(0, 0);
    fondoMapa->setZValue(-1);
    escena->addItem(fondoMapa);

    // ── Jojo ──
    if (jojo && !escena->items().contains(jojo)) {
        escena->addItem(jojo);
    }

    // ── Gyro ──
    gyroJefe = new Jojobro();
    gyroJefe->setDificultad(dificultad == "dificil"); //
    escena->addItem(gyroJefe);

    // ── HUD Survival (sin barras de DIO) ──
    interfazHUD = new StandUserStats(escena, StandUserStats::SURVIVAL);

    // ── Posición inicial de Jojo ──
    configurarJojo();

    // ── Anuncio de inicio ──
    if (interfazHUD) {
        interfazHUD->actualizarRelojYMarcador(tiempoRestante, 0, 0);
        interfazHUD->mostrarAnuncioCentral("¡SOBREVIVE!", QColor(255, 140, 0));
    }
    QTimer::singleShot(2000, this, [this]() {
        if (interfazHUD) interfazHUD->ocultarAnuncioCentral();
    });

    // ── Timers ──
    connect(gameLoopTimer,  &QTimer::timeout, this, &Nivel1::actualizarLoop);
    connect(timerUnSegundo, &QTimer::timeout, this, &Nivel1::actualizarSegundo);

    gameLoopTimer->start(1000 / 60);
    timerUnSegundo->start(1000);
}

// ==========================================
// CONFIGURAR JOJO AL INICIO
// ==========================================
void Nivel1::configurarJojo() {
    if (!jojo) return;
    jojo->setPos(100, 200);
    jojo->vida             = 100;
    jojo->direccion        = 3;   // mirando derecha
    jojo->enMovimiento     = false;
    jojo->atacando         = false;
    jojo->invencible       = false;
    jojo->contadorInvencible = 0;
    jojo->energiaUlti      = 0;
    jojo->ultiActiva       = false;
}

// ==========================================
// LOOP PRINCIPAL — 60 FPS
// ==========================================
void Nivel1::actualizarLoop() {
    if (nivelFinalizado || !jojo) return;

    // ── Movimiento WASD ──
    if (teclasPresionadas.contains(Qt::Key_W)) { jojo->moveUp();    jojo->setDireccion(1); }
    if (teclasPresionadas.contains(Qt::Key_S)) { jojo->moveDown();  jojo->setDireccion(0); }
    if (teclasPresionadas.contains(Qt::Key_A)) { jojo->moveLeft();  jojo->setDireccion(2); }
    if (teclasPresionadas.contains(Qt::Key_D)) { jojo->moveRight(); jojo->setDireccion(3); }

    bool enMov = teclasPresionadas.contains(Qt::Key_W) ||
                 teclasPresionadas.contains(Qt::Key_S) ||
                 teclasPresionadas.contains(Qt::Key_A) ||
                 teclasPresionadas.contains(Qt::Key_D);

    jojo->setEnMovimiento(enMov);

    if (enMov) {
        contadorFrames++;
        if (contadorFrames >= 8) {
            contadorFrames = 0;
            frameActualJugador = (frameActualJugador + 1) % 3;
            jojo->actualizarFrame(frameActualJugador);
        }
    } else {
        frameActualJugador = 0;
        jojo->actualizarFrame(0);
    }

    // ── Ataque con J (solo J, sin requerir A o D) ──
    bool atacando = teclasPresionadas.contains(Qt::Key_J);
    jojo->setAtacando(atacando);

    jojo->actualizarInvulnerabilidad();
    jojo->actualizarEfectosItems();

    // ── Gyro lanza bolas ──
    bool modoDificil = (dificultad == "dificil");
    gyroJefe->actualizarComportamiento(tiempoRestante, jojo->y(), modoDificil);


    QList<SteelBall*> nuevasBolas = gyroJefe->tomarNuevasBolas();
    for (SteelBall* b : nuevasBolas) {
        escena->addItem(b);
        esferasActivas.append(b);
    }

    // ── Steel Balls: física + colisión con Jojo ──
    for (int i = esferasActivas.size() - 1; i >= 0; --i) {
        SteelBall *ball = esferasActivas[i];
        ball->setRalentizada(jojo->estaUltiActiva());
        qDebug() << "ulti activa:" << jojo->estaUltiActiva();
        ball->avanzarFisica();

        // Bola fuera de pantalla → eliminar
        if (ball->x() < -100 || ball->x() > 1300 ||
            ball->y() < -100 || ball->y() > 600) {
            escena->removeItem(ball);
            esferasActivas.removeAt(i);
            delete ball;
            continue;
        }

        // Bola toca a Jojo → daño + explosión visual
        if (escena->collidingItems(ball).contains(jojo)) {
            int danio = (ball->getTipo() == SteelBall::RojaEsquivable) ? 20 : 10;
            jojo->recibirDanio(danio);

            if (!framesExplosion.isEmpty()) {
                EfectoExplosion ef;
                ef.frames         = framesExplosion;
                ef.frameActual    = 0;
                ef.contadorFrames = 0;
                ef.item = new QGraphicsPixmapItem(ef.frames[0]);
                ef.item->setPos(jojo->x() + 35 - ef.frames[0].width()  / 2.0,
                                jojo->y() + 50 - ef.frames[0].height() / 2.0);
                ef.item->setZValue(10);
                escena->addItem(ef.item);
                explosionesActivas.append(ef);
            }

            escena->removeItem(ball);
            esferasActivas.removeAt(i);
            delete ball;
        }
    }

    // ── Ataque de Jojo contra bolas verdes ──
    if (atacando) {
        QRectF attackBox = jojo->getAttackHitbox();
        for (SteelBall* ball : esferasActivas) {
            if (ball->getTipo() == SteelBall::VerdeGolpeable && !ball->estaCayendo()) {
                QRectF ballBox = ball->getHitbox().translated(ball->pos());
                if (attackBox.intersects(ballBox)) {
                    qDebug() << "ORA! Bola golpeada";
                    ball->recibirGolpe();
                    jojo->cargarEnergia(25);
                }
            }
        }
    }

    // ── Animación de explosiones ──
    for (int i = explosionesActivas.size() - 1; i >= 0; --i) {
        EfectoExplosion &ef = explosionesActivas[i];
        ef.contadorFrames++;
        if (ef.contadorFrames >= 9) {
            ef.contadorFrames = 0;
            ef.frameActual++;
            if (ef.frameActual < ef.frames.size()) {
                ef.item->setPixmap(ef.frames[ef.frameActual]);
            } else {
                escena->removeItem(ef.item);
                delete ef.item;
                explosionesActivas.removeAt(i);
            }
        }
    }

    // ── Spawn y recogida de ítems ──
    contadorSpawnItems++;
    int intervaloSpawn = (dificultad == "facil") ? 600 : 900;
    if (contadorSpawnItems >= intervaloSpawn) {
        contadorSpawnItems = 0;
        qreal posX = (std::rand() % 500) + 50;
        qreal posY = (std::rand() % 300) + 100;
        ItemJuego::TipoItem tipo = (std::rand() % 2 == 0)
                                       ? ItemJuego::Vida
                                       : ItemJuego::Velocidad;
        ItemJuego *nuevoItem = new ItemJuego(tipo, posX, posY);
        nuevoItem->setZValue(1);
        escena->addItem(nuevoItem);
        itemsActivos.append(nuevoItem);
    }

    for (int i = itemsActivos.size() - 1; i >= 0; --i) {
        ItemJuego *it = itemsActivos[i];
        it->actualizarAnimacion();
        if (jojo->collidesWithItem(it)) {
            if (it->getTipo() == ItemJuego::Vida)
                jojo->curar(25);
            else if (it->getTipo() == ItemJuego::Velocidad)
                jojo->aumentarVelocidad();
            escena->removeItem(it);
            itemsActivos.removeAt(i);
            delete it;
        }
    }

    // ── HUD ──
    if (interfazHUD) interfazHUD->actualizarEstadosNivel1(jojo);

    // ── Condición de derrota: Jojo sin vida ──
    if (jojo->vida <= 0) {
        terminarNivel(false);
    }
}

// ==========================================
// RELOJ — 1 segundo
// ==========================================
void Nivel1::actualizarSegundo() {
    if (nivelFinalizado) return;

    if (tiempoRestante > 0) {
        tiempoRestante--;
        if (interfazHUD)
            interfazHUD->actualizarRelojYMarcador(tiempoRestante, 0, 0);
    } else {
        // Tiempo agotado → victoria
        terminarNivel(true);
    }
}

// ==========================================
// TERMINAR NIVEL (único punto de salida)
// ==========================================
void Nivel1::terminarNivel(bool victoria) {
    if (nivelFinalizado) return;   // evita doble disparo
    nivelFinalizado = true;

    timerUnSegundo->stop();
    gameLoopTimer->stop();

    if (interfazHUD) {
        if (victoria)
            interfazHUD->mostrarAnuncioCentral("¡SOBREVIVISTE!", QColor(255, 215, 0));
        else
            interfazHUD->mostrarAnuncioCentral("K.O.", Qt::red);
    }

    QTimer::singleShot(2000, this, [this, victoria]() {
        emit combateTerminado(victoria);
    });
}

// ==========================================
// CONDICIONES (requeridas por la clase base)
// ==========================================
bool Nivel1::verificarCondicionVictoria() { return nivelFinalizado && tiempoRestante == 0; }
bool Nivel1::verificarCondicionDerrota()  { return nivelFinalizado && (jojo && jojo->vida <= 0); }

// ==========================================
// LIMPIAR NIVEL
// ==========================================
void Nivel1::vaciarEntidadesEscena() {
    for (auto &ef : explosionesActivas) {
        if (ef.item) { escena->removeItem(ef.item); delete ef.item; }
    }
    explosionesActivas.clear();

    for (SteelBall* b : esferasActivas) { escena->removeItem(b); delete b; }
    esferasActivas.clear();

    for (ItemJuego* it : itemsActivos) { escena->removeItem(it); delete it; }
    itemsActivos.clear();
}

void Nivel1::limpiarNivel() {
    if (gameLoopTimer  && gameLoopTimer->isActive())  gameLoopTimer->stop();
    if (timerUnSegundo && timerUnSegundo->isActive()) timerUnSegundo->stop();

    vaciarEntidadesEscena();

    if (fondoMapa)   { escena->removeItem(fondoMapa);  delete fondoMapa;  fondoMapa  = nullptr; }
    if (gyroJefe)    { escena->removeItem(gyroJefe);   delete gyroJefe;   gyroJefe   = nullptr; }
    if (interfazHUD) { delete interfazHUD; interfazHUD = nullptr; }
}

// ==========================================
// EXPLOSIÓN DORADA
// ==========================================
void Nivel1::cargarFramesExplosion() {
    framesExplosion.clear();
    QPixmap sprites(":/sprites_juego.png");

    struct { int x, w; } data[] = {
        {1099, 53}, {1170, 81}, {1266, 102}, {1370, 126}
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
                    abs(px.blue()  - fondoColor.blue())  < 10)
                    img.setPixelColor(x, y, QColor(0, 0, 0, 0));
            }
        }
        framesExplosion.append(QPixmap::fromImage(img));
    }
}

// ==========================================
// INPUT
// ==========================================
void Nivel1::procesarPresionTeclada(int tecla) {
    if (nivelFinalizado || !jojo) return;

    teclasPresionadas.insert(tecla);

    if (tecla == Qt::Key_H) {
        mostrarHitbox = !mostrarHitbox;
        jojo->setMostrarHitbox(mostrarHitbox);
        for (SteelBall* b : esferasActivas) {
            b->setProperty("mostrarHitbox", mostrarHitbox);
            b->update();
        }
    }
    if (tecla == Qt::Key_L) jojo->usarUlti();
}

void Nivel1::procesarLiberacionTeclada(int tecla) {
    if (nivelFinalizado || !jojo) return;
    teclasPresionadas.remove(tecla);
}