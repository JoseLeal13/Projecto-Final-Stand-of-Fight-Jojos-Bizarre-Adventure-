#include "dio.h"

static const float ESCALA = 1.8f;

DIO::DIO(Personaje* jojo) : Personaje()
{
    objetivo = jojo;
    estaDefendiendo = false;
    tiempoAtaque = 0;
    frameActual = 0;
    contadorAnimacion = 0;
    mirandoDerecha = false; // DIO arranca mirando a la izquierda hacia Jotaro

    std::srand(std::time(nullptr)); // Inicializar semilla aleatoria

    cargarSprites();
    setPos(800, 100); // Posición inicial en el ala derecha de la escena

    if (!spritesQUIETO.isEmpty()) {
        setPixmap(spritesQUIETO.at(0));
    } else {
        setPixmap(QPixmap(10,10));
        qDebug() << "ALERTA: No se cargaron los sprites de DIO.";
    }
}

void DIO::cargarSprites() {
    QImage imagen(":/sprites/SpritesJojoChampionship/Dio Brando The World sprites.png");
    QRgb colorFondo = imagen.pixel(0,0);
    imagen.setAlphaChannel(imagen.createMaskFromColor(colorFondo, Qt::MaskOutColor));
    QPixmap hoja = QPixmap::fromImage(imagen);

    // ── STAND (x6) ───────────────────────────────────────────────────────
    // TL=(7,1) BR=(37,75) → w=30, h=74. Stride=30
    for(int i = 0; i < 6; i++)
        spritesQUIETO.append(hoja.copy(7 + (i * 30), 1, 30, 74));

    // ── WALK (x4) ────────────────────────────────────────────────────────
    // TL=(7,140) BR=(62,204) → w=55, h=64. Stride=55
    for(int i = 0; i < 4; i++)
        spritesCAMINAR.append(hoja.copy(7 + (i * 55), 140, 55, 64));

    // ── JUMP (x3 + x2 + x3 = x8) ─────────────────────────────────────────
    // Grupo1: TL=(7,222) BR=(59,296) → w=52, h=74. x3, stride=52
    for(int i = 0; i < 3; i++)
        spritesSALTO.append(hoja.copy(7 + (i * 52), 222, 52, 74));
    // Grupo2: TL=(152,205) BR=(187,280) → w=35, h=75. x2, stride=35
    for(int i = 0; i < 2; i++)
        spritesSALTO.append(hoja.copy(152 + (i * 35), 205, 35, 75));
    // Grupo3: TL=(230,222) BR=(282,296) → w=52, h=74. x3, stride=52
    for(int i = 0; i < 3; i++)
        spritesSALTO.append(hoja.copy(230 + (i * 52), 222, 52, 74));

    // ── DEFENSA/GUARD (x6) ───────────────────────────────────────────────
    // TL=(61,596) BR=(112,667) → w=51, h=71. Stride=51
    for(int i = 0; i < 6; i++)
        spritesDEFENSA.append(hoja.copy(61 + (i * 51), 596, 51, 71));

    // ── BASICO 1 (x1 + x2 + x1 + x1 = x5... verificar) ──────────────────
    // Frame0: TL=(16,370) BR=(47,438) → w=31, h=68
    spritesBASICO1.append(hoja.copy(16, 370, 31, 68));
    // Frames 1-2: TL=(42,370) BR=(97,438) → w=55, h=68. x2, stride=55
    for(int i = 0; i < 2; i++)
        spritesBASICO1.append(hoja.copy(42 + (i * 55), 370, 55, 68));
    // Frame3: TL=(144,370) BR=(179,438) → w=35, h=68
    spritesBASICO1.append(hoja.copy(144, 370, 35, 68));
    // Frame4: TL=(179,370) → w=? Asumiendo stride similar ~35
    spritesBASICO1.append(hoja.copy(179, 370, 35, 68));

    // ── BASICO 2 (x5 + x4 = x9) ──────────────────────────────────────────
    // Grupo1: TL=(15,442) BR=(63,511) → w=48, h=69. x5, stride=48
    for(int i = 0; i < 5; i++)
        spritesBASICO2.append(hoja.copy(15 + (i * 48), 442, 48, 69));
    // Grupo2: TL=(303,442) BR=(346,511) → w=43, h=69. x4, stride=43
    for(int i = 0; i < 4; i++)
        spritesBASICO2.append(hoja.copy(303 + (i * 43), 442, 43, 69));

    // ── FUERTE 1 ─────────────────────────────────────────────────────────
    // DIO pose (x2): TL=(132,1225) BR=(169,1291) → w=37, h=66. stride=37
    for(int i = 0; i < 2; i++)
        spritesFUERTE1.append(hoja.copy(132 + (i * 37), 1225, 37, 66));
    // DIO pose (x2 más): TL=(207,1225) BR=(253,1291) → w=46, h=66. stride=46
    for(int i = 0; i < 2; i++)
        spritesFUERTE1.append(hoja.copy(207 + (i * 46), 1225, 46, 66));

    // The World Fuerte1 (x5): TL=(203,1666) BR=(267,1733) → w=64, h=67. stride=64
    for(int i = 0; i < 5; i++)
        spritesFUERTE1.append(hoja.copy(203 + (i * 64), 1666, 64, 67));
    // The World último frame: TL=(552,1666) BR=(586,1733) → w=34, h=67
    spritesFUERTE1.append(hoja.copy(552, 1666, 34, 67));

    // ── FUERTE 2 ─────────────────────────────────────────────────────────
    // DIO pose (x5): TL=(173,2697) BR=(230,2762) → w=57, h=65. stride=57
    for(int i = 0; i < 5; i++)
        spritesFUERTE2.append(hoja.copy(173 + (i * 57), 2697, 57, 65));
    // The World grupo1 (x4): TL=(172,2775) BR=(222,2843) → w=50, h=68. stride=50
    for(int i = 0; i < 4; i++)
        spritesFUERTE2.append(hoja.copy(172 + (i * 50), 2775, 50, 68));
    // The World grupo2 (x2): TL=(369,2778) BR=(442,2843) → w=73, h=65. stride=73
    for(int i = 0; i < 2; i++)
        spritesFUERTE2.append(hoja.copy(369 + (i * 73), 2778, 73, 65));
    // The World grupo3 (x2): TL=(168,2852) BR=(242,2920) → w=74, h=68. stride=74
    for(int i = 0; i < 2; i++)
        spritesFUERTE2.append(hoja.copy(168 + (i * 74), 2852, 74, 68));
    // The World último (x2): TL=(242,2352)... // VERIFICAR - coord parece incorrecta en lectura
    // Usando por ahora: TL=(242,2920) BR=(296,2920+68) → w=54, h=68
    for(int i = 0; i < 2; i++)
        spritesFUERTE2.append(hoja.copy(242 + (i * 54), 2852, 54, 68));

    // ── ESPECIAL ─────────────────────────────────────────────────────────
    // DIO pose (x5): TL=(193,2537) BR=(241,2604) → w=48, h=67. stride=48
    for(int i = 0; i < 5; i++)
        spritesESPECIAL.append(hoja.copy(193 + (i * 48), 2537, 48, 67));
    // DIO última pose: repetir frame 4 o agregar 1 más
    spritesESPECIAL.append(hoja.copy(193 + (5 * 48), 2537, 48, 67));

    // The World grupo1 (x1): TL=(174,2291) BR=(226,2362) → w=52, h=71
    spritesESPECIAL.append(hoja.copy(174, 2291, 52, 71));
    // The World grupo2 (x3): TL=(229,2268) BR=(311,2362) → w=82, h=94. stride=82
    for(int i = 0; i < 3; i++)
        spritesESPECIAL.append(hoja.copy(229 + (i * 82), 2268, 82, 94));
    // The World grupo3 (x4): TL=(133,2366) BR=(279,2440) → w=146, h=74. stride=146
    for(int i = 0; i < 4; i++)
        spritesESPECIAL.append(hoja.copy(133 + (i * 146), 2366, 146, 74));
    // The World grupo4 (x2): TL=(172,2454) BR=(267,2527) → w=95, h=73. stride=95
    for(int i = 0; i < 2; i++)
        spritesESPECIAL.append(hoja.copy(172 + (i * 95), 2454, 95, 73));

    // ── DAÑO 1 (x5) ──────────────────────────────────────────────────────
    // TL=(23,3327) BR=(63,3397) → w=40, h=70. stride=40
    for(int i = 0; i < 5; i++)
        spritesDANO1.append(hoja.copy(23 + (i * 40), 3327, 40, 70));

    // ── DAÑO 2 (x4 + x3 = x7) ────────────────────────────────────────────
    // Grupo1 (x4): TL=(22,3442) BR=(71,3466+h) → w=49
    // Imagen3: TL=(22,3342) BR=(71,3466) → h=124 no tiene sentido
    // Más probable: y=3342, h=70 (consistente con Daño1)
    // VERIFICAR estas coordenadas con tu hoja
    for(int i = 0; i < 4; i++)
        spritesDANO2.append(hoja.copy(22 + (i * 49), 3342, 49, 70));
    // Grupo2 (x3): TL=(296,3342) BR=(349,3412) → w=53, stride=53
    for(int i = 0; i < 3; i++)
        spritesDANO2.append(hoja.copy(296 + (i * 53), 3342, 53, 70));

    // ── STANDUP (x1 + x2 + ... = x5?) ────────────────────────────────────
    // Frame0: TL=(25,3439) BR=(93,3546) → w=68, h=107
    spritesSTANDUP.append(hoja.copy(25, 3439, 68, 107));
    // Frames 1-2: TL=(93,3439) BR=(141,3546) → w=48, h=107. stride=48
    for(int i = 0; i < 2; i++)
        spritesSTANDUP.append(hoja.copy(93 + (i * 48), 3439, 48, 107));
    // Frames 3-4: TL=(93,3490) BR=(141,3546) → w=48, h=56. stride=48
    // VERIFICAR: imagen dice x4 para un subgrupo
    for(int i = 0; i < 2; i++)
        spritesSTANDUP.append(hoja.copy(93 + (i * 48), 3490, 48, 56));

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

// ══ Cerebro de DIO
void DIO::ejecutarCerebro(float dx, float distancia) {
    // Si está sufriendo daño o muerto, no piensa
    bool enAnimDano = (estadoDano == DANO1 || estadoDano == DANO2 || estadoDano == STANDUP);
    if (enAnimDano || estadoDano == MUERTO) return;

    // Orientación automática permanente para mirar a Jotaro cara a cara
    mirandoDerecha = (dx > 0);

    // ── REGLA: CONDICIÓN A MITAD DE VIDA (Lógica en blanco) ──────────────────
    // Suponiendo una vida máxima de 100 (ajusta el valor según tu inicialización)
    if (puntosdevida <= 50) {
        // [Espacio reservado: Coloca aquí tu lógica futura para cuando DIO esté en apuros]
    }

    // ── REGLA: SALTO HACIA ATRÁS POST-ESPECIAL ───────────────────────────────
    // Se ejecuta inmediatamente en el primer frame libre tras terminar "Za Warudo"
    if (postEspecialJump) {
        postEspecialJump = false;
        if (estaDefendiendo) defensa();
        vx = (dx > 0) ? -7.5f : 7.5f; // Impulso fuerte en dirección contraria a Jotaro
        if (enSuelo) saltar();
        ticksEspera = 20; // Breve pausa táctica al aterrizar del escape
        return;
    }

    // ── CONTROL DEL TEMPORIZADOR DE ESPERA (Pausas y meditación) ─────────────
    if (ticksEspera > 0) {
        ticksEspera--;
        vx = 0;
        return;
    }

    // ── REGLA: CARGA ESPECIAL TOTALMENTE LLENA (Prioridad Absoluta) ──────────
    if (barradeCarga >= 100) {
        if (distancia <= 50) { // Rango ideal de golpeo del Especial
            if (estaDefendiendo) defensa();
            habilidadEspecial();
            postEspecialJump = true; // Dejamos el flag listo para cuando el tiempo vuelva a fluir
            return;
        } else {
            // Persecución activa: Avanza hacia Jotaro con chance de saltar
            if (estaDefendiendo) defensa();

            // Pequeña probabilidad (15%) de avanzar saltando de forma agresiva
            if (enSuelo && (std::rand() % 100 < 15)) {
                vx = (dx > 0) ? 6.0f : -6.0f;
                saltar();
            } else {
                vx = (dx > 0) ? 5.0f : -5.0f; // Camina rápido
            }
            return;
        }
    }

    // ── LÓGICA DE COMPORTAMIENTO POR DISTANCIAS Y PROBABILIDADES ──────────────

    // RANGO 1: DISTANCIA CERCANA (Menor o igual a 100)
    if (distancia <= 50) {
        yaHizoPausaLejos = false; // Resetea el seguro de pausa lejana al acercarse
        if (estaDefendiendo) defensa();
        vx = 0; // Se detiene en seco para ejecutar el ataque

        int dadoCercano = std::rand() % 100;
        if (dadoCercano < 80) {
            atacar();        // 80% Probabilidad: Combo Básico (¡Muda!)
        } else {
            atacarFuerte(2); // 20% Probabilidad: Ataque Fuerte 2 con The World
        }
        return;
    }

    // RANGO 2: DISTANCIA MEDIA (Entre 100 y 220) - Zona de Ataques Fuertes
    else if (distancia > 50 && distancia <= 100) {
        yaHizoPausaLejos = false; // Resetea el seguro de pausa lejana
        if (estaDefendiendo) defensa();

        int dadoMedio = std::rand() % 100;
        if (dadoMedio < 80) {
            atacarFuerte(1); // 80% Probabilidad: Ataque Fuerte 1
        } else {
            atacarFuerte(2); // 20% Probabilidad: Ataque Fuerte 2
        }
        // Nota: Al terminar el ataque, el bucle de "moverse()" dejará de bloquear este cerebro,
        // haciendo que vuelva a evaluar la distancia y forzándolo a perseguir/actuar en rango cercano.
        return;
    }

    // RANGO 3: DISTANCIA LEJANA (Mayor a 220) - Estado de Acecho y Decisión 50/50
    else {
        // Primera entrada a distancia lejana: Hace la pose dramática de 2 segundos
        if (!yaHizoPausaLejos) {
            // 24 ticks * 5 (frecuencia en moverse()) = 120 frames totales (~2 segundos a 60fps)
            ticksEspera = 24;
            yaHizoPausaLejos = true;
            vx = 0;
            return;
        }

        // Tras cumplir los 2 segundos quietos, toma la decisión de aproximación 50/50
        if (estaDefendiendo) defensa();
        int dadoLejos = std::rand() % 100;

        if (dadoLejos < 50) {
            // 50% Opción A: "Lanzarse" velozmente con un salto largo hacia Jotaro
            vx = (dx > 0) ? 6.5f : -6.5f;
            if (enSuelo) saltar();
        } else {
            // 50% Opción B: Caminar firmemente hacia Jotaro con probabilidad menor de salto
            if (enSuelo && (std::rand() % 100 < 10)) { // 10% probabilidad de saltar en la caminata
                vx = (dx > 0) ? 4.5f : -4.5f;
                saltar();
            } else {
                vx = (dx > 0) ? 4.5f : -4.5f;
            }
        }
    }
}

void DIO::moverse() {
    if (!scene()) return;

    if (estadoDano == MUERTO) {
        if (!spritesSTANDUP.isEmpty()) {
            QPixmap f = spritesSTANDUP.at(0);
            if (!mirandoDerecha) f = f.transformed(QTransform().scale(-1,1));
            setPixmap(f);
        }
        return;
    }

    // 1. Lectura del entorno geométrico (Distancias a Jotaro)
    float dx = objetivo->x() - x();
    float dy = objetivo->y() - y();
    float distancia = std::sqrt(dx*dx + dy*dy);

    bool enAnimDano = (estadoDano == DANO1 || estadoDano == DANO2 || estadoDano == STANDUP);

    // 2. Ejecutar lógica del Agente Inteligente cada 5 ticks de reloj (Ventana de decisión para fluidez)
    if (++ticksDecision >= 5) {
        ticksDecision = 0;
        if (!enAnimDano && !estaAtacando && !stand) {
            ejecutarCerebro(dx, distancia);
        }
    }

    // 3. Restricciones físicas de movimiento según estados
    if (!enAnimDano) {
        if ((estaAtacando || estaDefendiendo) && estadoDano == NORMAL) {
            vx = 0; // Solo se detiene si ataca o defiende en estado normal
        }
    }

    // 4. Integrador Cinemático (Gravedad y colisiones)
    if (estadoDano == DANO2) {
        vy += aceleracion_y;
        if (!verificarColision(x(), y() + vy)) setPos(x(), y() + vy);
        else { if (vy > 0) { enSuelo = true; vy = 0; } }
        if (!verificarColision(x() + vx, y())) setPos(x() + vx, y());
        if (enSuelo) vx = 0;
    } else {
        vy += aceleracion_y;
        if (!verificarColision(x(), y() + vy)) setPos(x(), y() + vy);
        else { if (vy > 0) { enSuelo = true; vy = 0; } }

        // Detección de paredes/obstáculos para salto automático
        if (!verificarColision(x() + vx, y())) {
            setPos(x() + vx, y());
        } else {
            if (enSuelo && !enAnimDano && !estaAtacando) {
                vy = -13;
                enSuelo = false;
            }
        }
    }

    // 5. Ciclo de sub-rutinas mecánicas
    actualizarAnimDano();
    actualizarAtaque();
    actualizarAtaquesFuertes();
    actualizarEspecial();

    // 6. Renderizador de estados de daño
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

    // 7. Renderizador y composición gráfica de ataques con "The World"
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
        int limiteDio;
        if (faseCombo == 3) limiteDio = 2;
        else if (faseCombo == 5) limiteDio = 4;
        else limiteDio = 3;

        if (stand) {
            if (faseCombo != 5 && frameActual < limiteDio) frameActual++;
        } else {
            frameActual++;
        }
    }

    if (frameActual >= animacionActual->size() && !stand)
        frameActual = bucle ? 0 : animacionActual->size() - 1;

    // Renderizado final (Canvas dinámico unificado)
    if (stand) {
        if (faseCombo == 5) { // Composición avanzada de Habilidad Especial (The World)
            QPixmap dioPix = spritesESPECIAL.at(qBound(0, frameDioEsp, 4));
            int idxSP = qBound(6, 6 + frameTWEsp, 15);
            QPixmap twPix = spritesESPECIAL.at(idxSP);

            if (!mirandoDerecha) {
                dioPix = dioPix.transformed(QTransform().scale(-1, 1));
                twPix  = twPix.transformed(QTransform().scale(-1, 1));
            }

            QPixmap combinado(dioPix.width() + twPix.width() + 150, qMax(dioPix.height(), twPix.height()) + 30);
            combinado.fill(Qt::transparent);
            QPainter painter(&combinado);

            int dioX = mirandoDerecha ? 0 : twPix.width() + 10;
            int dioY = combinado.height() - dioPix.height() - 5;
            painter.drawPixmap(dioX, dioY, dioPix);

            int twX = mirandoDerecha ? dioPix.width() : 0;
            int twY = combinado.height() - twPix.height() - 5;
            painter.drawPixmap(twX, twY, twPix);
            painter.end();

            setPixmap(combinado);
            setOffset(mirandoDerecha ? 0 : -(twPix.width() + 10), -dioY);
            return;
        }

        // Ataques fuertes 1 y 2
        int limiteDio, offsetSP;
        if (faseCombo == 3)      { limiteDio = 2; offsetSP = 6; }
        else if (faseCombo == 4) { limiteDio = 3; offsetSP = 4; }
        else                     { limiteDio = 3; offsetSP = 0; }

        QPixmap dioPix = animacionActual->at(qMin(frameActual, limiteDio));
        int idxSP = offsetSP + frameActualStand;
        if (idxSP >= animacionActual->size()) idxSP = animacionActual->size() - 1;
        QPixmap twPix = animacionActual->at(idxSP);

        if (!mirandoDerecha) {
            dioPix = dioPix.transformed(QTransform().scale(-1, 1));
            twPix  = twPix.transformed(QTransform().scale(-1, 1));
        }

        int anchoExtra = (faseCombo == 5) ? 150 : 40;
        QPixmap combinado(dioPix.width() + twPix.width() + anchoExtra, qMax(dioPix.height(), twPix.height()) + 30);
        combinado.fill(Qt::transparent);
        QPainter painter(&combinado);

        int dioX = mirandoDerecha ? 0 : twPix.width() + 10;
        int dioY = combinado.height() - dioPix.height() - 5;
        painter.drawPixmap(dioX, dioY, dioPix);

        int twX = mirandoDerecha ? dioPix.width() : 0;
        int twY = combinado.height() - twPix.height() - 5;
        painter.drawPixmap(twX, twY, twPix);
        painter.end();

        setPixmap(combinado);
        setOffset(mirandoDerecha ? 0 : -(twPix.width() + 10), -dioY);
        return;
    }

    int idx = qBound(0, frameActual, animacionActual->size() - 1);
    QPixmap frameFinal = animacionActual->at(idx);
    if (!mirandoDerecha) frameFinal = frameFinal.transformed(QTransform().scale(-1,1));
    setPixmap(frameFinal);
    setOffset(0, 0);
}

