#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "nivel2.h"
// #include "nivel1.h"

class JoestarChampionship : public QMainWindow {
    Q_OBJECT

public:
    JoestarChampionship(QWidget *parent = nullptr);
    ~JoestarChampionship();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    // Menú Principal
    void mostrarSeleccionNivel();
    // 💡 Quitamos mostrarAyuda() y mostrarCreditos() de aquí porque ahora se manejan con Lambdas directas

    // Selección de Nivel
    void seleccionarDificultadNivel1();
    void intentarJugarNivel2();
    void regresarAlMenuPrincipal();

    // Flujo del Juego
    void comenzarNivel1(const QString& dificultad);
    void comenzarNivel2();
    void capturarFinDelJuego(bool victoria);
    void reintentarUltimoNivel();

private:
    void crearMenuPrincipal();
    void crearPantallaSeleccion();
    void crearPantallaPostJuego();
    void crearPantallaAyuda();
    void crearPantallaCreditos();
    void configurarVideoFondo();
    QSet<int> teclasPresionadas;

    QStackedWidget *controladorPantallas;

    // Pantallas de interfaz
    QWidget *pantallaMenuPrincipal;
    QWidget *pantallaSeleccionNivel;
    QWidget *pantallaPostJuego;
    QWidget *pantallaAyuda;
    QWidget *pantallaCreditos;

    QPushButton *btnNivel2;
    QLabel *lblResultadoPostJuego;

    // Controladores de estado del torneo
    Nivel2 *nivel2Activo;
    int idUltimoNivelJugado;
    QString dificultadUltimoNivel1;
    bool nivel1Completado;
};
