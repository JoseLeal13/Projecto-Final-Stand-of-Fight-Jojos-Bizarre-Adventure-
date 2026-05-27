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

    // Inicializar estadísticas globales del torneo
    roundActual = 1;
    tiempoRestanteRound = 120; // Sincronizado a los 2 minutos originales de tu propuesta
    KOsJotaro = 0;
    KOsDio = 0;
    rondaEnTransicion = false;

    // Timer dedicado para el flujo natural del tiempo segundo a segundo
    timerUnSegundo = new QTimer(this);
}

Nivel2::~Nivel2() {
    limpiarNivel();
}

void Nivel2::iniciarNivel() {
    int anchoEscena = 1200;
    int altoEscena  = 500;

    // 1. Fondo del mapa de la Jaula MMA
    QPixmap fondo(":/sprites/SpritesJojoChampionship/JoestarChampionship Jaula MMA.png");
    fondoMapa = new QGraphicsPixmapItem(
        fondo.scaled(anchoEscena, altoEscena, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    fondoMapa->setZValue(-1);
    escena->addItem(fondoMapa);

    // 2. Construcción física del mapa (Suelo y Paredes)
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

    // 3. Instanciar dinámicamente a DIO
    dio = new DIO(jojo);
    escena->addItem(dio);

    // 4. Inicializar el HUD de Estadísticas completo
    interfazHUD = new StandUserStats(escena);

    // Colocar los personajes en sus esquinas de origen y configurar la ronda 1
    cargarPosicionesIniciales();

    // 5. Conectar y encender los Timers
    connect(gameLoopTimer, &QTimer::timeout, this, &Nivel2::actualizarLoop);
    connect(timerUnSegundo, &QTimer::timeout, this, &Nivel2::actualizarSegundo);

    gameLoopTimer->start(1000 / 60);
    timerUnSegundo->start(1000);
}

void Nivel2::cargarPosicionesIniciales() {
    rondaEnTransicion = true; // Bloquea el flujo normal de pelea durante la preparación

    int yPersonaje = 400 - (80 * 1.8);

    // ── REINICIO CON VARIABLES INTERNAS EXISTENTES DE JOTARO ──
    if (jojo) {
        jojo->setVelocidadX(0);
        jojo->setPos(150, yPersonaje);
        jojo->setVida(300);
        jojo->setBarraCarga(0);
        jojo->estadoDano = Jojo::NORMAL;
        jojo->estaAtacando = false;
        jojo->puedeAtacar = true;
        jojo->faseCombo = 0;
        jojo->setFrameActual(0); // Forzar frame estático base
    }

    // ── REINICIO CON VARIABLES INTERNAS EXISTENTES DE DIO ──
    if (dio) {
        dio->setVelocidadX(0);
        dio->setPos(950, yPersonaje);
        dio->setVida(300);
        dio->setBarraCarga(0);
        dio->estadoDano = DIO::NORMAL;
        dio->estaAtacando = false;
        dio->puedeAtacar = true;
        dio->faseCombo = 0;
        dio->ticksEspera = 0;
        dio->postEspecialJump = false;
    }

    tiempoRestanteRound = 120; // Restablecer el reloj a 2 minutos completos

    if (interfazHUD) {
        interfazHUD->actualizarRelojYMarcador(tiempoRestanteRound, KOsJotaro, KOsDio);
        interfazHUD->mostrarAnuncioCentral(QString("ROUND %1").arg(roundActual), QColor(255, 140, 0));
        interfazHUD->actualizarEstados(jojo, dio);
    }

    // Retardo de 2 segundos de lectura previa al combate
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
    // ── MODO PASIVO DE TRANSICIÓN (K.O. / ESPERA) ──
    if (rondaEnTransicion) {
        if (jojo) {
            jojo->setVelocidadX(0);
            // NO llamamos a jojo->moverse() para evitar inputs accidentales.
            // Solo dejamos correr sus ticks de animación para desvanecer ráfagas o ataques.
            jojo->actualizarAtaque();
            jojo->actualizarAtaquesFuertes();
            jojo->actualizarEspecial();
        }
        if (dio) {
            dio->setVelocidadX(0);
            // SOLUCIÓN CRÍTICA: NO llamamos a dio->moverse().
            // Al omitir moverse(), evitamos que ejecute internamente su 'ejecutarCerebro()',
            // apagando por completo su IA ofensiva mientras dure el cartel de K.O.
            dio->actualizarAtaque();
            dio->actualizarAtaquesFuertes();
            dio->actualizarEspecial();
        }
        if (interfazHUD && jojo && dio) {
            interfazHUD->actualizarEstados(jojo, dio);
        }
        return; // Detiene el flujo de físicas y colisiones destructivas
    }

    // ── CICLO DE COMBATE ACTIVO EN TIEMPO REAL ──
    if (jojo) {
        if (Personaje::tiempoDetenido) {
            jojo->setVelocidadX(0);
        }
        else {
            // Tope anti-hipervelocidad
            if (jojo->getVelocidadX() > 7)  jojo->setVelocidadX(9);
            if (jojo->getVelocidadX() < -7) jojo->setVelocidadX(-9);

            jojo->moverse();
            jojo->actualizarAtaque();
            jojo->actualizarAtaquesFuertes();
            jojo->actualizarEspecial();
        }

        if (dio) {
            dio->moverse(); // Aquí sí ejecuta su toma de decisiones lógicas de la IA
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
    rondaEnTransicion = true; // Activa el modo de congelamiento de IA de inmediato

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
    else if (ganador == "TIEMPO") {
        mensajeAnuncio = "TIME UP";
        colorAnuncio = Qt::yellow;
        if (jojo && dio) {
            if (jojo->getVida() >= dio->getVida()) {
                KOsJotaro++;
                nombreGanador = "KUJO JOTARO";
            } else {
                KOsDio++;
                nombreGanador = "DIO BRANDO";
            }
        }
    }

    if (interfazHUD) {
        interfazHUD->mostrarAnuncioCentral(mensajeAnuncio, colorAnuncio);
    }

    if (Personaje::tiempoDetenido) {
        Personaje::tiempoDetenido = false;
        aplicarEfectoZaWarudo(false);
    }

    // --- PRIMER PASO: 3 SEGUNDOS DE ESPERA TRAS EL K.O. ---
    // Ambos personajes limpian de forma pasiva sus estados gráficos remanentes sin atacarse.
    QTimer::singleShot(3000, this, [this, ganador, nombreGanador]() {

        // Forzado estricto de vuelta al estado base NORMAL
        if (jojo) {
            jojo->estadoDano = Jojo::NORMAL;
            jojo->estaAtacando = false;
            jojo->puedeAtacar = true;
            jojo->faseCombo = 0;
        }
        if (dio) {
            dio->estadoDano = DIO::NORMAL;
            dio->estaAtacando = false;
            dio->puedeAtacar = true;
            dio->faseCombo = 0;
        }

        if (interfazHUD) {
            interfazHUD->mostrarAnuncioCentral(QString("GANADOR: %1").arg(nombreGanador), Qt::yellow);
        }

        // --- SEGUNDO PASO: 1.5 SEGUNDOS EXTRAS EN REPOSO ---
        QTimer::singleShot(1500, this, [this]() {
            roundActual++;

            if (verificarCondicionVictoria() || verificarCondicionDerrota()) {
                timerUnSegundo->stop();
                gameLoopTimer->stop();

                if (interfazHUD) {
                    if (KOsJotaro >= 3) {
                        interfazHUD->mostrarAnuncioCentral("¡VICTORIA FINAL: JOTARO!", Qt::green);
                    } else {
                        interfazHUD->mostrarAnuncioCentral("¡VICTORIA FINAL: DIO!", Qt::red);
                    }
                }
            } else {
                // Siguiente asalto: Reposiciona los personajes completamente limpios
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
