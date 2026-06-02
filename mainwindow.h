#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QSet>
#include "jotaro.h"
#include <QList>
// #include "gyro.h"       // Descomenta cuando agregues a Gyro
#include "steelball.h"  // Descomenta cuando agregues la bola

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; } // Requerido para el puntero ui
QT_END_NAMESPACE

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
    // Cambiado de updateGame() a actualizar() para que haga match con tu .cpp actual
    void actualizar();

private:
    Ui::MainWindow *ui;

    // El motor gráfico que usa tu amigo
    QGraphicsScene *scene;
    QGraphicsView *view;
    QTimer *timer;

    QGraphicsPixmapItem *itemEscenario;
    QGraphicsPixmapItem *gyroItem;
    QSet<int> teclasPresionadas; // Almacén de teclas activas


    jotaro *jotaro_player;       // Puntero a tu objeto adaptado
    QPixmap fondoPixmap;




    // Banderas de control de movimiento (Tus variables originales intactas)
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

    // ── NUEVAS VARIABLES PARA EL SURVIVAL DE LAS STEEL BALLS ──
    QList<SteelBall*> esferasActivas; // El almacén de las esferas que viajan por el mapa
    int contadorSpawnBolas;           // El reloj que mide cuándo Gyro lanza otra bola
    int tiempoSurvival;               // Segundos que faltan para ganar (ej: 30)
    int framesParaSegundo;            // Cuenta los ticks hasta llegar a 60 (1 segundo real)

};

#endif // MAINWINDOW_H