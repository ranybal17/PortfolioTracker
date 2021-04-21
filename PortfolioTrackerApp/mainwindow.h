#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <GL/GLU.h>
#include <QSurfaceFormat>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonValue>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void test();

private slots:
    void AddTransaction();
    void Cross();
    void managerFinished(QNetworkReply *reply);
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_Quantity_textChanged(const QString &arg1);

    void on_CoinPrice_textChanged(const QString &arg1);

    void on_buy_clicked();

    void on_Sell_clicked();

    void on_Transfer_clicked();

    void on_AddTransaction_2_clicked();

    void on_StatisticsBtn_clicked();

    void on_ConvertBtn_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
};
#endif // MAINWINDOW_H
