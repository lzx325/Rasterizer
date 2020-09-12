#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <memory>
#include <armadillo>
#include <QMainWindow>
#include "IndexedMesh.h"
#include "rasterizer.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void reset();
    void render();

private slots:
    void on_cam_rot_bt_clicked();

    void on_zoom_bt_clicked();

    void on_reset_bt_clicked();

    void on_texture_bt_clicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<IndexedMesh> mesh_ptr;
    arma::fvec3 center;
    arma::fvec3 eye;
    std::vector<arma::fvec3> light_sources;
    float theta,phi,fov;
    std::shared_ptr<Rasterizer> rasterizer;
    bool use_texture;

};
#endif // MAINWINDOW_H
