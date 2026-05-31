#include "joestarchampionship.h"
#include <QKeyEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

JoestarChampionship::JoestarChampionship(QWidget *parent) : QMainWindow(parent) {
    setFixedSize(1200, 550);

    nivel2Activo = nullptr;
    nivel1Completado = false;
    idUltimoNivelJugado = 1;
    dificultadUltimoNivel1 = "facil";

    // Creamos contenedor base neutral para la ventana central
    QWidget *contenedorBase = new QWidget(this);
    setCentralWidget(contenedorBase);

    // 1. Creamos el label de fondo asignándolo al CONTENEDOR BASE
    lblVideoFondo = new QLabel(contenedorBase);
    lblVideoFondo->setGeometry(0, 0, 1200, 550);
    lblVideoFondo->setScaledContents(true);

    // 2. El controlador de pantallas también va dentro del CONTENEDOR BASE
    controladorPantallas = new QStackedWidget(contenedorBase);
    controladorPantallas->setGeometry(0, 0, 1200, 550);

    // 3. Inicializamos el video (ahora que las variables existen en el .h)
    configurarVideoFondo();

    // 4. Construcción de todas las interfaces
    crearMenuPrincipal();        // Índice 0
    crearPantallaSeleccion();    // Índice 1
    crearPantallaPostJuego();    // Índice 2
    crearPantallaAyuda();        // Índice 3
    crearPantallaCreditos();     // Índice 4

    controladorPantallas->addWidget(pantallaMenuPrincipal);
    controladorPantallas->addWidget(pantallaSeleccionNivel);
    controladorPantallas->addWidget(pantallaPostJuego);
    controladorPantallas->addWidget(pantallaAyuda);
    controladorPantallas->addWidget(pantallaCreditos);

    // 5. Aplicamos transparencias absolutas a los menús
    controladorPantallas->setStyleSheet("background: transparent;");
    pantallaMenuPrincipal->setStyleSheet("background: transparent;");
    pantallaSeleccionNivel->setStyleSheet("background: transparent;");
    pantallaPostJuego->setStyleSheet("background: transparent;");
    pantallaAyuda->setStyleSheet("background: transparent;");
    pantallaCreditos->setStyleSheet("background: transparent;");

    controladorPantallas->setCurrentIndex(0);

    // Capas en orden estricto de software: Video atrás, botones al frente
    lblVideoFondo->lower();
    controladorPantallas->raise();

    this->setFocus();
}

JoestarChampionship::~JoestarChampionship() {
    if (nivel2Activo) delete nivel2Activo;
}

void JoestarChampionship::configurarVideoFondo() {
    reproductorVideo = new QMediaPlayer(this);

    salidaAudio = new QAudioOutput(this);
    salidaAudio->setVolume(0.0);
    reproductorVideo->setAudioOutput(salidaAudio);

    videoSink = new QVideoSink(this);
    reproductorVideo->setVideoOutput(videoSink);

    connect(videoSink, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &frame) {
        QImage img = frame.toImage();
        if (!img.isNull()) {
            lblVideoFondo->setPixmap(QPixmap::fromImage(img));
        }
    });

    reproductorVideo->setSource(QUrl("qrc:/menú/Joestarchampionship.menu.mp4"));

    connect(reproductorVideo, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia) {
            reproductorVideo->setPosition(0);
            reproductorVideo->play();
        }
    });

    reproductorVideo->play();

    // --- Configuración de la música Overdrive (MP3) ---
    salidaAudioMusica = new QAudioOutput(this);
    musicaMenu = new QMediaPlayer(this);
    musicaMenu->setAudioOutput(salidaAudioMusica);
    musicaMenu->setSource(QUrl("qrc:/menú/Jojo Battle Tendency OST_ Overdrive [NO INTRO].mp3"));

    connect(musicaMenu, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia) {
            musicaMenu->setPosition(0);
            musicaMenu->play();
        }
    });
    musicaMenu->play();

    salidaAudioNivel1 = new QAudioOutput(this);
    musicaNivel1 = new QMediaPlayer(this);
    musicaNivel1->setAudioOutput(salidaAudioNivel1);
    musicaNivel1->setSource(QUrl("qrc:/menú/JOJO Steel Ball Run OST_ Gyro Zeppeli Theme _ EPIC VERSION Fan-Made.mp3"));
    connect(musicaNivel1, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia) {
            musicaNivel1->setPosition(0);
            musicaNivel1->play();
        }
    });

    salidaAudioNivel2 = new QAudioOutput(this);
    musicaNivel2 = new QMediaPlayer(this);
    musicaNivel2->setAudioOutput(salidaAudioNivel2);
    musicaNivel2->setSource(QUrl("qrc:/menú/Jotaro Theme but it's EPIC VERSION Star Platinum Over Heaven.mp3"));
    connect(musicaNivel2, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia) {
            musicaNivel2->setPosition(0);
            musicaNivel2->play();
        }
    });
}

