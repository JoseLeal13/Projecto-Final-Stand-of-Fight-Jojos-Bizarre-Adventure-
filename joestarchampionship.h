#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QVideoFrame>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSet>
#include <QDialog>
#include "nivel2.h"

class JoestarChampionship : public QMainWindow {
    Q_OBJECT

public:
    JoestarChampionship(QWidget *parent = nullptr);
    ~JoestarChampionship();

public slots:
    void pausarMusicaNivelDetenido();
    void reanudarMusicaNivelDetenido();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    // Menú Principal
    void mostrarSeleccionNivel();

    // Selección de Nivel
    void seleccionarDificultadNivel1();
    void intentarJugarNivel2();
    void regresarAlMenuPrincipal();

    // Flujo del Juego
    void comenzarNivel1(const QString& dificultad);
    void comenzarNivel2();
    void capturarFinDelJuego(bool victoria);
    void reintentarUltimoNivel();

    //Otras opciones
    void alternarMutearAudio();

private:
    void crearMenuPrincipal();
    void crearPantallaSeleccion();
    void crearPantallaPostJuego();
    void crearPantallaAyuda();
    void crearPantallaCreditos();
    void configurarVideoFondo();
    void reiniciarVideoFondo();
    void detenerMultimediaMenu();
    void reproducirMultimediaMenu();
    void gestionarMusicaNivel(int nivel);
    void mostrarEpilogoNivel1(bool victoria);
    void mostrarEpilogoNivel2(bool victoria);
    void crearPantallaEpilogoFinal();
    void crearPantallaDificultadNivel1();

    QSet<int> teclasPresionadas;
    QStackedWidget *controladorPantallas;

    // Objetos multimedia para el Video (.MP4)
    QLabel *lblVideoFondo;
    QVideoSink *videoSink;
    QMediaPlayer *reproductorVideo;
    QAudioOutput *salidaAudio;

    // Controlador para la música (.MP3)
    QMediaPlayer *musicaMenu;
    QAudioOutput *salidaAudioMusica;
    QMediaPlayer *musicaNivel1;
    QAudioOutput *salidaAudioNivel1;
    QMediaPlayer *musicaNivel2;
    QAudioOutput *salidaAudioNivel2;

    // Pantallas de interfaz
    QWidget *pantallaMenuPrincipal;
    QWidget *pantallaSeleccionNivel;
    QWidget *pantallaPostJuego;
    QWidget *pantallaAyuda;
    QWidget *pantallaCreditos;
    QWidget *pantallaDificultadNivel1;

    QPushButton *btnMute;
    bool audioMutado = false;

    QPushButton *btnNivel2;
    QLabel *lblResultadoPostJuego;

    // Controladores de estado del torneo
    Nivel2 *nivel2Activo;
    int idUltimoNivelJugado;
    QString dificultadUltimoNivel1;
    bool nivel1Completado;

    QWidget *pantallaEpilogoFinal;
    QLabel *lblImagenGanador;
    QLabel *lblTextoFinal;
    QPushButton *btnAceptarFinal;
    QSoundEffect *audioRisaDio;
    QSoundEffect *sonidoVictoriaGyro;
    QSoundEffect *sonidoVictoriaJojo;
};
