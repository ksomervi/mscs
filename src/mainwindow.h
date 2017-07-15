#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
//#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>

class QComboBox;
class QGroupBox;
class QPushButton;
class QTextEdit;
class QTextLine;


class MainWindow : public QFrame
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void createUi();
    void createPortGroupBox();
    void getAvailablePorts();

    QMenuBar *mb;
    QByteArray cmdByte;

    QTextEdit *textArea;
    QPushButton *sendButton;
    QComboBox *cmdCb;

    QStatusBar *statusBar;

    QGroupBox *portGroupBox;
    QPushButton *connectButton;
    QComboBox *portCb;

    QSerialPort *serial;
    QString buffer;

private slots:
    void loadCommands();
    void openSerialPort();
    void sendCommand();
    void writeData(const QByteArray &data);
    void readData();

};

#endif // MAINWINDOW_H
