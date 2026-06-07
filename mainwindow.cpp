#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "joestarchampionship.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Creamos la ventana principal del juego (la que tiene el StackedWidget y los menús)
    JoestarChampionship *juego = new JoestarChampionship(this);

    // 2. La ponemos como el widget central de MainWindow para que ocupe toda la pantalla
    setCentralWidget(juego);

    // 3. Ajustamos el tamaño de MainWindow al tamaño exacto configurado en el juego
    this->setFixedSize(1200, 550);
}

MainWindow::~MainWindow() {
    delete ui;
}
