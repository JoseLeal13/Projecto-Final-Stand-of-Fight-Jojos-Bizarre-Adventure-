#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivel.h"
#include "standuserstats.h"
#include "jojobro.h"             //esto lo tienes que cambiar que es tu gyro.h
//#include "steelball.h"
//#include "item.h"
#include <QList>

class Nivel1 : public Nivel {
    Q_OBJECT

private:
    jojobro* gyroJefe;
    StandUserStats* interfazHUD;
    QGraphicsPixmapItem* fondoMapa;

    // Listas de control de entidades heredadas del prototipo anterior
    //QList<SteelBall*> esferasActivas;
    //QList<ItemJuego*> itemsActivos;

    // Estructura interna para las explosiones doradas autónomas
    struct EfectoExplosion {
        QGraphicsPixmapItem* item;
        QList<QPixmap> frames;
        int frameActual;
        int contadorFrames;
    };
    QList<EfectoExplosion> explosionesActivas;
    QList<QPixmap> framesExplosion;

    // Control de Rondas y Tiempos
    int roundActual;
    int tiempoRestanteRound; // Inicializado en 60s
    int KOsJotaro;           // Rondas sobrevividas con éxito
    int KOsGyro;             // Rondas perdidas (por si cae a 0 de vida)
    bool rondaEnTransicion;
    bool finRoundProcesado;
    QTimer* timerUnSegundo;

    void cargarPosicionesIniciales();
    void procesarFinRound(const QString& ganador);
    void cargarFramesExplosion();
    void vaciarEntidadesEscena();

private slots:
    void actualizarSegundo();

public:
    Nivel1(QGraphicsScene* escenaCompartida, Jojo* personajeJojo, const QString& dificultad, QObject* parent = nullptr);
    ~Nivel1();

    void iniciarNivel() override;
    bool verificarCondicionVictoria() override;
    bool verificarCondicionDerrota() override;
    void limpiarNivel() override;

    // Métodos puente para interactuar con los inputs capturados por el controlador principal
    void procesarPresionTeclada(int tecla);
    void procesarLiberacionTeclada(int tecla);
    bool isRondaEnTransicion() const { return rondaEnTransicion; }

protected slots:
    void actualizarLoop() override;

signals:
    void combateTerminado(bool gano);
};

#endif // NIVEL1_H
