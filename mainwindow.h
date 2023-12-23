#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
struct Edge {
    int source;
    int destination;
    int weight;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:

    int on_genbutton_clicked();
    void on_save_clicked();
    void on_paste_button_clicked();

private:
    Ui::MainWindow *ui;
    QGridLayout *gridLayout_2;
    void clearGridLayout(QGridLayout *layout);
    void drawGraph(const QVector<QVector<int>>& matrix);
    void displayWeightMatrix(const QVector<QVector<int>>& matrix);
    QVector<QVector<int>> createSymmetricMatrix(int size);
      void bellmanFord(const QVector<Edge>& edges, int numVertices, int sourceVertex);


};
#endif // MAINWINDOW_H
