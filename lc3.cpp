#include "lc3.h"
#include "ui_lc3.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QScrollBar>


LC3Registers registers;
LC3Cycle instructions;

int index;
QString fileName;

lc3::lc3(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::lc3)
{
    ui->setupUi(this);
    memoryFill();


QString uploadFilestylesheet = R"(
    QPushButton {
        background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,
                                          stop:0 rgba(0, 0, 67, 255),
                                          stop:1 rgba(0, 0, 87, 255));
        border: 2px solid rgba(0, 0, 67, 255);
        border-radius: 5px;
        color: white;
        font-size: 12pt;
        padding: 8px 16px;
    }

    QPushButton:hover {
        background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,
                                          stop:0 rgba(0, 0, 87, 255),
                                          stop:1 rgba(0, 0, 107, 255));
        border: 2px solid rgba(0, 0, 87, 255);
    }

    QPushButton:pressed {
        background-color: rgba(0, 0, 47, 255);
        border: 2px solid rgba(0, 0, 67, 255);
    }
)";
//ui->Upload_code->setStyleSheet(uploadFilestylesheet);


    // Define the stylesheet as a QString
    QString RUNstylesheet = R"(
       QPushButton {
           background-color: rgb(0, 0, 67); /* Dark blue background */
           color: white; /* White text color */
           padding: 5px;
           border-radius: 8px;
           border: 1px solid #1a1a1a; /* Dark border */
       }
       QPushButton:hover {
           background-color: #3f466e; /* Dark hover color */
       }
       QPushButton:pressed {
           background-color: #003366; /* Slightly lighter pressed color */
       }
       QPushButton:focus {
           outline: none;
           border: 2px solid #ffffff; /* White border on focus */
       }
    )";
    QString RLabelstylesheet = R"(
         QLabel {
             background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(210, 238, 255, 255), stop:1 rgba(184, 221, 255, 255));
             border: 1px solid #4f93d5; /* Blue border */
             border-radius: 5px;
             padding: 5px;
         }
    )";
    QString R_inLabelstylesheet = R"(
         QLabel {
             background-color: qlineargradient(spread:pad, x1:1, y1:0.579, x2:0, y2:0.569, stop:0 rgba(25, 150, 176, 248), stop:0.98 rgba(237, 255, 251, 255), stop:1 rgba(0, 0, 0, 0));
             border: 1px solid #4f93d5; /* Blue border */
             border-radius: 5px;
             padding: 5px;
         }
    )";
    QString otherR_inLabelstylesheet = R"(
         QLabel {
             background-color: qlineargradient(spread:pad, x1:0, y1:0.499455, x2:1, y2:0.506, stop:0 rgba(14, 81, 103, 248), stop:0.98 rgba(202, 235, 255, 255), stop:1 rgba(0, 0, 0, 0));
             border: 1px solid #4f93d5; /* Blue border */
             border-radius: 5px;
             padding: 5px;
         }
    )";
    ui->IR_in->setStyleSheet(otherR_inLabelstylesheet);
    ui->PC_in->setStyleSheet(otherR_inLabelstylesheet);
    ui->MAR_in->setStyleSheet(otherR_inLabelstylesheet);
    ui->MDR_in->setStyleSheet(otherR_inLabelstylesheet);
    QString phaseLabelstylesheet = R"(
QLabel {
    color: #333; /* Text color: Dark gray */
    font-size: 26px; /* Font size */
    font-weight: bold; /* Bold text */
    padding: 10px; /* Padding around the label text */
    border: 2px solid #008d94; /* Border: Teal color */
    border-radius: 5px; /* Rounded corners */
}
    )";
    ui->Phase->setStyleSheet(phaseLabelstylesheet);


    QString conditionTableLabelstylesheet = R"(
    QTableWidget {
        background-color: #86b4c4; /* Light blue background */
        border: 1px solid #b5d8ff; /* Border color */
        border-radius: 5px; /* Rounded corners */
        font-size: 14px; /* Font size */
        font-family: Arial, sans-serif; /* Font family */
    }

    QTableWidget::item {
        padding: 10px; /* Padding around each item */
        border-bottom: 1px solid #b5d8ff; /* Light border between rows */
    }

    QTableWidget::item:selected {
        background-color: #cde8ff; /* Light blue background for selected item */
    }

    QHeaderView::section {
        background-color: #305b6b; /* Blue background for header */
        color: white; /* White text color for header */
        padding: 8px; /* Padding inside header */
        border: 1px solid #357db2; /* Darker blue border */
        border-top-left-radius: 5px; /* Rounded top-left corner for header */
        border-top-right-radius: 5px; /* Rounded top-right corner for header */
    }

    QHeaderView {
        font-size: 14px; /* Font size for headers */
        font-weight: bold; /* Bold font weight for headers */
}
    )";


