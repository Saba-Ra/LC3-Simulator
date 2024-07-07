#ifndef LC3_H
#define LC3_H

#include <QMainWindow>
#include "LC3Cycle.h"
#include "ui_lc3.h"
#include "lc3memory.h"
#include "Files.h"
#include "assembler.h"

extern LC3Registers registers;


QT_BEGIN_NAMESPACE
namespace Ui { class lc3; }
QT_END_NAMESPACE
extern int index;
class lc3 : public QMainWindow
{
    Q_OBJECT

public:
    lc3(QWidget *parent = nullptr);
    ~lc3();
    QVector<QLabel*> memoryLabels;
    int sc=1;
    void updateRegisters();
    void updateMemory(int index);
    void scrollToUpdatedMemory();

    void memoryFill();

private slots:
    void on_RUN_clicked();

    void on_Upload_code_clicked();

    void on_pushButtonStartasm_clicked();

private:
    Ui::lc3 *ui;

};
bool readFromFile(LC3Memory&, uint16_t, const QString&);
#endif // LC3_H
