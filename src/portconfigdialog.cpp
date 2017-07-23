#include "portconfigdialog.h"
#include "ui_portconfigdialog.h"

PortConfigDialog::PortConfigDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PortConfigDialog)
{
  ui->setupUi(this);
  log = NULL;

  if (!serial) {
    serial = new QSerialPort(parent);
  }
//  getAvailablePorts();
  const auto serialPortInfos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
    ui->portNameComboBox->addItem(serialPortInfo.portName());
  }

  //initialize baudrates
  baudrates.push_back(qMakePair(QSerialPort::Baud9600, tr("9600")));
  baudrates.push_back(qMakePair(QSerialPort::Baud19200, tr("19200")));
  baudrates.push_back(qMakePair(QSerialPort::Baud38400, tr("38400")));
  baudrates.push_back(qMakePair(QSerialPort::Baud57600, tr("57600")));
  baudrates.push_back(qMakePair(QSerialPort::Baud115200, tr("115200")));
  for (int i=0; i<baudrates.size(); i++) {
    ui->baudRateComboBox->addItem(baudrates.at(i).second,baudrates.at(i).first);
  }

  ui->dataBitsSpinBox->setMinimum(5);
  ui->dataBitsSpinBox->setMinimum(8);
  ui->dataBitsSpinBox->setValue(8);

  // initialize parity settings (odd, even, none)
  ui->parityComboBox->addItem("None",QSerialPort::NoParity);
  ui->parityComboBox->addItem("Even",QSerialPort::EvenParity);
  ui->parityComboBox->addItem("Odd",QSerialPort::OddParity);

  ui->stopBitsSpinBox->setMinimum(1);
  ui->stopBitsSpinBox->setMaximum(2);
  ui->stopBitsSpinBox->setValue(1);

}

PortConfigDialog::~PortConfigDialog()
{
  delete ui;
}

void PortConfigDialog::getAvailablePorts() {
  const auto serialPortInfos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
    ui->portNameComboBox->addItem(serialPortInfo.portName());
  }
}

void PortConfigDialog::on_buttonBox_accepted()
{
  QString portName = ui->portNameComboBox->currentText();
  serial->setPortName(portName);
  // For now, we'll hardcode the port configuration parameters.
  // TODO: Add a dialog to the some of the parameters
//  log->append(tr("baudrate: %1").arg(ui->baudRateComboBox->currentData().toString()));
  serial->setBaudRate(ui->baudRateComboBox->currentData().toInt());
  serial->setDataBits(QSerialPort::Data8);
//  log->append(tr("parity: %1").arg(ui->parityComboBox->currentData().toString()));

  serial->setParity(QSerialPort::Parity(ui->parityComboBox->currentData().toInt()));

  serial->setStopBits(QSerialPort::StopBits(ui->stopBitsSpinBox->value()));
  serial->setFlowControl(QSerialPort::NoFlowControl);

  serial->open(QIODevice::ReadWrite);

}

void PortConfigDialog::serialDevice(QSerialPort *sp) {
  serial = sp;
}

QSerialPort* PortConfigDialog::serialDevice() {
  return serial;
}

void PortConfigDialog::setLog(QTextEdit *l) {
  log = l;
}

