#include <QPixmap>
#include <QString>
#include <QMessageBox>
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


    reset();
    render();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reset(){
    ui->theta_le->setText(tr("90"));
    ui->phi_le->setText(tr("0"));
    ui->zoom_le->setText(tr("1"));
    ui->texture_cb->setCheckState(Qt::CheckState::Unchecked);
    mesh_ptr=std::shared_ptr<IndexedMesh>(new IndexedMesh("KAUST_Beacon.obj"));
    center={125,125,125};
    theta=M_PI*(1./2.);
    phi=M_PI*(1./4.);
    eye=center+200*arma::fvec3{sinf(theta)*sinf(phi),cosf(theta),sinf(theta)*cosf(phi)};
    std::cout<<eye<<std::endl;
    light_sources=std::vector<arma::fvec3>();
    light_sources.push_back({0,-500,0});
    light_sources.push_back({200,500,200});
    fov=1.f;
    use_texture=false;
    rasterizer=std::shared_ptr<Rasterizer>(new Rasterizer(mesh_ptr,eye,center,light_sources,fov,"diablo3_pose_diffuse.tga"));

}

void MainWindow::render(){
    rasterizer->set_matrix(eye,center,fov,use_texture);
    QImage qimg=rasterizer->get_image().mirrored();
    ui->imagepane->setPixmap(QPixmap::fromImage(qimg));
}
void MainWindow::on_cam_rot_bt_clicked()
{
    auto theta=ui->theta_le->text().toFloat();
    auto phi=ui->phi_le->text().toFloat();
    theta=theta/180*M_PI;
    phi=phi/180*M_PI;
    if(theta>=0 and theta<=M_PI+1e-3 and phi>=-M_PI-1e-3 and phi<=M_PI+1e-3){
        phi+=M_PI/4;
        eye=center+200*arma::fvec3{sinf(theta)*sinf(phi),cosf(theta),sinf(theta)*cosf(phi)};
        render();
    } else{
        QMessageBox::warning(this,tr(" "),tr("Angle value out of range"));
    }
}

void MainWindow::on_zoom_bt_clicked()
{
    float val=ui->zoom_le->text().toFloat();
    val=1/val;
    if(val<=3 and val>=0.3){
        fov=val;
        render();
    } else{
        QMessageBox::warning(this,tr(" "),tr("Zoom value out of range"));
    }
}

void MainWindow::on_reset_bt_clicked()
{
    reset();
    render();
}

void MainWindow::on_texture_bt_clicked()
{
    if(ui->texture_cb->isChecked()){
        use_texture=true;
    } else{
        use_texture=false;
    }
    render();
}
