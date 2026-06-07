#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "standuserstats.h"
#include "dio.h"
#include <QWidget>
#include <QKeyEvent>

class Nivel2 : public Nivel {
    Q_OBJECT

private:
    DIO *dio;
    Jojo *dummy;
    StandUserStats* interfazHUD;
    QGraphicsPixmapItem* fondoMapa;

    // Control del efecto Za Warudo
    bool efectoGrisActivo;
    QGraphicsColorizeEffect *efectoJotaro;
    void aplicarEfectoZaWarudo(bool activar);

    int roundActual;            // Lleva el conteo de 1 a 10
    int tiempoRestanteRound;    // Cuenta regresiva desde 120 segundos (2 min)
    int KOsJotaro;              // Contador de victorias de Jotaro (Max 3)
    int KOsDio;                 // Contador de victorias de DIO (Max 3)
    bool rondaEnTransicion;     // Evita actualizaciones físicas en las pausas de K.O.
    QTimer* timerUnSegundo;     // El reloj secundario que descuenta de 1s en 1s
    bool finRoundProcesado;

    void procesarFinRound(const QString& ganador);
    void cargarPosicionesIniciales();

    QSet<int> teclasPresionadas;

private slots:
    void actualizarSegundo();   // Se ejecuta cada 1000ms para el cronómetro

public:
    Nivel2(QGraphicsScene* escenaCompartida, Jojo* personajeJojo, QObject* parent = nullptr);
    ~Nivel2();

    void iniciarNivel() override;
    bool verificarCondicionVictoria() override;
    bool verificarCondicionDerrota() override;
    void limpiarNivel() override;
    bool isRondaEnTransicion() const { return rondaEnTransicion; }

    void procesarPresionTeclada(QKeyEvent *event);
    void procesarLiberacionTeclada(QKeyEvent *event);

protected slots:
    void actualizarLoop() override; // Aquí va la física 2D lateral y las hitboxes

signals:
    void combateTerminado(bool gano);
};

#endif // NIVEL2_H
