#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <string>
#include <QString>
#include <QListView>
#include <QTableWidget>
#include <QPalette>
#include <QFile>

QString coin;int count = 0;
QString coin_update[1000], request_coin;
double quantity, price = 0, TotalSpent, price_update;
int row = 0;
double mod(double);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    warning  = new QLabel(ui->TransactionFrame);
    warning->setStyleSheet("color:red;margin-left:16px;margin-top:5px;font-weight:600;font-size:10px;");
    warning->hide();

    ui->TransactionWidget->hide();
    ui->TransferFrame->hide();
    ui->ColorChangeFrame->hide();

    QStringList list_items;//populate Combo Box...
    list_items<<"Bitcoin BTC"<<"Ethereum ETH"<<"Chainlink LINK"<<"Algorand ALGO"<<"Uniswap UNI"<<"Cardano ADA"<<"Polkadot DOT"<<"Cosmos ATOM";
    ui->comboBox->addItems(list_items);
    QFile inputFile("C:/Users/ranyb/Documents/Qt_Projects/PortfolioTrackerApp/supported_coins.txt");
    if(inputFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"file open";
        QTextStream in (&inputFile);
        while(!in.atEnd())
        {
            QString line = in.readLine();
            for(int i=0; i<list_items.length(); i++)
            {
                if(QString::compare(line, list_items[i], Qt::CaseInsensitive) != 0)
                {
                    ui->comboBox->addItem(line);
                    break;
                }
            }
        }
        inputFile.close();
    }
    ui->TotalSpent->setAcceptDrops(false);
    ui->ToQuantity->setAcceptDrops(false);
    ui->tableWidget->setUpdatesEnabled(true);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setCursor(Qt::PointingHandCursor);

    ui->tableWidget->setColumnCount(4);
    QStringList labels;
    labels<<"Name"<<"Price"<<"Holdings"<<"Profit/Loss";
    ui->tableWidget->setHorizontalHeaderLabels(labels);
    ui->tableWidget->setColumnWidth(0,250);
    ui->tableWidget->setColumnWidth(1,130);
    ui->tableWidget->setColumnWidth(2,131);
    ui->tableWidget->setColumnWidth(3,130);

    ListView();

    timer = new QTimer(this);
    QObject::connect(timer , SIGNAL(timeout()), this, SLOT(update()));
    timer->start(3000);

    manager = new QNetworkAccessManager();
    update_price = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(managerFinished(QNetworkReply*)));
    QObject::connect(update_price, SIGNAL(finished(QNetworkReply*)), this, SLOT(getPrice(QNetworkReply*)));
    QObject::connect(ui->AddTransaction, SIGNAL(released()), this, SLOT(AddTransaction()));
    QObject::connect(ui->Cross, SIGNAL(released()), this, SLOT(Cross()));
    QObject::connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxIndexChanged()));

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
    ui->comboBox->setCurrentIndex(0);
    ui->Sell->setChecked(false);
    ui->buy->setChecked(false);
    ui->Transfer->setChecked(false);
    ui->Sell->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
    ui->buy->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
     ui->Transfer->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
    warning->hide();
}

void MainWindow::comboBoxIndexChanged()
{
    QRegularExpression rx(" ");
    QStringList query = ui->comboBox->itemText(ui->comboBox->currentIndex()).split(rx);
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
        price = usd.toDouble();
        qDebug()<<price;
        ui->CoinPrice->setText(QString::number(price));
}

void MainWindow::on_Quantity_textChanged(const QString &arg1)
{
    quantity = arg1.toDouble();
//    QString bucks = ui->CoinPrice->text();
//    QRegularExpression space(" ");
//    QStringList list = bucks.split(space);
//    price = list[1].toDouble();
    TotalSpent = (quantity)*ui->CoinPrice->text().toDouble();
    ui->TotalSpent->setText("$"+QString::number(TotalSpent));
}

