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
    estaCurando = false;
    comboTimeStopPaso = 0;

    ticksJugadorLejos = 0;
    contadorGolpesRecibidos = 0;
    modoHiperagresivoActivo = false;
    contraataqueActivo = false;
    ticksContraataque = 0;
    estadoCuracion = CUR_IDLE;
    ticksDefensaCuracion = 0;

    std::srand(std::time(nullptr)); // Inicializar semilla aleatoria

    cargarSprites();

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
    for(int i = 0; i < 6; i++)
        spritesQUIETO.append(hoja.copy(7 + (i * 30), 1, 30, 74));

    // ── WALK (x4) ────────────────────────────────────────────────────────
    for(int i = 0; i < 4; i++)
        spritesCAMINAR.append(hoja.copy(7 + (i * 55), 140, 55, 64));

    // ── JUMP (x3 + x2 + x3 = x8) ─────────────────────────────────────────
    for(int i = 0; i < 3; i++)
        spritesSALTO.append(hoja.copy(7 + (i * 52), 222, 52, 74));
    for(int i = 0; i < 2; i++)
        spritesSALTO.append(hoja.copy(152 + (i * 35), 205, 35, 75));
    for(int i = 0; i < 3; i++)
        spritesSALTO.append(hoja.copy(230 + (i * 52), 222, 52, 74));

    // ── DEFENSA/GUARD (x6) ───────────────────────────────────────────────
    for(int i = 0; i < 6; i++)
        spritesDEFENSA.append(hoja.copy(61 + (i * 51), 596, 51, 71));

    // ── BASICO 1 (x1 + x2 + x1 + x1 = x5... verificar) ──────────────────
    spritesBASICO1.append(hoja.copy(16, 370, 31, 68));
    for(int i = 0; i < 2; i++)
        spritesBASICO1.append(hoja.copy(42 + (i * 55), 370, 55, 68));
    spritesBASICO1.append(hoja.copy(144, 370, 35, 68));
    spritesBASICO1.append(hoja.copy(179, 370, 35, 68));

    // ── BASICO 2 (x5 + x4 = x9) ──────────────────────────────────────────
    for(int i = 0; i < 5; i++)
        spritesBASICO2.append(hoja.copy(15 + (i * 48), 442, 48, 69));
    for(int i = 0; i < 4; i++)
        spritesBASICO2.append(hoja.copy(303 + (i * 43), 442, 43, 69));

    // ── FUERTE 1 ─────────────────────────────────────────────────────────
    for(int i = 0; i < 2; i++)
        spritesFUERTE1.append(hoja.copy(132 + (i * 37), 1225, 37, 66));
    for(int i = 0; i < 2; i++)
        spritesFUERTE1.append(hoja.copy(207 + (i * 46), 1225, 46, 66));
    for(int i = 0; i < 5; i++)
        spritesFUERTE1.append(hoja.copy(203 + (i * 64), 1666, 64, 67));
    spritesFUERTE1.append(hoja.copy(552, 1666, 34, 67));

    // ── FUERTE 2 ─────────────────────────────────────────────────────────
    for(int i = 0; i < 5; i++)
        spritesFUERTE2.append(hoja.copy(173 + (i * 57), 2697, 57, 65));
    for(int i = 0; i < 4; i++)
        spritesFUERTE2.append(hoja.copy(172 + (i * 50), 2775, 50, 68));
    for(int i = 0; i < 2; i++)
        spritesFUERTE2.append(hoja.copy(369 + (i * 73), 2778, 73, 65));
    for(int i = 0; i < 2; i++)
        spritesFUERTE2.append(hoja.copy(168 + (i * 74), 2852, 74, 68));
    for(int i = 0; i < 2; i++)
        spritesFUERTE2.append(hoja.copy(242 + (i * 54), 2852, 54, 68));

    // ── ESPECIAL ─────────────────────────────────────────────────────────
    for(int i = 0; i < 5; i++)
        spritesESPECIAL.append(hoja.copy(193 + (i * 48), 2537, 48, 67));
    spritesESPECIAL.append(hoja.copy(193 + (5 * 48), 2537, 48, 67));
    spritesESPECIAL.append(hoja.copy(174, 2291, 52, 71));
    for(int i = 0; i < 3; i++)
        spritesESPECIAL.append(hoja.copy(229 + (i * 82), 2268, 82, 94));
    for(int i = 0; i < 4; i++)
        spritesESPECIAL.append(hoja.copy(133 + (i * 146), 2366, 146, 74));
    for(int i = 0; i < 2; i++)
        spritesESPECIAL.append(hoja.copy(172 + (i * 95), 2454, 95, 73));

    // ── DAÑO 1 (x5) ──────────────────────────────────────────────────────
    for(int i = 0; i < 5; i++)
        spritesDANO1.append(hoja.copy(23 + (i * 40), 3327, 40, 70));

    // ── DAÑO 2 (x4 + x3 = x7) ────────────────────────────────────────────
    for(int i = 0; i < 4; i++)
        spritesDANO2.append(hoja.copy(22 + (i * 49), 3342, 49, 70));
    for(int i = 0; i < 3; i++)
        spritesDANO2.append(hoja.copy(296 + (i * 53), 3342, 53, 70));

    // ── STANDUP (x1 + x2 + ... = x5?) ────────────────────────────────────
    spritesSTANDUP.append(hoja.copy(25, 3439, 68, 107));
    for(int i = 0; i < 2; i++)
        spritesSTANDUP.append(hoja.copy(93 + (i * 48), 3439, 48, 107));
    for(int i = 0; i < 2; i++)
        spritesSTANDUP.append(hoja.copy(93 + (i * 48), 3490, 48, 56));
    for(int i = 0; i < 2; i++)
        spritesCURACION.append(hoja.copy(26 + (i * 35), 3037, 36, 69));
    for(int i = 0; i < 9; i++) {
        int anchoFrame = (i == 8) ? 39 : 44;
        spritesCURACION.append(hoja.copy(100 + (i * 44), 3038, anchoFrame, 68));
    }
    for(int i = 0; i < 2; i++)
        spritesCURACION.append(hoja.copy(27 + (i * 35), 3119, 35, 69));
    for(int i = 0; i < 2; i++)
        spritesCURACION.append(hoja.copy(27 + (i * 35), 3119, 35, 69));

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
    escalarLista(spritesCURACION);
    escalarLista(spritesTIMESTOP);
}

