#include "standuserstats.h"

StandUserStats::StandUserStats(QGraphicsScene* scene) {
    QFont fuenteMarcador("Impact", 14);
    QFont fuenteReloj("Impact", 26, QFont::Bold);
    QFont fuenteAnuncio("Impact", 48);

    // Inicializar los punteros obligatoriamente antes de usarlos para evitar Crashes (SegFault)
    textoTemporizador = new QGraphicsTextItem();
    textoMarcadorKOs = new QGraphicsTextItem();
    textoAnuncioCentral = new QGraphicsTextItem();

    // ==================================================
    // ESTADÍSTICAS DE JOTARO (Esquina Inferior Izquierda)
    // ==================================================
    int jojoX = 30;
    int jojoY = 420; // Posicionado estratégicamente sobre el suelo [cite: 32]

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
    jojoTexto = new QGraphicsTextItem("Kujo Jotaro");
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
    dioTexto = new QGraphicsTextItem("DIO");
    dioTexto->setFont(fuenteMarcador);
    dioTexto->setDefaultTextColor(Qt::white);
    dioTexto->setPos(1200 - 150, dioY - 25);
    dioTexto->setZValue(10);
    scene->addItem(dioTexto);

    // ==========================================================
    // NUEVA ENTRADA: INTERFAZ DE CRONÓMETRO, MARCADOR Y ANUNCIOS
    // ==========================================================

    // Texto del Reloj Central (Arriba a la derecha fijo, modo Arcade)
    textoTemporizador->setDefaultTextColor(QColor(255, 215, 0)); // Color oro llamativo
    textoTemporizador->setFont(fuenteReloj);
    textoTemporizador->setZValue(15);
    // Posición estática ideal en la esquina superior derecha
    textoTemporizador->setPos(1060, 20);
    scene->addItem(textoTemporizador);

    // Texto del Contador de K.Os (Alineado justo abajo de la pantalla gigante del fondo)
    textoMarcadorKOs->setFont(fuenteMarcador);
    textoMarcadorKOs->setDefaultTextColor(QColor(255, 255, 255)); // Blanco puro para contrastar
    textoMarcadorKOs->setZValue(15);
    scene->addItem(textoMarcadorKOs);

    // Gran letrero para alertas del sistema (K.O. / ROUND X / VICTORIA)
    textoAnuncioCentral->setFont(fuenteAnuncio);
    textoAnuncioCentral->setZValue(20); // Prioridad absoluta sobre los sprites
    scene->addItem(textoAnuncioCentral);
}

void StandUserStats::actualizarEstados(Personaje* jojo, Personaje* dio) {
    // ── REFRESCAR DATOS DE JOTARO ──
    if (jojo) {
        // Vida (Base 300) [cite: 25]
        float pctVidaJojo = qBound(0.0f, (float)jojo->getVida() / 300.0f, 1.0f);
        jojoBarraVida->setRect(jojoFondoVida->rect().x(), jojoFondoVida->rect().y(),
                               ANCHO_BARRA_VIDA * pctVidaJojo, ALTO_BARRA_VIDA);

        // Carga Especial (Base 100) [cite: 26, 27]
        float pctCargaJojo = qBound(0.0f, (float)jojo->getCarga() / 100.0f, 1.0f);
        jojoBarraCarga->setRect(jojoFondoCarga->rect().x(), jojoFondoCarga->rect().y(),
                                ANCHO_BARRA_CARGA * pctCargaJojo, ALTO_BARRA_CARGA);

        // Si Star Platinum está listo, la barra se tiñe de un morado místico [cite: 31, 38]
        if (jojo->getCarga() >= 100) {
            jojoBarraCarga->setBrush(QBrush(QColor(148, 0, 211))); // Morado Star Platinum
        } else {
            jojoBarraCarga->setBrush(QBrush(QColor(0, 191, 255))); // Azul celeste base
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

void StandUserStats::actualizarRelojYMarcador(int segundosRestantes, int kosJojo, int kosDio) {
    // Dar formato de MM:SS al tiempo entero [cite: 25]
    int minutos = segundosRestantes / 60;
    int segundos = segundosRestantes % 60;
    textoTemporizador->setPlainText(QString("%1:%2").arg(minutos).arg(segundos, 2, 10, QChar('0')));

    // ¡CORRECCIÓN CRÍTICA!: Quitamos el centrado automático para que el reloj respete
    // la posición estática fija de la esquina superior derecha (X=1060, Y=20)
    textoTemporizador->setPos(1060, 20);

    // Actualizar el marcador numérico de K.Os [cite: 25]
    textoMarcadorKOs->setPlainText(QString("%1 -K.O.S─ %2").arg(kosJojo).arg(kosDio));

    // Centrar el contador de K.Os horizontalmente pero bajarlo en el eje Y
    // para que se alinee perfectamente abajo de la marquesina estática del sprite
    qreal marcadorX = 600 - (textoMarcadorKOs->boundingRect().width() / 2);
    textoMarcadorKOs->setPos(marcadorX, 165); // <--- Bajado estratégicamente a Y=165
}

void StandUserStats::mostrarAnuncioCentral(const QString& mensaje, const QColor& color) {
    textoAnuncioCentral->setPlainText(mensaje);
    textoAnuncioCentral->setDefaultTextColor(color);

    // Centrado perfecto en la pantalla (1200x500)
    qreal x = 600 - (textoAnuncioCentral->boundingRect().width() / 2);
    qreal y = 220 - (textoAnuncioCentral->boundingRect().height() / 2);
    textoAnuncioCentral->setPos(x, y);
}

void StandUserStats::ocultarAnuncioCentral() {
    textoAnuncioCentral->setPlainText("");
}