void JoestarChampionship::crearMenuPrincipal() {
    pantallaMenuPrincipal = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(pantallaMenuPrincipal);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *titulo = new QLabel("JOESTAR CHAMPIONSHIP", pantallaMenuPrincipal);
    titulo->setStyleSheet("font-family: 'Impact'; font-size: 55px; color: gold; margin-bottom: 20px;");
    layout->addWidget(titulo);

    QPushButton *btnJugar = new QPushButton("JUGAR", pantallaMenuPrincipal);
    QPushButton *btnAyuda = new QPushButton("AYUDA", pantallaMenuPrincipal);
    QPushButton *btnCreditos = new QPushButton("CRÉDITOS", pantallaMenuPrincipal);
    QPushButton *btnSalir = new QPushButton("SALIR", pantallaMenuPrincipal);

    QString estiloBoton = "QPushButton { font-family: 'Impact'; font-size: 22px; background-color: rgba(15,15,15,0.85); color: white; border: 2px solid white; padding: 8px; margin: 5px; min-width: 220px; }"
                          "QPushButton::hover { background-color: gold; color: black; border-color: gold; }";

    btnJugar->setStyleSheet(estiloBoton);
    btnAyuda->setStyleSheet(estiloBoton);
    btnCreditos->setStyleSheet(estiloBoton);
    btnSalir->setStyleSheet(estiloBoton);

    layout->addWidget(btnJugar);
    layout->addWidget(btnAyuda);
    layout->addWidget(btnCreditos);
    layout->addWidget(btnSalir);

    btnMute = new QPushButton(pantallaMenuPrincipal);
    btnMute->setGeometry(30, 30, 50, 50);
    btnMute->setCheckable(true);
    btnMute->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   background: transparent;"
        "   border-image: url(':/menú/iconoAltavoz.png') 0 0 0 0 stretch stretch;" //Estira el PNG al tamaño exacto de 50x50
        "}"
        "QPushButton:checked {"
        "   border-image: url(':/menú/iconoAltavoz2.png') 0 0 0 0 stretch stretch;" //Aplica la misma escala al mutear
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.15);"
        "   border-radius: 25px;"
        "}"
        );

    connect(btnMute, &QPushButton::clicked, this, &JoestarChampionship::alternarMutearAudio);
    connect(btnJugar, &QPushButton::clicked, this, &JoestarChampionship::mostrarSeleccionNivel);
    connect(btnAyuda, &QPushButton::clicked, this, [this](){ controladorPantallas->setCurrentIndex(3); });
    connect(btnCreditos, &QPushButton::clicked, this, [this](){ controladorPantallas->setCurrentIndex(4); });
    connect(btnSalir, &QPushButton::clicked, qApp, &QApplication::quit);
}