// ══ Cerebro de DIO
void DIO::ejecutarCerebro(float dx, float distancia) {
    // ── NUEVA INYECCIÓN DE SEGURIDAD PARA TIMESTOP ──
    if (Personaje::tiempoDetenido) {
        estaDefendiendo = false;
        contraataqueActivo = false;
        return;
    }

    bool enAnimDano = (estadoDano == DANO1 || estadoDano == DANO2 || estadoDano == STANDUP);
    if (enAnimDano || estadoDano == MUERTO) return;
    mirandoDerecha = (dx > 0);

    // ── CAPA 1: CONTROL DE CURACIÓN ACTIVA ──
    if (estaCurando) {
        if (distancia < 120) {
            estaCurando = false;
        } else {
            ejecutarCuracion();
            return;
        }
    }

    if (!enSuelo && (vx > 5.0f || vx < -5.0f)) return;

    // Contador de huida
    if (distancia > 150) {
        ticksJugadorLejos++;
        if (ticksJugadorLejos > 200 && !modoHiperagresivoActivo) {
            modoHiperagresivoActivo = true;
            qDebug() << "[APRENDIZAJE] Jotaro lleva mucho tiempo huyendo. DIO entra en modo hiperagresivo.";
        }
    } else {
        ticksJugadorLejos = 0;
        modoHiperagresivoActivo = false;
    }

    // Sistema de Contraataque
    if (contraataqueActivo) {
        if (ticksContraataque > 0) {
            ticksContraataque--;
            estaDefendiendo = true;
            vx = 0;
            return;
        } else {
            contraataqueActivo = false;
            estaDefendiendo = false;
            contadorGolpesRecibidos = 0;
            actualizarAuraVisual();
            qDebug() << "[CONTRAATAQUE] DIO rompe el escudo y contraataca.";
            atacarFuerte(1);
            QTimer::singleShot(1800, [this]() {
                if (estadoDano == NORMAL && !stand && !Personaje::tiempoDetenido)
                    atacarFuerte(2);
            });
            return;
        }
    }
    if (ticksDefensaRestantes > 0) {
        vx = 0;
        return;
    }
    // ── CAPA 2: CONDICIÓN CRÍTICA ──
    if (puntosdevida <= 150 && distancia <= 110 && !estaDefendiendo && estadoCuracion == CUR_IDLE && cooldownDefensaCritica == 0) {
        estaDefendiendo = true;
        ticksDefensaRestantes = 120; // 2 segundos exactos corriendo en moverse()
        vx = 0;
        qDebug() << "[CEREBRO] DIO decide escudarse.";
        return;
    }
    // ── CAPA 3: PROCESAMIENTO DE LAS FASES DE CURACIÓN ──
    if (estadoCuracion != CUR_IDLE) {
        if (estadoCuracion == CUR_DEFENSA) {
            if (ticksDefensaCuracion > 0) {
                ticksDefensaCuracion--;
                estaDefendiendo = true;
                vx = 0;
                return;
            } else {
                estadoCuracion = CUR_SALTANDO;
                estaDefendiendo = false;
                actualizarAuraVisual();
                float direccionEscape = (dx > 0) ? -14.0f : 14.0f;
                if (x() < 250 && direccionEscape < 0) direccionEscape = 14.0f;
                if (x() > 950 && direccionEscape > 0) direccionEscape = -14.0f;
                vx = direccionEscape;
                if (enSuelo) saltar();
                qDebug() << "[CURACIÓN] DIO salta para escapar.";
                return;
            }
        }
        if (estadoCuracion == CUR_SALTANDO) {
            if (enSuelo) {
                estadoCuracion = CUR_ATERRIZANDO;
                vx = 0;
                qDebug() << "[CURACIÓN] DIO aterrizó. Decidiendo acción...";
            }
            return;
        }
        if (estadoCuracion == CUR_ATERRIZANDO) {
            estadoCuracion = CUR_TIMESTOP;
            qDebug() << "[CURACIÓN] DIO aterrizó e inicia canalización de TimeStop táctico.";

            preparandoTimeStop = true;
            ticksPreTimeStop = 120;
            stand = true;
            estaAtacando = true;
            faseCombo = 5;
            danioAcumulado = 0;
            frameDioEsp = 0; frameTWEsp = 0;
            ralentDioEsp = 0; ralentTWEsp = 0;
            return;
        }
        if (estadoCuracion == CUR_TIMESTOP) {
            if (!Personaje::tiempoDetenido) {
                estadoCuracion = CUR_IDLE;
                actualizarAuraVisual();
            }
            return;
        }
    }

    // Capa 4: Post-Especial
    if (postEspecialJump) {
        postEspecialJump = false;
        if (estaDefendiendo) estaDefendiendo = false;
        float escapeX = (dx > 0) ? -12.0f : 12.0f;
        if (x() < 250) escapeX = 12.0f;
        if (x() > 950) escapeX = -12.0f;
        vx = escapeX;
        if (enSuelo) saltar();
        ticksEspera = 24;
        return;
    }

    if (puntosdevida < 150 && ticksEspera > 0 && distancia > 100 && enSuelo) {
        ejecutarCuracion();
        return;
    }
    if (ticksEspera > 0) {
        ticksEspera--;
        vx = 0;
        return;
    }

    // ── CAPA 5: DECISIÓN DE ZA WARUDO / AMAGO (CON BARRA 100) ──
    if (barradeCarga >= 100) {
        int chanceTimeStop = std::rand() % 100;
        // 35% de probabilidad de activar el proceso de TimeStop desde lejos o neutral
        if (chanceTimeStop < 35) {
            preparandoTimeStop = true;
            stand = true;
            estaAtacando = true;
            faseCombo = 5;
            danioAcumulado = 0;
            frameDioEsp = 0; frameTWEsp = 0;
            ralentDioEsp = 0; ralentTWEsp = 0;

            // 50% de probabilidad de que sea un AMAGO CORTO
            if (std::rand() % 100 < 50) {
                ticksPreTimeStop = 60; // 1 segundo de amago
                amagoEspecialCorto = true;
                qDebug() << "ZA WARUDO!";
            } else {
                ticksPreTimeStop = 120; // 2 segundos (TimeStop real)
                amagoEspecialCorto = false;
                qDebug() << "DIO inicia canalización REAL de ZA WARUDO... ¡2 segundos!";
            }
            return;
        }
    }

    // ── CAPA 6: COMPORTAMIENTO ESTÁNDAR POR DISTANCIAS ──
    float velocidadPersecucion = modoHiperagresivoActivo ? 9.0f : 6.5f;

    if (distancia <= 50) {
        yaHizoPausaLejos = false;
        vx = 0;
        int dadoCercano = std::rand() % 100;

        // Si tiene la barra llena, añadimos un 40% de probabilidad de soltar la ráfaga de golpes en su cara
        if (barradeCarga >= 100 && dadoCercano < 40) {
            habilidadEspecial();
        }
        else {
            int umbralFuerte = modoHiperagresivoActivo ? 85 : 75;
            int dadoAtaque = std::rand() % 100;
            if (dadoAtaque < umbralFuerte) atacar();
            else atacarFuerte(2);
        }
        return;
    }
    else if (distancia > 50 && distancia <= 100) {
        yaHizoPausaLejos = false;
        int dadoMedio = std::rand() % 100;

        // A media distancia, si tiene barra, un 30% de probabilidad de activar el especial directo
        if (barradeCarga >= 100 && dadoMedio < 30) {
            habilidadEspecial();
        }
        else {
            int dadoAtaque = std::rand() % 100;
            if (dadoAtaque < 60) atacarFuerte(1);
            else atacarFuerte(2);
        }
        return;
    }
    else {
        if (!yaHizoPausaLejos) {
            ticksEspera = modoHiperagresivoActivo ? 5 : 15;
            yaHizoPausaLejos = true;
            vx = 0;
            return;
        }

        int dadoLejos = std::rand() % 100;
        if (dadoLejos < 50) {
            if (cooldownCuracionTicks == 0 && estadoCuracion == CUR_IDLE) {
                int chanceCurarseLejos = std::rand() % 100;
                if (chanceCurarseLejos < 8) {
                    ejecutarCuracion();
                    return;
                }
            }
            vx = (dx > 0) ? velocidadPersecucion : -velocidadPersecucion;
            if (enSuelo && modoHiperagresivoActivo) saltar();
        } else {
            vx = (dx > 0) ? 4.5f : -4.5f;
        }
    }
}

