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

#define APPLICATION_TITLE "MSCS"
#define MSCS_VERSION     "0.1"


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
    QMenu *helpMenu;

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
    void about();
    void save();

};

#endif // MAINWINDOW_H
