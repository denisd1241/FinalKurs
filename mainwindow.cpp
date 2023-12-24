#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QLabel>
#include <QGridLayout>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QInputDialog>
#include <QRandomGenerator>
#include <limits>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QStringListModel>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->gridLayoutWidget->setStyleSheet("background-color: #ffffff; border: 1px solid #333;");

}

MainWindow::~MainWindow()
{
    delete ui;

}

QVector<QVector<int>> MainWindow::createMatrix(int size) {
    srand(time(0));

    QVector<QVector<int>> matrix(size, QVector<int>(size, 0));
    QCheckBox *check = ui->checkBox;
    if(!check->isChecked()){
        for (int i = 0; i < size; ++i) {
            for (int j = i; j < size; ++j) {
                int randomValue = rand() % 2;
                matrix[i][j] = randomValue;
                matrix[j][i] = randomValue;
            }
        }
    }else
    {
        for (int i = 0; i < size; ++i) {
            for (int j = i; j < size; ++j) {
                matrix[i][j] = rand() % 2;
            }
        }
    }
    return matrix;
}


void MainWindow::clearGridLayout(QGridLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

void MainWindow::drawGraph(const QVector<QVector<int>>& matrix)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    int nodeRadius = 30;

    QVector<QPoint> nodePositions(matrix.size());
    int coord[matrix.size()];
    // Randomly place nodes
    for (int i = 0; i < matrix.size(); ++i) {
        int x = rand () % 400 +100; // рандомная генерация положения вершин
        int y = rand () % 400 +100;
        coord[i] = x;
        nodePositions[i] = QPoint(x, y);

        QGraphicsEllipseItem *node = new QGraphicsEllipseItem(x, y, nodeRadius, nodeRadius);
        QBrush brush(Qt::black);
        node->setBrush(brush);
        scene->addItem(node);

        QGraphicsTextItem *label = new QGraphicsTextItem(QString::number(i + 1));
        label->setPos(nodePositions[i].x() + nodeRadius / 2 - label->boundingRect().width() / 2, nodePositions[i].y() + nodeRadius / 2 - label->boundingRect().height() / 2 - label->boundingRect().y());
        label->setDefaultTextColor(Qt::white);
        QFont font = label->font();
        font.setPointSize(10);
        label->setFont(font);
        scene->addItem(label);
    }
    QVector<QVector<int>> weights = matrix;
    for(int i = 0; i < weights.size(); i++){
        for(int j = 0; j < i; j++){
            if(weights[i][j] == 1){
                weights[i][j] = rand() % 20 - 10;
            }

        }
    }
    QVector<Edge> edgesList;

    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = i; j < matrix.size(); ++j) {

            if(weights[i][j] == 1){
                weights[i][j] = rand() % 20 - 10;
                if (weights[i][j] == 0){
                    weights[i][j] = rand() % 20 - 10;
                }
            }
            qDebug() << weights[i][j];
            if (i != j && matrix[i][j] > 0) {
                Edge edge;
                edge.source = i;
                edge.destination = j;
                edge.weight = weights[i][j];

                edgesList.append(edge);
            }
        }
    }

    connect(ui->fordbell_button, &QPushButton::clicked, this, [=]() {
    on_fordbell_button_clicked(edgesList, matrix.size());
    });
            for (const Edge& edge : edgesList) {
        qDebug() << "Source:" << edge.source << "Destination:" << edge.destination << "Weight:" << edge.weight;
    }
    for (const Edge& edge : edgesList) {
        // Отрисовка линий
        QGraphicsLineItem *line = new QGraphicsLineItem(nodePositions[edge.source].x() + nodeRadius / 2, nodePositions[edge.source].y() + nodeRadius / 2,
                                                        nodePositions[edge.destination].x() + nodeRadius / 2, nodePositions[edge.destination].y() + nodeRadius / 2);
        QPen pen(Qt::black);
        line->setPen(pen);
        scene->addItem(line);

        // Вычисление позиций
        int labelX = (nodePositions[edge.source].x() + nodePositions[edge.destination].x()) / 2;
        int labelY = (nodePositions[edge.source].y() + nodePositions[edge.destination].y()) / 2;

        QGraphicsTextItem *weightLabel = new QGraphicsTextItem(QString::number(edge.weight));
        weightLabel->setPos(labelX - weightLabel->boundingRect().width() / 2, labelY - weightLabel->boundingRect().height() / 2);
        QFont font = weightLabel->font();
        font.setPointSize(10);
        weightLabel->setFont(font);
        scene->addItem(weightLabel);


        weightLabel->setDefaultTextColor(Qt::red);

        if (ui->checkBox->isChecked()) {
            double angle = std::atan2(nodePositions[edge.destination].y() - nodePositions[edge.source].y(), nodePositions[edge.destination].x() - nodePositions[edge.source].x());
            double arrowSize = 10.0;

            QGraphicsPolygonItem *arrowhead = new QGraphicsPolygonItem();
            QPolygonF arrowPolygon;
            arrowPolygon << QPointF(-arrowSize, -arrowSize) << QPointF(0, 0) << QPointF(-arrowSize, arrowSize);
            arrowhead->setPolygon(arrowPolygon);
            arrowhead->setPos(nodePositions[edge.destination].x() + nodeRadius / 2, nodePositions[edge.destination].y() + nodeRadius / 2);
            arrowhead->setRotation(qRadiansToDegrees(angle));
            arrowhead->setBrush(Qt::red);

            scene->addItem(arrowhead);
        }
    }
}



