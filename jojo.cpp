#include "jojo.h"
#include "dio.h"

static const float ESCALA = 1.8f;

Jojo::Jojo() : Personaje()
{
    setPixmap(QPixmap(":/sprites/SpritesJojoChampionship/Jotaro Kujo sprites.png").scaled(60*1.8, 90*1.8));
    estaDefendiendo = false;
    tiempoAtaque = 0;
    frameActual = 0;
    contadorAnimacion = 0;
    mirandoDerecha = true;

    animoActual = NORMAL_ANIMO;
    ticksSinAtacar = 0;
    contadorGolpesRecibidos = 0;
    proximoAtaquePotenciado = false;
    elAuraEfecto = nullptr;

    sonidoBasico = new QSoundEffect(this);
    sonidoBasico->setSource(QUrl("qrc:/Efectos/EfectosdeAudio/Star platinum single ora.wav"));
    sonidoBasico->setVolume(0.85f);

    sonidoFuerte1 = new QSoundEffect(this);
    sonidoFuerte1->setSource(QUrl("qrc:/Efectos/EfectosdeAudio/ORA ORA ORA - Sound effect.wav"));
    sonidoFuerte1->setVolume(0.85f);

    sonidoFuerte2 = new QSoundEffect(this);
    sonidoFuerte2->setSource(QUrl("qrc:/Efectos/EfectosdeAudio/Star platinum single ora.wav"));
    sonidoFuerte2->setVolume(0.85f);

    sonidoEspecial = new QSoundEffect(this);
    sonidoEspecial->setSource(QUrl("qrc:/Efectos/EfectosdeAudio/Ora Ora Ora Ora Ora sound effect.wav"));
    sonidoEspecial->setVolume(1.0f);

    // ── Inicializar variables de nivel 1 con valores por defecto ──
    esNivel1           = false;
    speed              = 4;
    direccion          = 1;
    enMovimiento       = false;
    atacando           = false;
    vida               = 100;
    invencible         = false;
    contadorInvencible = 0;
    energiaUlti        = 0;
    ultiActiva         = false;
    timerUlti          = 0;
    velocidadBonus     = 0;
    timerVelocidad     = 0;

    cargarSprites();
    if (!spritesQUIETO.isEmpty()) {
        setPixmap(spritesQUIETO.at(0));
    } else {
        setPixmap(QPixmap(10,10));
        qDebug() << "ALERTA: No se cargaron los sprites.";
    }
}

void Jojo::cargarSprites() {
    QImage imagen(":/sprites/SpritesJojoChampionship/Jotaro Kujo sprites.png");
    QRgb colorFondo = imagen.pixel(0,0);
    imagen.setAlphaChannel(imagen.createMaskFromColor(colorFondo, Qt::MaskOutColor));
    QPixmap hoja = QPixmap::fromImage(imagen);

    // Stand (Quieto)
    for(int i = 0; i < 6; i++)
        spritesQUIETO.append(hoja.copy(7 + (i * 41), 29, 42, 67));
    // Walk (Caminar)
    for(int i = 0; i < 8; i++)
        spritesCAMINAR.append(hoja.copy(313 + (i * 48), 29, 47, 67));
    // Guard (defensa)
    for(int i = 0; i < 2; i++)
        spritesDEFENSA.append(hoja.copy(165 + (i * 40), 126, 39, 70));
    // Jump (Salto)
    for(int i = 0; i < 8; i++)
        spritesSALTO.append(hoja.copy(415 + (i * 48), 126, 47, 72));
    // Basico1
    for(int i = 0; i < 4; i++)
        spritesBASICO1.append(hoja.copy(7 + (i * 50), 707, 50, 70));
    // Basico2
    for(int i = 0; i < 2; i++)
        spritesBASICO2.append(hoja.copy(262 + (i * 37), 707, 37, 70));
    for(int i = 0; i < 3; i++)
        spritesBASICO2.append(hoja.copy(336 + (i * 60), 707, 60, 67));
    // Fuerte1
    for(int i=0; i<6; i++)
        spritesFUERTE1.append(hoja.copy(4 + (i*41), 1080, 38, 72));
    spritesFUERTE1.append(hoja.copy(258, 1045, 45, 110));
    for(int i=0; i<4; i++)
        spritesFUERTE1.append(hoja.copy(318 + (i*82), 1045, 87, 110));
    for(int i=0; i<1; i++)
        spritesFUERTE1.append(hoja.copy(652 + (i*90), 1045, 90, 110));
    for(int i=0; i<2; i++)
        spritesFUERTE1.append(hoja.copy(752 + (i*82), 1045, 87, 110));
    for(int i=0; i<1; i++)
        spritesFUERTE1.append(hoja.copy(935 + (i*70), 1045, 50, 110));
    //Fuerte2 (4 jotaro, 8 SP)
    for(int i=0; i<4; i++)
        spritesFUERTE2.append(hoja.copy(4 + (i*41), 1572, 36, 68));
    for(int i=0; i<2; i++)
        spritesFUERTE2.append(hoja.copy(300 + (i*70), 1572, 65, 68));
    for(int i=0; i<2; i++)
        spritesFUERTE2.append(hoja.copy(435 + (i*55), 1572, 55, 68));
    for(int i=0; i<4; i++)
        spritesFUERTE2.append(hoja.copy(555 + (i*86), 1572, 78, 68));
    for(int i=0; i<2; i++)
        spritesFUERTE2.append(hoja.copy(900 + (i*50), 1572, 40, 68));
    //Especial (5 jotaro, 28 SP)
    for(int i=0; i<5; i++)
        spritesESPECIAL.append(hoja.copy(7 + (i*40), 2300, 33, 68));
    spritesESPECIAL.append(hoja.copy(200, 2300, 35, 68));
    for(int i=0; i<12; i++)
        spritesESPECIAL.append(hoja.copy(235 + (i*90), 2270, 80, 98));
    for(int i=0; i<2; i++)
        spritesESPECIAL.append(hoja.copy(1295 + (i*78), 2260, 70, 103));
    for(int i=0; i<8; i++)
        spritesESPECIAL.append(hoja.copy(7 + (i*80), 2381, 77, 110));
    for(int i=0; i<3; i++)
        spritesESPECIAL.append(hoja.copy(745 + (i*45), 2424, 40, 72));
    // ── DAÑO 1: 4 sprites
    for(int i = 0; i < 4; i++)
        spritesDANO1.append(hoja.copy(7 + (i * 40), 233, 40, 62));
    // ── DAÑO 2: 5 sprites
    for(int i = 0; i < 5; i++)
        spritesDANO2.append(hoja.copy(600 - (i * 62), 234, 58, 62));
    // ── STANDUP: 5 sprites
    for(int i = 0; i < 3; i++)
        spritesSTANDUP.append(hoja.copy(607 + (i * 65), 234, 58, 60));
    for(int i = 0; i < 2; i++)
        spritesSTANDUP.append(hoja.copy(800 + (i * 40), 234, 40, 60));

    float escala = 1.8f;

    auto escalarLista = [&](QList<QPixmap>& lista) {
        for (QPixmap& p : lista)
            p = p.scaled(p.width() * escala, p.height() * escala,
                         Qt::KeepAspectRatio, Qt::FastTransformation);
    };

    escalarLista(spritesQUIETO);
    escalarLista(spritesCAMINAR);
    escalarLista(spritesSALTO);
    escalarLista(spritesDEFENSA);
    escalarLista(spritesBASICO1);
    escalarLista(spritesBASICO2);
    escalarLista(spritesFUERTE1);
    escalarLista(spritesFUERTE2);
    escalarLista(spritesESPECIAL);
    escalarLista(spritesDANO1);
    escalarLista(spritesDANO2);
    escalarLista(spritesSTANDUP);
}

