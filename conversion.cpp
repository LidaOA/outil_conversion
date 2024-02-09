#include "conversion.h"
#include "ui_conversion.h"
#include <string>
#include <iostream>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include <QStringBuilder>

using namespace std;

WindowConversion::WindowConversion(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WindowConversion)
{
    ui->setupUi(this);
}


WindowConversion::~WindowConversion()
{
    delete ui;
}

void WindowConversion::on_widgets_devise_1_itemSelectionChanged()
{
    qDebug() << "1";
    QListWidgetItem *item = ui->widgets_devise_1->currentItem();
    QLabel *label = ui->label_rate;
    bool update_rep = label->text().contains(">",Qt::CaseInsensitive);

    bool same_devise = false;
    if(ui->widgets_devise_2->currentRow() != -1)
    {
        same_devise = (ui->widgets_devise_1->currentItem()->text().trimmed() == ui->widgets_devise_2->currentItem()->text().trimmed());
    }
    qInfo() << update_rep << "  "  << same_devise;
    if (update_rep && !same_devise)
    {
        int idx,idx2;
        idx = label->text().indexOf(":",Qt::CaseInsensitive);
        idx2 = label->text().indexOf("-",Qt::CaseInsensitive);
        QString devise_prev = label->text().sliced(idx+2,(idx2-idx)-2);
        //qDebug() << devise_prev;
        QString new_text_rate = label->text().replace(devise_prev,item->text(),Qt::CaseInsensitive);
        qDebug() << new_text_rate;
        ui->label_rate->setText(new_text_rate);

    }
    else if (!update_rep && !same_devise)
    {
        int idx = ui->label_rate->text().indexOf(":",Qt::CaseInsensitive);
        QString devise_prev = ui->label_rate->text().sliced(idx+2);
        QString text_rate = label->text().replace(devise_prev,item->text(),Qt::CaseInsensitive);
        qDebug() << text_rate;
        ui->label_rate->setText(text_rate);
    }
    WindowConversion::update();

}


void WindowConversion::on_widgets_devise_2_itemSelectionChanged()
{

    QLabel *label = ui->label_rate;
    QListWidgetItem *item = ui->widgets_devise_2->currentItem();

    bool is_convertible = label->text().contains(">",Qt::CaseInsensitive);
    if (!is_convertible)
    {
        QString text_rate = label->text() + "->" + item->text();
        ui->label_rate->setText(text_rate);
    }
    else
    {
        int idx = label->text().indexOf(QChar('>'),Qt::CaseInsensitive);
        QString devise_2 = label->text().sliced(idx+1);
        QString text_rate = label->text().replace(">" + devise_2,">" + item->text());
        QString devise_1 = ui->widgets_devise_1->currentItem()->text();

        bool same_devise = (ui->widgets_devise_1->currentItem()->text().trimmed() == ui->widgets_devise_2->currentItem()->text().trimmed());

        qDebug() << text_rate << " " << same_devise;
        if (same_devise)
        {
            ui->label_rate->setText("Taux de conversion : " + devise_1);
        }
        else
        {
            ui->label_rate->setText(text_rate);
        }
    }

    WindowConversion::update();
}

tuple<QString,QString> WindowConversion::get_devise_selection()
{
    QString text_1 = ui->widgets_devise_1->currentItem()->text();
    QString text_2 = ui->widgets_devise_2->currentItem()->text();
    QString devise_1 = text_1 ,devise_2 = text_2;
    if (text_1.contains(QChar('('),Qt::CaseInsensitive))
    {
        int idx = text_1.indexOf(QChar('('),Qt::CaseInsensitive);
        devise_1 = text_1.sliced(0,idx);
    }
    if (text_2.contains(QChar('('),Qt::CaseInsensitive))
    {
        int idx = text_2.indexOf(QChar('('),Qt::CaseInsensitive);
        devise_2 = text_2.sliced(0,idx);
    }
    return {devise_1.trimmed(),devise_2.trimmed()};
}