void JoestarChampionship::crearPantallaSeleccion() {
    pantallaSeleccionNivel = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(pantallaSeleccionNivel);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *subtitulo = new QLabel("SELECCIONA TU DESAFÍO", pantallaSeleccionNivel);
    subtitulo->setStyleSheet("font-family: 'Impact'; font-size: 35px; color: white; margin-bottom: 20px;");
    layout->addWidget(subtitulo);

    QPushButton *btnNivel1 = new QPushButton("NIVEL 1: ENFRENTAMIENTO URBANO", pantallaSeleccionNivel);
    btnNivel2 = new QPushButton("NIVEL 2: JAULA MMA (BLOQUEADO)", pantallaSeleccionNivel);
    QPushButton *btnVolver = new QPushButton("REGRESAR AL MENÚ", pantallaSeleccionNivel);

    QString estiloActivo = "QPushButton { font-family: 'Impact'; font-size: 20px; background-color: rgba(15,15,15,0.85); color: white; border: 2px solid white; padding: 10px; margin: 5px; min-width: 320px; } QPushButton::hover { background-color: darkorange; }";

    btnNivel1->setStyleSheet(estiloActivo);
    btnVolver->setStyleSheet(estiloActivo);

    btnNivel2->setStyleSheet("QPushButton { font-family: 'Impact'; font-size: 20px; background-color: rgba(50,50,50,0.8); color: #777777; border: 2px solid #555555; padding: 10px; margin: 5px; min-width: 320px; }");

    layout->addWidget(btnNivel1);
    layout->addWidget(btnNivel2);
    layout->addWidget(btnVolver);

    connect(btnNivel1, &QPushButton::clicked, this, &JoestarChampionship::seleccionarDificultadNivel1);
    connect(btnNivel2, &QPushButton::clicked, this, &JoestarChampionship::intentarJugarNivel2);
    connect(btnVolver, &QPushButton::clicked, this, &JoestarChampionship::regresarAlMenuPrincipal);
}

void JoestarChampionship::crearPantallaPostJuego() {
    pantallaPostJuego = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(pantallaPostJuego);
    layout->setAlignment(Qt::AlignCenter);

    lblResultadoPostJuego = new QLabel("", pantallaPostJuego);
    lblResultadoPostJuego->setStyleSheet("font-family: 'Impact'; font-size: 60px; margin-bottom: 30px;");
    layout->addWidget(lblResultadoPostJuego);

    QPushButton *btnReintentar = new QPushButton("REINTENTAR ASALTO", pantallaPostJuego);
    QPushButton *btnMenu = new QPushButton("SALIR AL MENÚ PRINCIPAL", pantallaPostJuego);

    QString estilo = "QPushButton { font-family: 'Impact'; font-size: 22px; background-color: black; color: white; border: 2px solid white; padding: 12px; margin: 5px; min-width: 250px; } QPushButton::hover { background-color: white; color: black; }";
    btnReintentar->setStyleSheet(estilo);
    btnMenu->setStyleSheet(estilo);

    layout->addWidget(btnReintentar);
    layout->addWidget(btnMenu);

    connect(btnReintentar, &QPushButton::clicked, this, &JoestarChampionship::reintentarUltimoNivel);
    connect(btnMenu, &QPushButton::clicked, this, &JoestarChampionship::regresarAlMenuPrincipal);
}