void DIO::moverse() {
    if (!scene()) return;
    if (cooldownCuracionTicks > 0 && !Personaje::tiempoDetenido) {
        cooldownCuracionTicks--;
    }
    if (cooldownDefensaCritica > 0 && !Personaje::tiempoDetenido) {
        cooldownDefensaCritica--;
    }
    // ── Control frame a frame de la duración del escudo ──
    if (ticksDefensaRestantes > 0 && !Personaje::tiempoDetenido) {
        ticksDefensaRestantes--;
        if (ticksDefensaRestantes == 0) {
            estaDefendiendo = false; // Quita el escudo
            cooldownDefensaCritica = 300; // 5 segundos de cooldown para que no lo repita al instante
            qDebug() << "[CEREBRO] El escudo de 2 segundos de DIO expiró. Regresa a la normalidad.";
        }
    }
    if (estadoDano == MUERTO) {
        if (!spritesSTANDUP.isEmpty()) {
            QPixmap f = spritesSTANDUP.at(0);
            if (!mirandoDerecha) f = f.transformed(QTransform().scale(-1,1));
            setPixmap(f);
        }
        return;
    }
    float dx = objetivo->x() - x();
    float dy = objetivo->y() - y();
    float distancia = std::sqrt(dx*dx + dy*dy);
    bool enAnimDano = (estadoDano == DANO1 || estadoDano == DANO2 || estadoDano == STANDUP);

    // ── CORRECCIÓN TIEMPO DETENIDO ──
    // Si el tiempo está detenido por DIO, él NO DEBE defenderse bajo ninguna circunstancia.
    if (Personaje::tiempoDetenido && Personaje::electorDelTiempo == this) {
        estaDefendiendo = false;
        contraataqueActivo = false; // Cancela bloqueos accidentales

        mirandoDerecha = (dx > 0);

        // Secuenciador del combo dinámico
        if (comboTimeStopPaso == 1) {
            if (distancia > 65) {
                vx = (dx > 0) ? 10.0f : -10.0f;
            } else {
                vx = 0;
                comboTimeStopPaso = 2;
            }
        }
        if (comboTimeStopPaso == 2) {
            vx = 0;
            if (!stand && !estaAtacando && !estaCurando) {
                int decisionAtaqueTS = std::rand() % 100;

                if (decisionAtaqueTS < 30) {
                    qDebug() << "MUDA MUDA MUDA MUDA MDUA MUDA.";
                    estaAtacando = true;
                    stand = true;
                    faseCombo = 5;
                    frameActual = 0;
                    frameDioEsp = 0;
                    frameTWEsp = 0;
                    ralentDioEsp = 0;
                    ralentTWEsp = 0;
                }
                else if (decisionAtaqueTS < 60) atacar();
                else if (decisionAtaqueTS < 85) atacarFuerte(1);
                else atacarFuerte(2);
            }
        }
    }

    // ── INTEGRADOR DE DECISIONES OPTIMIZADO Y REACTIVO ──
    if (!Personaje::tiempoDetenido) {
        // SEGURIDAD: Si el rival está pegando o muy cerca, podemos saltarnos los 5 ticks de espera
        // para defendernos instantáneamente (aquí puedes añadir lógica si Jotaro.estaAtacando)

        if (++ticksDecision >= 5) {
            ticksDecision = 0;
            if (!enAnimDano && !estaAtacando && !stand && !estaCurando) {
                ejecutarCerebro(dx, distancia);
            }
        }
    } else if (estaCurando) {
        ejecutarCuracion();
    }

    // Restricciones físicas de movimiento según estados
    if (!enAnimDano) {
        bool bloquearMovimiento = (estaAtacando || estaDefendiendo || estaCurando) && !Personaje::tiempoDetenido;
        if (bloquearMovimiento || (estaCurando && Personaje::tiempoDetenido)) {
            vx = 0;
        }
    }

    // Cinemática (Físicas de movimiento aplicadas al mapa)
    vy += aceleracion_y;
    if (!verificarColision(x(), y() + vy)) {
        setPos(x(), y() + vy);
    } else if (vy > 0) {
        enSuelo = true;
        vy = 0;
        if (estadoDano == DANO2 || estadoDano == STANDUP) vx = 0;
    }
    if (!verificarColision(x() + vx, y())) {
        setPos(x() + vx, y());
    } else {
        vx = 0;
    }

    // Sub-rutinas mecánicas y renderizado
    actualizarAnimDano();
    actualizarAtaque();
    actualizarAtaquesFuertes();
    actualizarEspecial();

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

    // Selector de animación
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
    } else if (estaCurando) {
        animacionActual = &spritesCURACION;
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

    // Controlador de velocidad de frames
    static int ralentizadorJ = 0;
    if (++ralentizadorJ >= 5) {
        ralentizadorJ = 0;
        if (estaCurando) {
            frameActual++;
            ejecutarCuracion();
            if (frameActual >= spritesCURACION.size() - 1) {
                estaCurando = false;
                estadoCuracion = CUR_IDLE;
                frameActual = 0;
                cooldownCuracionTicks = 300; // Cooldown para que no lo use spameado
                actualizarAuraVisual();
                if (Personaje::tiempoDetenido && Personaje::electorDelTiempo == this) {
                    comboTimeStopPaso = 1;
                    qDebug() << "[CURACIÓN COMPLETADA] ¡SHINE JOJO! (Modo TimeStop)";
                } else {
                    qDebug() << "[CURACIÓN COMPLETADA] DIO reacciona instantáneamente.";
                    ticksDecision = 0;
                    ejecutarCerebro(dx, distancia);
                }
                return;
            }
        } else {
            int limiteDio = (faseCombo == 3) ? 2 : (faseCombo == 5 ? 4 : 3);
            if (stand) {
                if (faseCombo != 5 && frameActual < limiteDio) frameActual++;
            } else {
                frameActual++;
            }
        }
    }

    if (frameActual >= animacionActual->size()) {
        frameActual = bucle ? 0 : animacionActual->size() - 1;
    }

    // Compositor gráfico
    if (stand) {
        if (faseCombo == 5) {
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
        vy = -15;
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
    if (estadoDano != NORMAL) return;

    qDebug() << "¡ZA WARUDO TOKI WO TOMARE!";
    // ── 1. CONFIGURACIÓN DE ESTADOS ──
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
    // ── 2. LÓGICA DE APROXIMACIÓN DINÁMICA  ──
    // Calculamos dónde está Jotaro en este milisegundo exacto
    float dx = objetivo->x() - x();
    float distancia = std::abs(dx);
    mirandoDerecha = (dx > 0);

    if (distancia > 60) {
        // Si Jotaro está lejos, DIO se lanza hacia él con un sprint violento
        vx = mirandoDerecha ? 13.0f : -13.0f;
        qDebug() << "[ESPECIAL] Jotaro está lejos (" << distancia << "px). DIO ejecuta Dash de aproximación.";
    } else {
        // Si ya está pegado a ti, se frena en seco para concentrar el daño
        vx = 0;
        qDebug() << "[ESPECIAL] Jotaro está en rango cerrado. Iniciando golpes inmediatamente.";
    }

    // Tu temporizador de seguridad original
    QTimer::singleShot(5000, [this]() {
        puedeAtacar = true;
    });
}

void DIO::actualizarEspecial() {
    if (!stand || faseCombo != 5) return;

    if (preparandoTimeStop) {
        if (ticksPreTimeStop > 0) {
            ticksPreTimeStop--;
            if (frameDioEsp < 4) {
                if (++ralentDioEsp >= 7) {
                    ralentDioEsp = 0;
                    frameDioEsp++;
                }
            }
            frameTWEsp = 0;
            ralentTWEsp = 0;
            return;
        }
        else {
            // ¡El amago terminó de cargar!
            if (amagoEspecialCorto) {
                qDebug() << "[AMAGO DETECTADO] DIO cancela Za Warudo, hace un dash hacia Jojo y tira RÁFAGA.";
                preparandoTimeStop = false;
                amagoEspecialCorto = false;
                barradeCarga = 0; // Gasta la barra

                // Impulso súper rápido hacia la dirección de Jotaro
                float dx = objetivo->x() - x();
                vx = (dx > 0) ? 14.0f : -14.0f;

                // Reiniciamos los frames de The World para que empiece la ráfaga de puños inmediatamente
                faseCombo = 5;
                stand = true;
                estaAtacando = true;
                frameTWEsp = 0;
                ralentTWEsp = 0;
                setOffset(0, 0);
                return;
            }
            else {
                // TIMESTOP REAL
                preparandoTimeStop = false;
                timeStop();

                int decisionMundo = std::rand() % 100;
                if (decisionMundo < 50) {
                    qDebug() << "[TIMESTOP TÁCTICO] DIO decide curarse primero en el tiempo detenido.";
                    estadoCuracion = CUR_EJECUTANDO;
                    estaCurando = true;
                    frameActual = 0;
                    stand = false;
                    estaAtacando = false;
                    faseCombo = 0;
                } else {
                    qDebug() << "[TIMESTOP TÁCTICO] DIO decide ir directo a romper a Jotaro.";
                    comboTimeStopPaso = 1;
                    vx = mirandoDerecha ? 10.0f : -10.0f;
                    estadoCuracion = CUR_IDLE;
                    stand = false;
                    estaAtacando = false;
                    faseCombo = 0;
                }
                setOffset(0, 0);
                return;
            }
        }
    }
    if (frameTWEsp < 10) {
        if (++ralentTWEsp >= 7) {
            ralentTWEsp = 0;
            evaluarHitboxEspecial();
            frameTWEsp++;
        }
    }
    if (frameTWEsp >= 10) {
        stand = false; estaAtacando = false; faseCombo = 0;
        frameActual = 0; frameActualStand = 0;
        frameDioEsp = 0; frameTWEsp = 0;
        ralentDioEsp = 0; ralentTWEsp = 0;
        vx = 0; // Detener el desplazamiento del amago
        setOffset(0, 0);
        actualizarAuraVisual();
        qDebug() << ">>> LA RÁFAGA DE GOLPES HA TERMINADO <<<";
    }
}

void DIO::timeStop() {
    if (Personaje::tiempoDetenido) return;
    qDebug() << "¡¡ ZA WARUDO !! - TOKI WO TOMARE .";
    Personaje::tiempoDetenido = true;
    Personaje::electorDelTiempo = this;

    QTimer::singleShot(5000, [this]() {
        if (Personaje::tiempoDetenido && Personaje::electorDelTiempo == this) {
            Personaje::tiempoDetenido = false;
            Personaje::electorDelTiempo = nullptr;
            qDebug() << ">>> El tiempo vuelve a fluir <<<";
            if (estaCurando) {
                estaCurando = false;
                vx = 0;
            }
        }
    });
}

void DIO::ejecutarCuracion() {
    // Si por alguna razón la animación se apaga, reiniciamos el estado
    if (!estaCurando) {
        estadoCuracion = CUR_IDLE;
        return;
    }
    // Inyección de vida paulatina frame a frame (solo en los frames centrales de curación)
    if (frameActual >= 2 && frameActual <= 10) {
        if (puntosdevida < 300) { // Asumiendo 300 como vida máxima
            puntosdevida += 1; // Cura 1 punto por frame activo
        }
    }
    // El fin de la animación (frame 13) ahora se centraliza aquí para limpiar la máquina de estados
    if (frameActual >= spritesCURACION.size() - 1) {
        estaCurando = false;
        estadoCuracion = CUR_IDLE; // ¡Liberamos la máquina de estados!
        frameActual = 0;
        cooldownCuracionTicks = 300; // 5 segundos de cooldown para que no abuse
        vx = 0;
        qDebug() << "[CURACIÓN] DIO ha terminado de curarse y vuelve a CUR_IDLE.";
    }
}

void DIO::defensa() {
    if (estadoDano == DANO2 || estadoDano == STANDUP || estadoDano == MUERTO) return;
    estaDefendiendo = !estaDefendiendo;
    actualizarAuraVisual();
}

// Hitboxes optimizadas con offsets corregidos de tu versión local
void DIO::evaluarHitboxBasico() {
    float anchoHit = 20 * ESCALA, altoHit = 20 * ESCALA;
    float offX = mirandoDerecha ? 65.0f : -anchoHit + 10.0f;
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
    float offX = mirandoDerecha ? 85.0f : -anchoHit - 10.0f;
    QRectF rect(x() + offX, y() - 15, anchoHit, altoHit);
    procesarDano(rect, 2);
    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::yellow, 2));
        QTimer::singleShot(50, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
    }
}

