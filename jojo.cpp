#include "jojo.h"

Jojo::Jojo() : Personaje()
{
    setPixmap(QPixmap(":/sprites/SpritesJojoChampionship/Jotaro Kujo sprites.png").scaled(60, 90));
    estaDefendiendo = false;
    tiempoAtaque = 0;
    frameActual = 0;
    contadorAnimacion = 0;
    mirandoDerecha = true;
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
}

// ═══════════════════════════════════════════════════════════════════════════
//  ACTIVAR DAÑO 1 — golpe leve (acumulado < 15)
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::activarDano1() {
    estadoDano = DANO1;
    frameDano = 0;
    ralentizadorDano = 0;
    // Quitamos la línea que calculaba vx aquí
    qDebug() << "[DAÑO1] Golpe leve recibido. Acumulado:" << danioAcumulado;
}

// ═══════════════════════════════════════════════════════════════════════════
//  ACTIVAR DAÑO 2 — golpe fuerte (acumulado >= 15) o muerte
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::activarDano2(bool mitadEmpuje) {
    estadoDano = DANO2;
    frameDano = 0;
    ralentizadorDano = 0;
    estaAtacando = false;
    stand = false;
    faseCombo = 0;
    frameActualStand = 0;

    // Empuje fuerte: horizontal considerable + impulso vertical
    float empujeX = mirandoDerecha ? -6.0f : 6.0f;
    float empujeY = -10.0f;
    if (mitadEmpuje) { empujeX *= 0.5f; empujeY *= 0.5f; }
    vx = empujeX;
    vy = empujeY;
    enSuelo = false;
    qDebug() << "[DAÑO2] Golpe fuerte recibido. Acumulado:" << danioAcumulado;
}

