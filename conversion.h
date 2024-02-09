#ifndef WINDOWCONVERSION_H
#define WINDOWCONVERSION_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include<list>
#include <tuple>

QT_BEGIN_NAMESPACE
namespace Ui { class WindowConversion; }
QT_END_NAMESPACE
using namespace std;

class WindowConversion : public QWidget
{
    Q_OBJECT

public:
    WindowConversion(QWidget *parent = nullptr);
    ~WindowConversion();

private slots:

    void on_widgets_devise_1_itemSelectionChanged();

    void on_widgets_devise_2_itemSelectionChanged();

    void on_button_convert_clicked();

    void loadFinished(QNetworkReply*);

    tuple<QString,QString> get_devise_selection();

    QString get_content(QNetworkReply *reply);

    QString get_code(QString content_devises,QString devise);

    void go_conversion(QNetworkReply*);

    void convert_amount(double current_value, double exchange_rate);

    void on_cursor_1_valueChanged(double arg1);

private:
    QString URL_codes = "https://fr.iban.com/currency-codes";
    QString URL_convert = "https://www.mataf.net/fr/conversion/monnaie-";

    double exchange_rate = 0;

    Ui::WindowConversion *ui;
};
#endif // WINDOWCONVERSION_H
