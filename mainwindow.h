#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QPixmap>
#include <QVector>
#include <QTransform>
#include "jotaro.h"
#include "gyrozeppeli.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:

    void paintEvent(QPaintEvent *event) override;
    //void paintEvent(QPaintEvent *);//para dibujar el evento
    void keyPressEvent(QKeyEvent *event);//para WASD
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void updateGame();//un loop para el juego

private:
    jotaro jotaro;
    gyrozeppeli gyrozeppeli;

    QPixmap fondo;
    QPixmap sprites;
    QPixmap frameJotaro;
    // Frames jotaro
    QVector<QPixmap> framesQuieto[4];
    QVector<QPixmap> framesMovimiento[4];
    QVector<QPixmap> framesAtaqueDerecha;
    QVector<QPixmap> framesAtaqueIzquierda;

    // Frames gyro
    QPixmap frameGyro;

    int frameActual;
    int contadorFrames;
    int retardoFrames;  // cada cuántos ticks cambia el frame

    int direccion;      // 0=arriba 1=abajo 2=izquierda 3=derecha
    bool enMovimiento;

    QTimer *timer;//FPS
    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;

    void cargarFrames();
    QPixmap getFrameActual();
    QPixmap reflejarHorizontal(const QPixmap &original);
    QPixmap quitarFondo(const QPixmap &original);

    //ataques
    bool atacando;
    bool mostrarHitbox;
};

#endif // MAINWINDOW_H