void DIO::evaluarHitboxFuerte2() {
    if (frameActualStand >= 4 && frameActualStand <= 8) {
        float anchoHit = 65 * ESCALA, altoHit = 40 * ESCALA;
        float offX = mirandoDerecha ? 80.0f : -anchoHit - 20.0f;
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
    float offX = mirandoDerecha ? 90.0f : -anchoHit + 5.0f;
    QRectF rect(x() + offX, y() - 20, anchoHit, 90);
    procesarDano(rect, 4);
    if (Personaje::modoDebug) {
        QGraphicsRectItem* dr = scene()->addRect(rect, QPen(Qt::yellow, 2));
        QTimer::singleShot(40, [dr]() { if (dr && dr->scene()) { dr->scene()->removeItem(dr); delete dr; } });
    }
}

void DIO::procesarDano(QRectF area, int cantidad) {
    QList<QGraphicsItem*> items = scene()->items(area);
    bool golpeoAAlguien = false;
    for (QGraphicsItem* item : items) {
        Jojo* personajeGolpeado = dynamic_cast<Jojo*>(item);
        if (personajeGolpeado) {
            golpeoAAlguien = true;
            personajeGolpeado->recibirDanoConOrigen(cantidad, this->x());
        }
    }

    if (golpeoAAlguien && barradeCarga < 100 && faseCombo <= 4) {
        barradeCarga += 5;
        if (barradeCarga > 100) barradeCarga = 100;

        // ¡ACTUALIZACIÓN AQUÍ! Si se cargó por completo, encendemos el aura
        if (barradeCarga >= 100) {
            actualizarAuraVisual();
        }
    }
}

// ── RESTABLECIMIENTO DE LAS FÍSICAS DE RETROCESO (REPARADO) ──
void DIO::recibirDano(int cantidad) {
    if (estadoDano == MUERTO) return;
    if (estaDefendiendo) cantidad /= 2;
    puntosdevida -= cantidad;
    danioAcumulado += cantidad;
    contadorGolpesRecibidos++;

    if (contadorGolpesRecibidos >= 50 && !contraataqueActivo && !estaDefendiendo && !preparandoTimeStop
        && estadoDano == NORMAL && estadoCuracion == CUR_IDLE) {
        contraataqueActivo = true;
        ticksContraataque = 120;
        estaAtacando = false;
        stand = false;
        faseCombo = 0;
        frameActual = 0;
    }
    qDebug() << "¡DIO recibió" << cantidad << "de daño! Vida restante:" << puntosdevida;
    if (puntosdevida <= 0) {
        puntosdevida = 0;
        estadoDano = MUERTO;
        frameDano = 0;
        estaAtacando = false;
        stand = false;
        preparandoTimeStop = false;
        faseCombo = 0;
        vx = 0; vy = 0;
        setOffset(0, 0);
        qDebug() << ">>> DIO HA SIDO ELIMINADO <<<";
        return;
    }
    // ── SEGUNDO FILTRO: ¿ESTABA CANALIZANDO EL TIMESTOP? ──
    if (preparandoTimeStop) {
        if (danioAcumulado >= 22) {
            qDebug() << "[¡INTERRUMPIDO!] Jotaro golpeó a DIO con fuerza. Se cancela Za Warudo.";

            preparandoTimeStop = false;
            ticksPreTimeStop = 0;
            estaAtacando = false;
            stand = false;
            faseCombo = 0;
            setOffset(0, 0);

            this->activarDano2(false);
            danioAcumulado = 0;

            vx = (objetivo->x() < this->x()) ? 8.0f : -8.0f;
        }
        return; // Si sigue vivo pero en fase de amago, salimos aquí para no romper las animaciones
    }
    // ── TERCER FILTRO: EVALUAR HYPER ARMOR DE COMBOS ──
    if (stand && (faseCombo == 3 || faseCombo == 4 || faseCombo == 5)) {
        qDebug() << "[HYPER ARMOR] ¡DIO resiste el impacto y continúa con The World!";
    } else {
        estaAtacando = false;
        stand = false;
        faseCombo = 0;
        frameActual = 0;
        frameActualStand = 0;
        setOffset(0, 0);
    }
    float direccionEmpuje = (objetivo->x() < this->x()) ? 0.5f : -0.5f;
    // Si tiene Hyper Armor activa, se procesa el empuje leve y salimos
    if (stand && (faseCombo == 3 || faseCombo == 4 || faseCombo == 5)) {
        estadoDano = NORMAL;
        vx = direccionEmpuje * 2.0f;
        return;
    }
    if (estaDefendiendo) {
        if (danioAcumulado >= 22) {
            estaDefendiendo = false;
            ticksDefensaRestantes = 0;
            this->activarDano2(true);
            vx = direccionEmpuje * 10.0f;
            danioAcumulado = 0;
        }
        return;
    }
    // Reacción normal a impactos fuera de amagos o defensas
    if (danioAcumulado >= 22) {
        this->activarDano2(false);
        vx = direccionEmpuje * 10.0f;
        danioAcumulado = 0;
    } else {
        this->activarDano1();
        vx = direccionEmpuje * 1.0f;
    }
}

void DIO::recibirDanoConOrigen(int cantidad, float atacanteX) {
    if (estadoDano == MUERTO) return;
    // REGLA: Si está curándose, funciona como defensa (mitad de daño)
    bool defendiendo = estaDefendiendo || estaCurando;
    if (defendiendo) cantidad /= 2;
    puntosdevida -= cantidad;
    danioAcumulado += cantidad;

    // Sistema de aprendizaje: contar golpes consecutivos recibidos
    contadorGolpesRecibidos++;
    if (contadorGolpesRecibidos >= 8 && !contraataqueActivo && !estaDefendiendo && !preparandoTimeStop
        && estadoDano == NORMAL && estadoCuracion == CUR_IDLE) {
        qDebug() << "[APRENDIZAJE] Jotaro lleva demasiados golpes seguidos. DIO activa contraataque.";
        contraataqueActivo = true;
        ticksContraataque = 120; // 2 segundos de escudo
        estaAtacando = false;
        stand = false;
        faseCombo = 0;
        frameActual = 0;
        actualizarAuraVisual();
    }
    qDebug() << "¡DIO recibió con origen" << cantidad << "de daño! Vida restante:" << puntosdevida;
    // ── FILTRO DE MUERTE INMEDIATA ──
    if (puntosdevida <= 0) {
        puntosdevida = 0;
        estadoDano = MUERTO;
        frameDano = 0;
        estaAtacando = false;
        stand = false;
        estaCurando = false;
        preparandoTimeStop = false;
        faseCombo = 0;
        vx = 0; vy = 0;
        setOffset(0, 0);
        qDebug() << ">>> DIO HA SIDO ELIMINADO CON ORIGEN <<<";
        return;
    }
    // ── NUEVO: Si lo golpean con origen mientras amaga el TimeStop ──
    if (preparandoTimeStop) {
        if (danioAcumulado >= 22) {
            qDebug() << "[¡INTERRUMPIDO ORIGEN!] Se cancela Za Warudo.";
            preparandoTimeStop = false;
            ticksPreTimeStop = 0;
            estaAtacando = false;
            stand = false;
            faseCombo = 0;
            setOffset(0, 0);

            this->activarDano2(defendiendo);
            danioAcumulado = 0;
            vx = (atacanteX < this->x()) ? 10.0f : -10.0f;
        }
        return;
    }

    float direccionEmpuje = (atacanteX < this->x()) ? 0.5f : -0.5f;

    // Filtro Hyper Armor...
    if (stand && (faseCombo == 3 || faseCombo == 4 || faseCombo == 5)) {
        estadoDano = NORMAL;
        vx = direccionEmpuje * 4.0f;
        return;
    }

    if (danioAcumulado >= 22) {
        estaCurando = false;
        estaDefendiendo = false; // Rompemos la defensa si el impacto es masivo
        this->activarDano2(defendiendo);
        vx = direccionEmpuje * 10.0f;
        danioAcumulado = 0;
    } else {
        // Si se está defendiendo, absorbe el golpe sin perder su postura (sin activar DANO1)
        if (defendiendo) {
            vx = direccionEmpuje * 0.5f; // Un pequeño empuje de bloqueo casi imperceptible
        } else {
            if (!estaCurando) {
                this->activarDano1();
            }
            vx = direccionEmpuje * 1.0f;
        }
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
            contadorGolpesRecibidos = 0;
        }
        break;
    }
}

