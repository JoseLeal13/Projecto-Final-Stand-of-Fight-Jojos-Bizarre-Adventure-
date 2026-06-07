#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivel.h"
#include "standuserstats.h"
#include "jojobro.h"
#include "steelball.h"
#include "item.h"
#include <QList>
#include <QSet>
#include <QTimer>


class Nivel1 : public Nivel {
    Q_OBJECT

public:
    Nivel1(QGraphicsScene* escenaCompartida, Jojo* personajeJojo, const QString& dificultad, QObject* parent = nullptr);
    ~Nivel1();

    void iniciarNivel()              override;
    bool verificarCondicionVictoria() override;
    bool verificarCondicionDerrota()  override;
    void limpiarNivel()              override;

    void procesarPresionTeclada(int tecla);
    void procesarLiberacionTeclada(int tecla);

signals:
    void combateTerminado(bool gano);

protected slots:
    void actualizarLoop() override;

private slots:
    void actualizarSegundo();

private:
    // ── Entidades ──────────────────────────────────────────────────────────
    Jojobro*              gyroJefe;
    StandUserStats*       interfazHUD;
    QGraphicsPixmapItem*  fondoMapa;

    // ── Estado del nivel ───────────────────────────────────────────────────
    QString  dificultad;
    int      tiempoRestante;     // segundos que faltan (empieza en 60)
    bool     nivelFinalizado;    // true una vez que terminarNivel() se ejecuta
    bool     mostrarHitbox;

    QSet<int> teclasPresionadas;
    QTimer*   timerUnSegundo;

    // ── Contadores de animación del jugador ────────────────────────────────
    int contadorFrames     = 0;
    int frameActualJugador = 0;

    // ── Entidades activas ──────────────────────────────────────────────────
    QList<SteelBall*>  esferasActivas;
    QList<ItemJuego*>  itemsActivos;
    int                contadorSpawnItems;

    // ── Explosiones visuales ───────────────────────────────────────────────
    struct EfectoExplosion {
        QGraphicsPixmapItem* item;
        QList<QPixmap>       frames;
        int                  frameActual;
        int                  contadorFrames;
    };
    QList<EfectoExplosion> explosionesActivas;
    QList<QPixmap>         framesExplosion;

    // ── Métodos internos ───────────────────────────────────────────────────
    void configurarJojo();
    void terminarNivel(bool victoria);
    void vaciarEntidadesEscena();
    void cargarFramesExplosion();

};

#endif // NIVEL1_H