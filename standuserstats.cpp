#include "standuserstats.h"
#include "jojo.h"   // Necesario para actualizarEstadosNivel1()

// ═══════════════════════════════════════════════════════════════════════════
//  CONSTRUCTOR UNIFICADO
//  Según el modo delega a construirHUDSurvival() o construirHUDCombate()
// ═══════════════════════════════════════════════════════════════════════════
StandUserStats::StandUserStats(QGraphicsScene* scene, Modo modo)
    : modoActual(modo), escenaAsignada(scene)
{
    // Los tres elementos de texto se crean siempre, existen en ambos modos.
    QFont fuenteMarcador("Impact", 14);
    QFont fuenteReloj   ("Impact", 26, QFont::Bold);
    QFont fuenteAnuncio ("Impact", 48);

    textoTemporizador = new QGraphicsTextItem();
    textoTemporizador->setDefaultTextColor(QColor(255, 215, 0));   // dorado
    textoTemporizador->setFont(fuenteReloj);
    textoTemporizador->setZValue(15);
    textoTemporizador->setPos(1060, 20);
    scene->addItem(textoTemporizador);

    textoMarcadorKOs = new QGraphicsTextItem();
    textoMarcadorKOs->setFont(fuenteMarcador);
    textoMarcadorKOs->setDefaultTextColor(Qt::white);
    textoMarcadorKOs->setZValue(15);
    scene->addItem(textoMarcadorKOs);

    textoAnuncioCentral = new QGraphicsTextItem();
    textoAnuncioCentral->setFont(fuenteAnuncio);
    textoAnuncioCentral->setZValue(20);
    scene->addItem(textoAnuncioCentral);

    // Delegar la construcción de barras y etiquetas al modo correcto
    if (modoActual == SURVIVAL) {
        construirHUDSurvival(scene);
    } else {
        construirHUDCombate(scene);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  HUD SURVIVAL — Solo Jotaro del Nivel1
//  Vida base 100, energía de ulti, reloj y marcador de KOs.
//  NO crea ningún elemento de DIO.
//
//  Layout:
//    ┌──────────────────────────────────────────────────────────────┐
//    │  [Reloj dorado - arriba derecha]                             │
//    │  [K.Os - centro]                                             │
//    │                                                              │
//    │  KUJO JOTARO          [Anuncio central]                      │
//    │  [████ Vida ████]                                            │
//    │  [██ Energía/Ulti]                                           │
//    └──────────────────────────────────────────────────────────────┘
// ═══════════════════════════════════════════════════════════════════════════
void StandUserStats::construirHUDSurvival(QGraphicsScene* scene)
{
    QFont fuenteEtiqueta("Impact", 13);

    // ── Posición: esquina inferior izquierda ──────────────────────────────
    const int X    = 30;
    const int Y    = 430;   // fila de la barra de vida
    const int Y2   = Y + 26; // fila de la barra de energía

    // ── Fondo de vida ─────────────────────────────────────────────────────
    jojoFondoVida = new QGraphicsRectItem(X, Y, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    jojoFondoVida->setBrush(QBrush(QColor(40, 40, 40)));
    jojoFondoVida->setPen(QPen(Qt::white, 1.5));
    jojoFondoVida->setZValue(10);
    scene->addItem(jojoFondoVida);

    // ── Barra de vida (verde → rojo según porcentaje, actualizada cada tick) ──
    jojoBarraVida = new QGraphicsRectItem(X, Y, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    jojoBarraVida->setBrush(QBrush(QColor(0, 220, 50)));
    jojoBarraVida->setPen(Qt::NoPen);
    jojoBarraVida->setZValue(11);
    scene->addItem(jojoBarraVida);

    // ── Fondo de energía / ulti ───────────────────────────────────────────
    jojoFondoCarga = new QGraphicsRectItem(X, Y2, ANCHO_BARRA_CARGA, ALTO_BARRA_CARGA);
    jojoFondoCarga->setBrush(QBrush(QColor(40, 40, 40)));
    jojoFondoCarga->setPen(QPen(Qt::white, 1));
    jojoFondoCarga->setZValue(10);
    scene->addItem(jojoFondoCarga);

    // ── Barra de energía (azul, morado al 100%) ───────────────────────────
    jojoBarraCarga = new QGraphicsRectItem(X, Y2, 0, ALTO_BARRA_CARGA); // empieza vacía
    jojoBarraCarga->setBrush(QBrush(QColor(0, 191, 255)));
    jojoBarraCarga->setPen(Qt::NoPen);
    jojoBarraCarga->setZValue(11);
    scene->addItem(jojoBarraCarga);

    // ── Etiqueta del jugador ──────────────────────────────────────────────
    jojoTexto = new QGraphicsTextItem("KUJO JOTARO");
    jojoTexto->setFont(fuenteEtiqueta);
    jojoTexto->setDefaultTextColor(Qt::white);
    jojoTexto->setPos(X, Y - 24);
    jojoTexto->setZValue(10);
    scene->addItem(jojoTexto);

    // Los punteros de DIO se dejan en nullptr (no se crean)
    dioFondoVida  = nullptr;
    dioBarraVida  = nullptr;
    dioFondoCarga = nullptr;
    dioBarraCarga = nullptr;
    dioTexto      = nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
//  HUD COMBATE — Nivel2 (Jotaro vs DIO), comportamiento original intacto
// ═══════════════════════════════════════════════════════════════════════════
void StandUserStats::construirHUDCombate(QGraphicsScene* scene)
{
    QFont fuenteMarcador("Impact", 14);

    // ── JOTARO (esquina inferior izquierda) ───────────────────────────────
    const int jojoX = 30;
    const int jojoY = 420;

    jojoFondoVida = new QGraphicsRectItem(jojoX, jojoY, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    jojoFondoVida->setBrush(QBrush(QColor(40, 40, 40)));
    jojoFondoVida->setPen(QPen(Qt::white, 1.5));
    jojoFondoVida->setZValue(10);
    scene->addItem(jojoFondoVida);

    jojoBarraVida = new QGraphicsRectItem(jojoX, jojoY, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    jojoBarraVida->setBrush(QBrush(QColor(0, 220, 50)));
    jojoBarraVida->setPen(Qt::NoPen);
    jojoBarraVida->setZValue(11);
    scene->addItem(jojoBarraVida);

    jojoFondoCarga = new QGraphicsRectItem(jojoX, jojoY + 25, ANCHO_BARRA_CARGA, ALTO_BARRA_CARGA);
    jojoFondoCarga->setBrush(QBrush(QColor(40, 40, 40)));
    jojoFondoCarga->setPen(QPen(Qt::white, 1));
    jojoFondoCarga->setZValue(10);
    scene->addItem(jojoFondoCarga);

    jojoBarraCarga = new QGraphicsRectItem(jojoX, jojoY + 25, ANCHO_BARRA_CARGA, ALTO_BARRA_CARGA);
    jojoBarraCarga->setBrush(QBrush(QColor(0, 191, 255)));
    jojoBarraCarga->setPen(Qt::NoPen);
    jojoBarraCarga->setZValue(11);
    scene->addItem(jojoBarraCarga);

    jojoTexto = new QGraphicsTextItem("Kujo Jotaro");
    jojoTexto->setFont(fuenteMarcador);
    jojoTexto->setDefaultTextColor(Qt::white);
    jojoTexto->setPos(jojoX, jojoY - 25);
    jojoTexto->setZValue(10);
    scene->addItem(jojoTexto);

    // ── DIO (esquina inferior derecha) ────────────────────────────────────
    const int dioX = 1200 - ANCHO_BARRA_VIDA - 30;
    const int dioY = 420;

    dioFondoVida = new QGraphicsRectItem(dioX, dioY, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    dioFondoVida->setBrush(QBrush(QColor(40, 40, 40)));
    dioFondoVida->setPen(QPen(Qt::white, 1.5));
    dioFondoVida->setZValue(10);
    scene->addItem(dioFondoVida);

    dioBarraVida = new QGraphicsRectItem(dioX, dioY, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    dioBarraVida->setBrush(QBrush(QColor(230, 180, 0)));
    dioBarraVida->setPen(Qt::NoPen);
    dioBarraVida->setZValue(11);
    scene->addItem(dioBarraVida);

    dioFondoCarga = new QGraphicsRectItem(dioX + (ANCHO_BARRA_VIDA - ANCHO_BARRA_CARGA), dioY + 25, ANCHO_BARRA_CARGA, ALTO_BARRA_CARGA);
    dioFondoCarga->setBrush(QBrush(QColor(40, 40, 40)));
    dioFondoCarga->setPen(QPen(Qt::white, 1));
    dioFondoCarga->setZValue(10);
    scene->addItem(dioFondoCarga);

    dioBarraCarga = new QGraphicsRectItem(dioX + (ANCHO_BARRA_VIDA - ANCHO_BARRA_CARGA), dioY + 25, ANCHO_BARRA_CARGA, ALTO_BARRA_CARGA);
    dioBarraCarga->setBrush(QBrush(QColor(200, 0, 50)));
    dioBarraCarga->setPen(Qt::NoPen);
    dioBarraCarga->setZValue(11);
    scene->addItem(dioBarraCarga);

    dioTexto = new QGraphicsTextItem("DIO");
    dioTexto->setFont(fuenteMarcador);
    dioTexto->setDefaultTextColor(Qt::white);
    dioTexto->setPos(1200 - 150, dioY - 25);
    dioTexto->setZValue(10);
    scene->addItem(dioTexto);
}

// ═══════════════════════════════════════════════════════════════════════════
//  ACTUALIZAR ESTADOS — Nivel2 (Personaje*), vida base 500
//  Sin cambios respecto al original para no romper el Nivel2.
// ═══════════════════════════════════════════════════════════════════════════
void StandUserStats::actualizarEstados(Personaje* jojo, Personaje* dio)
{
    // ── Jotaro ────────────────────────────────────────────────────────────
    if (jojo && jojoBarraVida && jojoFondoVida) {
        float pct = qBound(0.0f, (float)jojo->getVida() / 500.0f, 1.0f);
        jojoBarraVida->setRect(jojoFondoVida->rect().x(), jojoFondoVida->rect().y(),
                               ANCHO_BARRA_VIDA * pct, ALTO_BARRA_VIDA);

        float pctCarga = qBound(0.0f, (float)jojo->getCarga() / 100.0f, 1.0f);
        jojoBarraCarga->setRect(jojoFondoCarga->rect().x(), jojoFondoCarga->rect().y(),
                                ANCHO_BARRA_CARGA * pctCarga, ALTO_BARRA_CARGA);

        jojoBarraCarga->setBrush(QBrush(
            jojo->getCarga() >= 100 ? QColor(148, 0, 211) : QColor(0, 191, 255)
            ));
    }

    // ── DIO ───────────────────────────────────────────────────────────────
    if (dio && dioBarraVida && dioFondoVida) {
        float pct          = qBound(0.0f, (float)dio->getVida() / 500.0f, 1.0f);
        float nuevoAncho   = ANCHO_BARRA_VIDA * pct;
        float delta        = ANCHO_BARRA_VIDA - nuevoAncho;
        dioBarraVida->setRect(dioFondoVida->rect().x() + delta, dioFondoVida->rect().y(),
                              nuevoAncho, ALTO_BARRA_VIDA);

        float pctCarga     = qBound(0.0f, (float)dio->getCarga() / 100.0f, 1.0f);
        float nuevoAnchoCarga = ANCHO_BARRA_CARGA * pctCarga;
        float deltaCarga   = ANCHO_BARRA_CARGA - nuevoAnchoCarga;
        dioBarraCarga->setRect(dioFondoCarga->rect().x() + deltaCarga, dioFondoCarga->rect().y(),
                               nuevoAnchoCarga, ALTO_BARRA_CARGA);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  ACTUALIZAR ESTADOS NIVEL1 — Jojo*, vida base 100, energía por getEnergia()
//  Llamado desde nivel1.cpp en lugar de actualizarEstados().
//
//  Además cambia el color de la barra de vida según el porcentaje:
//    > 50% → verde   (0, 220, 50)
//    > 25% → amarillo(220, 200, 0)
//    ≤ 25% → rojo    (220, 30, 0)
// ═══════════════════════════════════════════════════════════════════════════
void StandUserStats::actualizarEstadosNivel1(Jojo* jojo)
{
    if (!jojo || !jojoBarraVida || !jojoFondoVida) return;

    // ── Vida (base 100) ───────────────────────────────────────────────────
    float pctVida = qBound(0.0f, (float)jojo->vida / 100.0f, 1.0f);
    jojoBarraVida->setRect(
        jojoFondoVida->rect().x(),
        jojoFondoVida->rect().y(),
        ANCHO_BARRA_VIDA * pctVida,
        ALTO_BARRA_VIDA
        );

    // Color dinámico según el porcentaje de vida
    if (pctVida > 0.50f) {
        jojoBarraVida->setBrush(QBrush(QColor(0, 220, 50)));       // verde
    } else if (pctVida > 0.25f) {
        jojoBarraVida->setBrush(QBrush(QColor(220, 200, 0)));      // amarillo
    } else {
        jojoBarraVida->setBrush(QBrush(QColor(220, 30, 0)));       // rojo crítico
    }

    // ── Energía / Ulti (base 100, vía getEnergia()) ───────────────────────
    if (jojoBarraCarga && jojoFondoCarga) {
        float pctEnergia = qBound(0.0f, (float)jojo->getEnergia() / 100.0f, 1.0f);
        jojoBarraCarga->setRect(
            jojoFondoCarga->rect().x(),
            jojoFondoCarga->rect().y(),
            ANCHO_BARRA_CARGA * pctEnergia,
            ALTO_BARRA_CARGA
            );

        // Al llegar al 100% se vuelve morado (ulti disponible)
        jojoBarraCarga->setBrush(QBrush(
            pctEnergia >= 1.0f ? QColor(148, 0, 211) : QColor(0, 191, 255)
            ));
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  RELOJ Y MARCADOR — Sin cambios, funciona igual para ambos modos
// ═══════════════════════════════════════════════════════════════════════════
void StandUserStats::actualizarRelojYMarcador(int segundosRestantes, int kosJojo, int kosDio)
{
    int minutos  = segundosRestantes / 60;
    int segundos = segundosRestantes % 60;
    textoTemporizador->setPlainText(
        QString("%1:%2").arg(minutos).arg(segundos, 2, 10, QChar('0'))
        );
    textoTemporizador->setPos(1060, 20);

    textoMarcadorKOs->setPlainText(
        QString("%1 ─K.O.S─ %2").arg(kosJojo).arg(kosDio)
        );
    qreal marcadorX = 600 - (textoMarcadorKOs->boundingRect().width() / 2.0);
    textoMarcadorKOs->setPos(marcadorX, 165);
}

// ═══════════════════════════════════════════════════════════════════════════
//  ANUNCIOS CENTRALES — Sin cambios
// ═══════════════════════════════════════════════════════════════════════════
void StandUserStats::mostrarAnuncioCentral(const QString& mensaje, const QColor& color)
{
    textoAnuncioCentral->setPlainText(mensaje);
    textoAnuncioCentral->setDefaultTextColor(color);
    qreal x = 600 - (textoAnuncioCentral->boundingRect().width()  / 2.0);
    qreal y = 220 - (textoAnuncioCentral->boundingRect().height() / 2.0);
    textoAnuncioCentral->setPos(x, y);
}

void StandUserStats::ocultarAnuncioCentral()
{
    textoAnuncioCentral->setPlainText("");
}