#include "jojo.h"

Jojo::Jojo() : Personaje()
{

    setPixmap(QPixmap(":/sprites/SpritesJojoChampionship/Jotaro Kujo sprites.png").scaled(60, 90));
    estaDefendiendo = false;
    tiempoAtaque = 0;

    // Inicializar variables de animación
    frameActual = 0;
    contadorAnimacion = 0;
    mirandoDerecha = true;

    cargarSprites();

    if (!spritesQUIETO.isEmpty()) {
        setPixmap(spritesQUIETO.at(0));
    } else {
        // Si fallan las coordenadas, le ponemos una imagen genérica para que no crashee
        setPixmap(QPixmap(10,10));
        qDebug() << "ALERTA: No se cargaron los sprites. Revisa las coordenadas en cargarSprites()";
    }
}

void Jojo::cargarSprites() {
    QImage imagen(":/sprites/SpritesJojoChampionship/Jotaro Kujo sprites.png");
    QRgb colorFondo = imagen.pixel(0,0);
    imagen.setAlphaChannel(imagen.createMaskFromColor(colorFondo, Qt::MaskOutColor));
    QPixmap hoja = QPixmap::fromImage(imagen);

    // --- STAND (7,29 hasta 7,94 de alto | 17,19 hasta 17,94 de largo) ---
    // Interpretando tus puntos: X_inicio=7, Y_inicio=29. Ancho=47, Alto=65.
    for(int i = 0; i < 6; i++) {
        spritesQUIETO.append(hoja.copy(7 + (i * 41), 29, 42, 67));
    }

    // --- WALK (Caminar) ---
    // Mantengo el inicio en 313 que mediste antes para que no se desfase
    for(int i = 0; i < 8; i++) {
        spritesCAMINAR.append(hoja.copy(313 + (i * 48), 29, 47, 67));
    }

    // --- GUARD (165,126 hasta 165,198 de alto | 205,126 hasta 205,198 de largo) ---
    // X=165, Y=126. Ancho=40 (205-165), Alto=72 (198-126)
    for(int i = 0; i < 2; i++) {
        spritesDEFENSA.append(hoja.copy(165 + (i * 40), 126, 39, 70));
    }

    // --- JUMP (417,126 hasta 417,199 | 468,126 hasta 468,199) ---
    // X=417, Y=126. Ancho=51 (468-417), Alto=73 (199-126)
    for(int i = 0; i < 8; i++) {
        spritesSALTO.append(hoja.copy(415 + (i * 48), 126, 47, 72));
    }

    // --- ATAQUE COMBO ---
    // Golpe 1 (4 sprites: 43x64 aprox)
    for(int i = 0; i < 4; i++) {
        spritesBASICO1.append(hoja.copy(7 + (i * 50), 707, 50, 70));
    }

    // Golpe 2 - Parte A (2 sprites: 37x64 aprox)
    for(int i = 0; i < 2; i++) {
        spritesBASICO2.append(hoja.copy(262 + (i * 37), 707, 37, 70));
    }

    // Golpe 2 - Parte B (3 sprites: 53x64 aprox)
    for(int i = 0; i < 3; i++) {
        spritesBASICO2.append(hoja.copy(336 + (i * 60), 707, 60, 67));
    }

    //FUERTE 1
    // Jotaro (6 sprites) — Y corregido de 1083→1093, H de 72→61
    for(int i=0; i<6; i++)
        spritesFUERTE1.append(hoja.copy(5 + (i*35), 1083, 38, 72));

    // Star Platinum (9 sprites totales: 1 inicial + 8 de golpes)
    spritesFUERTE1.append(hoja.copy(260, 1045, 55, 110)); // El sprite de transición

    for(int i=0; i<8; i++)
        spritesFUERTE1.append(hoja.copy(312 + (i*90), 1045, 95, 110));

    // --- FUERTE 2 (S + J) ---
    // Jotaro (4 sprites)

    for(int i=0; i<4; i++)
        spritesFUERTE2.append(hoja.copy(5 + (i*35), 1572, 36, 68));

    // Star Platinum (10 sprites)
    for(int i=0; i<10; i++)
        spritesFUERTE2.append(hoja.copy(163 + (i*50), 1572, 56, 68));

}

