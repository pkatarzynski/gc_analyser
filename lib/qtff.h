#ifndef QTFF_H
#define QTFF_H

#include "ctff.h"

#include <QtGui/QApplication>
#include <QClipboard>
#include <QMainWindow>
#include <QTableWidget>
#include <QDir>

class qtff : public CTff
{
public:
    qtff();

    void Display(QTableWidget *tb);
    int GetValues(QTableWidget *tb);
    char *GetSNName(int ind);
    int FillFromClipboard(QTableWidget *tb);
    void MakePNFile(QString pn_file);
    int LoadFromFile(QString tff_file, QString sn_file);
    void SaveToFile(QString file_name);

    bool Updating;
};

#endif // QTFF_H