//    ui->tableWidget->setFixedSize(320, 100); // Set width and height as needed
    // Calculate the size based on contents
//    QSize size = ui->tableWidget->sizeHint();
//    ui->tableWidget->resize(size);




//    QTableWidgetItem *item;
//    item = new QTableWidgetItem("0x0");
//    item->setTextAlignment(Qt::AlignCenter); // Center align text in the cell


//    // After finding the QTableWidget
//    ui->tableWidget->setFrameStyle(QFrame::NoFrame);
//    // Center align contents horizontally and vertically
//    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
//    ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
//    ui->tableWidget->setStyleSheet(conditionTableLabelstylesheet);

    // Set initial values for the cells
    for (int col = 0; col < 3; ++col) {
        QTableWidgetItem *item;
        item = new QTableWidgetItem("0x0");
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(0, col, item);
    }

    // Apply stylesheet with custom colors and fonts
    ui->tableWidget->setStyleSheet("QTableWidget { border: 1px solid #d0d0d0; }"
                               "QTableWidget::item { padding: 5px; text-align: center; border: 1px solid #b5d8ff; border-radius: 5px; background-color: #86b4c4; font: 18pt \"Simplified Arabic Fixed\"; font-weight: bold;}"
                               "QHeaderView::section { background-color: #305b6b; color: white; padding: 4px; border: 1px solid #d0d0d0; border-top-left-radius: 5px; border-top-right-radius: 5px; font-weight: bold; }"
                               "QTableWidget::item:selected { background-color: #2980b9; color: white; }");

    // Resize columns to content
    ui->tableWidget->resizeColumnsToContents();
    // Make columns stretch to fit the widget
        for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }
        // Make rows stretch to fit the widget
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
            ui->tableWidget->verticalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }


   // ui->RUN->setStyleSheet(RUNstylesheet);
    ui->R0_in->setStyleSheet(R_inLabelstylesheet);
    ui->R1_in->setStyleSheet(R_inLabelstylesheet);
    ui->R2_in->setStyleSheet(R_inLabelstylesheet);
    ui->R3_in->setStyleSheet(R_inLabelstylesheet);
    ui->R4_in->setStyleSheet(R_inLabelstylesheet);
    ui->R5_in->setStyleSheet(R_inLabelstylesheet);
    ui->R6_in->setStyleSheet(R_inLabelstylesheet);
    ui->R7_in->setStyleSheet(R_inLabelstylesheet);

    QString startPushButtonstylesheet = R"(
QPushButton {
    background-color: #008d94; /* Base color: Teal */
    color: white; /* White text */
    border: 2px solid #008d94; /* Teal border */
    border-radius: 8px; /* Rounded corners */
    padding: 10px 15px; /* Padding inside the button */
    font-size: 18px; /* Font size */
}

QPushButton:hover {
    background-color: #007a80; /* Darker teal when hovered */
    border-color: #007a80; /* Darker teal border when hovered */
}

QPushButton:pressed {
    background-color: #053340; /* Slightly darker teal when pressed */
    border-color: #00676c; /* Slightly darker teal border when pressed */
}

)";

    ui->pushButtonStartasm->setStyleSheet(startPushButtonstylesheet);

//border-top-left-radius: 5px; /* Rounded top-left corner for header */
//border-top-right-radius: 5px; /* Rounded top-right corner for header */
    ui->memoryTable->setStyleSheet("QTableWidget { background-color: #008d94; color: white; border: 1px solid #d0d0d0; }"
                               "QTableWidget::item { padding: 5px; border: 1px solid #ccc; font: 18pt \"Simplified Arabic Fixed\";}"
                               "QHeaderView::section { background-color: #053340; color: white; padding: 4px; border: 1px solid #d0d0d0; border-top-left-radius: 5px; border-top-right-radius: 5px; font-weight: bold;  }"
                               "QTableWidget::item:selected { background-color: #007a80; color: white; }");

}

lc3::~lc3()
{
    delete ui;
}