void Jojo::moverse() {
    if (!scene()) return;

    // 1. FÍSICA BÁSICA (Tu código está bien aquí)
    if (estaAtacando || estaDefendiendo) vx = 0;
    vy += aceleracion_y;

    if (!verificarColision(x(), y() + vy)) setPos(x(), y() + vy);
    else { if (vy > 0) enSuelo = true; vy = 0; }

    if (!verificarColision(x() + vx, y())) setPos(x() + vx, y());

    // 2. SELECCIÓN DE LISTA
    QList<QPixmap> *animacionActual = nullptr;
    bool bucle = true;

    if (stand) {
        animacionActual = (faseCombo == 3) ? &spritesFUERTE1 : &spritesFUERTE2;
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

    // 3. CONTROL DE FRAMES DE JOTARO
    static int ralentizadorJ = 0;
    if (++ralentizadorJ >= 8) {
        ralentizadorJ = 0;
        int limiteJojo = (faseCombo == 3) ? 2 : 3;
        if (stand) {
            if (frameActual < limiteJojo) frameActual++;
        } else {
            frameActual++;
        }
    }

    if (frameActual >= animacionActual->size() && !stand) {
        frameActual = bucle ? 0 : animacionActual->size() - 1;
    }

    // 4. ORIENTACIÓN
    if (vx > 0) mirandoDerecha = true;
    else if (vx < 0) mirandoDerecha = false;

    // 5. RENDERIZADO (CORREGIDO PARA EL SUELO)
    if (stand) {
        int limiteJojo = (faseCombo == 3) ? 2 : 3;
        int offsetSP = (faseCombo == 3) ? 3 : 4;

        QPixmap jojoPix = animacionActual->at(qMin(frameActual, limiteJojo));
        int idxSP = qBound(0, offsetSP + frameActualStand, animacionActual->size() - 1);
        QPixmap spPix = animacionActual->at(idxSP);

        if (!mirandoDerecha) {
            jojoPix = jojoPix.transformed(QTransform().scale(-1, 1));
            spPix = spPix.transformed(QTransform().scale(-1, 1));
        }

        // Lienzo extra grande para que nada se corte
        int anchoTotal = jojoPix.width() + spPix.width() + 100;
        int altoTotal = jojoPix.height() + spPix.height() + 100;

        QPixmap combinado(anchoTotal, altoTotal);
        combinado.fill(Qt::transparent);
        QPainter painter(&combinado);

        // JOTARO: Siempre se dibuja en el mismo punto del lienzo (su "base")
        int jojoY = altoTotal - jojoPix.height() - 20;
        int jojoX = anchoTotal / 2 - jojoPix.width() / 2;
        painter.drawPixmap(jojoX, jojoY, jojoPix);

        // STAR PLATINUM: Posición relativa a Jotaro
        if (faseCombo == 3) { // W+J (Sale arriba)
            painter.drawPixmap(jojoX + (mirandoDerecha ? 10 : -30), jojoY - spPix.height() + 20, spPix);
        } else { // S+J (Sale al lado)
            painter.drawPixmap(jojoX + (mirandoDerecha ? 30 : -spPix.width()), jojoY - 10, spPix);
        }
        painter.end();

        setPixmap(combinado);
        // OFFSET: Esto es lo que evita que se hunda.
        // Alineamos el centro inferior del Pixmap con la posición real (x,y)
        setOffset(- (anchoTotal / 2 - jojoPix.width() / 2), - (altoTotal - jojoPix.height() - 20));
        return;
    }
    else {
        // RENDER NORMAL
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
        vy = -13; // Impulso hacia arriba
        enSuelo = false;
    }
}

void Jojo::atacar() {
    // Solo inicia si puede atacar, no está defendiendo y no está ya atacando
    if (puedeAtacar && !estaAtacando && !estaDefendiendo) {
        estaAtacando = true;
        puedeAtacar = false; // Bloqueamos el uso del botón
        danoAplicado = false;
        frameActual = 0;
        faseCombo = 1;

        qDebug() << "ORA! Iniciando combo...";

        // --- EL DELAY DE 2 SEGUNDOS ---
        // Después de 2000ms, puedeAtacar volverá a ser true
        QTimer::singleShot(1000, [this]() {
            puedeAtacar = true;
            qDebug() << "Ataque listo de nuevo.";
        });
    }
}

void Jojo::actualizarAtaque() {
    if (!estaAtacando) return;

    QList<QPixmap>* anim = (faseCombo == 1) ? &spritesBASICO1 : &spritesBASICO2;

    // --- GENERACIÓN DE LA HITBOX ---
    // Ajustamos el frame 2 o 3 según veas que el brazo está estirado
    if (frameActual == 2 && !danoAplicado) {

        // Si mira a la derecha (+), la hitbox sale adelante. Si mira a la izquierda (-), atrás.
        float anchoHitbox = 50;
        float altoHitbox = 40;
        float posXHitbox = mirandoDerecha ? (x() + 45) : (x() - anchoHitbox + 10);
        float posYHitbox = y() + 15; // Ajustar a la altura del hombro/pecho

        QRectF rectGolpe(posXHitbox, posYHitbox, anchoHitbox, altoHitbox);

        // Chequeo de colisión de ataque
        QList<QGraphicsItem*> chocados = scene()->items(rectGolpe);
        for (QGraphicsItem* item : chocados) {
            // Suponiendo que tus enemigos tienen un Type específico (ej: UserType + 2)
            if (item->type() == UserType + 2) {
                qDebug() << "¡Impacto confirmado en fase" << faseCombo << "!";
                danoAplicado = true;
                // item->recibirDano(15);
            }
        }
    }

    // --- TRANSICIÓN AUTOMÁTICA DEL COMBO ---
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
    stand = true; // Activamos a Star Platinum
    puedeAtacar = false;
    danoAplicado = false;
    frameActual = 0; // Jotaro inicia su pose
    frameActualStand = 0; // Star Platinum inicia desde el primer frame
    faseCombo = (tipo == 1) ? 3 : 4; // 3 para W+J, 4 para S+J

    // Delay de 2 segundos para volver a usar ataques fuertes
    QTimer::singleShot(3000, [this]() {
        puedeAtacar = true;
    });
}

void Jojo::actualizarAtaquesFuertes() {
    if (!stand) return;

    // DETERMINAR CUÁNDO TERMINA JOTARO
    int limiteJojo = (faseCombo == 3) ? 2 : 3;

    // SI JOTARO NO HA TERMINADO, NO HACEMOS NADA AQUÍ
    // (Jotaro se mueve en la función moverse() mediante frameActual)
    if (frameActual < limiteJojo) {
        return;
    }

    // SI LLEGAMOS AQUÍ, JOTARO YA TERMINÓ. AHORA ACTÚA STAR PLATINUM.
    QList<QPixmap>* animCompleta = (faseCombo == 3) ? &spritesFUERTE1 : &spritesFUERTE2;
    int totalSpritesSP = (faseCombo == 3) ? 9 : 10;

    // 1. HITBOXES (Igual que antes)
    if (faseCombo == 3) {
        float offX = mirandoDerecha ? 50 : -80;
        QRectF rectSP(x() + offX, y() - 30, 80, 70);
        procesarDano(rectSP, 5);
    } else if (faseCombo == 4) {
        if (frameActualStand == 7 && !danoAplicado) {
            float offX = mirandoDerecha ? 60 : -110;
            QRectF rectSP(x() + offX, y() + 10, 110, 50);
            procesarDano(rectSP, 30);
            danoAplicado = true;
        }
    }

    // 2. AVANCE DE STAR PLATINUM
    static int ralentizadorStand = 0;
    ralentizadorStand++;
    if (ralentizadorStand >= 7) {
        ralentizadorStand = 0;
        frameActualStand++; // Esto es lo que hace que SP se mueva
    }

    // 3. FINALIZACIÓN TOTAL
    if (frameActualStand >= totalSpritesSP) {
        stand = false;
        estaAtacando = false;
        faseCombo = 0;
        frameActual = 0;
        frameActualStand = 0;
        setOffset(0,0); // REINICIAR OFFSET
        qDebug() << "Stand desactivado, volviendo a normal";
    }
}

void Jojo::habilidadEspecial() {
    if (barradeCarga >= 100) {
        qDebug() << "¡STAR PLATINUM: Za Warudo!";

        barradeCarga = 0;
    } else {
        qDebug() << "Yare yare daze. Carga necesaria: 100. Actual:" << barradeCarga;
    }
}

void Jojo::defensa()
{
    estaDefendiendo = !estaDefendiendo;
    if (estaDefendiendo) {
        qDebug() << "Jojo se está cubriendo.";
        // Reducir daño recibido en la lógica de colisión futura
    }
}

void Jojo::procesarDano(QRectF area, int cantidad) {
    QList<QGraphicsItem*> items = scene()->items(area);
    for (QGraphicsItem* item : items) {
        if (item->type() == UserType + 2) { // Asumiendo que DIO/Enemigos son Type + 2
            qDebug() << "¡ORA! Daño causado:" << cantidad;
            // Llamaria un metodo tipo cuando cree a DIO: static_cast<Enemigo*>(item)->recibirDano(cantidad);
        }
    }
}
