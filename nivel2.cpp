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
    tiempoRestanteRound = 120; // 2 minutos en segundos
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
    rondaEnTransicion = true; // Congela la física durante la preparación

    // Valores de escala y salud iniciales
    int yPersonaje = 400 - (80 * 1.8);

    // ── REINICIO ABSOLUTO DE JOTARO ──
    if (jojo) {
        jojo->setPos(150, yPersonaje);
        jojo->setVida(300);
        jojo->setBarraCarga(0);
        jojo->setVelocidadX(0);
        jojo->estadoDano = Jojo::NORMAL;
        jojo->estaAtacando = false;
        jojo->puedeAtacar = true;
        jojo->faseCombo = 0;
    }

    // ── REINICIO ABSOLUTO DE DIO ──
    if (dio) {
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

    tiempoRestanteRound = 120; // Resetear reloj a 2 minutos

    // Forzar actualización inmediata del HUD gráfico
    if (interfazHUD) {
        interfazHUD->actualizarRelojYMarcador(tiempoRestanteRound, KOsJotaro, KOsDio);
        interfazHUD->mostrarAnuncioCentral(QString("ROUND %1").arg(roundActual), QColor(255, 140, 0));

        // Asegurar que las barras gráficas del HUD reflejen el 100% de vida y 0% de especial inmediatamente
        interfazHUD->actualizarEstados(jojo, dio);
    }

    // Retardo de 2 segundos para que el jugador lea el número de round antes de pelear
    QTimer::singleShot(2000, this, [this]() {
        if (interfazHUD) interfazHUD->ocultarAnuncioCentral();
        rondaEnTransicion = false; // Se reanudan las acciones físicas
    });
}

void Nivel2::actualizarSegundo() {
    // Si el tiempo del mundo está detenido por el Stand de DIO o la ronda acabó, el reloj físico no baja
    if (Personaje::tiempoDetenido || rondaEnTransicion) return;

    if (tiempoRestanteRound > 0) {
        tiempoRestanteRound--;
        if (interfazHUD) {
            interfazHUD->actualizarRelojYMarcador(tiempoRestanteRound, KOsJotaro, KOsDio);
        }
    } else {
        // Se acabó el tiempo del round (Llegó a 0)
        procesarFinRound("TIEMPO");
    }
}

void Nivel2::actualizarLoop() {
    // Si estamos mostrando un cartel de K.O o cargando posiciones, saltamos la física de este frame
    if (rondaEnTransicion) return;

    if (jojo) {
        if (Personaje::tiempoDetenido) {
            jojo->setVelocidadX(0);
        }
        else {
            // Tope anti-hipervelocidad (con el 40% extra calculado = 9)
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

    // Verificar si en este tick ocurrió un K.O. por vaciado de vida
    if (jojo && jojo->getVida() <= 0) {
        procesarFinRound("DIO");
    }
    else if (dio && dio->getVida() <= 0) {
        procesarFinRound("JOTARO");
    }
}

void Nivel2::procesarFinRound(const QString& ganador) {
    rondaEnTransicion = true; // Pausa el juego de inmediato

    if (interfazHUD) {
        interfazHUD->mostrarAnuncioCentral("K.O.", Qt::red);
    }

    QString nombreGanador = "";

    // Asignar el punto de K.O. según corresponda
    if (ganador == "JOTARO") {
        KOsJotaro++;
        nombreGanador = "KUJO JOTARO";
    }
    else if (ganador == "DIO") {
        KOsDio++;
        nombreGanador = "DIO BRANDO";
    }
    else if (ganador == "TIEMPO") {
        // En caso de empate por tiempo, gana quien tenga más vida en ese instante
        if (jojo->getVida() >= dio->getVida()) {
            KOsJotaro++;
            nombreGanador = "KUJO JOTARO";
        } else {
            KOsDio++;
            nombreGanador = "DIO BRANDO";
        }
    }

    // Desactivar efectos residuales de Za Warudo por si acaso
    if (Personaje::tiempoDetenido) {
        Personaje::tiempoDetenido = false;
        aplicarEfectoZaWarudo(false);
    }

    // Esperar 2 segundos mostrando "K.O." y luego anunciar al ganador del asalto
    QTimer::singleShot(2000, this, [this, nombreGanador]() {
        if (interfazHUD) {
            interfazHUD->mostrarAnuncioCentral(QString("GANADOR: %1").arg(nombreGanador), Qt::yellow);
        }

        // Esperar otros 2 segundos antes de evaluar si el juego termina o pasa al siguiente asalto
        QTimer::singleShot(2500, this, [this]() {
            roundActual++;

            // Verificar condiciones absolutas de fin de partida (3 K.Os o fin del asalto 10)
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
                // El combate continúa: reacomodar el mapa y los combatientes
                cargarPosicionesIniciales();
            }
        });
    });
}

bool Nivel2::verificarCondicionVictoria() {
    // Gana el jugador si completa 3 K.Os primero
    return (KOsJotaro >= 3);
}

bool Nivel2::verificarCondicionDerrota() {
    // Pierde si DIO hace 3 K.Os o si se llega al límite del Round 10 (Donde DIO gana por normativa)
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