void lc3::updateRegisters()
{
    ui->R0_in->setText("0x" + QString::number(registers.getR(0), 16).toUpper());
    ui->R1_in->setText("0x" + QString::number(registers.getR(1), 16).toUpper());
    ui->R2_in->setText("0x" + QString::number(registers.getR(2), 16).toUpper());
    ui->R3_in->setText("0x" + QString::number(registers.getR(3), 16).toUpper());
    ui->R4_in->setText("0x" + QString::number(registers.getR(4), 16).toUpper());
    ui->R5_in->setText("0x" + QString::number(registers.getR(5), 16).toUpper());
    ui->R6_in->setText("0x" + QString::number(registers.getR(6), 16).toUpper());
    ui->R7_in->setText("0x" + QString::number(registers.getR(7), 16).toUpper());
    ui->MAR_in->setText("0x" + QString::number(registers.set_get_MAR(), 16).toUpper());
    ui->MDR_in->setText("0x" + QString::number(registers.set_get_MDR(), 16).toUpper());
    //ui->Negative_in->setText("0x" + QString::number((registers.set_get_CC() >> 2) & 0x1, 16).toUpper());
    ui->PC_in->setText("0x" + QString::number(registers.set_get_PC(), 16).toUpper());
   // ui->Positive_in->setText("0x" + QString::number(registers.set_get_CC() & 0x1, 16).toUpper());
    //ui->Zero_in->setText("0x" + QString::number((registers.set_get_CC() >> 1) & 0x1, 16).toUpper());
    ui->IR_in->setText("0x" + QString::number(registers.set_get_IR(), 16).toUpper());

    QTableWidgetItem *item0 = new QTableWidgetItem("0x" + QString::number((registers.set_get_CC() >> 2) & 0x1, 16).toUpper());
    item0->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(0, 0, item0);

    QTableWidgetItem *item1 = new QTableWidgetItem("0x" + QString::number((registers.set_get_CC() >> 1) & 0x1, 16).toUpper());
    item1->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(0, 1, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem("0x" + QString::number(registers.set_get_CC() & 0x1, 16).toUpper());
    item2->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(0, 2, item2);
}

void lc3::updateMemory(int index)
{
    if (index < ui->memoryTable->rowCount())
    {
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString("0x%1").arg(memory.read(index), 4, 16, QChar('0')).toUpper());
        ui->memoryTable->setItem(index, 1, valueItem);
        // Scroll to the updated memory location
        ui->memoryTable->scrollToItem(valueItem, QAbstractItemView::PositionAtCenter);
    }
}

void lc3::on_RUN_clicked()
{
    if (sc == -1)
    {
        // HALT
        if (instructions.isHalt())
        {
            QMessageBox::information(this, "Program Done", "The program has reached the HALT instruction and is done.");
        }
    }
    else if (sc == 1)
    {
        instructions.fetch(memory);
        if (instructions.isHalt())
        {
            QMessageBox::information(this, "Program Done", "The program has reached the HALT instruction and is done.");
            sc = -1;
        }
        else
        {
            updateRegisters();
            ui->Phase->setText("Fetch");
            sc++;
        }
    }
    else if (sc == 2)
    {
        instructions.decode();
        updateRegisters();
        ui->Phase->setText("Decode");
        sc++;
    }
    else if (sc == 3)
    {
        instructions.evaluateAddress(memory);
        updateRegisters();
        ui->Phase->setText("Evaluate Address");
        sc++;
    }
    else if (sc == 4)
    {
        instructions.fetchOperands(memory);
        updateRegisters();
        ui->Phase->setText("Fetch Operands");
        sc++;

    } else if(sc == 5){
        instructions.execute();
        updateRegisters();
        ui->Phase->setText("Execute");
        sc++;
    }
    else if (sc == 6)
    {
        instructions.storeResults(memory);
        updateRegisters();
        ui->Phase->setText("Store Results");
        updateMemory(index);
        sc = 1;
    }
}

void lc3::on_Upload_code_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Assembly Files (*.asm)"));
    if (!fileName.isEmpty())
    {
        if (fileName.endsWith(".asm", Qt::CaseInsensitive))
        {
            QMessageBox::information(this, tr("File Selected"), tr("You selected:\n%1").arg(fileName));
        }
        else
        {
            QMessageBox::warning(this, tr("Invalid File"), tr("Please select a file with an .asm extension."));
        }
    }
    else
    {
        QMessageBox::warning(this, tr("No File Selected"), tr("No file was selected."));
    }
}

void lc3::memoryFill()
{

    ui->memoryTable->setRowCount(0xFFFF);
    ui->memoryTable->setColumnCount(2);
    QStringList headers = {"Address", "Value"};
    ui->memoryTable->setHorizontalHeaderLabels(headers);
    ui->memoryTable->verticalHeader()->setVisible(false); // Hide the vertical hea

    for (size_t i = 0; i < 0xFFFF; ++i)
    {
        QTableWidgetItem *addressItem = new QTableWidgetItem(QString("0x%1").arg(i, 4, 16, QChar('0')).toUpper());
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString("0x%1").arg(memory.read(i), 4, 16, QChar('0')).toUpper());

        ui->memoryTable->setItem(i, 0, addressItem);
        ui->memoryTable->setItem(i, 1, valueItem);
    }
}

void lc3::on_pushButtonStartasm_clicked()
{
    startAssembly(fileName);
    globalFile.readFromBinaryFile(0x3000);
    registers.set_get_PC()=0x3000;
    index = 0x3000;
    memoryFill();
    updateMemory(index); // Ensure memory is filled and visible
}