// ═══════════════════════════════════════════════════════════════════════════
//  ACTIVAR DAÑO 1 — golpe leve (acumulado < 15)
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::activarDano() {
    estadoDano = DANO1;
    frameDano = 0;
    ralentizadorDano = 0;

    qDebug() << "[DAÑO1] Golpe leve recibido. Acumulado:" << danioAcumulado;
}

// ═══════════════════════════════════════════════════════════════════════════
// ACTIVAR DAÑO 2: activarEstadoDano(bool) -> Para golpes fuertes/knockback
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::activarDano(bool mitadEmpuje) {
    estadoDano = DANO2;
    frameDano = 0;
    ralentizadorDano = 0;

    // SI ESTÁ EN ATAQUE FUERTE O ESPECIAL, TIENE ARMADURA IMPARABLE
    if (stand && (faseCombo == 3 || faseCombo == 4 || faseCombo == 5)) {
        qDebug() << "[HYPER ARMOR] ¡Jotaro resiste el aturdimiento y continúa el ataque!";
    } else {
        estaAtacando = false;
        stand = false;
        faseCombo = 0;
        frameActualStand = 0;
    }

    // El empuje físico se aplica igual
    float empujeX = mirandoDerecha ? -6.0f : 6.0f;
    float empujeY = -10.0f;

    if (mitadEmpuje) {
        empujeX *= 0.5f;
        empujeY *= 0.5f;
    }

    vx = empujeX;
    vy = empujeY;
    enSuelo = false;

    qDebug() << "[DAÑO2] Golpe fuerte recibido. Acumulado:" << danioAcumulado;
}

// ═══════════════════════════════════════════════════════════════════════════
//  recibirDano — lógica principal con acumulación
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::recibirDano(int cantidad) {

    recibirDano(cantidad, this->x());
}

