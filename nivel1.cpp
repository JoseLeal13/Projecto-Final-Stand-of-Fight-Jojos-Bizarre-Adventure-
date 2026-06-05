/*
#include "nivel1.h"
#include <QPixmap>
#include <QImage>
#include <cstdlib>
#include <ctime>
#include <QDebug>

Nivel1::Nivel1(QGraphicsScene* escenaCompartida, Jojo* personajeJojo, const QString& dificultad, QObject* parent)
    : Nivel(escenaCompartida, personajeJojo, parent)
{
    gyroJefe = nullptr;
    interfazHUD = nullptr;
    fondoMapa = nullptr;

    // Guardamos la dificultad en una variable de clase para Gyro
    this->dificultadNivel = dificultad;

    roundActual = 1;
    tiempoRestanteRound = 60; // 1 minuto exacto por ronda
    KOsJotaro = 0;
    KOsGyro = 0;
    rondaEnTransicion = false;
    finRoundProcesado = false;

    timerUnSegundo = new QTimer(this);
    cargarFramesExplosion();
}

Nivel1::~Nivel1() {
    limpiarNivel();
}

void Nivel1::iniciarNivel() {
    // Escena igual de grande que Nivel 2: 1200 x 500
    int anchoEscena = 1200;
    int altoEscena  = 500;

    // Carga de fondo escalado a la jaula/arena correspondiente al Nivel 1
    QPixmap fondo(":/fondo_juego.png");
    fondoMapa = new QGraphicsPixmapItem(
        fondo.scaled(anchoEscena, altoEscena, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    fondoMapa->setZValue(-1);
    escena->addItem(fondoMapa);

    // Añadir al protagonista persistente si no está en la escena
    if (jojo) {
        if (!escena->items().contains(jojo)) {
            escena->addItem(jojo);
        }
    }

    // Inicializar la inteligencia artificial autónoma de Gyro
    gyroJefe = new Gyro();
    escena->addItem(gyroJefe);

    // Reutilizamos el HUD gráfico para pintar el cronómetro en la esquina superior derecha
    interfazHUD = new StandUserStats(escena);

    cargarPosicionesIniciales();

    // Conexiones de los loops de actualización
    connect(gameLoopTimer, &QTimer::timeout, this, &Nivel1::actualizarLoop);
    connect(timerUnSegundo, &QTimer::timeout, this, &Nivel1::actualizarSegundo);

    gameLoopTimer->start(1000 / 60);
    timerUnSegundo->start(1000);
}

void Nivel1::cargarPosicionesIniciales() {
    rondaEnTransicion = true;
    finRoundProcesado = false;

    // Limpiar proyectiles residuales del round anterior
    vaciarEntidadesEscena();

    // Colocar a Jotaro en su punto de origen (Adaptado a dimensiones 1200x500)
    if (jojo) {
        jojo->setVelocidadX(0);
        jojo->setVelocidadY(0);
        jojo->setPos(200, 250);
        jojo->setVida(500); // Restaurar salud al máximo al inicio de ronda
        jojo->setBarraCarga(0);
        jojo->setFrameActual(0);
        jojo->setEnMovimiento(false);
        jojo->setAtacando(false);
    }

    // Reposicionar a Gyro en el extremo opuesto del escenario
    if (gyroJefe) {
        gyroJefe->setPos(1000, 200);
    }

    tiempoRestanteRound = 60; // 1 minuto exacto por round

    if (interfazHUD) {
        // Enviar parámetros al HUD común (Rondas ganadas de Jotaro actúan como KOs)
        interfazHUD->actualizarRelojYMarcador(tiempoRestanteRound, KOsJotaro, KOsGyro);
        interfazHUD->mostrarAnuncioCentral(QString("ROUND %1").arg(roundActual), QColor(0, 191, 255));

        // El HUD simula el segundo usuario usando a Gyro como entidad ficticia para no romper la firma
        if (jojo && gyroJefe) {
            interfazHUD->actualizarEstados(jojo, jojo);
        }
    }

    QTimer::singleShot(2000, this, [this]() {
        if (interfazHUD) interfazHUD->ocultarAnuncioCentral();
        rondaEnTransicion = false;
    });
}

void Nivel1::actualizarSegundo() {
    if (rondaEnTransicion) return;

    if (tiempoRestanteRound > 0) {
        tiempoRestanteRound--;
        if (interfazHUD) {
            interfazHUD->actualizarRelojYMarcador(tiempoRestanteRound, KOsJotaro, KOsGyro);
        }
    } else {
        // Al terminarse los 60 segundos el jugador gana la ronda por supervivencia
        processarFinRound("JOTARO");
    }
}

void Nivel1::actualizarLoop() {
    if (!jojo || !gyroJefe) return;

    // Congelar dinámicas si la ronda está cambiando o en anuncio central
    if (rondaEnTransicion) {
        jojo->setVelocidadX(0);
        return;
    }

    // ── 1. ACTUALIZAR EFECTOS E INMUNIDADES DE JOTARO ──
    jojo->actualizarInvulnerabilidad();
    jojo->actualizarEfectosItems();

    // ── 2. ACTUALIZAR ANIMACIONES DE EXPLOSIÓN DORADA ──
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

    // Verificar caída súbita de salud del jugador (Derrota instantánea de ronda)
    if (jojo->getVida() <= 0) {
        processarFinRound("GYRO");
        return;
    }

    // ── 3. ACTUALIZAR LAS FÍSICAS DE LAS BOLAS DE ACERO ──
    bool camaraLenta = jojo->estaUltiActiva();

    for (int i = 0; i < esferasActivas.size(); ++i) {
        SteelBall *ball = esferasActivas[i];

        if (!camaraLenta || (rand() % 3 == 0)) {
            ball->avanzarFisica();
        }

        // Se eliminan si salen del rango de la nueva escena (1200 de ancho)
        if (ball->x() < -100 || ball->x() > 1300 || ball->y() < -100 || ball->y() > 600) {
            escena->removeItem(ball);
            esferasActivas.removeAt(i);
            delete ball;
            --i;
            continue;
        }

        if (escena->collidingItems(ball).contains(jojo)) {
            if (ball->getTipo() == SteelBall::RojaEsquivable) {
                jojo->recibirDanio(20);
            } else {
                jojo->recibirDanio(10);
            }

            if (!framesExplosion.isEmpty()) {
                EfectoExplosion ef;
                ef.frames = framesExplosion;
                ef.frameActual = 0;
                ef.contadorFrames = 0;
                ef.item = new QGraphicsPixmapItem(ef.frames[0]);
                ef.item->setPos(jojo->x() + 35 - ef.frames[0].width() / 2.0,
                                jojo->y() + 50 - ef.frames[0].height() / 2.0);
                ef.item->setZValue(10);
                escena->addItem(ef.item);
                explosionesActivas.append(ef);
            }

            escena->removeItem(ball);
            esferasActivas.removeAt(i);
            delete ball;
            --i;
        }
    }

    // ── 4. DETECTAR CONTRAATAQUES ORA A LAS BOLAS VERDES ──
    if (jojo->estaAtacando) {
        for (int i = 0; i < esferasActivas.size(); ++i) {
            SteelBall *ball = esferasActivas[i];
            if (ball->getTipo() == SteelBall::VerdeGolpeable && !ball->estaCayendo()) {
                QRectF attackGlobal = jojo->getAttackHitbox();
                QRectF hitboxBolaGlobal = ball->getHitbox().translated(ball->pos());

                if (attackGlobal.intersects(hitboxBolaGlobal)) {
                    ball->recibirGolpe();
                    jojo->cargarEnergia(25); // Incrementa la barra de habilidad definitiva
                }
            }
        }
    }

    // ── 5. RUTINA COMPORTAMIENTO GYRO INTELIGENTE ──
    gyroJefe->actualizarComportamiento(tiempoRestanteRound, jojo->y());

    QList<SteelBall*> nuevasBolas = gyroJefe->tomarNuevasBolas();
    for (SteelBall* b : nuevasBolas) {
        escena->addItem(b);
        esferasActivas.append(b);
    }

    // ── 6. SPAWN ALEATORIO DE ITEMS DE AYUDA ──
    if (rand() % 300 == 7) {
        ItemJuego::TipoItem tipoRnd = (rand() % 2 == 0) ? ItemJuego::Vida : ItemJuego::Velocidad;
        qreal posX = (rand() % 800) + 200;
        qreal posY = (rand() % 250) + 150;

        ItemJuego *nuevoItem = new ItemJuego(tipoRnd, posX, posY);
        nuevoItem->setZValue(1);
        escena->addItem(nuevoItem);
        itemsActivos.append(nuevoItem);
    }

    // Colisión absorber items
    for (int i = itemsActivos.size() - 1; i >= 0; --i) {
        ItemJuego *it = itemsActivos[i];
        if (jojo->collidesWithItem(it)) {
            if (it->getTipo() == ItemJuego::Vida) {
                jojo->curar(25);
            } else if (it->getTipo() == ItemJuego::Velocidad) {
                jojo->aumentarVelocidad();
            }
            escena->removeItem(it);
            itemsActivos.removeAt(i);
            delete it;
        }
    }

    // Mantener la sincronización visual del HUD de barras
    if (interfazHUD) {
        interfazHUD->actualizarEstados(jojo, jojo);
    }
}

void Nivel1::processarFinRound(const QString& ganador) {
    if (finRoundProcesado) return;
    finRoundProcesado = true;
    rondaEnTransicion = true;

    QString mensajeAnuncio = "K.O.";
    QColor colorAnuncio = Qt::red;

    if (ganador == "JOTARO") {
        KOsJotaro++; // El jugador sobrevivió con éxito el minuto completo
        mensajeAnuncio = "ROUND CLEAR";
        colorAnuncio = Qt::green;
    } else if (ganador == "GYRO") {
        KOsGyro++; // La vida del jugador bajó a cero antes de tiempo
        mensajeAnuncio = "FAILED";
    }

    if (interfazHUD) {
        interfazHUD->mostrarAnuncioCentral(mensajeAnuncio, colorAnuncio);
    }

    // Pausa dramática de 3 segundos
    QTimer::singleShot(3000, this, [this, ganador]() {
        roundActual++;

        if (verificarCondicionVictoria() || verificarCondicionDerrota()) {
            timerUnSegundo->stop();
            gameLoopTimer->stop();

            if (interfazHUD) {
                if (KOsJotaro >= 3 || (roundActual > 3 && KOsJotaro > KOsGyro)) {
                    interfazHUD->mostrarAnuncioCentral("VICTORIA: PRUEBA SUPERADA", Qt::green);
                    // CORRECCIÓN: Emitir 'nivelTerminado' en vez de 'combateTerminado'
                    QTimer::singleShot(2000, this, [this]() { emit nivelTerminado(true); });
                } else {
                    interfazHUD->mostrarAnuncioCentral("DERROTA: FALLASTE LA PRUEBA", Qt::red);
                    // CORRECCIÓN: Emitir 'nivelTerminado' en vez de 'combateTerminado'
                    QTimer::singleShot(2000, this, [this]() { emit nivelTerminado(false); });
                }
            }
        } else {
            // Avanzar a la siguiente ronda restaurando los estados originales
            cargarPosicionesIniciales();
        }
    });
}

void Nivel1::procesarPresionTeclada(int tecla) {
    if (!jojo || rondaEnTransicion) return;

    jojo->setEnMovimiento(true);

    if (tecla == Qt::Key_S || tecla == Qt::Key_Down) {
        jojo->moveDown();
        jojo->setDireccion(0);
    }
    else if (tecla == Qt::Key_W || tecla == Qt::Key_Up) {
        jojo->moveUp();
        jojo->setDireccion(1);
    }
    else if (tecla == Qt::Key_A || tecla == Qt::Key_Left) {
        jojo->moveLeft();
        jojo->setDireccion(2);
    }
    else if (tecla == Qt::Key_D || tecla == Qt::Key_Right) {
        jojo->moveRight();
        jojo->setDireccion(3);
    }
    else if (tecla == Qt::Key_J) {
        jojo->estaAtacando = true;
    }
    else if (tecla == Qt::Key_L) {
        jojo->usarUlti();
    }
}

void Nivel1::procesarLiberacionTeclada(int tecla) {
    if (!jojo) return;

    if (tecla == Qt::Key_J) {
        jojo->estaAtacando = false;
    }
    jojo->setEnMovimiento(false);
}

bool Nivel1::verificarCondicionVictoria() {
    return (KOsJotaro >= 3);
}

bool Nivel1::verificarCondicionDerrota() {
    return (KOsGyro >= 1 || roundActual > 3);
}

void Nivel1::vaciarEntidadesEscena() {
    for (SteelBall* b : esferasActivas) { escena->removeItem(b); delete b; }
    esferasActivas.clear();

    for (ItemJuego* it : itemsActivos) { escena->removeItem(it); delete it; }
    itemsActivos.clear();

    for (auto &ef :  explosionesActivas) { escena->removeItem(ef.item); delete ef.item; }
    explosionesActivas.clear();
}

void Nivel1::limpiarNivel() {
    if (gameLoopTimer->isActive()) gameLoopTimer->stop();
    if (timerUnSegundo->isActive()) timerUnSegundo->stop();

    vaciarEntidadesEscena();

    if (fondoMapa) { escena->removeItem(fondoMapa); delete fondoMapa; fondoMapa = nullptr; }
    if (gyroJefe) { escena->removeItem(gyroJefe); delete gyroJefe; gyroJefe = nullptr; }
    if (interfazHUD) { delete interfazHUD; interfazHUD = nullptr; }
}

void Nivel1::cargarFramesExplosion() {
    QPixmap sprites(":/sprites_juego.png");
    struct { int x, w; } data[] = { {1099, 53}, {1170, 81}, {1266, 102}, {1370, 126} };
    QColor fondoColor(30, 27, 60);

    for (auto &d : data) {
        QPixmap frame = sprites.copy(d.x, 811, d.w, 88);
        QImage img = frame.toImage().convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; y < img.height(); y++) {
            for (int x = 0; x < img.width(); x++) {
                QColor px = img.pixelColor(x, y);
                if (abs(px.red() - fondoColor.red()) < 10 &&
                    abs(px.green() - fondoColor.green()) < 10 &&
                    abs(px.blue() - fondoColor.blue()) < 10) {
                    img.setPixelColor(x, y, QColor(0,0,0,0));
                }
            }
        }
        framesExplosion.append(QPixmap::fromImage(img));
    }
}
*/