void MainWindow::on_CoinPrice_textChanged(const QString &arg1)
{
    quantity = ui->Quantity->text().toDouble();
//    QString bucks = arg1;
//    QRegularExpression space(" ");
//    QStringList list = bucks.split(space);
//    price = list[1].toDouble();
    TotalSpent = (quantity)*arg1.toDouble();
    ui->TotalSpent->setText("$"+QString::number(TotalSpent));
}

void MainWindow::on_buy_clicked()
{
    ui->Sell->setChecked(false);
    ui->Transfer->setChecked(false);
    if(ui->buy->isChecked()){
        ui->TransferFrame->close();
        ui->Total_label->setText("Total Spent");
        ui->buy->setStyleSheet("QPushButton{background-color: #171924;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
        ui->Sell->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
        ui->Transfer->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
    }
    if(!(ui->buy->isChecked())){
        ui->buy->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
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
        ui->Sell->setStyleSheet("QPushButton{background-color: #171924;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
        ui->buy->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
        ui->Transfer->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
    }
    if(!(ui->Sell->isChecked())){
        ui->Sell->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
    }
    test();
}

void MainWindow::on_Transfer_clicked()
{
    ui->Sell->setChecked(false);
    ui->buy->setChecked(false);
    if(ui->Transfer->isChecked()){
        ui->TransferFrame->show();
        ui->Transfer->setStyleSheet("QPushButton{background-color: #171924;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
        ui->Sell->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
        ui->buy->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
    }
    if(!(ui->Transfer->isChecked())){
        ui->Transfer->setStyleSheet("QPushButton{background-color: #323546;border: none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;font-size: 14px;	font-weight: 600;color: white;margin:5px;}QPushButton:hover{background-color: #171924;margin:5px;}");
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
    // variable definatoins...
    int x = 2;int index;double profitloss_prior;
    coin = ui->comboBox->itemText(ui->comboBox->currentIndex());
    quantity = ui->Quantity->text().toDouble();
    double profitloss = (quantity*price) - (quantity*(ui->CoinPrice->text().toDouble()));

    //warning messages...
    if(!ui->buy->isChecked()&&!ui->Sell->isChecked()&&!ui->Transfer->isChecked()){

        warning->setText("Please select a transaction type(Buy, Sell or Transfer)");
        warning->show();
    }
    else if(QString::compare(coin, "Select coin", Qt::CaseInsensitive) == 0){
        warning->setText("Please Select a coin from drop down");
        warning->show();
    }
    // buy function...
    if (ui->buy->isChecked() && QString::compare(coin, "Select coin", Qt::CaseInsensitive) != 0){
        // if there are rows present in table we search for the coin in those rows and x=0 only if the coin is found.
        if(ui->tableWidget->rowCount()){
            for(int i=0; i<ui->tableWidget->rowCount(); i++){
                x = QString::compare(coin, ui->tableWidget->item(i,0)->text(), Qt::CaseInsensitive);
                if(x == 0){
                    index = i;//save the row number where coin is found.
                    break;
                }
            }
        }
        //now if the coin is found we add to existing row instead of making new one.
        if(x == 0){
            if(ui->tableWidget->item(index,3)->text().startsWith("-")){
                profitloss_prior = (-1)*ui->tableWidget->item(index,3)->text().remove(0, 2).toDouble();
            }
            else{
                profitloss_prior = ui->tableWidget->item(index,3)->text().remove(0, 2).toDouble();
            }
             QRegularExpression rx("\n");
             QStringList split_index = ui->tableWidget->item(index,2)->text().split(rx);
//           double holdings_prior = split_index[0].replace("$", "").toDouble();
             double quantity_prior = split_index[1].replace(")","").replace("(","").toDouble();
             quantity += quantity_prior;
             profitloss += profitloss_prior;
             ui->tableWidget->item(index,2)->setText("$"+QString::number(quantity*price)+"\n("+QString::number(quantity)+")");
             if(profitloss>=0){
                   ui->tableWidget->item(index,3)->setText("+$"+QString::number(mod(profitloss)));
                   ui->tableWidget->item(index,3)->setForeground(QColor(0,230,0));
             }
             else{
                   ui->tableWidget->item(index,3)->setText("-$"+QString::number(mod(profitloss)));
                   ui->tableWidget->item(index,3)->setForeground(QColor(230,0,0));
             }
       }

       //if there are no rows present in table or coin wasnt found in any row, we make a new row with new items.
       else{
           row = ui->tableWidget->rowCount();
           ui->tableWidget->insertRow(ui->tableWidget->rowCount());
           Name = new QTableWidgetItem;
           Price = new QTableWidgetItem;
           Holdings = new QTableWidgetItem;
           ProfitLoss = new QTableWidgetItem;
           Name->setText(ui->comboBox->itemText(ui->comboBox->currentIndex()));
           Price->setText("$"+QString::number(price));
           Holdings->setText("$"+QString::number(quantity*price)+"\n("+QString::number(ui->Quantity->text().toDouble())+")");
           if(profitloss>=0){
              ProfitLoss->setText("+$"+QString::number(mod(profitloss)));
              ProfitLoss->setForeground(QColor(0,230,0));
           }
           else {
              ProfitLoss->setText("-$"+QString::number(mod(profitloss)));
              ProfitLoss->setForeground(QColor(230,0,0));
           }

         ui->tableWidget->setItem(row, 0, Name);
         ui->tableWidget->setItem(row, 1, Price);
         ui->tableWidget->setItem(row, 2, Holdings);
         ui->tableWidget->setItem(row, 3, ProfitLoss);

         ui->tableWidget->setRowHeight(row,50);
         row++;
        }
         Cross();
    }
    //sell function...
    if(ui->Sell->isChecked() && QString::compare(coin, "Select coin", Qt::CaseInsensitive) != 0){
        // if there are rows present in table we search for the coin in those rows and x=0 only if the coin is found.
        if(ui->tableWidget->rowCount()){
            for(int i=0; i<ui->tableWidget->rowCount(); i++){
                x = QString::compare(coin, ui->tableWidget->item(i,0)->text(), Qt::CaseInsensitive);
                if(x == 0){
                    index = i;//save the row number where coin is found.
                    break;
                }
            }
        }
        if(x == 0){
            if(ui->tableWidget->item(index,3)->text().startsWith("-")){
                profitloss_prior = (-1)*ui->tableWidget->item(index,3)->text().remove(0, 2).toDouble();
            }
            else{
                profitloss_prior = ui->tableWidget->item(index,3)->text().remove(0, 2).toDouble();
            }
            QRegularExpression newline("\n");
            QStringList splitstring = ui->tableWidget->item(index,2)->text().split(newline);
            double quantity_prior = splitstring[1].replace("(","").replace(")","").toDouble();
            quantity = quantity_prior - quantity;
            profitloss = profitloss_prior - profitloss;
            ui->tableWidget->item(index,2)->setText("$"+QString::number(quantity*price)+"\n("+QString::number(quantity)+")");
            if(profitloss>=0){
                  ui->tableWidget->item(index,3)->setText("+$"+QString::number(mod(profitloss)));
                  ui->tableWidget->item(index,3)->setForeground(QColor(0,230,0));
            }
            else{
                  ui->tableWidget->item(index,3)->setText("-$"+QString::number(mod(profitloss)));
                  ui->tableWidget->item(index,3)->setForeground(QColor(230,0,0));
            }
            Cross();
        }
        else{
            warning->setText("Please add buy transaction for "+coin+" first.");
            warning->show();
        }
    }
    //Transfer function...
    if(ui->Transfer->isChecked() && QString::compare(coin, "Select coin", Qt::CaseInsensitive) != 0){
        // if there are rows present in table we search for the coin in those rows and x=0 only if the coin is found.
        if(ui->tableWidget->rowCount()){
            for(int i=0; i<ui->tableWidget->rowCount(); i++){
                x = QString::compare(coin, ui->tableWidget->item(i,0)->text(), Qt::CaseInsensitive);
                if(x == 0){
                    index = i;//save the row number where coin is found.
                    break;
                }
            }
        }

        int TransferIn = QString::compare(ui->TransferBox->currentText(), "Transfer In", Qt::CaseInsensitive);
        if(x == 0){
            if(TransferIn == 0){
                quantity = ui->Quantity_2->text().toDouble();
                profitloss = quantity*price;
                if(ui->tableWidget->item(index,3)->text().startsWith("-")){
                    profitloss_prior = (-1)*ui->tableWidget->item(index,3)->text().remove(0, 2).toDouble();
                }
                else{
                    profitloss_prior = ui->tableWidget->item(index,3)->text().remove(0, 2).toDouble();
                }
                QRegularExpression newline("\n");
                QStringList SplitList = ui->tableWidget->item(index,2)->text().split(newline);
                double quantity_prior = SplitList[1].replace("(","").replace(")","").toDouble();
                quantity += quantity_prior;
                profitloss += profitloss_prior;
                ui->tableWidget->item(index,2)->setText("$"+QString::number(quantity*price)+"\n("+QString::number(quantity)+")");
                if(profitloss>=0){
                      ui->tableWidget->item(index,3)->setText("+$"+QString::number(mod(profitloss)));
                      ui->tableWidget->item(index,3)->setForeground(QColor(0,230,0));
                }
                else{
                      ui->tableWidget->item(index,3)->setText("-$"+QString::number(mod(profitloss)));
                      ui->tableWidget->item(index,3)->setForeground(QColor(230,0,0));
                }

            }
            else{
                quantity = ui->Quantity_2->text().toDouble();
                profitloss = quantity*price;
                if(ui->tableWidget->item(index,3)->text().startsWith("-")){
                    profitloss_prior = (-1)*ui->tableWidget->item(index,3)->text().remove(0, 2).toDouble();
                }
                else{
                    profitloss_prior = ui->tableWidget->item(index,3)->text().remove(0, 2).toDouble();
                }
                QRegularExpression newline("\n");
                QStringList SplitList = ui->tableWidget->item(index,2)->text().split(newline);
                double quantity_prior = SplitList[1].replace("(","").replace(")","").toDouble();
                quantity = quantity_prior - quantity;
                profitloss = profitloss_prior - profitloss;
                ui->tableWidget->item(index,2)->setText("$"+QString::number(quantity*price)+"\n("+QString::number(quantity)+")");
                if(profitloss>=0){
                      ui->tableWidget->item(index,3)->setText("+$"+QString::number(mod(profitloss)));
                      ui->tableWidget->item(index,3)->setForeground(QColor(0,230,0));
                }
                else{
                      ui->tableWidget->item(index,3)->setText("-$"+QString::number(mod(profitloss)));
                      ui->tableWidget->item(index,3)->setForeground(QColor(230,0,0));
                }
            }
            Cross();
        }

        else{
            if(TransferIn == 0){
                quantity = ui->Quantity_2->text().toDouble();
                profitloss = quantity*price;
                row = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                Name = new QTableWidgetItem;
                Price = new QTableWidgetItem;
                Holdings = new QTableWidgetItem;
                ProfitLoss = new QTableWidgetItem;
                Name->setText(ui->comboBox->itemText(ui->comboBox->currentIndex()));
                Price->setText("$"+QString::number(price));
                Holdings->setText("$"+QString::number(quantity*price)+"\n("+QString::number(ui->Quantity_2->text().toDouble())+")");
                if(profitloss>=0){
                   ProfitLoss->setText("+$"+QString::number(mod(profitloss)));
                   ProfitLoss->setForeground(QColor(0,230,0));
                }
                else {
                   ProfitLoss->setText("-$"+QString::number(mod(profitloss)));
                   ProfitLoss->setForeground(QColor(230,0,0));
                }

              ui->tableWidget->setItem(row, 0, Name);
              ui->tableWidget->setItem(row, 1, Price);
              ui->tableWidget->setItem(row, 2, Holdings);
              ui->tableWidget->setItem(row, 3, ProfitLoss);

              ui->tableWidget->setRowHeight(row,50);
              row++;
              Cross();
            }

           else{
                warning->setText("Please add buy transaction for "+coin+" first.");
                warning->show();
            }
        }
    }
}

void MainWindow::on_StatisticsBtn_clicked()
{
    ui->ConvertBtn->setChecked(false);
    ui->StatsFrame->show();
    if(ui->StatisticsBtn->isChecked()){
        ui->StatisticsBtn->setStyleSheet("QPushButton{background-color: white;border:none;border: 1px solid #a1a7bb;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;color: #323546;	margin:5px;}QPushButton:hover{color:#323546;	background-color:white;	margin:5px;}");
        ui->ConvertBtn->setStyleSheet("QPushButton{background-color: #2c303f;border:none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;color: #a1a7bb;	margin:5px;}QPushButton:hover{color:#323546;	background-color:white;	margin:5px;}");
    }
    if(!ui->StatisticsBtn->isChecked()){
        ui->StatisticsBtn->setStyleSheet("QPushButton{background-color: #2c303f;border:none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;color: #a1a7bb;	margin:5px;}QPushButton:hover{color:#323546;	background-color:white;	margin:5px;}");
    }
    test();
}

void MainWindow::on_ConvertBtn_clicked()
{
    ui->StatisticsBtn->setChecked(false);
    ui->StatsFrame->close();
    if(ui->ConvertBtn->isChecked()){
        ui->ConvertBtn->setStyleSheet("QPushButton{background-color: white;border:none;border: 1px solid #a1a7bb;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;color: #323546;	margin:5px;}QPushButton:hover{color:#323546;	background-color:white;	margin:5px;}");
        ui->StatisticsBtn->setStyleSheet("QPushButton{background-color: #2c303f;border:none;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;color: #a1a7bb;	margin:5px;}QPushButton:hover{color:#323546;	background-color:white;	margin:5px;}");
    }
    if(!ui->ConvertBtn->isChecked()){
        ui->ConvertBtn->setStyleSheet("QPushButton{background-color: #2c303f;border:none;;border-radius: 8px;padding: 0px 24px;word-spacing: normal;text-decoration: none;color: #a1a7bb;	margin:5px;}QPushButton:hover{color:#323546;	background-color:white;	margin:5px;}");
    }
    test();
}

void MainWindow::ListView()
{
    QListView * listView1 = new QListView(ui->comboBox);
    listView1->setStyleSheet("QListView{ background-color:rgb(30, 30, 30);border:none;}QListView::item {color:white; margin:3px; border:none;background-color:rgb(30, 30, 30);border-radius:8px;font-weight:600;font-size:14px; text-decoration:none;padding-top:5px;padding-bottom:5px;} QListView::item:selected { margin:3px; background-color:#323546;}");
    ui->comboBox->setView(listView1);

    QListView * listView2 = new QListView(ui->ConvertComboBox);
    listView2->setStyleSheet("QListView{ background-color:rgb(30, 30, 30);border:none;}QListView::item {color:white; margin:3px; border:none;background-color:rgb(30, 30, 30);border-radius:8px;font-weight:600;font-size:14px; text-decoration:none;padding-top:5px;padding-bottom:5px;} QListView::item:selected { margin:3px;background-color:#323546; }");
    ui->ConvertComboBox->setView(listView2);

    QListView * listView3 = new QListView(ui->ToComboBox);
    listView3->setStyleSheet("QListView{ background-color:rgb(30, 30, 30);border:none;}QListView::item {color:white; margin:3px; border:none;background-color:rgb(30, 30, 30);border-radius:8px;font-weight:600;font-size:14px; text-decoration:none;padding-top:5px;padding-bottom:5px;}QListView::item:selected { margin:3px;background-color:#323546;}");
    ui->ToComboBox->setView(listView3);

    QListView * listView4 = new QListView(ui->TransferBox);
    listView4->setStyleSheet("QListView{ background-color:rgb(30, 30, 30);border:none;}QListView::item {color:white; margin:3px; border:none;background-color:rgb(30, 30, 30);border-radius:8px;font-weight:600;font-size:14px; text-decoration:none;padding-top:5px;padding-bottom:5px;}QListView::item:selected {margin:3px;background-color:#323546;}");
    ui->TransferBox->setView(listView4);
}

void MainWindow::update()
{
    QRegularExpression space(" ");
    QString temp, url;
    QStringList splitlist;

    if(count<ui->tableWidget->rowCount()){

        temp = ui->tableWidget->item(count,0)->text();
        splitlist = temp.split(space);
        coin_update[count] = splitlist[0].toLower();
        request_coin = coin_update[count];
        qDebug()<<"Crypto: "+request_coin;
        url = "https://api.coingecko.com/api/v3/simple/price?ids="+request_coin+"&vs_currencies=usd";
        request.setUrl(QUrl(url));
        update_price->get(request);
        count++;
    }
    else{
        count = 0;
    }

}

void MainWindow::getPrice(QNetworkReply *reply)
{
    if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        QString answer = (QString)reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        QJsonValue value = jsonObject.value(QString(request_coin));
        qDebug() << value;
        QJsonObject item = value.toObject();
        qDebug() << item;
        QJsonValue usd = item["usd"];
        qDebug() << usd;
        price_update = usd.toDouble();
        qDebug()<<price_update;
        double profitloss_prior = 0,quantity,holding_prior,holding_update,profit_update = 0,current_balance = 0;
        if(price_update != 0){
            for(int i=0; i<ui->tableWidget->rowCount(); i++){
                QString text = ui->tableWidget->item(i,0)->text();
                QRegularExpression rx(" ");
                QStringList list = text.split(rx);
                int x = QString::compare(request_coin, list[0].toLower(), Qt::CaseInsensitive);
                if(x == 0){
                    qDebug()<<"found row for "<<request_coin<<": "<<i;
                    ui->tableWidget->item(i,1)->setText("$"+QString::number(price_update));
                    if(ui->tableWidget->item(i,3)->text().startsWith("-")){
                        profitloss_prior = (-1)*ui->tableWidget->item(i,3)->text().remove(0, 2).toDouble();
                    }
                    else{
                        profitloss_prior = ui->tableWidget->item(i,3)->text().remove(0, 2).toDouble();
                    }
                    QString holdings = ui->tableWidget->item(i,2)->text();
                    QStringList split = holdings.split(QRegularExpression("\n"));
                    quantity = split[1].replace("(","").replace(")","").toDouble();
                    holding_prior = split[0].replace("$","").toDouble();
                    holding_update = quantity*price_update;
                    profit_update = holding_update - holding_prior;
                    profit_update += profitloss_prior;
                    ui->tableWidget->item(i,2)->setText("$"+QString::number(holding_update)+"\n("+QString::number(quantity)+")");
                    if(profit_update>=0){
                          ui->tableWidget->item(i,3)->setText("+$"+QString::number(mod(profit_update)));
                          ui->tableWidget->item(i,3)->setForeground(QColor(0,230,0));
                    }
                    else{
                          ui->tableWidget->item(i,3)->setText("-$"+QString::number(mod(profit_update)));
                          ui->tableWidget->item(i,3)->setForeground(QColor(230,0,0));
                    }
                }
                QStringList update = ui->tableWidget->item(i,2)->text().split(QRegularExpression("\n"));
                current_balance  += update[0].replace("$","").toDouble();
                if(ui->tableWidget->item(i,3)->text().startsWith("-")){
                    profitloss_prior = (-1)*ui->tableWidget->item(i,3)->text().remove(0, 2).toDouble();
                }
                else{
                    profitloss_prior = ui->tableWidget->item(i,3)->text().remove(0, 2).toDouble();
                }
                qDebug()<<"prior: "<<profitloss_prior<<"i: "<<i;
                profit_update += profitloss_prior;

            }
            ui->CurrentBalanceLabel->setText("$"+QString::number(current_balance));
            qDebug()<<"profit_update: "<<profit_update;
            ui->TotalProfitLoss_2->setText("$"+QString::number(profit_update));
        }

}
//returns garbage value fix it...
double mod(double f){
    double m;
    if(f>=0){
        m=f;
    }
    if(f<0){
        m=-1*f;
    }
    return m;
}
