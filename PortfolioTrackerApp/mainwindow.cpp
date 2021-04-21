#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <string>
#include <QString>

QString coin;
double quantity, price, TotalSpent;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->TransactionWidget->hide();
    ui->TransferFrame->hide();
    ui->ColorChangeFrame->hide();

    QStringList list_items;
    list_items<<"Bitcoin BTC"<<"Ethereum ETH"<<"Chainlink LINK"<<"Algorand ALGO"<<"Uniswap UNI"<<"Cardano ADA"<<"Polkadot DOT"<<"Cosmos ATOM";
    ui->comboBox->addItems(list_items);
    ui->TotalSpent->setAcceptDrops(false);

    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(managerFinished(QNetworkReply*)));

    connect(ui->AddTransaction, SIGNAL(released()), this, SLOT(AddTransaction()));
    connect(ui->Cross, SIGNAL(released()), this, SLOT(Cross()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddTransaction()
{
    ui->TransactionWidget->show();
    ui->TransactionWidget->raise();
    ui->ColorChangeFrame->show();
}

void MainWindow::Cross(){
    ui->ColorChangeFrame->hide();
    ui->TransactionWidget->close();
    ui->CoinPrice->clear();
    ui->Quantity->clear();
    ui->Quantity_2->clear();
}


void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    QRegularExpression rx(" ");
    QStringList query = arg1.split(rx);
    coin = query[0].toLower();
    qDebug()<<coin;
    QString url = "https://api.coingecko.com/api/v3/simple/price?ids="+coin+"&vs_currencies=usd";
    request.setUrl(QUrl(url));
    manager->get(request);
}

void MainWindow::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }

        QString answer = (QString)reply->readAll();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());

        QJsonObject jsonObject = jsonResponse.object();

        QJsonValue value = jsonObject.value(QString(coin));

        qDebug() << value;

        QJsonObject item = value.toObject();

        qDebug() << item;

        QJsonValue usd = item["usd"];

        qDebug() << usd;

        double price = usd.toDouble();

        ui->CoinPrice->setText(QString::number(price));
}

void MainWindow::on_Quantity_textChanged(const QString &arg1)
{

    quantity = arg1.toDouble();
//    QString bucks = ui->CoinPrice->text();
//    QRegularExpression space(" ");
//    QStringList list = bucks.split(space);
//    price = list[1].toDouble();
    price = ui->CoinPrice->text().toDouble();
    TotalSpent = quantity*price;
    ui->TotalSpent->setText("$"+QString::number(TotalSpent));
}

void MainWindow::on_CoinPrice_textChanged(const QString &arg1)
{
    quantity = ui->Quantity->text().toDouble();
//    QString bucks = arg1;
//    QRegularExpression space(" ");
//    QStringList list = bucks.split(space);
//    price = list[1].toDouble();
    price = arg1.toDouble();
    TotalSpent = quantity*price;
    ui->TotalSpent->setText("$"+QString::number(TotalSpent));
}

void MainWindow::on_buy_clicked()
{
    ui->Sell->setChecked(false);
    ui->Transfer->setChecked(false);
    if(ui->buy->isChecked()){
        ui->TransferFrame->close();
        ui->Total_label->setText("Total Spent");
        ui->buy->setStyleSheet("background-color: #171924;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
        ui->Sell->setStyleSheet("background-color: #323546;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
        ui->Transfer->setStyleSheet("background-color: #323546;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
    }
    if(!(ui->buy->isChecked())){
        ui->buy->setStyleSheet("background-color: #323546;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
    }
    test();
}

void MainWindow::on_Sell_clicked()
{
    ui->buy->setChecked(false);
    ui->Transfer->setChecked(false);
    if(ui->Sell->isChecked()){
        ui->TransferFrame->close();
        ui->Total_label->setText("Total Recieved");
        ui->Sell->setStyleSheet("background-color: #171924;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
        ui->buy->setStyleSheet("background-color: #323546;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
        ui->Transfer->setStyleSheet("background-color: #323546;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
    }
    if(!(ui->Sell->isChecked())){
        ui->Sell->setStyleSheet("background-color: #323546;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
    }
    test();
}

void MainWindow::on_Transfer_clicked()
{
    ui->Sell->setChecked(false);
    ui->buy->setChecked(false);
    if(ui->Transfer->isChecked()){
        ui->TransferFrame->show();
        ui->Transfer->setStyleSheet("background-color: #171924;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
        ui->Sell->setStyleSheet("background-color: #323546;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
        ui->buy->setStyleSheet("background-color: #323546;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
    }
    if(!(ui->Transfer->isChecked())){
        ui->Transfer->setStyleSheet("background-color: #323546;border: none;border-radius: 8px;"
                               "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                "font-size: 14px;	font-weight: 600;color: white;margin:5px;");
    }
    test();
}

void MainWindow::test(){
    if(ui->buy->isChecked()){
        qDebug()<<"buy checked";
    }
    if(ui->Sell->isChecked()){
        qDebug()<<"Sell checked";
    }
    if(ui->Transfer->isChecked()){
        qDebug()<<"Transfer checked";
    }
    if(ui->ConvertBtn->isChecked()){
        qDebug()<<"Convert checked";
    }
    if(ui->StatisticsBtn->isChecked()){
        qDebug()<<"Stats checked";
    }
}

void MainWindow::on_AddTransaction_2_clicked()
{
    ui->TransactionWidget->close();
    ui->ColorChangeFrame->hide();
    ui->CoinPrice->clear();
    ui->Quantity->clear();
    ui->Quantity_2->clear();
}


void MainWindow::on_StatisticsBtn_clicked()
{
    ui->ConvertBtn->setChecked(false);
    ui->StatsFrame->show();
    if(ui->StatisticsBtn->isChecked()){
        ui->StatisticsBtn->setStyleSheet("background-color: #323546;border:none;border: 1px solid #a1a7bb;border-radius: 8px;"
                                      "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                      "color: white;	margin:5px;");
        ui->ConvertBtn->setStyleSheet("background-color: #222531;border:none;border-radius: 8px;"
                                      "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                      "color: #a1a7bb;	margin:5px;");
    }
    if(!ui->StatisticsBtn->isChecked()){
        ui->StatisticsBtn->setStyleSheet("background-color: #222531;border:none;;border-radius: 8px;"
                                      "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                      "color: #a1a7bb;	margin:5px;");
    }
    test();
}

void MainWindow::on_ConvertBtn_clicked()
{
    ui->StatisticsBtn->setChecked(false);
    ui->StatsFrame->close();
    if(ui->ConvertBtn->isChecked()){
        ui->ConvertBtn->setStyleSheet("background-color: #323546;border:none;border: 1px solid #a1a7bb;border-radius: 8px;"
                                      "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                      "color: white;	margin:5px;");
        ui->StatisticsBtn->setStyleSheet("background-color: #222531;border:none;border-radius: 8px;"
                                      "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                      "color: #a1a7bb;	margin:5px;");
    }
    if(!ui->ConvertBtn->isChecked()){
        ui->ConvertBtn->setStyleSheet("background-color: #222531;border:none;;border-radius: 8px;"
                                      "padding: 0px 24px;word-spacing: normal;text-decoration: none;"
                                      "color: #a1a7bb;	margin:5px;");
    }
    test();
}
