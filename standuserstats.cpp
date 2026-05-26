#include "standuserstats.h"

StandUserStats::StandUserStats(QGraphicsScene* scene) {
    QFont fuenteMarcador("Impact", 14);

    // ==================================================
    // ESTADÍSTICAS DE JOTARO (Esquina Inferior Izquierda)
    // ==================================================
    int jojoX = 30;
    int jojoY = 420; // Posicionado estratégicamente sobre el suelo

    // Fondo barra de vida
    jojoFondoVida = new QGraphicsRectItem(jojoX, jojoY, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    jojoFondoVida->setBrush(QBrush(QColor(40, 40, 40)));
    jojoFondoVida->setPen(QPen(Qt::white, 1.5));
    jojoFondoVida->setZValue(10);
    scene->addItem(jojoFondoVida);

    // Barra de vida (Verde)
    jojoBarraVida = new QGraphicsRectItem(jojoX, jojoY, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    jojoBarraVida->setBrush(QBrush(QColor(0, 220, 50)));
    jojoBarraVida->setPen(Qt::NoPen);
    jojoBarraVida->setZValue(11);
    scene->addItem(jojoBarraVida);

    // Fondo barra de carga especial
    jojoFondoCarga = new QGraphicsRectItem(jojoX, jojoY + 25, ANCHO_BARRA_CARGA, ALTO_BARRA_CARGA);
    jojoFondoCarga->setBrush(QBrush(QColor(40, 40, 40)));
    jojoFondoCarga->setPen(QPen(Qt::white, 1));
    jojoFondoCarga->setZValue(10);
    scene->addItem(jojoFondoCarga);

    // Barra de carga (Azul Celeste)
    jojoBarraCarga = new QGraphicsRectItem(jojoX, jojoY + 25, ANCHO_BARRA_CARGA, ALTO_BARRA_CARGA);
    jojoBarraCarga->setBrush(QBrush(QColor(0, 191, 255)));
    jojoBarraCarga->setPen(Qt::NoPen);
    jojoBarraCarga->setZValue(11);
    scene->addItem(jojoBarraCarga);

    // Nombre del Usuario
    jojoTexto = new QGraphicsTextItem("JOTARO KUJO");
    jojoTexto->setFont(fuenteMarcador);
    jojoTexto->setDefaultTextColor(Qt::white);
    jojoTexto->setPos(jojoX, jojoY - 25);
    jojoTexto->setZValue(10);
    scene->addItem(jojoTexto);


    // ==================================================
    // ESTADÍSTICAS DE DIO (Esquina Inferior Derecha)
    // ==================================================
    int dioX = 1200 - ANCHO_BARRA_VIDA - 30; // Ajustado al ancho de tu escena (1200)
    int dioY = 420;

    // Fondo barra de vida DIO
    dioFondoVida = new QGraphicsRectItem(dioX, dioY, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    dioFondoVida->setBrush(QBrush(QColor(40, 40, 40)));
    dioFondoVida->setPen(QPen(Qt::white, 1.5));
    dioFondoVida->setZValue(10);
    scene->addItem(dioFondoVida);

    // Barra de vida DIO (Amarillo Oro)
    dioBarraVida = new QGraphicsRectItem(dioX, dioY, ANCHO_BARRA_VIDA, ALTO_BARRA_VIDA);
    dioBarraVida->setBrush(QBrush(QColor(230, 180, 0)));
    dioBarraVida->setPen(Qt::NoPen);
    dioBarraVida->setZValue(11);
    scene->addItem(dioBarraVida);

    // Fondo barra de carga DIO
    dioFondoCarga = new QGraphicsRectItem(dioX + (ANCHO_BARRA_VIDA - ANCHO_BARRA_CARGA), dioY + 25, ANCHO_BARRA_CARGA, ALTO_BARRA_CARGA);
    dioFondoCarga->setBrush(QBrush(QColor(40, 40, 40)));
    dioFondoCarga->setPen(QPen(Qt::white, 1));
    dioFondoCarga->setZValue(10);
    scene->addItem(dioFondoCarga);

    // Barra de carga DIO (Rojo Sangre)
    dioBarraCarga = new QGraphicsRectItem(dioX + (ANCHO_BARRA_VIDA - ANCHO_BARRA_CARGA), dioY + 25, ANCHO_BARRA_CARGA, ALTO_BARRA_CARGA);
    dioBarraCarga->setBrush(QBrush(QColor(200, 0, 50)));
    dioBarraCarga->setPen(Qt::NoPen);
    dioBarraCarga->setZValue(11);
    scene->addItem(dioBarraCarga);

    // Nombre del Usuario
    dioTexto = new QGraphicsTextItem("DIO BRANDON");
    dioTexto->setFont(fuenteMarcador);
    dioTexto->setDefaultTextColor(Qt::white);
    dioTexto->setPos(1200 - 150, dioY - 25);
    dioTexto->setZValue(10);
    scene->addItem(dioTexto);
}

void StandUserStats::actualizarEstados(Personaje* jojo, Personaje* dio) {
    // ── REFRESCAR DATOS DE JOTARO ──
    if (jojo) {
        // Vida (Base 300)
        float pctVidaJojo = qBound(0.0f, (float)jojo->getVida() / 300.0f, 1.0f);
        jojoBarraVida->setRect(jojoFondoVida->rect().x(), jojoFondoVida->rect().y(),
                               ANCHO_BARRA_VIDA * pctVidaJojo, ALTO_BARRA_VIDA);

        // Carga Especial (Base 100)
        float pctCargaJojo = qBound(0.0f, (float)jojo->getCarga() / 100.0f, 1.0f);
        jojoBarraCarga->setRect(jojoFondoCarga->rect().x(), jojoFondoCarga->rect().y(),
                                ANCHO_BARRA_CARGA * pctCargaJojo, ALTO_BARRA_CARGA);

        // Si Star Platinum está listo, la barra se tiñe de un morado místico
        if (jojo->getCarga() >= 100) {
            jojoBarraCarga->setBrush(QBrush(QColor(148, 0, 211)));
        } else {
            jojoBarraCarga->setBrush(QBrush(QColor(0, 191, 255)));
        }
    }

    // ── REFRESCAR DATOS DE DIO ──
    if (dio) {
        // La vida de DIO se drena de derecha a izquierda
        float pctVidaDio = qBound(0.0f, (float)dio->getVida() / 300.0f, 1.0f);
        float nuevoAnchoVida = ANCHO_BARRA_VIDA * pctVidaDio;
        float deltaVida = ANCHO_BARRA_VIDA - nuevoAnchoVida;
        dioBarraVida->setRect(dioFondoVida->rect().x() + deltaVida, dioFondoVida->rect().y(),
                              nuevoAnchoVida, ALTO_BARRA_VIDA);

        // La carga de DIO se drena igual
        float pctCargaDio = qBound(0.0f, (float)dio->getCarga() / 100.0f, 1.0f);
        float nuevoAnchoCarga = ANCHO_BARRA_CARGA * pctCargaDio;
        float deltaCarga = ANCHO_BARRA_CARGA - nuevoAnchoCarga;
        dioBarraCarga->setRect(dioFondoCarga->rect().x() + deltaCarga, dioFondoCarga->rect().y(),
                               nuevoAnchoCarga, ALTO_BARRA_CARGA);
    }
}