void Jojo::recibirDano(int cantidad, float atacanteX) {
    if (estadoDano == MUERTO) return;

    // --- LÓGICA DE CONTADOR GOLPES GOLPEADOS (Aura Roja de Furia) ---
    if (animoActual != ENOJADO) {
        contadorGolpesRecibidos++;
        if (contadorGolpesRecibidos >= 40) {
            animoActual = ENOJADO;
            proximoAtaquePotenciado = true;
            actualizarAuraVisual();
            qDebug() << "[ENOJADO ACTIVO] ¡Jotaro ha recibido 40 golpes! Siguiente ataque hará +40% de daño.";
        }
    }

    bool defendiendo = estaDefendiendo;
    if (defendiendo) cantidad /= 2;

    puntosdevida -= cantidad;
    qDebug() << "¡Jojo recibió" << cantidad << "de daño! Vida:" << puntosdevida;

    if (puntosdevida <= 0) {
        puntosdevida = 0;
        estadoDano = MUERTO;
        frameDano = 0;
        estaAtacando = false;
        stand = false;
        vx = 0; vy = 0;
        if (sonidoBasico && sonidoBasico->isPlaying())   sonidoBasico->stop();
        if (sonidoFuerte1 && sonidoFuerte1->isPlaying()) sonidoFuerte1->stop();
        if (sonidoFuerte2 && sonidoFuerte2->isPlaying()) sonidoFuerte2->stop();
        if (sonidoEspecial && sonidoEspecial->isPlaying()) sonidoEspecial->stop();
        qDebug() << ">>> JOJO HA SIDO DERROTADO <<<";
        return;
    }
    danioAcumulado += cantidad;

    // Calculamos la dirección del empuje basándonos en las posiciones
    float direccionEmpuje = 0.0f;
    if (atacanteX < this->x())       direccionEmpuje = 0.5f;
    else if (atacanteX > this->x())  direccionEmpuje = -0.5f;

    if (stand && (faseCombo == 3 || faseCombo == 4 || faseCombo == 5)) {
        estadoDano = NORMAL;
        vx = direccionEmpuje * 4.0f;
        return;
    }

    if (defendiendo) {
        if (danioAcumulado >= 22) {
            this->activarDano(true);
            vx = direccionEmpuje * 10.0f;
            danioAcumulado = 0;
        }
        return;
    }

    if (danioAcumulado >= 22) {
        this->activarDano(false);
        vx = direccionEmpuje * 10.0f;
        danioAcumulado = 0;
        if (sonidoBasico && sonidoBasico->isPlaying())   sonidoBasico->stop();
        if (sonidoFuerte1 && sonidoFuerte1->isPlaying()) sonidoFuerte1->stop();
        if (sonidoFuerte2 && sonidoFuerte2->isPlaying()) sonidoFuerte2->stop();
        if (sonidoEspecial && sonidoEspecial->isPlaying()) sonidoEspecial->stop();
    } else {
        this->activarDano();
        vx = direccionEmpuje * 1.0f;
        if (sonidoBasico && sonidoBasico->isPlaying()) sonidoBasico->stop();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  actualizarAnimDano — avanza frames de Daño1/Daño2/StandUp
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::actualizarAnimDano() {
    if (estadoDano == NORMAL || estadoDano == MUERTO) return;

    if (++ralentizadorDano < 7) return;
    ralentizadorDano = 0;

    switch (estadoDano) {

    case DANO1:
        frameDano++;
        if (frameDano >= spritesDANO1.size()) {
            frameDano = spritesDANO1.size() - 1;
            estadoDano = NORMAL;
            danioAcumulado = 0;
            vx = 0;
        }
        break;

    case DANO2:
        frameDano++;
        if (frameDano >= spritesDANO2.size()) {
            frameDano = spritesDANO2.size() - 1;
            if (enSuelo) {
                estadoDano = STANDUP;
                frameDano = 0;
                danioAcumulado = 0;
                vx = 0;
            }
        }
        break;

    case STANDUP:
        frameDano++;
        if (frameDano >= spritesSTANDUP.size()) {
            estadoDano = NORMAL;
            frameDano = 0;
            danioAcumulado = 0;
        }
        break;

    default: break;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  moverse
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::moverse() {
    if (!scene()) return;

    if (estadoDano == MUERTO) {
        vx = 0;
        float gravedadActual = aceleracion_y;
        if (animoActual == ENOJADO) {
            gravedadActual = aceleracion_y * 1.3f;
        }
        vy += gravedadActual;
        if (!verificarColision(x(), y() + vy)) {
            setPos(x(), y() + vy);
        } else if (vy > 0) {
            enSuelo = true;
            vy = 0;
        }
        if (!spritesSTANDUP.isEmpty()) {
            int idx = qBound(0, frameDano, spritesSTANDUP.size() - 1);
            QPixmap f = spritesSTANDUP.at(idx);

            if (!mirandoDerecha) {
                f = f.transformed(QTransform().scale(-1, 1));
            }
            setPixmap(f);
            setOffset(0, 0);
        }
        return;
    }

    if (!estaAtacando && !estaDefendiendo && estadoDano == NORMAL) {
        if (animoActual == NORMAL_ANIMO) {
            ticksSinAtacar++;
            if (ticksSinAtacar >= 240) {
                animoActual = CALCULADOR;
                ticksSinAtacar = 0;
                actualizarAuraVisual();
                qDebug() << "[CALCULADOR ACTIVO] Jotaro analiza los movimientos de DIO. +20% Velocidad.";
            }
        }
    } else {
        if (animoActual == CALCULADOR) {
            animoActual = NORMAL_ANIMO;
            ticksSinAtacar = 0;
            actualizarAuraVisual();
            qDebug() << "[CALCULADOR DESACTIVADO] Concentración interrumpida.";
        }
    }

    float multiplicadorVelocidad = 1.0f;
    if (animoActual == CALCULADOR) {
        multiplicadorVelocidad = 1.2f;
    } else if (animoActual == ENOJADO) {
        multiplicadorVelocidad = 0.7f;
    }

    bool enAnimDano = (estadoDano == DANO1 || estadoDano == DANO2 || estadoDano == STANDUP);

    float vxFinal = vx;

    if (esDummy) {
        if (!enAnimDano) vxFinal = 0;
    } else {
        if (!enAnimDano) {
            if (estaAtacando || estaDefendiendo) {
                vxFinal = 0;
            } else {
                vxFinal = vx * multiplicadorVelocidad;
            }
        }
    }

    float gravedadActual = aceleracion_y;
    if (animoActual == ENOJADO) {
        gravedadActual = aceleracion_y * 1.3f;
    }

    if (estadoDano == DANO2) {
        vy += gravedadActual;
        if (!verificarColision(x(), y() + vy)) setPos(x(), y() + vy);
        else { if (vy > 0) { enSuelo = true; vy = 0; } }
        if (!verificarColision(x() + vxFinal, y())) setPos(x() + vxFinal, y());
        if (enSuelo) vx = 0;
    } else {
        vy += gravedadActual;
        if (!verificarColision(x(), y() + vy)) setPos(x(), y() + vy);
        else { if (vy > 0) { enSuelo = true; vy = 0; } }
        if (!verificarColision(x() + vxFinal, y())) setPos(x() + vxFinal, y());
    }

    actualizarAnimDano();

    if (enAnimDano && !stand) {
        QList<QPixmap>* animDano = nullptr;
        if (estadoDano == DANO1)   animDano = &spritesDANO1;
        if (estadoDano == DANO2)   animDano = &spritesDANO2;
        if (estadoDano == STANDUP) animDano = &spritesSTANDUP;

        if (animDano && !animDano->isEmpty()) {
            int idx = qBound(0, frameDano, animDano->size() - 1);
            QPixmap f = animDano->at(idx);
            if (!mirandoDerecha) f = f.transformed(QTransform().scale(-1,1));
            setPixmap(f);
            setOffset(0, 0);
        }
        return;
    }

    QList<QPixmap> *animacionActual = nullptr;
    bool bucle = true;

    if (stand) {
        if (faseCombo == 3) animacionActual = &spritesFUERTE1;
        else if (faseCombo == 4) animacionActual = &spritesFUERTE2;
        else if (faseCombo == 5) animacionActual = &spritesESPECIAL;
        bucle = false;
    } else if (estaAtacando) {
        animacionActual = (faseCombo == 1) ? &spritesBASICO1 : &spritesBASICO2;
        bucle = false;
    } else if (estaDefendiendo) {
        animacionActual = &spritesDEFENSA;
        bucle = false;
    } else if (!enSuelo) {
        animacionActual = &spritesSALTO;
        bucle = true;
    } else if (vx != 0) {
        animacionActual = &spritesCAMINAR;
        bucle = true;
    } else {
        animacionActual = &spritesQUIETO;
        bucle = true;
    }

    if (!animacionActual || animacionActual->isEmpty()) return;

    static int ralentizadorJ = 0;
    if (++ralentizadorJ >= 5) {
        ralentizadorJ = 0;
        int limiteJojo;
        if (faseCombo == 3) limiteJojo = 2;
        else if (faseCombo == 5) limiteJojo = 4;
        else limiteJojo = 3;

        if (stand) {
            if (faseCombo != 5 && frameActual < limiteJojo) frameActual++;
        } else {
            frameActual++;
        }
    }

    if (frameActual >= animacionActual->size() && !stand)
        frameActual = bucle ? 0 : animacionActual->size() - 1;

    if (!estaAtacando) {
        if (vx > 0) mirandoDerecha = true;
        else if (vx < 0) mirandoDerecha = false;
    }

    if (stand) {
        if (faseCombo == 5) {
            QPixmap jojoPix = spritesESPECIAL.at(qBound(0, frameJotaroEsp, 4));
            int idxSP = qBound(6, 6 + frameSPEsp, spritesESPECIAL.size() - 1);
            QPixmap spPix = spritesESPECIAL.at(idxSP);

            if (!mirandoDerecha) {
                jojoPix = jojoPix.transformed(QTransform().scale(-1, 1));
                spPix   = spPix.transformed(QTransform().scale(-1, 1));
            }

            QPixmap combinado(jojoPix.width() + spPix.width() + 150, qMax(jojoPix.height(), spPix.height()) + 30);
            combinado.fill(Qt::transparent);
            QPainter painter(&combinado);

            int jojoX = mirandoDerecha ? 0 : spPix.width() + 10;
            int jojoY = combinado.height() - jojoPix.height() - 5;
            painter.drawPixmap(jojoX, jojoY, jojoPix);

            int spX = mirandoDerecha ? jojoPix.width() : 0;
            int spY = combinado.height() - spPix.height() - 5;
            painter.drawPixmap(spX, spY, spPix);
            painter.end();

            setPixmap(combinado);
            setOffset(mirandoDerecha ? 0 : -(spPix.width() + 10), -jojoY);
            return;
        }

        int limiteJojo, offsetSP;
        if (faseCombo == 3)      { limiteJojo = 2; offsetSP = 6; }
        else if (faseCombo == 4) { limiteJojo = 3; offsetSP = 4; }
        else                     { limiteJojo = 3; offsetSP = 0; }

        QPixmap jojoPix = animacionActual->at(qMin(frameActual, limiteJojo));
        int idxSP = offsetSP + frameActualStand;
        if (idxSP >= animacionActual->size()) idxSP = animacionActual->size() - 1;
        QPixmap spPix = animacionActual->at(idxSP);

        if (!mirandoDerecha) {
            jojoPix = jojoPix.transformed(QTransform().scale(-1, 1));
            spPix   = spPix.transformed(QTransform().scale(-1, 1));
        }

        int anchoExtra = (faseCombo == 5) ? 150 : 40;
        QPixmap combinado(jojoPix.width() + spPix.width() + anchoExtra, qMax(jojoPix.height(), spPix.height()) + 30);
        combinado.fill(Qt::transparent);
        QPainter painter(&combinado);

        int jojoX = mirandoDerecha ? 0 : spPix.width() + 10;
        int jojoY = combinado.height() - jojoPix.height() - 5;
        painter.drawPixmap(jojoX, jojoY, jojoPix);

        int spX = mirandoDerecha ? jojoPix.width() : 0;
        int spY = combinado.height() - spPix.height() - 5;
        painter.drawPixmap(spX, spY, spPix);
        painter.end();

        setPixmap(combinado);
        setOffset(mirandoDerecha ? 0 : -(spPix.width() + 10), -jojoY);
        return;
    }

    int idx = qBound(0, frameActual, animacionActual->size() - 1);
    QPixmap frameFinal = animacionActual->at(idx);
    if (!mirandoDerecha) frameFinal = frameFinal.transformed(QTransform().scale(-1,1));
    setPixmap(frameFinal);
    setOffset(0, 0);
}

void Jojo::saltar() {
    if (enSuelo && !estaDefendiendo && estadoDano == NORMAL) {
        vy = -15;
    }
    enSuelo = false;
}

void Jojo::atacar() {
    if (puedeAtacar && !estaAtacando && !estaDefendiendo && !stand && estadoDano == NORMAL) {
        estaAtacando = true;
        puedeAtacar = false;
        danoAplicado = false;
        frameActual = 0;
        faseCombo = 1;
        danioAcumulado = 0;
        qDebug() << "ORA! Iniciando combo...";
        if (sonidoBasico) {
            sonidoBasico->stop();
            sonidoBasico->play();
        }
        QTimer::singleShot(1000, [this]() {
            puedeAtacar = true;
            qDebug() << "Ataque listo de nuevo.";
        });
    }
}

void Jojo::actualizarAtaque() {
    if (!estaAtacando) return;

    QList<QPixmap>* anim = (faseCombo == 1) ? &spritesBASICO1 : &spritesBASICO2;

    if (frameActual == 2 && !danoAplicado)
        evaluarHitboxBasico();
    if (frameActual >= anim->size() - 1) {
        if (faseCombo == 1) {
            faseCombo = 2;
            frameActual = 0;
            danoAplicado = false;
        } else {
            if (animoActual == ENOJADO) {
                animoActual = NORMAL_ANIMO;
                contadorGolpesRecibidos = 0;
                proximoAtaquePotenciado = false;
                actualizarAuraVisual();
                qDebug() << "[ENOJADO TERMINADO] La furia de Jotaro se ha disipado.";
            }
            estaAtacando = false;
            faseCombo = 0;
        }
    }
}

void Jojo::atacar(int tipo) {
    if (stand || !puedeAtacar || estaAtacando || estaDefendiendo || estadoDano != NORMAL) return;

    estaAtacando = true;
    stand = true;
    puedeAtacar = false;
    danoAplicado = false;
    frameActual = 0;
    frameActualStand = 0;
    danioAcumulado = 0;
    faseCombo = (tipo == 1) ? 3 : 4;

    if (tipo == 1 && sonidoFuerte1) {
        sonidoFuerte1->stop();
        sonidoFuerte1->play();
    } else if (tipo == 2 && sonidoFuerte2) {
        sonidoFuerte2->stop();
        sonidoFuerte2->play();
    }

    QTimer::singleShot(2000, [this]() { puedeAtacar = true; });
}

void Jojo::actualizarAtaquesFuertes() {
    if (!stand || (faseCombo != 3 && faseCombo != 4)) return;

    if (++ralentizadorStand >= 7) {
        ralentizadorStand = 0;
        frameActualStand++;
        if (faseCombo == 3){
            evaluarHitboxFuerte1();
        }else if (faseCombo == 4){
            evaluarHitboxFuerte2();
        }
    }

    int limiteJojo = (faseCombo == 3) ? 2 : 3;
    if (frameActual < limiteJojo) {
        if (++ralentizadorJFuerte >= 7) {
            ralentizadorJFuerte = 0;
            frameActual++;
        }
    }

    int totalSpritesSP = (faseCombo == 3) ? 9 : 10;
    if (frameActualStand >= totalSpritesSP) {
        if (animoActual == ENOJADO) {
            animoActual = NORMAL_ANIMO;
            contadorGolpesRecibidos = 0;
            proximoAtaquePotenciado = false;
            actualizarAuraVisual();
            qDebug() << "[ENOJADO TERMINADO] La furia de Jotaro se ha disipado.";
        }
        stand = false;
        estaAtacando = false;
        faseCombo = 0;
        frameActual = 0;
        frameActualStand = 0;
        setOffset(0, 0);
        qDebug() << "--- Ataque fuerte finalizado ---";
    }
}

void Jojo::habilidadEspecial() {
    if (stand || estadoDano != NORMAL) return;
    if (barradeCarga >= 100) {
        qDebug() << "¡STAR PLATINUM: Za Warudo!";
        estaAtacando   = true;
        stand          = true;
        faseCombo      = 5;
        danoAplicado   = false;
        barradeCarga   = 0;
        danioAcumulado = 0;
        puedeAtacar    = true;

        frameActual      = 0;
        frameActualStand = 0;
        frameJotaroEsp   = 0;
        frameSPEsp       = 0;
        ralentJotaroEsp  = 0;
        ralentSPEsp      = 0;
        ralentizadorStand = 0;
        if (sonidoEspecial) {
            sonidoEspecial->stop();
            sonidoEspecial->play();
        }
    } else {
        qDebug() << "Yare yare daze. Carga necesaria: 100. Actual:" << barradeCarga;
    }
}

void Jojo::actualizarEspecial() {
    if (!stand || faseCombo != 5) return;

    if (frameJotaroEsp < 5) {
        if (++ralentJotaroEsp >= 7) {
            ralentJotaroEsp = 0;
            frameJotaroEsp++;
        }
    }

    if (frameSPEsp < 28) {
        if (++ralentSPEsp >= 6) {
            ralentSPEsp = 0;

            if (frameSPEsp < 22)
                evaluarHitboxEspecial();
            frameSPEsp++;
        }
    }

    if (frameSPEsp >= 28) {
        if (animoActual == ENOJADO) {
            animoActual = NORMAL_ANIMO;
            contadorGolpesRecibidos = 0;
            proximoAtaquePotenciado = false;
            qDebug() << "[ENOJADO TERMINADO] La furia de Jotaro se ha disipado.";
        }
        stand            = false;
        estaAtacando     = false;
        faseCombo        = 0;
        frameActual      = 0;
        frameActualStand = 0;
        frameJotaroEsp   = 0;
        frameSPEsp       = 0;
        ralentJotaroEsp  = 0;
        ralentSPEsp      = 0;
        setOffset(0, 0);
        actualizarAuraVisual();
        qDebug() << ">>> ESPECIAL FINALIZADO CORRECTAMENTE <<<";
    }
}

void Jojo::defensa() {
    if (estadoDano == DANO2 || estadoDano == STANDUP || estadoDano == MUERTO) return;
    estaDefendiendo = !estaDefendiendo;
    if (estaDefendiendo) qDebug() << "Jojo se está cubriendo.";
}

// ═══════════════════════════════════════════════════════════════════════════
//  HITBOXES
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::evaluarHitboxBasico() {
    std::pair<float, float> dimBase = std::make_pair(20.0f, 20.0f);
    std::pair<float, float> dim = dimBase * ESCALA;

    float despX = mirandoDerecha ? 65.0f : (-dim.first + 10.0f);

    std::pair<float, float> posPersonaje = std::make_pair(x() + despX, y() + 25.0f);

    QRectF rect(posPersonaje.first, posPersonaje.second, dim.first, dim.second);
    procesarDano(rect, 5);
    danoAplicado = true;

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 2));
        QTimer::singleShot(100, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
    }
}

void Jojo::evaluarHitboxFuerte1() {
    std::pair<float, float> dim = std::make_pair(60.0f, 45.0f) * ESCALA;

    float despX = mirandoDerecha ? 85.0f : (-dim.first - 10.0f);
    std::pair<float, float> posFinal = std::make_pair(x() + despX, y() - 15.0f);

    QRectF rect(posFinal.first, posFinal.second, dim.first, dim.second);
    procesarDano(rect, 2);

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 2));
        QTimer::singleShot(50, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
    }
}