void DIO::saltar() {
    if (enSuelo && !estaDefendiendo && estadoDano == NORMAL) {
        vy = -13;
        enSuelo = false;
    }
}

void DIO::atacar() {
    if (puedeAtacar && !estaAtacando && !estaDefendiendo && !stand && estadoDano == NORMAL) {
        estaAtacando = true;
        puedeAtacar = false;
        danoAplicado = false;
        frameActual = 0;
        faseCombo = 1;
        danioAcumulado = 0;

        qDebug() << "DIO: MUDA!";
        QTimer::singleShot(1000, [this]() { puedeAtacar = true; });
    }
}

void DIO::actualizarAtaque() {
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

void DIO::atacarFuerte(int tipo) {
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

void DIO::actualizarAtaquesFuertes() {
    if (!stand || (faseCombo != 3 && faseCombo != 4)) return;

    if (++ralentizadorStand >= 7) {
        ralentizadorStand = 0;
        frameActualStand++;
        if (faseCombo == 3) evaluarHitboxFuerte1();
        else if (faseCombo == 4) evaluarHitboxFuerte2();
    }

    int limiteDio = (faseCombo == 3) ? 2 : 3;
    if (frameActual < limiteDio) {
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
    }
}

void DIO::habilidadEspecial() {
    if (stand || estadoDano != NORMAL) return;
    if (barradeCarga >= 100) {
        qDebug() << "¡ZA WARUDO: TOKI WO TOMARE!";
        estaAtacando   = true;
        stand          = true;
        faseCombo      = 5;
        danoAplicado   = false;
        barradeCarga   = 0;
        danioAcumulado = 0;
        puedeAtacar    = true;

        frameActual = 0;
        frameActualStand = 0;
        frameDioEsp = 0;
        frameTWEsp = 0;
        ralentDioEsp = 0;
        ralentTWEsp = 0;
        ralentizadorStand = 0;

        QTimer::singleShot(5000, [this]() {
            puedeAtacar = true;
        });
    }
}

void DIO::actualizarEspecial() {
    if (!stand || faseCombo != 5) return;

    // DIO: 5 frames de pose
    if (frameDioEsp < 5) {
        if (++ralentDioEsp >= 7) {
            ralentDioEsp = 0;
            frameDioEsp++;
        }
    }

    // The World: 10 frames en ráfaga
    if (frameTWEsp < 10) {
        if (++ralentTWEsp >= 6) {
            ralentTWEsp = 0;
            evaluarHitboxEspecial();  // hitbox en todos los frames de TW
            frameTWEsp++;
        }
    }

    if (frameTWEsp >= 10) {
        stand = false; estaAtacando = false; faseCombo = 0;
        frameActual = 0; frameActualStand = 0;
        frameDioEsp = 0; frameTWEsp = 0;
        ralentDioEsp = 0; ralentTWEsp = 0;
        setOffset(0, 0);
        qDebug() << ">>> EL TIEMPO VUELVE A FLUIR <<<";
    }
}

void DIO::defensa() {
    if (estadoDano == DANO2 || estadoDano == STANDUP || estadoDano == MUERTO) return;
    estaDefendiendo = !estaDefendiendo;
}

void DIO::evaluarHitboxBasico() {
    float anchoHit = 20 * ESCALA, altoHit = 20 * ESCALA;
    float offX = mirandoDerecha ? 45.0f : -anchoHit + 10.0f;
    QRectF rect(x() + offX, y() + 25, anchoHit, altoHit);
    procesarDano(rect, 5);
    danoAplicado = true;

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::yellow, 2));
        QTimer::singleShot(100, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
    }
}

