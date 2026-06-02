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
#include "steelball.h"
// Sin clase EfectoVisual -- la explosion dorada la manejamos con un struct simple aqui mismo

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; } // Requerido para el puntero ui
QT_END_NAMESPACE

// Struct para la explosion dorada cuando una bola golpea a Jotaro.
// Guardamos los frames, en cual vamos, el contador de ticks y el item en la escena.
// Es más simple que crear una clase entera para esto.
struct EfectoExplosion {
    QList<QPixmap> frames;
    int frameActual;
    int contadorFrames;
    QGraphicsPixmapItem *item; // El item que vive en la escena y muestra el pixmap
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
    QList<SteelBall*> esferasActivas;
    int contadorSpawnBolas;
    int tiempoSurvival;
    int framesParaSegundo;

    // Lista de explosiones doradas activas (cuando una bola golpea a Jotaro)
    // Cada explosion es un struct simple con sus frames y un QGraphicsPixmapItem en la escena
    QList<EfectoExplosion> explosionesActivas;

    // Frames de la explosion dorada, los cargamos una vez en el constructor
    // y los reutilizamos para cada explosion que aparezca
    QList<QPixmap> framesExplosion;
    void cargarFramesExplosion(); // Carga los 4 frames dorados del sprite sheet

};

#endif // MAINWINDOW_H