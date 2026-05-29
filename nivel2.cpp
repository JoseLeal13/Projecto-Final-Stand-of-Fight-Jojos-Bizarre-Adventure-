#include "nivel2.h"
#include <QPixmap>
#include <QImage>

Nivel2::Nivel2(QGraphicsScene* escenaCompartida, Jojo* personajeJojo, QObject* parent)
    : Nivel(escenaCompartida, personajeJojo, parent)
{
    dio = nullptr;
    dummy = nullptr;
    interfazHUD = nullptr;
    fondoMapa = nullptr;
    efectoJotaro = nullptr;
    efectoGrisActivo = false;

    roundActual = 1;
    tiempoRestanteRound = 90; // AJUSTADO: 1 minuto y 30 segundos exactos (90s)
    KOsJotaro = 0;
    KOsDio = 0;
    rondaEnTransicion = false;

    timerUnSegundo = new QTimer(this);
}

Nivel2::~Nivel2() {
    limpiarNivel();
}

void Nivel2::iniciarNivel() {
    int anchoEscena = 1200;
    int altoEscena  = 500;

    QPixmap fondo(":/sprites/SpritesJojoChampionship/JoestarChampionship Jaula MMA.png");
    fondoMapa = new QGraphicsPixmapItem(
        fondo.scaled(anchoEscena, altoEscena, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    fondoMapa->setZValue(-1);
    escena->addItem(fondoMapa);

    int ySuperficieSuelo = 400;
    int grosorSuelo = 20;
    obstaculo *suelo = new obstaculo(0, ySuperficieSuelo, anchoEscena, grosorSuelo);
    obstaculo *paredIzquierda = new obstaculo(110 - 40, 0, 40, altoEscena);
    obstaculo *paredDerecha   = new obstaculo(anchoEscena - 110, 0, 40, altoEscena);
    escena->addItem(suelo);
    escena->addItem(paredIzquierda);
    escena->addItem(paredDerecha);

    if (jojo) {
        if (!escena->items().contains(jojo)) {
            escena->addItem(jojo);
        }
    }

    dio = new DIO(jojo);
    escena->addItem(dio);

    interfazHUD = new StandUserStats(escena);

    cargarPosicionesIniciales();

    connect(gameLoopTimer, &QTimer::timeout, this, &Nivel2::actualizarLoop);
    connect(timerUnSegundo, &QTimer::timeout, this, &Nivel2::actualizarSegundo);

    gameLoopTimer->start(1000 / 60);
    timerUnSegundo->start(1000);
}

void Nivel2::cargarPosicionesIniciales() {
    rondaEnTransicion = true;

    int yPersonaje = 400 - (80 * 1.8);

    // ── REINICIO JOTARO (Vida Máxima 500) ──
    if (jojo) {
        jojo->setVelocidadX(0);
        jojo->setVelocidadY(0);
        jojo->setPos(150, yPersonaje);
        jojo->setVida(500); // Vida a 500
        jojo->setBarraCarga(0);
        jojo->estadoDano = Jojo::NORMAL;
        jojo->estaAtacando = false;
        jojo->puedeAtacar = true;
        jojo->faseCombo = 0;
        jojo->setFrameActual(0);
    }

    // ── REINICIO DIO (Vida Máxima 500) ──
    if (dio) {
        dio->setVelocidadX(0);
        dio->setVelocidadY(0); // CORRECCIÓN: Resetear inercia de caída inmediatamente
        dio->setPos(950, yPersonaje);
        dio->setVida(500); // Vida a 500
        dio->setBarraCarga(0);
        dio->estadoDano = DIO::NORMAL;
        dio->estaAtacando = false;
        dio->puedeAtacar = true;
        dio->faseCombo = 0;
        dio->ticksEspera = 0;
        dio->postEspecialJump = false;
    }

    tiempoRestanteRound = 90; // AJUSTADO: Recomenzar con 1:30 minutos

    if (interfazHUD) {
        interfazHUD->actualizarRelojYMarcador(tiempoRestanteRound, KOsJotaro, KOsDio);
        interfazHUD->mostrarAnuncioCentral(QString("ROUND %1").arg(roundActual), QColor(255, 140, 0));
        interfazHUD->actualizarEstados(jojo, dio);
    }

    QTimer::singleShot(2000, this, [this]() {
        if (interfazHUD) interfazHUD->ocultarAnuncioCentral();
        rondaEnTransicion = false;
    });
}

void Nivel2::actualizarSegundo() {
    if (Personaje::tiempoDetenido || rondaEnTransicion) return;

    if (tiempoRestanteRound > 0) {
        tiempoRestanteRound--;
        if (interfazHUD) {
            interfazHUD->actualizarRelojYMarcador(tiempoRestanteRound, KOsJotaro, KOsDio);
        }
    } else {
        procesarFinRound("TIEMPO");
    }
}

void Nivel2::actualizarLoop() {
    // ── MODO PASIVO DE TRANSICIÓN (K.O. / ESPERA DE ASALTO) ──
    if (rondaEnTransicion) {
        if (jojo) {
            jojo->setVelocidadX(0); // Forzar quietud horizontal
            jojo->moverse();        // <--- Dejar que se ejecute para que aplique SU propia gravedad y colisión de suelo
            jojo->actualizarAtaque();
            jojo->actualizarAtaquesFuertes();
            jojo->actualizarEspecial();
        }
        if (dio) {
            dio->setVelocidadX(0); // Forzar quietud horizontal
            dio->moverse();        // <--- Dejar que procese su gravedad nativa sin activar la IA ofensiva
            dio->actualizarAtaque();
            dio->actualizarAtaquesFuertes();
            dio->actualizarEspecial();
        }
        if (interfazHUD && jojo && dio) {
            interfazHUD->actualizarEstados(jojo, dio);
        }
        return; // Bloquea el resto del ciclo de daño/combate activo
    }

    // ── CICLO DE COMBATE ACTIVO EN TIEMPO REAL ──
    if (jojo) {
        if (Personaje::tiempoDetenido) {
            jojo->setVelocidadX(0);
        }
        else {
            if (jojo->getVelocidadX() > 7)  jojo->setVelocidadX(9);
            if (jojo->getVelocidadX() < -7) jojo->setVelocidadX(-9);

            jojo->moverse();
            jojo->actualizarAtaque();
            jojo->actualizarAtaquesFuertes();
            jojo->actualizarEspecial();
        }

        if (dio) {
            dio->moverse();
        }

        // CONTROL VISUAL DE ZA WARUDO
        if (Personaje::tiempoDetenido) {
            if (!efectoGrisActivo) {
                efectoGrisActivo = true;
                aplicarEfectoZaWarudo(true);
            }
        } else {
            if (efectoGrisActivo) {
                efectoGrisActivo = false;
                aplicarEfectoZaWarudo(false);
            }
        }
    }

    if (interfazHUD && jojo && dio) {
        interfazHUD->actualizarEstados(jojo, dio);
    }

    // Verificar condiciones de K.O.
    if (jojo && jojo->getVida() <= 0) {
        procesarFinRound("DIO");
    }
    else if (dio && dio->getVida() <= 0) {
        procesarFinRound("JOTARO");
    }
}

void Nivel2::procesarFinRound(const QString& ganador) {
    rondaEnTransicion = true;

    QString mensajeAnuncio = "K.O.";
    QColor colorAnuncio = Qt::red;
    QString nombreGanador = "";

    if (ganador == "JOTARO") {
        KOsJotaro++;
        nombreGanador = "KUJO JOTARO";
        if (dio) dio->setVida(0);
    }
    else if (ganador == "DIO") {
        KOsDio++;
        nombreGanador = "DIO BRANDO";
        if (jojo) jojo->setVida(0);
    }
    // Empate por tiempo: no suma K.Os a nadie
    else if (ganador == "TIEMPO") {
        mensajeAnuncio = "TIME UP";
        colorAnuncio = Qt::yellow;
        nombreGanador = "EMPATE (SIN K.O.)";
    }

    if (interfazHUD) {
        interfazHUD->mostrarAnuncioCentral(mensajeAnuncio, colorAnuncio);
    }

    if (Personaje::tiempoDetenido) {
        Personaje::tiempoDetenido = false;
        aplicarEfectoZaWarudo(false);
    }

    // --- 3 SEGUNDOS DE ESPERA TRAS EL FIN DEL ROUND ---
    QTimer::singleShot(3000, this, [this, ganador, nombreGanador]() {

        // CORRECCIÓN: Usamos única y estrictamente los setters públicos de tus clases
        if (jojo) {
            jojo->estadoDano = Jojo::NORMAL;
            jojo->estaAtacando = false;
            jojo->puedeAtacar = true;
            jojo->faseCombo = 0;
            jojo->setVelocidadX(0);
        }
        if (dio) {
            dio->estadoDano = DIO::NORMAL;
            dio->estaAtacando = false;
            dio->puedeAtacar = true;
            dio->faseCombo = 0;
            dio->setVelocidadX(0);
        }

        if (interfazHUD) {
            if (ganador == "TIEMPO") {
                interfazHUD->mostrarAnuncioCentral(nombreGanador, Qt::white);
            } else {
                interfazHUD->mostrarAnuncioCentral(QString("GANADOR: %1").arg(nombreGanador), Qt::yellow);
            }
        }

        // --- SEGUNDO PASO: REPOSO DE SEGURIDAD Y CAMBIO DE ROUND ---
        QTimer::singleShot(1500, this, [this]() {
            roundActual++;

            if (verificarCondicionVictoria() || verificarCondicionDerrota()) {
                timerUnSegundo->stop();
                gameLoopTimer->stop();

                if (interfazHUD) {
                    if (KOsJotaro >= 3) {
                        interfazHUD->mostrarAnuncioCentral("¡VICTORIA FINAL: JOTARO!", Qt::green);
                        // Esperamos un momento para que el jugador vea el cartel y emitimos victoria
                        QTimer::singleShot(2000, this, [this]() { emit combateTerminado(true); });
                    } else if (KOsDio >= 3) {
                        interfazHUD->mostrarAnuncioCentral("¡VICTORIA FINAL: DIO!", Qt::red);
                        QTimer::singleShot(2000, this, [this]() { emit combateTerminado(false); });
                    } else {
                        interfazHUD->mostrarAnuncioCentral("¡FIN DEL TORNEO: EMPATE!", Qt::white);
                        QTimer::singleShot(2000, this, [this]() { emit combateTerminado(false); });
                    }
                }
            } else {
                cargarPosicionesIniciales();
            }
        });
    });
}

bool Nivel2::verificarCondicionVictoria() {
    return (KOsJotaro >= 3);
}

bool Nivel2::verificarCondicionDerrota() {
    return (KOsDio >= 3 || roundActual > 10);
}

void Nivel2::aplicarEfectoZaWarudo(bool activar) {
    if (!fondoMapa) return;
    if (activar) {
        QPixmap fondoActual = fondoMapa->pixmap();
        QImage img = fondoActual.toImage();
        img.invertPixels(QImage::InvertRgb);
        fondoMapa->setPixmap(QPixmap::fromImage(img));
        if (jojo) {
            efectoJotaro = new QGraphicsColorizeEffect(this);
            efectoJotaro->setColor(QColor(70, 80, 95));
            efectoJotaro->setStrength(0.85);
            jojo->setGraphicsEffect(efectoJotaro);
        }
    } else {
        QPixmap fondoOriginal(":/sprites/SpritesJojoChampionship/JoestarChampionship Jaula MMA.png");
        fondoMapa->setPixmap(fondoOriginal.scaled(1200, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        if (jojo && jojo->graphicsEffect()) {
            jojo->setGraphicsEffect(nullptr);
            efectoJotaro = nullptr;
        }
    }
}

void Nivel2::limpiarNivel() {
    if (gameLoopTimer->isActive()) gameLoopTimer->stop();
    if (timerUnSegundo->isActive()) timerUnSegundo->stop();

    if (fondoMapa) { escena->removeItem(fondoMapa); delete fondoMapa; fondoMapa = nullptr; }
    if (dio) { escena->removeItem(dio); delete dio; dio = nullptr; }
    if (dummy) { escena->removeItem(dummy); delete dummy; dummy = nullptr; }
    if (interfazHUD) { delete interfazHUD; interfazHUD = nullptr; }
}