void DIO::evaluarHitboxFuerte1() {
    float anchoHit = 60 * ESCALA, altoHit = 45 * ESCALA;
    float offX = mirandoDerecha ? 50.0f : -anchoHit - 10.0f;
    QRectF rect(x() + offX, y() - 15, anchoHit, altoHit);
    procesarDano(rect, 2);

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::yellow, 2));
        QTimer::singleShot(50, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
    }
}

void DIO::evaluarHitboxFuerte2() {
    if (frameActualStand >= 4 && frameActualStand <= 8) {
        float anchoHit = 55 * ESCALA, altoHit = 40 * ESCALA;
        float offX = mirandoDerecha ? 55.0f : -anchoHit - 20.0f;
        QRectF rect(x() + offX, y() + 15, anchoHit, altoHit);
        procesarDano(rect, 5);

        if (Personaje::modoDebug) {
            QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::yellow, 3));
            QTimer::singleShot(50, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
        }
    }
}

void DIO::evaluarHitboxEspecial() {
    float anchoHit = 80 * ESCALA;
    float offX = mirandoDerecha ? 50.0f : -anchoHit + 5.0f;
    QRectF rect(x() + offX, y() - 20, anchoHit, 90);
    procesarDano(rect, 2);

    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::yellow, 2));
        QTimer::singleShot(40, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
    }
}

