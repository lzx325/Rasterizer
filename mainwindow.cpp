#include <QPixmap>
#include <QString>
#include <armadillo>
#include <IndexedMesh.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <cmath>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "rasterizer.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    using namespace std;

    ui->setupUi(this);

    this->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);

    auto mesh_ptr=std::shared_ptr<IndexedMesh>(new IndexedMesh("KAUST_Beacon.obj"));
    arma::fvec3 center={125,125,125};
    float theta=M_PI*(1./2.), phi=M_PI*(1./4.);
    arma::fvec3 eye=center+200*arma::fvec3{sinf(theta)*sinf(phi),cosf(theta),sinf(theta)*cosf(phi)};
    std::cout<<eye<<std::endl;
    std::vector<arma::fvec3> light_sources;
    light_sources.push_back({0,-500,0});
    light_sources.push_back({200,500,200});
//    light_sources.push_back({500,500,0});
    float fov=1.f;
    Rasterizer rasterizer(mesh_ptr,eye,center,light_sources,fov);
    QImage qimg=rasterizer.get_image().mirrored();
    ui->imagepane->setPixmap(QPixmap::fromImage(qimg));
}

MainWindow::~MainWindow()
{
    delete ui;
}