// ═══════════════════════════════════════════════════════════════════════════
//  recibirDano — lógica principal con acumulación
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::recibirDano(int cantidad) {
    if (estadoDano == MUERTO) return;

    // Aplicar reducción por defensa
    bool defendiendo = estaDefendiendo;
    if (defendiendo) cantidad /= 2;

    // Descontar vida
    puntosdevida -= cantidad;
    qDebug() << "¡Jojo recibió" << cantidad << "de daño! Vida:" << puntosdevida;

    // ── MUERTE ────────────────────────────────────────────────────────────
    if (puntosdevida <= 0) {
        puntosdevida = 0;
        estadoDano = MUERTO;
        frameDano = 0;
        estaAtacando = false;
        stand = false;
        vx = 0; vy = 0;
        qDebug() << ">>> JOJO HA SIDO DERROTADO <<<";
        return;
    }

    // ── ACUMULAR DAÑO ─────────────────────────────────────────────────────
    danioAcumulado += cantidad;

    // ── DEFENSA: ignora Daño1, solo activa Daño2 si acumulado >= 15 ──────
    if (defendiendo) {
        if (danioAcumulado >= 20) {
            activarDano2(true); // mitad de empuje
            danioAcumulado = 0;
        }
        // si < 15 mientras defiende: ignorar animación
        return;
    }

    // ── SIN DEFENSA ───────────────────────────────────────────────────────
    if (danioAcumulado >= 20) {
        activarDano2(false);
        danioAcumulado = 0;
    } else {
        // < 15: activar o reiniciar Daño1
        activarDano1();
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
            // Terminó Daño1 → volver a normal, resetear acumulado
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
            // Solo pasar a StandUp cuando esté en el suelo
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
            // Terminó StandUp → volver a normal
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
    //if (!scene() || esDummy) return;

    // ── Si está muerto: mostrar primer frame StandUp y congelar ──────────
    if (estadoDano == MUERTO) {
        if (!spritesSTANDUP.isEmpty()) {
            QPixmap f = spritesSTANDUP.at(0);
            if (!mirandoDerecha) f = f.transformed(QTransform().scale(-1,1));
            setPixmap(f);
        }
        return;
    }

    // ── Física: en animaciones de daño no puede moverse ──────────────────
    bool enAnimDano = (estadoDano == DANO1 || estadoDano == DANO2 || estadoDano == STANDUP);

    if (esDummy) {
        // EL DUMMY: No responde a teclas.
        // Si NO está sufriendo daño, se queda quieto en su sitio.
        if (!enAnimDano) {
            vx = 0;
        }
        // Si está en DANO2, conservará el vx/vy del empuje que le dio activarDano2()
    } else {
        if (enAnimDano) {
            // Solo aplica gravedad y empuje, no input del jugador
            vx = 0; // detener movimiento horizontal en DANO1/STANDUP
            // En DANO2 vx ya viene con el empuje, no lo anulamos aquí
        } else {
            if (estaAtacando || estaDefendiendo) vx = 0;
        }
    }

    // En Daño2 conservamos el vx del empuje hasta que toque el suelo
    if (estadoDano == DANO2) {
        vy += aceleracion_y;
        if (!verificarColision(x(), y() + vy)) setPos(x(), y() + vy);
        else { if (vy > 0) { enSuelo = true; vy = 0; } }
        if (!verificarColision(x() + vx, y())) setPos(x() + vx, y());
        // una vez en suelo, detener empuje horizontal
        if (enSuelo) vx = 0;
    } else {
        vy += aceleracion_y;
        if (!verificarColision(x(), y() + vy)) setPos(x(), y() + vy);
        else { if (vy > 0) { enSuelo = true; vy = 0; } }
        if (!verificarColision(x() + vx, y())) setPos(x() + vx, y());
    }

    // ── Actualizar animación de daño ─────────────────────────────────────
    actualizarAnimDano();

    // ── Render de daño ────────────────────────────────────────────────────
    if (enAnimDano) {
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

    // ── Selección de animación normal ─────────────────────────────────────
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
    if (++ralentizadorJ >= 7) {
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

    if (vx > 0) mirandoDerecha = true;
    else if (vx < 0) mirandoDerecha = false;

    if (stand) {

        // ══ ESPECIAL (faseCombo 5): render con contadores independientes ══
        if (faseCombo == 5) {
            // Jotaro: índices 0..4 de spritesESPECIAL
            QPixmap jojoPix = spritesESPECIAL.at(
                qBound(0, frameJotaroEsp, 4)
                );
            // SP: empieza en índice 6 (el 5 es Jotaro quieto/transición)
            int idxSP = qBound(6, 6 + frameSPEsp, spritesESPECIAL.size() - 1);
            QPixmap spPix = spritesESPECIAL.at(idxSP);

            if (!mirandoDerecha) {
                jojoPix = jojoPix.transformed(QTransform().scale(-1, 1));
                spPix   = spPix.transformed(QTransform().scale(-1, 1));
            }

            QPixmap combinado(jojoPix.width() + spPix.width() + 150,
                              qMax(jojoPix.height(), spPix.height()) + 30);
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
            return;  // ← sale aquí, no toca nada de fases 3/4
        }

        // ══ FUERTES (faseCombo 3 y 4)
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
        QPixmap combinado(jojoPix.width() + spPix.width() + anchoExtra,
                          qMax(jojoPix.height(), spPix.height()) + 30);
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
        vy = -13;
        enSuelo = false;
    }
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
            estaAtacando = false;
            faseCombo = 0;
        }
    }
}

void Jojo::atacarFuerte(int tipo) {
    if (stand || !puedeAtacar || estaAtacando || estaDefendiendo || estadoDano != NORMAL) return;

    estaAtacando = true;
    stand = true;
    puedeAtacar = false;
    danoAplicado = false;
    frameActual = 0;
    frameActualStand = 0;
    danioAcumulado = 0;
    faseCombo = (tipo == 1) ? 3 : 4;

    QTimer::singleShot(2000, [this]() { puedeAtacar = true; });
}

void Jojo::actualizarAtaquesFuertes() {
    if (!stand || (faseCombo != 3 && faseCombo != 4)) return;

    if (++ralentizadorStand >= 7) {
        ralentizadorStand = 0;
        frameActualStand++;
        if (faseCombo == 3) evaluarHitboxFuerte1();
        else if (faseCombo == 4) evaluarHitboxFuerte2();
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

        // Reseteo limpio de todos los contadores del especial
        frameActual      = 0;
        frameActualStand = 0;
        frameJotaroEsp   = 0;
        frameSPEsp       = 0;
        ralentJotaroEsp  = 0;
        ralentSPEsp      = 0;
        ralentizadorStand = 0;
    } else {
        qDebug() << "Yare yare daze. Carga necesaria: 100. Actual:" << barradeCarga;
    }
}

void Jojo::actualizarEspecial() {
    if (!stand || faseCombo != 5) return;

    // ── Jotaro: 5 frames rápidos (cada 6 ticks), luego se congela ────────
    if (frameJotaroEsp < 5) {
        if (++ralentJotaroEsp >= 7) {
            ralentJotaroEsp = 0;
            frameJotaroEsp++;
        }
    }
    // frameJotaroEsp queda en 4 (último frame) y no avanza más → congelado

    // ── SP: 28 frames en ráfaga
    if (frameSPEsp < 28) {
        if (++ralentSPEsp >= 6) {
            ralentSPEsp = 0;

            if (frameSPEsp < 22)
                evaluarHitboxEspecial();
            frameSPEsp++;
        }
    }

    // ── Fin: SP terminó sus 28 frames ────────────────────────────────────
    if (frameSPEsp >= 28) {
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
    float anchoHit = 20, altoHit = 20; // Un poco más ancha para compensar el puño
    // Si mira a la derecha: se genera adelante. Si mira a la izquierda: resta el ancho completo
    float offX = mirandoDerecha ? 45.0f : -anchoHit + 10.0f;

    QRectF rect(x() + offX, y() + 25, anchoHit, altoHit);
    procesarDano(rect, 5);
    danoAplicado = true;

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 2));
        QTimer::singleShot(100, [dr]() {
            if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; }
        });
    }
}

