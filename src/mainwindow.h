#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
//#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QPair>
#include <QVector>

//#include "portconfigdialog.h"

#define APPLICATION_TITLE "MSCS"
#define MSCS_VERSION     "0.1"

namespace Ui {
 class MainWindow;
}

class MainWindow : public QFrame
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void createUi();
    void createMenubar();
    void createPortGroupBox();
    void getAvailablePorts();

    QMenuBar *mb;
    QMenu *fileMenu;

    QMenu *editMenu;
    QAction *debugAct;

    QMenu *helpMenu;

    QByteArray cmdByte;

    QTextEdit *textArea;
    QPushButton *sendButton;
    QComboBox *cmdCb;

    QStatusBar *statusBar;

    QGroupBox *portGroupBox;
    QTabWidget *portTabs;
    QWidget *connectTab;
    QPushButton *connectButton;
    QComboBox *portCb;

    QWidget *portSettingsTab;
    QComboBox *baudComboBox;
    QSpinBox *dataBitsSpinBox;
    QComboBox *parityComboBox;
    QSpinBox *stopBitsSpinBox;

    QVector< QPair<QSerialPort::BaudRate, QString> > baudrates;

    QSerialPort *serial;
    QString buffer;

    //PortConfigDialog *portDialog;
    bool verbose_output;

private slots:
    void loadCommands();
    void on_connectButton_clicked();
//    void openSerialPort();
    void closeSerialPort();
    void sendCommand();
    void writeData(const QByteArray &data);
    void readData();
    void about();
    void save();
    void setDebug();

};

#endif // MAINWINDOW_H