void JoestarChampionship::crearPantallaAyuda() {
    pantallaAyuda = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(pantallaAyuda);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *tituloAyuda = new QLabel("GUÍA DE MOVIMIENTOS", pantallaAyuda);
    tituloAyuda->setStyleSheet("font-family: 'Impact'; font-size: 40px; color: gold; margin-bottom: 15px;");
    layout->addWidget(tituloAyuda);

    QLabel *contenido = new QLabel(
        "• A / D o Flechas : Desplazarse Lateralmente\n"
        "• W / S o Flechas : Desplazarse Verticalmente\n"
        "• Espacio         : Saltar / Evadir\n"
        "• J               : Ataque Débil (Combo Rápido)\n"
        "• K               : Cubrirse / Activar Defensa\n"
        "• W + J / S + J   : Ataque Fuerte1 del nivel 2\n"
        "• S + J           : Ataque Fuerte2 del nivel 2\n"
        "• L               : Habilidad Especial del nivel 2\n\n",
        pantallaAyuda
        );
    contenido->setStyleSheet("font-family: 'Consolas'; font-size: 18px; color: white; line-height: 150%; background-color: rgba(10,10,10,0.8); padding: 20px; border: 1px solid #333333;");
    layout->addWidget(contenido);

    QPushButton *btnVolver = new QPushButton("ENTENDIDO", pantallaAyuda);
    QString estiloBoton = "QPushButton { font-family: 'Impact'; font-size: 20px; background-color: rgba(15,15,15,0.85); color: white; border: 2px solid white; padding: 8px; margin-top: 15px; min-width: 180px; } QPushButton::hover { background-color: gold; color: black; border-color: gold; }";
    btnVolver->setStyleSheet(estiloBoton);
    layout->addWidget(btnVolver);

    connect(btnVolver, &QPushButton::clicked, this, &JoestarChampionship::regresarAlMenuPrincipal);
}

void JoestarChampionship::crearPantallaCreditos() {
    pantallaCreditos = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(pantallaCreditos);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *tituloCreditos = new QLabel("CRÉDITOS DE DESARROLLO", pantallaCreditos);
    tituloCreditos->setStyleSheet("font-family: 'Impact'; font-size: 40px; color: gold; margin-bottom: 20px;");
    layout->addWidget(tituloCreditos);

    QLabel *contenido = new QLabel(
        "JOESTAR CHAMPIONSHIP\n\n"
        "Desarrollado de manera nativa utilizando C++ y Qt Framework.\n"
        "Arquitectura de físicas y renderizado basado en QGraphicsScene.\n\n"
        "Todos los derechos de personajes reservados a Hirohiko Araki y David Production.\n\n"
        "Videojuego creado por José Alejandro Pabón Leal y Emanuel Guerra Tuberquia.",
        pantallaCreditos
        );
    contenido->setAlignment(Qt::AlignCenter);
    contenido->setStyleSheet("font-family: 'Impact'; font-size: 18px; color: white; background-color: rgba(10,10,10,0.8); padding: 25px; border: 1px solid #333333;");
    layout->addWidget(contenido);

    QPushButton *btnVolver = new QPushButton("REGRESAR", pantallaCreditos);
    QString estiloBoton = "QPushButton { font-family: 'Impact'; font-size: 20px; background-color: rgba(15,15,15,0.85); color: white; border: 2px solid white; padding: 8px; margin-top: 20px; min-width: 180px; } QPushButton::hover { background-color: gold; color: black; border-color: gold; }";
    btnVolver->setStyleSheet(estiloBoton);
    layout->addWidget(btnVolver);

    connect(btnVolver, &QPushButton::clicked, this, &JoestarChampionship::regresarAlMenuPrincipal);
}

void JoestarChampionship::mostrarSeleccionNivel() {
    if (nivel1Completado) {
        btnNivel2->setText("NIVEL 2: JAULA MMA");
        btnNivel2->setStyleSheet("QPushButton { font-family: 'Impact'; font-size: 20px; background-color: rgba(15,15,15,0.85); color: white; border: 2px solid white; padding: 10px; margin: 5px; min-width: 320px; } QPushButton::hover { background-color: gold; color: black; border-color: gold; }");
    }
    controladorPantallas->setCurrentIndex(1);
}

void JoestarChampionship::seleccionarDificultadNivel1() {
    comenzarNivel1("facil");
}

void JoestarChampionship::intentarJugarNivel2() {
    if (nivel1Completado) {
        comenzarNivel2();
    }
}

void JoestarChampionship::comenzarNivel1(const QString& dificultad) {

    detenerMultimediaMenu();
    gestionarMusicaNivel(1);

    idUltimoNivelJugado = 1;
    dificultadUltimoNivel1 = dificultad;
    nivel1Completado = true;
    capturarFinDelJuego(true);
}

