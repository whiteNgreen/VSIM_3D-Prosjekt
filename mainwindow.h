#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QWidget;
class RenderWindow;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    ///Slot called from the File->Exit menu in the mainwindow.ui file
    void on_fileExit_triggered();

    void on_GameMode_toggled(bool checked);

    void on_PauseButton_clicked(bool checked);

private:
    ///called from the constructor. Initializes different parts of the program.
    void init();

    Ui::MainWindow *ui{nullptr};                //the GUI of the app
    QWidget *mRenderWindowContainer{nullptr};   //Qt container for the RenderWindow
    RenderWindow *mRenderWindow{nullptr};       //The class that actually renders OpenGL

    //Logger class uses private ui pointer from this class
    friend class Logger;

    QString t_GameMode{"Editor Mode"};
    QString t_Paused{"Paused"};
};

#endif // MAINWINDOW_H