QString WindowConversion::get_content(QNetworkReply *reply)
{
    QString content = reply->readAll();
    int idx_content = content.lastIndexOf(QString("thead"),Qt::CaseInsensitive);
    int idx_end = content.lastIndexOf(QString("/table"),Qt::CaseInsensitive);
    QString content_devises = content.sliced(idx_content,idx_end - idx_content);
    content_devises.squeeze();

    return content_devises;
}

QString WindowConversion::get_code(QString content_devises, QString devise)
{
    QStringList list_devises = content_devises.split(QString("/tr"),Qt::SkipEmptyParts);
    QString code = "0"; //par défaut
    for (int i = 0 ; i < list_devises.count() ; i++)
    {
        if(list_devises.at(i).contains(devise,Qt::CaseInsensitive))
        {
            list_devises[i].remove(QRegularExpression("[0-9]+"));
            QList list_feats_devise = list_devises[i].split(QString("\n"),Qt::SkipEmptyParts);
            list_feats_devise.removeLast();
            code = list_feats_devise[list_feats_devise.count()-2].remove(QRegularExpression("..[td].."));
            break;
        }
    }
    return code.trimmed();
}

//void WindowConversion::replace_codes


void WindowConversion::convert_amount(double current_value, double exchange_rate)
{

    ui->edit_out->setText(QString::number(current_value*exchange_rate));

    WindowConversion::update();
}

void WindowConversion::go_conversion(QNetworkReply *reply)
{

    if (reply->error() == QNetworkReply::NoError)
    {
        QString data = reply->readAll();
        int idx_start = data.lastIndexOf(QString("thead"),Qt::CaseInsensitive);
        int idx_end = data.indexOf(QString("<tfoot>"),Qt::CaseInsensitive);
        QString content = data.sliced(idx_start,idx_end - idx_start);
        QStringList list_rates_by_amounts = content.split(QString("</tr>"),Qt::SkipEmptyParts);
        list_rates_by_amounts.squeeze();

        QStringList rate_by_amount = list_rates_by_amounts[1].split(QString("</td>"),Qt::SkipEmptyParts);
        rate_by_amount.removeLast(); //required to erase empty val at the end

        double amount = rate_by_amount[0].sliced(rate_by_amount[0].indexOf(ui->edit_code1->text())).remove(QRegularExpression("[^0-9]")).toDouble();

        QString string_result =  rate_by_amount[rate_by_amount.size()-1].sliced(rate_by_amount[rate_by_amount.size()-1].indexOf(ui->edit_code2->text()));

        double value = string_result.remove(QRegularExpression("[^\\d+[\\.?\\d*]]?")).toDouble();

        exchange_rate = value/amount;

        convert_amount(ui->cursor_1->value(), exchange_rate);

    }
    else
    {
        qDebug() << "Impossible to access to : " << URL_convert;
    }

}

void WindowConversion::loadFinished(QNetworkReply *reply)
{
    auto [devise_1,devise_2] = get_devise_selection();

    QString content_devises = get_content(reply);
    QString code_1 = get_code(content_devises,devise_1);
    QString code_2 = get_code(content_devises,devise_2);

    if(code_1 == "0")
    {
        cout << "Error to find " << code_1.toStdString() << " in the currency codes " << endl;
        //afficher qmessagebox
        return;
    }
    else if (code_2 == "0")
    {
        cout << "Error to find " << code_2.toStdString() << " in the currency codes " << endl;
        // qmessagebox
        return;
    }

    ui->edit_code1->setText(code_1);
    ui->edit_code2->setText(code_2);

    WindowConversion::update();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString URL_request = URL_convert % code_1 % "-" % code_2 % "/table";
    manager->get(QNetworkRequest(QUrl(URL_request)));
    connect(manager, &QNetworkAccessManager::finished, this, &WindowConversion::go_conversion);
}


void WindowConversion::on_button_convert_clicked()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,&QNetworkAccessManager::finished,this,&WindowConversion::loadFinished);
    const QUrl url = QUrl(URL_codes);
    QNetworkRequest request(url);
    manager->get(request); //return QNetworkReply object opened for reading by emitting readyRead()signal

}


void WindowConversion::on_cursor_1_valueChanged(double arg1)
{
    double value = arg1;
    convert_amount(value,exchange_rate);
}