void JoestarChampionship::comenzarNivel2() {
    idUltimoNivelJugado = 2;

    detenerMultimediaMenu();
    gestionarMusicaNivel(2);

    if (nivel2Activo) {
        nivel2Activo->deleteLater();
        nivel2Activo = nullptr;
    }

    QGraphicsView* vistaJuego = new QGraphicsView(this);
    vistaJuego->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vistaJuego->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsScene* escenaCompartida = new QGraphicsScene(this);
    escenaCompartida->setSceneRect(0, 0, 1200, 500);
    vistaJuego->setScene(escenaCompartida);

    Jojo* jugadorJotaro = new Jojo();

    nivel2Activo = new Nivel2(escenaCompartida, jugadorJotaro, this);

    controladorPantallas->addWidget(vistaJuego);
    controladorPantallas->setCurrentWidget(vistaJuego);

    connect(nivel2Activo, &Nivel2::combateTerminado, this, &JoestarChampionship::capturarFinDelJuego);

    nivel2Activo->iniciarNivel();
    this->setFocus();
}

void JoestarChampionship::capturarFinDelJuego(bool victoria) {
    gestionarMusicaNivel(0);
    reproducirMultimediaMenu();

    if (idUltimoNivelJugado == 1 && victoria) {
        nivel1Completado = true;
    }

    if (idUltimoNivelJugado == 2) {
        QWidget* vistaABorrar = controladorPantallas->currentWidget();
        controladorPantallas->setCurrentIndex(2);

        if (vistaABorrar) {
            controladorPantallas->removeWidget(vistaABorrar);
            vistaABorrar->deleteLater();
        }
    } else {
        controladorPantallas->setCurrentIndex(2);
    }

    if (nivel2Activo) {
        nivel2Activo->deleteLater();
        nivel2Activo = nullptr;
    }

    if (victoria) {
        lblResultadoPostJuego->setText("¡VICTORIA ABSOLUTA!");
        lblResultadoPostJuego->setStyleSheet("font-family: 'Impact'; font-size: 55px; color: #00FF00;");
    } else {
        lblResultadoPostJuego->setText("¡GAME OVER!");
        lblResultadoPostJuego->setStyleSheet("font-family: 'Impact'; font-size: 55px; color: #FF0000;");
    }
}

void JoestarChampionship::reintentarUltimoNivel() {
    if (idUltimoNivelJugado == 1) {
        comenzarNivel1(dificultadUltimoNivel1);
    } else {
        comenzarNivel2();
    }
}

void JoestarChampionship::regresarAlMenuPrincipal() {
    reproducirMultimediaMenu();

    controladorPantallas->setCurrentIndex(0);
    this->setFocus();
}

void JoestarChampionship::reiniciarVideoFondo() {
    reproductorVideo->setPosition(0);
    reproductorVideo->play();
}

void JoestarChampionship::detenerMultimediaMenu() {
    if (reproductorVideo) reproductorVideo->stop();
    if (musicaMenu) musicaMenu->stop();
    if (lblVideoFondo) lblVideoFondo->hide();
}

void JoestarChampionship::reproducirMultimediaMenu() {
    if (lblVideoFondo) lblVideoFondo->show();

    if (reproductorVideo && reproductorVideo->playbackState() != QMediaPlayer::PlayingState) {
        reproductorVideo->play();
    }
    if (musicaMenu && musicaMenu->playbackState() != QMediaPlayer::PlayingState) {
        musicaMenu->play();
    }
}

void JoestarChampionship::gestionarMusicaNivel(int nivel) {
    if (musicaNivel1) musicaNivel1->stop();
    if (musicaNivel2) musicaNivel2->stop();

    if (salidaAudioNivel1) salidaAudioNivel1->setVolume(audioMutado ? 0.0 : 0.7);
    if (salidaAudioNivel2) salidaAudioNivel2->setVolume(audioMutado ? 0.0 : 0.7);

    if (nivel == 1 && musicaNivel1) {
        musicaNivel1->play();
    } else if (nivel == 2 && musicaNivel2) {
        musicaNivel2->play();
    }
}