void Jojo::evaluarHitboxFuerte2() {
    if (frameActualStand >= 4 && frameActualStand <= 8) {
        std::pair<float, float> dim = std::make_pair(55.0f, 40.0f) * ESCALA;

        float despX = mirandoDerecha ? 80.0f : (-dim.first - 20.0f);
        std::pair<float, float> posFinal = std::make_pair(x() + despX, y() + 15.0f);

        QRectF rect(posFinal.first, posFinal.second, dim.first, dim.second);
        procesarDano(rect, 5);

        if (Personaje::modoDebug) {
            QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 3));
            QTimer::singleShot(50, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
        }
    }
}

void Jojo::evaluarHitboxEspecial() {
    std::pair<float, float> dim = std::make_pair(80.0f, 90.0f);
    dim.first = dim.first * ESCALA;

    float despX = mirandoDerecha ? 90.0f : (-dim.first + 5.0f);
    std::pair<float, float> posFinal = std::make_pair(x() + despX, y() - 20.0f);

    QRectF rect(posFinal.first, posFinal.second, dim.first, dim.second);
    procesarDano(rect, 2);

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 2));
        QTimer::singleShot(40, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  procesarDano
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::procesarDano(QRectF area, int cantidad) {
    if (animoActual == ENOJADO && proximoAtaquePotenciado) {
        cantidad = static_cast<int>(cantidad * 1.4f);
        qDebug() << "[GOLPE DE FURIA] ¡Ataque potenciado con un 40% más de daño! Cantidad final:" << cantidad;
    }

    QList<QGraphicsItem*> items = scene()->items(area);
    bool golpeoAAlguien = false;

    for (QGraphicsItem* item : items) {
        Jojo* personajeGolpeado = dynamic_cast<Jojo*>(item);
        if (personajeGolpeado && personajeGolpeado != this) {
            golpeoAAlguien = true;
            personajeGolpeado->recibirDano(cantidad, this->x());
            continue;
        }

        DIO* dioGolpeado = dynamic_cast<DIO*>(item);
        if (dioGolpeado) {
            golpeoAAlguien = true;
            dioGolpeado->recibirDano(cantidad, this->x());
        }
    }

    if (golpeoAAlguien && barradeCarga < 100 && faseCombo <= 4) {
        barradeCarga += 5;
        if (barradeCarga > 100) barradeCarga = 100;
        qDebug() << " [HIT!] Barra Especial:" << barradeCarga << "%";

        if (barradeCarga >= 100) {
            actualizarAuraVisual();
        }
    }
}

void Jojo::actualizarAuraVisual() {
    if (Personaje::tiempoDetenido) return;

    if (this->graphicsEffect()) {
        this->setGraphicsEffect(nullptr);
        elAuraEfecto = nullptr;
    }
    if (barradeCarga >= 100) {
        elAuraEfecto = new QGraphicsDropShadowEffect();
        elAuraEfecto->setBlurRadius(55);
        elAuraEfecto->setColor(QColor(148, 0, 211, 255));
        elAuraEfecto->setOffset(0, 0);
        this->setGraphicsEffect(elAuraEfecto);
    }
    if (animoActual == CALCULADOR) {
        elAuraEfecto = new QGraphicsDropShadowEffect();
        elAuraEfecto->setBlurRadius(30);
        elAuraEfecto->setColor(QColor(0, 191, 255, 200));
        elAuraEfecto->setOffset(0, 0);
        this->setGraphicsEffect(elAuraEfecto);
    }
    else if (animoActual == ENOJADO) {
        elAuraEfecto = new QGraphicsDropShadowEffect();
        elAuraEfecto->setBlurRadius(45);
        elAuraEfecto->setColor(QColor(255, 0, 0, 220));
        elAuraEfecto->setOffset(0, 0);
        this->setGraphicsEffect(elAuraEfecto);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  MÉTODOS EXCLUSIVOS DE NIVEL 1 (jotaro.cpp)
//  Todos usan esNivel1 internamente o son exclusivos de top-down
// ═══════════════════════════════════════════════════════════════════════════

// ── Constantes del sprite sheet de tu jotaro ──
static const int ANCHO_FRAME_N1     = 70;
static const int ALTO_FRAME_N1      = 100;
static const int FRAMES_QUIETO_N1   = 1;
static const int FRAMES_MOV_N1      = 3;
static const int FRAMES_ATAQUE_N1   = 2;
static const int X_INICIO_ATAQUE_N1 = 720;
static const int Y_ATAQUE_N1        = 92;
static const int ANCHO_ATAQUE_N1    = 80;
static const int ALTO_ATAQUE_N1     = 81;
static const int MARGEN_FRAME_N1    = 12;
static const int X_INICIO_MOV_N1    = 250;

static const int Y_DIR_N1[4] = {
    94,   // 0 = Arriba
    190,  // 1 = Abajo
    290,  // 2 = Izquierda
    390   // 3 = Derecha
};

void Jojo::cargarFramesNivel1()
{
    QPixmap sprites(":/sprites/SpritesJojoChampionship/sprites_juego.png");


    for (int dir = 0; dir < 3; dir++) {
        for (int f = 0; f < FRAMES_QUIETO_N1; f++) {
            int xPos = X_INICIO_MOV_N1 + f * (ANCHO_FRAME_N1 + MARGEN_FRAME_N1);
            QPixmap frame = sprites.copy(xPos, Y_DIR_N1[dir], ANCHO_FRAME_N1, ALTO_FRAME_N1);
            framesQuieto[dir].append(quitarFondoN1(frame));
        }
        for (int f = 0; f < FRAMES_MOV_N1; f++) {
            int xPos = X_INICIO_MOV_N1 + f * ANCHO_FRAME_N1;
            QPixmap frame = sprites.copy(xPos, Y_DIR_N1[dir], ANCHO_FRAME_N1, ALTO_FRAME_N1);
            framesMovimiento[dir].append(quitarFondoN1(frame));
        }
    }

    for (int f = 0; f < FRAMES_QUIETO_N1; f++)
        framesQuieto[3].append(reflejarHorizontalN1(framesQuieto[2][f]));
    for (int f = 0; f < FRAMES_MOV_N1; f++)
        framesMovimiento[3].append(reflejarHorizontalN1(framesMovimiento[2][f]));

    for (int f = 0; f < FRAMES_ATAQUE_N1; f++) {
        int xPos = X_INICIO_ATAQUE_N1 + f * ANCHO_ATAQUE_N1;
        QPixmap frame = sprites.copy(xPos, Y_ATAQUE_N1,
                                     (f > 0) ? ANCHO_ATAQUE_N1 + 124 : ANCHO_ATAQUE_N1,
                                     ALTO_ATAQUE_N1);
        framesAtaqueDerecha.append(quitarFondoN1(frame));
    }
    for (int f = 0; f < FRAMES_ATAQUE_N1; f++)
        framesAtaqueIzquierda.append(reflejarHorizontalN1(framesAtaqueDerecha[f]));
}

QPixmap Jojo::quitarFondoN1(const QPixmap &original)
{
    QImage img = original.toImage().convertToFormat(QImage::Format_ARGB32);
    QColor fondoColor(30, 27, 60);

    for (int yPos = 0; yPos < img.height(); yPos++) {
        for (int xPos = 0; xPos < img.width(); xPos++) {
            QColor pixel = img.pixelColor(xPos, yPos);
            if (abs(pixel.red()   - fondoColor.red())   < 8 &&
                abs(pixel.green() - fondoColor.green()) < 8 &&
                abs(pixel.blue()  - fondoColor.blue())  < 8)
            {
                img.setPixelColor(xPos, yPos, QColor(0, 0, 0, 0));
            }
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap Jojo::reflejarHorizontalN1(const QPixmap &original)
{
    QTransform espejo;
    espejo.scale(-1, 1);
    return original.transformed(espejo);
}

void Jojo::moveUp() {
    float proximoY = y() - (speed + velocidadBonus);
    if (proximoY < 0) proximoY = 0;
    if (!verificarColision(x(), proximoY)) setPos(x(), proximoY);
}

void Jojo::moveDown() {
    float proximoY = y() + (speed + velocidadBonus);
    if (proximoY > 550 - 100) proximoY = 550 - 100; // 100 = alto del sprite
    if (!verificarColision(x(), proximoY)) setPos(x(), proximoY);
}

void Jojo::moveLeft() {
    float proximoX = x() - (speed + velocidadBonus);
    if (proximoX < 0) proximoX = 0;
    if (!verificarColision(proximoX, y())) setPos(proximoX, y());
}

void Jojo::moveRight() {
    float proximoX = x() + (speed + velocidadBonus);
    if (proximoX > 1000 - 70) proximoX = 1000 - 70;
    if (!verificarColision(proximoX, y())) setPos(proximoX, y());
}

void Jojo::setDireccion(int dir)  { direccion = dir; update(); }

void Jojo::setAtacando(bool status) {
    if (atacando != status) {
        qDebug() << "ATACANDO:" << status;
        prepareGeometryChange();
        atacando = status;
    }
    update();
}

void Jojo::setEnMovimiento(bool status) { enMovimiento = status; update(); }

void Jojo::actualizarFrame(int frameActualIndex) { frameActual = frameActualIndex; update(); }

void Jojo::recibirDanio(int cantidad)
{
    if (invencible) return;
    vida -= cantidad;
    if (vida < 0) vida = 0;
    invencible = true;
    contadorInvencible = 60;
}

void Jojo::actualizarInvulnerabilidad()
{
    if (invencible) {
        contadorInvencible--;
        if (contadorInvencible <= 0) {
            invencible = false;
        }
        update();
    }
    if (ultiActiva) {
        timerUlti--;
        if (timerUlti <= 0) {
            ultiActiva = false;
            timerUlti  = 0;
        }
    }


}

void Jojo::curar(int cantidad) {
    vida += cantidad;
    if (vida > 100) vida = 100;
}

void Jojo::aumentarVelocidad() {
    velocidadBonus = 1;
    timerVelocidad = 180;
}

void Jojo::cargarEnergia(int cantidad) {
    energiaUlti += cantidad;
    if (energiaUlti > 100) energiaUlti = 100;
}

void Jojo::usarUlti() {
    if (energiaUlti >= 100) {
        energiaUlti = 0;
        ultiActiva  = true;
        timerUlti   = 240;
        qDebug() << "¡¡ZA WARUDO!! El tiempo se ha ralentizado.";
    }
}

void Jojo::actualizarEfectosItems() {
    if (timerVelocidad > 0) {
        timerVelocidad--;
        if (timerVelocidad == 0) velocidadBonus = 0;
    }
    if (ultiActiva) {
        timerUlti--;
        if (timerUlti <= 0) ultiActiva = false;
    }
}

QRectF Jojo::getHitbox()
{
    return QRectF(x() + 18, y() + 10, 34, 85);
}

QRectF Jojo::getAttackHitbox()
{
    if (!atacando) return QRectF(0, 0, 0, 0);
    if (direccion == 2) return QRectF(x() - 90, y() + 25, 90, 45);
    return QRectF(x() + 60, y() + 25, 90, 45);
}

// ── boundingRect: nivel1 usa el del jotaro, nivel2 usa el del pixmap ──
QRectF Jojo::boundingRect() const
{
    if (esNivel1) {
        if (atacando && direccion == 2) return QRectF(-110, 0, 180, 100);
        if (atacando)                   return QRectF(0, 0, 160, 100);
        return QRectF(0, 0, 70, 100);
    }
    return QGraphicsPixmapItem::boundingRect();
}

// ── shape: nivel1 usa hitbox corporal ajustada, nivel2 usa la del pixmap ──
QPainterPath Jojo::shape() const
{
    if (esNivel1) {
        QPainterPath path;
        path.addRect(QRectF(18, 10, 34, 85));
        return path;
    }
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

// ── paint: nivel1 dibuja tus sprites top-down, nivel2 no hace nada extra ──
void Jojo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (esNivel1) {
        painter->save();

        if (invencible && (contadorInvencible % 8 > 4)) {
            // parpadeo de invencibilidad: no dibujamos nada este frame
        } else {
            QPixmap spriteADibujar;

            if (atacando) {
                if (framesAtaqueDerecha.isEmpty()) { painter->restore(); return; }
                int idx = frameActual % framesAtaqueDerecha.size();
                spriteADibujar = (direccion == 2)
                                     ? framesAtaqueIzquierda[idx]
                                     : framesAtaqueDerecha[idx];
            } else if (enMovimiento) {
                if (framesMovimiento[direccion].isEmpty()) { painter->restore(); return; }
                int idx = frameActual % framesMovimiento[direccion].size();
                spriteADibujar = framesMovimiento[direccion][idx];
            } else {
                if (framesQuieto[direccion].isEmpty()) { painter->restore(); return; }
                spriteADibujar = framesQuieto[direccion][0];
            }

            if (!spriteADibujar.isNull()) {
                int drawX = (atacando && direccion == 2) ? -100 : 0;
                painter->drawPixmap(drawX, 0, spriteADibujar);
            }
        }

        painter->restore();

        // Hitboxes de debug fuera del save/restore para que no se recorten
        if (mostrarHitboxInterna) {
            painter->setPen(QPen(Qt::red, 2, Qt::DashLine));
            painter->setBrush(QBrush(QColor(255, 0, 0, 50)));
            painter->drawRect(QRectF(18, 10, 34, 85));

            if (atacando) {
                painter->setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
                painter->setBrush(QBrush(QColor(0, 255, 255, 70)));
                if (direccion == 2)
                    painter->drawRect(QRectF(-90, 25, 90, 45));
                else
                    painter->drawRect(QRectF(60, 25, 90, 45));
            }
        }
        return; // no caer al paint del nivel 2
    }

    // Paint del nivel 2: deja que Qt dibuje el pixmap normalmente
    QGraphicsPixmapItem::paint(painter, option, widget);
}