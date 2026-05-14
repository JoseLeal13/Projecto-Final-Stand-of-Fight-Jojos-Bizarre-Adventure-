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
        qDebug() << "ALERTA: No se cargaron los sprites. Revisa las coordenadas en cargarSprites()";
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
        spritesESPECIAL.append(hoja.copy(7 + (i*36), 2300, 36, 68));
    spritesESPECIAL.append(hoja.copy(200, 2300, 35, 68));
    for(int i=0; i<12; i++)
        spritesESPECIAL.append(hoja.copy(235 + (i*90), 2270, 92, 98));
    for(int i=0; i<2; i++)
        spritesESPECIAL.append(hoja.copy(1312 + (i*75), 2260, 75, 103));
    for(int i=0; i<8; i++)
        spritesESPECIAL.append(hoja.copy(4 + (i*78), 2381, 78, 115));
    for(int i=0; i<3; i++)
        spritesESPECIAL.append(hoja.copy(745 + (i*46), 2424, 46, 72));
}

void Jojo::moverse() {
    if (!scene()) return;
    if (!scene() || esDummy) return;

    if (estaAtacando || estaDefendiendo) vx = 0;
    vy += aceleracion_y;

    if (!verificarColision(x(), y() + vy)) setPos(x(), y() + vy);
    else { if (vy > 0) enSuelo = true; vy = 0; }

    if (!verificarColision(x() + vx, y())) setPos(x() + vx, y());

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
            if (frameActual < limiteJojo) frameActual++;
        } else {
            frameActual++;
        }
    }

    if (frameActual >= animacionActual->size() && !stand) {
        frameActual = bucle ? 0 : animacionActual->size() - 1;
    }

    if (vx > 0) mirandoDerecha = true;
    else if (vx < 0) mirandoDerecha = false;

    if (stand) {
        int limiteJojo;
        int offsetSP;

        if (faseCombo == 3) {
            limiteJojo = 2;
            offsetSP = 6;
        } else if (faseCombo == 4) {
            limiteJojo = 3;
            offsetSP = 4;
        } else if (faseCombo == 5) {
            limiteJojo = 4;
            offsetSP = 6;
        } else {
            limiteJojo = 3;
            offsetSP = 0;
        }

        QPixmap jojoPix = animacionActual->at(qMin(frameActual, limiteJojo));

        int idxSP = offsetSP + (faseCombo == 5 ? qMax(0, frameActualStand - 6) : frameActualStand);
        if (idxSP >= animacionActual->size()) idxSP = animacionActual->size() - 1;

        QPixmap spPix = animacionActual->at(idxSP);

        if (!mirandoDerecha) {
            jojoPix = jojoPix.transformed(QTransform().scale(-1, 1));
            spPix = spPix.transformed(QTransform().scale(-1, 1));
        }

        // AJUSTES MÍNIMOS PARA QUE NO SE CORTE:
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
    else {
        int idx = qBound(0, frameActual, animacionActual->size() - 1);
        QPixmap frameFinal = animacionActual->at(idx);
        if (!mirandoDerecha) frameFinal = frameFinal.transformed(QTransform().scale(-1, 1));
        setPixmap(frameFinal);
        setOffset(0, 0);
    }
}

void Jojo::saltar()
{
    if (enSuelo && !estaDefendiendo) {
        vy = -13;
        enSuelo = false;
    }
}

void Jojo::atacar() {
    if (puedeAtacar && !estaAtacando && !estaDefendiendo) {
        estaAtacando = true;
        puedeAtacar = false;
        danoAplicado = false;
        frameActual = 0;
        faseCombo = 1;
        qDebug() << "ORA! Iniciando combo...";
        QTimer::singleShot(1000, [this]() {
            puedeAtacar = true;
            qDebug() << "Ataque listo de nuevo.";
        });
    }
}

// CAMBIO: reemplaza el bloque if(frameActual==2) por evaluarHitboxBasico()
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
    if (!puedeAtacar || estaAtacando || estaDefendiendo) return;

    estaAtacando = true;
    stand = true;
    puedeAtacar = false;
    danoAplicado = false;
    frameActual = 0;
    frameActualStand = 0;
    faseCombo = (tipo == 1) ? 3 : 4;

    QTimer::singleShot(2000, [this]() {
        puedeAtacar = true;
    });
}

