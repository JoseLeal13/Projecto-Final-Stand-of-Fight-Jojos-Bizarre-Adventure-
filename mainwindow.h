#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QSet>
#include <QList>

#include "jotaro.h"
#include "steelball.h"
#include "gyrozeppeli.h"  // 👨‍🎓 NOTA DE ESTUDIANTE: Cambiado de gyrozeppeli.h a gyro.h para que haga match
#include "item.h"  // 👨‍🎓 NOTA DE ESTUDIANTE: Agregado para que reconozca a la clase ItemJuego

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Struct para la explosion dorada cuando una bola golpea a Jotaro.
struct EfectoExplosion {
    QList<QPixmap> frames;
    int frameActual;
    int contadorFrames;
    QGraphicsPixmapItem *item;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Eventos de teclado estables de Qt
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void actualizar();

private:
    Ui::MainWindow *ui;

    // El motor gráfico del juego
    QGraphicsScene *scene;
    QGraphicsView *view;
    QTimer *timer;

    QGraphicsPixmapItem *itemEscenario;
    QGraphicsPixmapItem *gyroItem; // Lo dejamos por si acaso, aunque usaremos gyroJefe
    QSet<int> teclasPresionadas;   // Almacén de teclas activas

    jotaro *jotaro_player;       // Puntero a tu objeto adaptado
    QPixmap fondoPixmap;

    // Banderas de control de movimiento originales
    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;

    // Estados de animación que sincronizan con el bucle de la ventana
    bool enMovimiento;
    bool atacando;
    bool mostrarHitbox;

    int frameActual;
    int contadorFrames;
    int retardoFrames;

    // ── GESTIÓN DE LAS STEEL BALLS Y EFECTOS ──
    QList<SteelBall*> esferasActivas;
    int contadorSpawnBolas;
    int tiempoSurvival;
    int framesParaSegundo;

    // Lista de explosiones doradas activas
    QList<EfectoExplosion> explosionesActivas;

    // Frames de la explosion dorada
    QList<QPixmap> framesExplosion;
    void cargarFramesExplosion();

    // ── 🎒 NUEVAS ENTIDADES MODULARIZADAS CON POO ──
    Gyro *gyroJefe;                      // El objeto inteligente de Gyro Zeppeli
    QList<ItemJuego*> itemsActivos;      // Almacena los buffs de velocidad y corazones en pantalla
    int contadorSpawnItems;              // Reloj/Temporizador interno para soltar ítems
};

#endif // MAINWINDOW_H