int MainWindow::on_genbutton_clicked()
{
    QGridLayout *gridLayout = ui->gridLayout;
    bool ok;
    int matrixSize = QInputDialog::getInt(this, tr("Введите размер матрицы"), tr("Размер матрицы:"), 3, 1, 100, 1, &ok);

    if (ok) {
        clearGridLayout(gridLayout);
        QVector<QVector<int>> symmetricMatrix = createMatrix(matrixSize);

        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                QLabel *label = new QLabel(QString::number(symmetricMatrix[i][j]));
                label->setStyleSheet("border: none;");
                label->setAlignment(Qt::AlignCenter);
                label->setMinimumSize(30, 30);
                label->setMaximumSize(30, 30);
                gridLayout->addWidget(label, i, j);
            }
        }

        // Рисуем граф
        disconnect(ui->fordbell_button, &QPushButton::clicked, this, nullptr);
        drawGraph(symmetricMatrix);
    }
    return matrixSize;
}



void MainWindow::on_save_clicked()
{
    // Проверяем, была ли сгенерирована матрица
    if (ui->gridLayout->isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Матрица не была сгенерирована."));
        return;
    }

    // Открываем диалог для выбора имени файла
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить матрицу"), "", tr("Текстовые файлы (*.txt);;Все файлы (*)"));

    // Проверяем, был ли файл выбран
    if (fileName.isEmpty())
        return;

    // Открываем файл для записи
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Если не удалось открыть файл, выводим сообщение об ошибке
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл для записи"));
        return;
    }

    // Создаем поток для записи данных в файл
    QTextStream out(&file);

    // Получаем матрицу из интерфейса
    int matrixSize = ui->gridLayout->rowCount();
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            QLabel *label = qobject_cast<QLabel*>(ui->gridLayout->itemAtPosition(i, j)->widget());
            int value = label->text().toInt();
            out << value << " ";
        }
        out << "\n";
    }


    file.close();
}


void MainWindow::on_paste_button_clicked()
{
    // Открываем диалог для выбора имени файла
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выберите файл с матрицей"), "", tr("Текстовые файлы (*.txt);;Все файлы (*)"));

    // Проверяем, был ли файл выбран
    if (fileName.isEmpty())
        return;

    // Открываем файл для чтения
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Если не удалось открыть файл, выводим сообщение об ошибке
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл для чтения"));
        return;
    }

    // Создаем поток для чтения данных из файла
    QTextStream in(&file);

    // Читаем данные из файла и вставляем их в интерфейс
    QVector<QVector<int>> matrix;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(" ", Qt::SkipEmptyParts);
        QVector<int> row;
        for (const QString& value : values) {
            row.append(value.toInt());
        }
        matrix.append(row);
    }

    file.close();

    // Очищаем текущий интерфейс
    clearGridLayout(ui->gridLayout);

    // Вставляем матрицу в интерфейс
    int matrixSize = matrix.size();
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            QLabel *label = new QLabel(QString::number(matrix[i][j]));
            label->setStyleSheet("border: none;");
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumSize(30, 30);
            label->setMaximumSize(30, 30);
            ui->gridLayout->addWidget(label, i, j);
        }
    }

    // Рисуем граф
    drawGraph(matrix);
}

void MainWindow::on_fordbell_button_clicked(const QVector<Edge>& edges, int numVertices)
{
    bool ok = false;
    QVector<int> distance(numVertices, std::numeric_limits<int>::max());
    int src = QInputDialog::getInt(this, tr("Введите начальную вершину"), tr("Вершина:"), 1, 1, numVertices, 1, &ok) - 1;
    int sourceVertex = src;
    QVector<QString> shortestPaths;
    QListView *List = ui->listView;
    distance[sourceVertex] = 0;

    for (int i = 0; i < numVertices - 1; ++i) {
        for (const Edge& edge : edges) {
            if (distance[edge.source] != std::numeric_limits<int>::max() && distance[edge.source] + edge.weight < distance[edge.destination]) {
                distance[edge.destination] = distance[edge.source] + edge.weight;
            }
        }
    }
    shortestPaths.clear();

    // Заполняем вектор shortestPaths результатами
    for (int i = 0; i < numVertices; ++i) {
        QString path = QString("От  %1 вершины к %2 минимальное расстояние: %3").arg(sourceVertex + 1).arg(i + 1).arg(distance[i]);
        shortestPaths.append(path);
    }

    QStringListModel *model = new QStringListModel(shortestPaths);

    // Привязываем модель данных к QListView
    List->setModel(model);
    // Печать результатов
    qDebug() << "Результаты алгоритма Форда-Беллмана:";
    for (int i = 0; i < numVertices; ++i) {
        qDebug() << "От " << sourceVertex + 1 << "вершины к " << i + 1 << " минимальное расстояние: " << distance[i];
    }
}