void Jojo::actualizarAtaquesFuertes() {
    if (!stand || faseCombo == 5) return;

    static int ralentizadorStand = 0;
    if (++ralentizadorStand >= 7) {
        ralentizadorStand = 0;
        frameActualStand++;
        qDebug() << "Fase:" << faseCombo << "Frame SP (ACTUALIZADO):" << frameActualStand;

        // Hitbox solo al cambiar de frame
        if (faseCombo == 3) evaluarHitboxFuerte1();
        else if (faseCombo == 4) evaluarHitboxFuerte2();
    }

    int limiteJojo = (faseCombo == 3) ? 2 : 3;
    if (frameActual < limiteJojo) {
        static int ralentizadorJ = 0;
        if (++ralentizadorJ >= 7) {
            ralentizadorJ = 0;
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
        danoAplicado = false;
        setOffset(0, 0);
        qDebug() << "--- Ataque fuerte finalizado ---";
    }
}

void Jojo::habilidadEspecial() {
    if (barradeCarga >= 100) {
        qDebug() << "¡STAR PLATINUM: Za Warudo!";
        estaAtacando = true;
        stand = true;
        faseCombo = 5;
        frameActual = 0;
        frameActualStand = 0;
        danoAplicado = false;
        barradeCarga = 0;
    } else {
        qDebug() << "Yare yare daze. Carga necesaria: 100. Actual:" << barradeCarga;
    }

    QTimer::singleShot(5000, [this]() {
        puedeAtacar = true;
    });
}

void Jojo::actualizarEspecial() {
    if (faseCombo != 5 || !stand) return;

    static int ralentizadorEspecial = 0;
    if (++ralentizadorEspecial >= 6) {
        ralentizadorEspecial = 0;
        frameActualStand++;

        if (frameActualStand >= 6 && frameActualStand <= 30) {
            evaluarHitboxEspecial();
        }
    }

    if (frameActual < 4) {
        static int ralentizadorJ = 0;
        if (++ralentizadorJ >= 7) {
            ralentizadorJ = 0;
            frameActual++;
        }
    }

    if (frameActualStand >= 38) {
        stand = false;
        estaAtacando = false;
        faseCombo = 0;
        frameActual = 0;
        frameActualStand = 0;
        setOffset(0,0);
        qDebug() << "Especial finalizado correctamente";
    }
}

void Jojo::defensa()
{
    estaDefendiendo = !estaDefendiendo;
    if (estaDefendiendo) {
        qDebug() << "Jojo se está cubriendo.";
    }
}

void Jojo::evaluarHitboxBasico() {
    float anchoHit = 20, altoHit = 20;
    float offX = mirandoDerecha ? (x() + 40) : (x() - anchoHit + 5);
    QRectF rect(offX, y() + 25, anchoHit, altoHit);
    procesarDano(rect, 5);
    danoAplicado = true;

    if (Personaje::modoDebug) {
        // Color Azul para básico
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 2));
        QTimer::singleShot(100, [dr]() {
            if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; }
        });
    }
}

void Jojo::evaluarHitboxFuerte1() {
    float offX = mirandoDerecha ? 70.0f : -70.0f - 45.0f;
    QRectF rect(x() + offX, y() - 15, 45, 35);

    procesarDano(rect, 2);

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 2));
        QTimer::singleShot(50, [dr]() {
            if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; }
        });
    }
}

void Jojo::evaluarHitboxFuerte2() {
    // Star Platinum tiene el brazo extendido entre estos frames
    if (frameActualStand >= 4 && frameActualStand <= 8) {

        float anchoHit = 55;
        float altoHit = 40;
        float offX = mirandoDerecha ? 60.0f : -60.0f - anchoHit;
        QRectF rect(x() + offX, y() + 20, anchoHit, altoHit);

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

    if (frameActualStand >= 6 && frameActualStand <= 28) {
        float anchoHit = 80;
        float offX = mirandoDerecha ? 80.f : -80.f - anchoHit;
        QRectF rect(x() + offX, y() - 20, anchoHit, 90);

        procesarDano(rect, 2);

        if (Personaje::modoDebug) {
            QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::blue, 2));
            QTimer::singleShot(40, [dr]() {
                if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; }
            });
        }
    }
}

void Jojo::procesarDano(QRectF area, int cantidad) {
    QList<QGraphicsItem*> items = scene()->items(area);
    bool golpeoAAlguien = false;

    for (QGraphicsItem* item : items) {
        if (item->type() == UserType + 2 && item != this) {
            golpeoAAlguien = true;
            static_cast<Jojo*>(item)->recibirDano(cantidad);
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

void Jojo::recibirDano(int cantidad) {
    if (estaDefendiendo) cantidad /= 2;
    qDebug() << "¡Jojo recibió" << cantidad << "de daño!";
}

void Jojo::actualizarDano(){

}