void JoestarChampionship::alternarMutearAudio() {
    // Si el botón está "checked", significa que el usuario quiere mutar el juego
    audioMutado = btnMute->isChecked();

    if (audioMutado) {
        if (salidaAudioMusica) salidaAudioMusica->setVolume(0.0);
        if (salidaAudioNivel1) salidaAudioNivel1->setVolume(0.0);
        if (salidaAudioNivel2) salidaAudioNivel2->setVolume(0.0);
    } else {
        if (salidaAudioMusica) salidaAudioMusica->setVolume(0.7);
        if (salidaAudioNivel1) salidaAudioNivel1->setVolume(0.5);
        if (salidaAudioNivel2) salidaAudioNivel2->setVolume(0.5);
    }
}

void JoestarChampionship::pausarMusicaNivelDetenido() {
    if (musicaNivel2 && musicaNivel2->playbackState() == QMediaPlayer::PlayingState) {
        musicaNivel2->pause();
    }
}

void JoestarChampionship::reanudarMusicaNivelDetenido() {
    if (musicaNivel2 && !audioMutado) {
        musicaNivel2->play();
    }
}

void JoestarChampionship::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_F1) {
        Personaje::modoDebug = !Personaje::modoDebug;
        if (nivel2Activo && controladorPantallas->currentIndex() > 4) {
            if (QGraphicsView* vista = qobject_cast<QGraphicsView*>(controladorPantallas->currentWidget())) {
                if (vista->scene()) vista->scene()->update();
            }
        }
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_F2) {
        nivel1Completado = !nivel1Completado;
        if (controladorPantallas->currentIndex() == 1) {
            mostrarSeleccionNivel();
        }
        event->accept();
        return;
    }

    if (nivel2Activo && controladorPantallas->currentIndex() > 4) {
        teclasPresionadas.insert(event->key());

        Jojo* jojo = nivel2Activo->getJugador();
        if (!jojo) {
            QMainWindow::keyPressEvent(event);
            return;
        }

        switch (event->key()) {
        case Qt::Key_A:
        case Qt::Key_Left:
            if (jojo->getVelocidadX() == 0) jojo->setFrameActual(0);
            jojo->setVelocidadX(-7);
            break;

        case Qt::Key_D:
        case Qt::Key_Right:
            if (jojo->getVelocidadX() == 0) jojo->setFrameActual(0);
            jojo->setVelocidadX(7);
            break;

        case Qt::Key_Space:
            jojo->saltar();
            break;

        case Qt::Key_K:
            jojo->setDefensa(true);
            break;

        case Qt::Key_J:
            if (teclasPresionadas.contains(Qt::Key_W)) {
                jojo->atacarFuerte(1);
            } else if (teclasPresionadas.contains(Qt::Key_S)) {
                jojo->atacarFuerte(2);
            } else {
                jojo->atacar();
            }
            break;

        case Qt::Key_L:
            jojo->habilidadEspecial();
            break;
        }
        event->accept();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

void JoestarChampionship::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    if (nivel2Activo && controladorPantallas->currentIndex() > 4) {
        teclasPresionadas.remove(event->key());

        Jojo* jojo = nivel2Activo->getJugador();
        if (jojo) {
            int key = event->key();
            if (key == Qt::Key_A || key == Qt::Key_Left) {
                if (!teclasPresionadas.contains(Qt::Key_D) && !teclasPresionadas.contains(Qt::Key_Right)) {
                    jojo->setVelocidadX(0);
                }
            }
            else if (key == Qt::Key_D || key == Qt::Key_Right) {
                if (!teclasPresionadas.contains(Qt::Key_A) && !teclasPresionadas.contains(Qt::Key_Left)) {
                    jojo->setVelocidadX(0);
                }
            }

            if (key == Qt::Key_K) {
                jojo->setDefensa(false);
            }
        }
    }
    QMainWindow::keyReleaseEvent(event);
}