void DIO::procesarDano(QRectF area, int cantidad) {
    QList<QGraphicsItem*> items = scene()->items(area);
    bool golpeoAAlguien = false;

    for (QGraphicsItem* item : items) {
        // La hitbox de DIO busca impactar específicamente a la instancia de Jotaro
        Jojo* personajeGolpeado = dynamic_cast<Jojo*>(item);

        if (personajeGolpeado) {
            golpeoAAlguien = true;
            personajeGolpeado->recibirDanoConOrigen(cantidad, this->x());
        }
    }

    if (golpeoAAlguien && barradeCarga < 100) {
        barradeCarga += 5;
        if (barradeCarga > 100) barradeCarga = 100;
    }
}

void DIO::recibirDano(int cantidad) {
    if (estadoDano == MUERTO) return;

    // ── EVALUAR HYPER ARMOR ANTES DE RESETEAR ATAQUES ──
    if (stand && (faseCombo == 3 || faseCombo == 4 || faseCombo == 5)) {
        qDebug() << "[HYPER ARMOR] ¡DIO resiste el impacto y continúa con The World!";
    } else {
        // Solo si no está en un ataque pesado se cancelan sus acciones normales
        estaAtacando = false;
        stand = false;
        faseCombo = 0;
        frameActual = 0;
        frameActualStand = 0;
        setOffset(0, 0);
    }

    if (estaDefendiendo) cantidad /= 2;

    puntosdevida -= cantidad;
    qDebug() << "¡DIO recibió" << cantidad << "de daño! Vida restante:" << puntosdevida;

    if (puntosdevida <= 0) {
        puntosdevida = 0;
        estadoDano = MUERTO;
        frameDano = 0;
        estaAtacando = false;
        stand = false;
        vx = 0; vy = 0;
        qDebug() << ">>> DIO HA SIDO ELIMINADO <<<";
        return;
    }
    danioAcumulado += cantidad;

    float direccionEmpuje = (objetivo->x() < this->x()) ? 0.5f : -0.5f;

    // Si tiene la armadura activa, el daño se procesa pero no cambia a animación de dolor
    if (stand && (faseCombo == 3 || faseCombo == 4 || faseCombo == 5)) {
        estadoDano = NORMAL;
        vx = direccionEmpuje * 2.0f; // Pequeño empuje por el golpe sin romper el ataque
        return;
    }

    if (estaDefendiendo) {
        if (danioAcumulado >= 22) {
            this->activarDano2(true);
            vx = direccionEmpuje * 8.0f;
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

void DIO::recibirDanoConOrigen(int cantidad, float atacanteX) {
    if (estadoDano == MUERTO) return;

    bool defendiendo = estaDefendiendo;
    if (defendiendo) cantidad /= 2;

    puntosdevida -= cantidad;
    qDebug() << "¡DIO recibió" << cantidad << "de daño! Vida restante:" << puntosdevida;

    if (puntosdevida <= 0) {
        puntosdevida = 0;
        estadoDano = MUERTO;
        frameDano = 0;
        estaAtacando = false;
        stand = false;
        vx = 0; vy = 0;
        qDebug() << ">>> DIO HA SIDO DERROTADO <<<";
        return;
    }
    danioAcumulado += cantidad;

    float direccionEmpuje = (atacanteX < this->x()) ? 0.5f : -0.5f;

    // ── FILTRO HYPER ARMOR CON ORIGEN ──
    if (stand && (faseCombo == 3 || faseCombo == 4 || faseCombo == 5)) {
        estadoDano = NORMAL;
        vx = direccionEmpuje * 2.0f;
        return;
    }

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
    } else {
        this->activarDano1();
        vx = direccionEmpuje * 1.0f;
    }
}

void DIO::activarDano1() {
    estadoDano = DANO1;
    frameDano = 0;
    ralentizadorDano = 0;
}

void DIO::activarDano2(bool mitadEmpuje) {
    estadoDano = DANO2;
    frameDano = 0;
    ralentizadorDano = 0;

    // SI ESTÁ USANDO SU STAND EN FUERTES O ESPECIAL, CONSERVA LA ANIMACIÓN
    if (stand && (faseCombo == 3 || faseCombo == 4 || faseCombo == 5)) {
        qDebug() << "[HYPER ARMOR DANO2] ¡The World se mantiene firme!";
    } else {
        estaAtacando = false;
        stand = false;
        faseCombo = 0;
        frameActualStand = 0;
    }

    float empujeY = -10.0f;
    if (mitadEmpuje) { empujeY *= 0.5f; }
    vy = empujeY;
    enSuelo = false;
}

void DIO::actualizarAnimDano() {
    if (estadoDano == NORMAL || estadoDano == MUERTO) return;
    if (++ralentizadorDano < 8) return;
    ralentizadorDano = 0;

    switch (estadoDano) {
    case DANO1:
        frameDano++;
        if (frameDano >= spritesDANO1.size()) {
            estadoDano = NORMAL;
            danioAcumulado = 0;
            vx = 0;
        }
        break;
    case DANO2:
        frameDano++;
        if (frameDano >= spritesDANO2.size()) {
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
    }
}
