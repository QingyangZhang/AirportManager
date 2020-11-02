#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "airport.h"
#include "QSqlQuery"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent,QSqlQuery sql_query);
    ~Dialog();

private slots:


private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
