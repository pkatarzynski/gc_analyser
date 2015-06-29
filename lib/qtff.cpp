#include "qtff.h"

qtff::qtff()
{
    Clear();
}
//-------------------------------------------------------------------------------
void qtff::Display(QTableWidget *tb)
{

    Updating = true;
    tb->clear();

    tb->setColumnCount(sn_rank+1);
    tb->setRowCount(sn_cnt);

    QStringList sns;

    for (short s = 0; s < sn_cnt; s++)
    {
        tb->setRowHeight(s,20);
        if (items[s].type == snDen)
            sns << "Denominator";
        else
            sns << QString(items[s].in)+"->"+QString(items[s].out);
    }

    tb->setVerticalHeaderLabels(sns);

    QStringList powers;
    QString dummy;

    for (int s = sn_rank; s >= 0; s--)
    {

        dummy.sprintf("s^%d",s);
        powers << dummy;
    }
    tb->setHorizontalHeaderLabels(powers);

QTableWidgetItem *it;

    for ( int s = 0; s < sn_cnt; s++)
    {
        for (int r = 0; r <= sn_rank; r++)
        {
            if (blanks[s][r])
                dummy.sprintf("x");
            else
                dummy.sprintf("%f",params[s][r]);
            it = new QTableWidgetItem(dummy);
             tb->setItem(s,r,it);
        }
    }


    Updating = false;
}
//-------------------------------------------------------------------------------
int qtff::GetValues(QTableWidget *tb)
{

    Updating = true;
    bool conv_result;
    double f;

    if ( tb->rowCount() != sn_cnt) return 1;
    if ( tb->columnCount() != sn_rank+1) return 2;

    for (int s = 0 ; s < sn_cnt; s++)
    {
        for (int r = 0 ; r <= sn_rank; r++)
        {
            f = tb->item(s,r)->text().toDouble(&conv_result);
            if (conv_result)
                SetCoeff(s,r,f);
            else
                return 3;
        }
    }

    Updating = false;

    return 0;
}
//-------------------------------------------------------------------------------
char *qtff::GetSNName(int ind)
{
    return items[ind].name;
}
//-------------------------------------------------------------------------------
int qtff::FillFromClipboard(QTableWidget *tb)
{

    Updating = true;
    int row = tb->currentRow();
    tb->selectRow(row);

    QClipboard *clipboard = QApplication::clipboard();

    QString content = clipboard->text(QClipboard::Clipboard);

    content.remove("[");
    content.remove("]");
    content.remove(",");
    content.remove("=");
    QStringList list = content.split(" ", QString::SkipEmptyParts);
    float f;
    bool conv_ok;
    QString str;

    if (list.count() != sn_rank+1)
    {
       Updating = false;
        tb->selectRow(-1);
        return 1;
    }
    else
    {
        for (int i=0; i < list.count(); i++)
        {
            str = list.at(i);
            f = str.toFloat(&conv_ok);            
            if (conv_ok) SetCoeff(row,i,f);
            tb->item(row,i)->setText(str);
        }
    }
    Updating=false;    

    return 0;
}
//-------------------------------------------------------------------------------
void qtff::MakePNFile(QString pn_file)
{
    QDir dummy;
    dummy.setPath(pn_file);
    QString pn_native = dummy.toNativeSeparators(pn_file);

    CTff::MakePNFile(pn_native.toAscii().data());
}
//-------------------------------------------------------------------------------
int qtff::LoadFromFile(QString tff_file, QString sn_file)
{
    QDir tff;
    tff.setPath(tff_file);
    QString tff_path = tff.toNativeSeparators(tff_file);
    QDir sn;
    sn.setPath(sn_file);
    QString sn_path = tff.toNativeSeparators(sn_file);
    return CTff::LoadFromFile(tff_path.toAscii().data(),sn_path.toAscii().data());

    return 0;
}
//-------------------------------------------------------------------------------
void qtff::SaveToFile(QString file_name)
{
    QDir dummy;
    dummy.setPath(file_name);
    QString native_path = dummy.toNativeSeparators(file_name);
    CTff::SaveToFile(native_path.toAscii().data());
}
//-------------------------------------------------------------------------------