void DIO::actualizarAuraVisual() {
    if (Personaje::tiempoDetenido && Personaje::electorDelTiempo != this) return;
    if (this->graphicsEffect()) {
        this->setGraphicsEffect(nullptr);
        elAuraEfecto = nullptr; // Qt lo elimina automáticamente
    }

    // 1. Prioridad Máxima: Barra de Especial al 100% (Aura Morada de Star Platinum/Espacio-Tiempo)
    if (barradeCarga >= 100) {
        elAuraEfecto = new QGraphicsDropShadowEffect();
        elAuraEfecto->setBlurRadius(55);
        elAuraEfecto->setColor(QColor(138, 43, 226, 255));   // Morado Eléctrico/Violeta
        elAuraEfecto->setOffset(0, 0);
        this->setGraphicsEffect(elAuraEfecto);
    }
    // 2. Prioridad Media: Modo Hiperagresivo / Contraataque activo (Aura Dorada/Amarilla de The World)
    else if (modoHiperagresivoActivo || contraataqueActivo) {
        elAuraEfecto = new QGraphicsDropShadowEffect();
        elAuraEfecto->setBlurRadius(45);
        elAuraEfecto->setColor(QColor(255, 215, 0, 230));    // Oro Brillante intenso
        elAuraEfecto->setOffset(0, 0);
        this->setGraphicsEffect(elAuraEfecto);
    }
    // 3. Prioridad Baja: Modo Defensivo o Curándose (Aura Verde/Cian mística)
    else if (estaDefendiendo || estaCurando || modoDefensivoActivo) {
        elAuraEfecto = new QGraphicsDropShadowEffect();
        elAuraEfecto->setBlurRadius(35);
        elAuraEfecto->setColor(QColor(50, 205, 50, 200));    // Verde Lima Brillante
        elAuraEfecto->setOffset(0, 0);
        this->setGraphicsEffect(elAuraEfecto);
    }
}
