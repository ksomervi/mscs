#ifndef PORTCONFIGDIALOG_H
#define PORTCONFIGDIALOG_H

#include <QtWidgets>

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QVector>
#include <QPair>

namespace Ui {
  class PortConfigDialog;
}

class PortConfigDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PortConfigDialog(QWidget *parent = 0);
  ~PortConfigDialog();

  void setLog(QTextEdit *);

  void serialDevice(QSerialPort*);
  QSerialPort* serialDevice();


private slots:
  void on_buttonBox_accepted();

private:
  QVector< QPair<QSerialPort::BaudRate, QString> > baudrates;

  Ui::PortConfigDialog *ui;
//  QString portName;
  QSerialPort *serial;
//  QString buffer;
  QTextEdit *log;
  void getAvailablePorts();


};

#endif // PORTCONFIGDIALOG_H
