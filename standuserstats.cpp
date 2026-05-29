#include "standuserstats.h"

StandUserStats::StandUserStats(QGraphicsScene* scene) {
    QFont fuenteMarcador("Impact", 14);
    QFont fuenteReloj("Impact", 26, QFont::Bold);
    QFont fuenteAnuncio("Impact", 48);

    textoTemporizador = new QGraphicsTextItem();
    textoMarcadorKOs = new QGraphicsTextItem();
    textoAnuncioCentral = new QGraphicsTextItem();

    // ==================================================
    // ESTADÍSTICAS DE JOTARO (Esquina Inferior Izquierda)
    // ==================================================
    int jojoX = 30;
    int jojoY = 420;

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

    // ==================================================
    // ESTADÍSTICAS DE DIO (Esquina Inferior Derecha)
    // ==================================================
    int dioX = 1200 - ANCHO_BARRA_VIDA - 30;
    int dioY = 420;

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

    // ==========================================================
    // INTERFAZ DE CRONÓMETRO, MARCADOR Y ANUNCIOS
    // ==========================================================
    textoTemporizador->setDefaultTextColor(QColor(255, 215, 0));
    textoTemporizador->setFont(fuenteReloj);
    textoTemporizador->setZValue(15);
    textoTemporizador->setPos(1060, 20);
    scene->addItem(textoTemporizador);

    textoMarcadorKOs->setFont(fuenteMarcador);
    textoMarcadorKOs->setDefaultTextColor(QColor(255, 255, 255));
    textoMarcadorKOs->setZValue(15);
    scene->addItem(textoMarcadorKOs);

    textoAnuncioCentral->setFont(fuenteAnuncio);
    textoAnuncioCentral->setZValue(20);
    scene->addItem(textoAnuncioCentral);
}

void StandUserStats::actualizarEstados(Personaje* jojo, Personaje* dio) {
    // ── REFRESCAR DATOS DE JOTARO (Vida Base 500) ──
    if (jojo) {
        float pctVidaJojo = qBound(0.0f, (float)jojo->getVida() / 500.0f, 1.0f); // Dividido por 500
        jojoBarraVida->setRect(jojoFondoVida->rect().x(), jojoFondoVida->rect().y(),
                               ANCHO_BARRA_VIDA * pctVidaJojo, ALTO_BARRA_VIDA);

        float pctCargaJojo = qBound(0.0f, (float)jojo->getCarga() / 100.0f, 1.0f);
        jojoBarraCarga->setRect(jojoFondoCarga->rect().x(), jojoFondoCarga->rect().y(),
                                ANCHO_BARRA_CARGA * pctCargaJojo, ALTO_BARRA_CARGA);

        if (jojo->getCarga() >= 100) {
            jojoBarraCarga->setBrush(QBrush(QColor(148, 0, 211)));
        } else {
            jojoBarraCarga->setBrush(QBrush(QColor(0, 191, 255)));
        }
    }

    // ── REFRESCAR DATOS DE DIO (Vida Base 500) ──
    if (dio) {
        float pctVidaDio = qBound(0.0f, (float)dio->getVida() / 500.0f, 1.0f); // Dividido por 500
        float nuevoAnchoVida = ANCHO_BARRA_VIDA * pctVidaDio;
        float deltaVida = ANCHO_BARRA_VIDA - nuevoAnchoVida;
        dioBarraVida->setRect(dioFondoVida->rect().x() + deltaVida, dioFondoVida->rect().y(),
                              nuevoAnchoVida, ALTO_BARRA_VIDA);

        float pctCargaDio = qBound(0.0f, (float)dio->getCarga() / 100.0f, 1.0f);
        float nuevoAnchoCarga = ANCHO_BARRA_CARGA * pctCargaDio;
        float deltaCarga = ANCHO_BARRA_CARGA - nuevoAnchoCarga;
        dioBarraCarga->setRect(dioFondoCarga->rect().x() + deltaCarga, dioFondoCarga->rect().y(),
                               nuevoAnchoCarga, ALTO_BARRA_CARGA);
    }
}

void StandUserStats::actualizarRelojYMarcador(int segundosRestantes, int kosJojo, int kosDio) {
    int minutos = segundosRestantes / 60;
    int segundos = segundosRestantes % 60;
    textoTemporizador->setPlainText(QString("%1:%2").arg(minutos).arg(segundos, 2, 10, QChar('0')));
    textoTemporizador->setPos(1060, 20);

    textoMarcadorKOs->setPlainText(QString("%1 -K.O.S─ %2").arg(kosJojo).arg(kosDio));
    qreal marcadorX = 600 - (textoMarcadorKOs->boundingRect().width() / 2);
    textoMarcadorKOs->setPos(marcadorX, 165);
}

void StandUserStats::mostrarAnuncioCentral(const QString& mensaje, const QColor& color) {
    textoAnuncioCentral->setPlainText(mensaje);
    textoAnuncioCentral->setDefaultTextColor(color);

    qreal x = 600 - (textoAnuncioCentral->boundingRect().width() / 2);
    qreal y = 220 - (textoAnuncioCentral->boundingRect().height() / 2);
    textoAnuncioCentral->setPos(x, y);
}

void StandUserStats::ocultarAnuncioCentral() {
    textoAnuncioCentral->setPlainText("");
}