void Jojo::evaluarHitboxFuerte1() {
    float anchoHit = 60, altoHit = 45;
    // Star Platinum aparece más adelante, ajustamos los offsets fijos de la escena
    float offX = mirandoDerecha ? 50.0f : -anchoHit - 10.0f;

    QRectF rect(x() + offX, y() - 15, anchoHit, altoHit);
    procesarDano(rect, 2);

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 2));
        QTimer::singleShot(50, [dr]() {
            if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; }
        });
    }
}

void Jojo::evaluarHitboxFuerte2() {
    if (frameActualStand >= 4 && frameActualStand <= 8) {
        float anchoHit = 55, altoHit = 40;
        float offX = mirandoDerecha ? 55.0f : -anchoHit - 20.0f;

        QRectF rect(x() + offX, y() + 15, anchoHit, altoHit);
        procesarDano(rect, 5);

        if (Personaje::modoDebug) {
            QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 3));
            QTimer::singleShot(50, [dr]() {
                if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; }
            });
        }
    }
}

void Jojo::evaluarHitboxEspecial() {
    float anchoHit = 80; // El Ora Ora Ora necesita una pared de golpes grande
    float offX = mirandoDerecha ? 50.0f : -anchoHit + 5.0f;

    QRectF rect(x() + offX, y() - 20, anchoHit, 90);
    procesarDano(rect, 2);

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 2));
        QTimer::singleShot(40, [dr]() {
            if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; }
        });
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  procesarDano
// ═══════════════════════════════════════════════════════════════════════════
void Jojo::procesarDano(QRectF area, int cantidad) {
    QList<QGraphicsItem*> items = scene()->items(area);
    bool golpeoAAlguien = false;

    for (QGraphicsItem* item : items) {
        Jojo* personajeGolpeado = dynamic_cast<Jojo*>(item);

        if (personajeGolpeado && personajeGolpeado != this) {
            golpeoAAlguien = true;
            // Pasamos 'this->x()' para saber de qué lado vino el golpe
            personajeGolpeado->recibirDanoConOrigen(cantidad, this->x());
        }
    }

    if (golpeoAAlguien) {
        if (barradeCarga < 100) {
            barradeCarga += 5;
            if (barradeCarga > 100) barradeCarga = 100;
            qDebug() << " [HIT!] Barra Especial:" << barradeCarga << "%";
        }
    }
}

void Jojo::recibirDanoConOrigen(int cantidad, float atacanteX) {
    if (estadoDano == MUERTO) return;

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
        qDebug() << ">>> JOJO HA SIDO DERROTADO <<<";
        return;
    }
    danioAcumulado += cantidad;


    float direccionEmpuje = (atacanteX < this->x()) ? 0.5f : -0.5f;

    if (defendiendo) {
        if (danioAcumulado >= 22) {
            this->activarDano2(true);
            vx = direccionEmpuje * 3.0f;
            danioAcumulado = 0;
        }
        return;
    }

    if (danioAcumulado >= 22) {
        this->activarDano2(false);
        vx = direccionEmpuje * 8.0f;
        danioAcumulado = 0;
    } else {
        this->activarDano1();
        vx = direccionEmpuje * 1.0f;
    }
}
