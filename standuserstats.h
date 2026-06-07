#ifndef STANDUSERSTATS_H
#define STANDUSERSTATS_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QBrush>
#include <QPen>
#include <QFont>
#include "personaje.h"

// Declaración adelantada para no incluir jojo.h aquí
class Jojo;

class StandUserStats {

public:
    // ── MODO DE OPERACIÓN ──────────────────────────────────────────────────
    // SURVIVAL : HUD del Nivel1 (Jotaro vs Steel Balls)
    //            → Solo muestra vida de Jotaro (base 100), barra de energía,
    //              reloj y marcador de KOs. NO crea nada de DIO.
    // COMBATE  : HUD original del Nivel2 (Jotaro vs DIO)
    //            → Comportamiento original sin cambios.
    enum Modo { SURVIVAL, COMBATE };

    // Constructor unificado. Por defecto sigue siendo COMBATE para no romper Nivel2.
    StandUserStats(QGraphicsScene* scene, Modo modo = COMBATE);

    // ── ACTUALIZACIÓN DE BARRAS ────────────────────────────────────────────
    // Para Nivel2 (Personaje*): usa getVida()/getCarga() y vida base 500
    void actualizarEstados(Personaje* jojo, Personaje* dio);

    // Para Nivel1 (Jojo*): usa jojo->vida (base 100) y jojo->getEnergia()
    void actualizarEstadosNivel1(Jojo* jojo);

    // ── RELOJ Y MARCADOR ──────────────────────────────────────────────────
    void actualizarRelojYMarcador(int segundosRestantes, int kosJojo, int kosDio);

    // ── ANUNCIOS CENTRALES ────────────────────────────────────────────────
    void mostrarAnuncioCentral(const QString& mensaje, const QColor& color = Qt::red);
    void ocultarAnuncioCentral();

private:
    Modo modoActual;

    // ── COMPONENTES JOTARO (Nivel1 y Nivel2) ──────────────────────────────
    QGraphicsRectItem* jojoFondoVida   = nullptr;
    QGraphicsRectItem* jojoBarraVida   = nullptr;
    QGraphicsRectItem* jojoFondoCarga  = nullptr;
    QGraphicsRectItem* jojoBarraCarga  = nullptr;
    QGraphicsTextItem* jojoTexto       = nullptr;

    // ── COMPONENTES DIO (solo Nivel2 / COMBATE) ───────────────────────────
    QGraphicsRectItem* dioFondoVida    = nullptr;
    QGraphicsRectItem* dioBarraVida    = nullptr;
    QGraphicsRectItem* dioFondoCarga   = nullptr;
    QGraphicsRectItem* dioBarraCarga   = nullptr;
    QGraphicsTextItem* dioTexto        = nullptr;

    // ── COMPONENTES COMPARTIDOS ───────────────────────────────────────────
    QGraphicsTextItem* textoAnuncioCentral = nullptr;
    QGraphicsTextItem* textoTemporizador   = nullptr;
    QGraphicsTextItem* textoMarcadorKOs    = nullptr;
    QGraphicsScene*    escenaAsignada      = nullptr;

    // ── DIMENSIONES ───────────────────────────────────────────────────────
    const int ANCHO_BARRA_VIDA  = 250;
    const int ALTO_BARRA_VIDA   = 20;
    const int ANCHO_BARRA_CARGA = 200;
    const int ALTO_BARRA_CARGA  = 12;

    // ── CONSTRUCTORES INTERNOS (llamados desde el constructor público) ─────
    void construirHUDSurvival(QGraphicsScene* scene);
    void construirHUDCombate (QGraphicsScene* scene);
};

#endif // STANDUSERSTATS_H