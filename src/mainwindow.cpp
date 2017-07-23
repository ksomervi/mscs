#include "mainwindow.h"
#include <QVBoxLayout>

#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
  : QFrame(parent)
{
//  portDialog = NULL;
  serial = new QSerialPort(this);

  verbose_output = true;
  createUi();

  getAvailablePorts();

  connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
  statusBar->showMessage("Disconnected");

  loadCommands();
  connect(sendButton,&QPushButton::clicked,this, &MainWindow::sendCommand);

  cmdByte[0] = 'r';
  cmdByte[1] = 't';

//  portDialog = new PortConfigDialog(this);
//  portDialog->serialDevice(serial);

//  portDialog->setLog(textArea);
}

MainWindow::~MainWindow()
{

}
void MainWindow::closeEvent(QCloseEvent *event)
{
  closeSerialPort();
  event->accept();
}

void MainWindow::closeSerialPort()
{
  QString status;
  if (serial->isOpen()) {
    QString portName = serial->portName();

    this->textArea->append(tr("Closing port: %1").arg(portName));
    serial->close();
    status = tr("Disconnected...");
  }
}
void MainWindow::createUi()
{
  //Add menubar
  createMenubar();

  // Setup widgets to control the serial port interface.
  createPortGroupBox();

  textArea = new QTextEdit(this);

  cmdCb = new QComboBox(this);
  sendButton = new QPushButton("Send", this);

  QHBoxLayout *commandLayout = new QHBoxLayout;

  commandLayout->setSpacing(5);
  commandLayout->addStretch(1);
  commandLayout->addWidget(cmdCb);
  commandLayout->addWidget(sendButton);

  sendButton->setEnabled(false);
  statusBar = new QStatusBar(this);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setSpacing(3);
  mainLayout->setMargin(5);

  mainLayout->addWidget(mb);
//  QHBoxLayout *wrapLayout = new QHBoxLayout;
//  wrapLayout->addStretch(1);
//  wrapLayout->addWidget(portTabs);
  mainLayout->addWidget(portGroupBox);
//  mainLayout->addLayout(wrapLayout);

  mainLayout->addWidget(textArea);
  mainLayout->addLayout(commandLayout);
  mainLayout->addWidget(statusBar);
  setLayout(mainLayout);

  connect(connectButton,SIGNAL(clicked()), this, SLOT(on_connectButton_clicked()));

  setWindowTitle(APPLICATION_TITLE " v" MSCS_VERSION);
}

void MainWindow::createPortGroupBox()
{
  portGroupBox = new QGroupBox(tr("Port control"), this);

  portTabs = new QTabWidget(this);

  connectTab = new QWidget();
  QHBoxLayout *pcLayout = new QHBoxLayout;
  pcLayout->setMargin(5);
  pcLayout->setSpacing(5);

  //  portCb = new QComboBox(portGroupBox);
  portCb = new QComboBox(connectTab);

  connectButton = new QPushButton("Connect", connectTab);
  connectButton->setToolTip("Connect to target");

  pcLayout->addWidget(portCb);
  pcLayout->addSpacerItem(new QSpacerItem(40,20, QSizePolicy::Expanding));
  pcLayout->addWidget(connectButton);
  connectTab->setLayout(pcLayout);

  portSettingsTab = new QWidget();
  QHBoxLayout *psLayout = new QHBoxLayout;
  psLayout->setMargin(5);
  psLayout->setSpacing(5);

  baudComboBox = new QComboBox(this);
  //initialize baudrates
  baudrates.push_back(qMakePair(QSerialPort::Baud9600, tr("9600")));
  baudrates.push_back(qMakePair(QSerialPort::Baud19200, tr("19200")));
  baudrates.push_back(qMakePair(QSerialPort::Baud38400, tr("38400")));
  baudrates.push_back(qMakePair(QSerialPort::Baud57600, tr("57600")));
  baudrates.push_back(qMakePair(QSerialPort::Baud115200, tr("115200")));
  for (int i=0; i<baudrates.size(); i++) {
    baudComboBox->addItem(baudrates.at(i).second,baudrates.at(i).first);
  }

  psLayout->addWidget(baudComboBox);
  psLayout->addSpacerItem(new QSpacerItem(40,20, QSizePolicy::Expanding));

  dataBitsSpinBox = new QSpinBox(this);
  dataBitsSpinBox->setMinimum(5);
  dataBitsSpinBox->setMinimum(8);
  dataBitsSpinBox->setValue(8);
  psLayout->addWidget(dataBitsSpinBox);

  parityComboBox = new QComboBox(this);
  // initialize parity settings (odd, even, none)
  parityComboBox->addItem("None",QSerialPort::NoParity);
  parityComboBox->addItem("Even",QSerialPort::EvenParity);
  parityComboBox->addItem("Odd",QSerialPort::OddParity);
  psLayout->addWidget(parityComboBox);

  stopBitsSpinBox = new QSpinBox(this);
  stopBitsSpinBox->setMinimum(1);
  stopBitsSpinBox->setMaximum(2);
  stopBitsSpinBox->setValue(1);
  psLayout->addWidget(stopBitsSpinBox);

  portSettingsTab->setLayout(psLayout);
  portTabs->addTab(connectTab, tr("Connect"));
  portTabs->addTab(portSettingsTab, tr("Settings"));

  QVBoxLayout *ptLayout = new QVBoxLayout;
  ptLayout->setMargin(0);
  ptLayout->setSpacing(0);
  portGroupBox->setLayout(ptLayout);
  portGroupBox->layout()->addWidget(portTabs);

}

void MainWindow::createMenubar() {
  mb = new QMenuBar(this);
  QMenu *fileMenu = new QMenu("&File");
  QAction *saveAct = fileMenu->addAction(tr("&Save"), this, &MainWindow::save);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save activity log to file"));
  //connect(saveAct, &QAction::triggered, this, &MainWindow::save);

  QAction *quitAction = fileMenu->addAction(tr("&Quit"), this, &MainWindow::close);
  quitAction->setShortcuts(QKeySequence::Quit);
  quitAction->setStatusTip(tr("Exit the application"));

  mb->addMenu(fileMenu);

  QMenu *editMenu = new QMenu("&Edit");

  debugAct = editMenu->addAction(tr("Debug"), this, &MainWindow::setDebug);
  debugAct->setCheckable(true);
  debugAct->setChecked(verbose_output);
  debugAct->setStatusTip(tr("Enable verbose logging"));

  mb->addMenu(editMenu);

  QMenu *helpMenu = new QMenu("&Help");

  QAction *aboutAct = helpMenu->addAction(tr("&About..."), this, &MainWindow::about);
  aboutAct->setStatusTip(tr("Show the MSCS credits"));

  mb->addMenu(helpMenu);
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About MSCS"),
                     tr("MSCS is a simple interface simulator for controlling my project.\n"
                        "Kevin Somervill (kevin@somervill.org)\n"
                        "copyright 2017"));
}

void MainWindow::getAvailablePorts() {
  const auto serialPortInfos = QSerialPortInfo::availablePorts();
  //QString l = "Total ports found: %1";

  this->textArea->append(tr("Total ports found: %1").arg(serialPortInfos.count()));
  for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
    portCb->addItem(serialPortInfo.portName());
  }
}

void MainWindow::loadCommands() {
  this->cmdCb->addItem("Read Temp");
  this->cmdCb->addItem("Toggle LED");

}

void MainWindow::save() {

}

void MainWindow::sendCommand() {

  int idx = this->cmdCb->currentIndex();
  char cmd = cmdByte[idx];
  textArea->append(tr("TX: CB[%1] == %2").arg(idx).arg(cmd));
  serial->write(&cmd);
}

void MainWindow::on_connectButton_clicked()
{
  QString portName = portCb->currentText();
  QString status;
  QString msg;

  if (connectButton->text().compare("Connect") == 0) {

    this->textArea->append(tr("Opening port: %1").arg(portName));
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    QString l = "Total ports found: %1";

    textArea->append(l.arg(serialPortInfos.count()));


//    serial->setBaudRate(QSerialPort::Baud9600);
//    serial->setDataBits(QSerialPort::Data8);
//    serial->setParity(QSerialPort::NoParity);
//    serial->setStopBits(QSerialPort::OneStop);
//    serial->setFlowControl(QSerialPort::NoFlowControl);

    serial->setPortName(portName);
    serial->setBaudRate(baudComboBox->currentData().toInt());
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::Parity(parityComboBox->currentData().toInt()));
    serial->setStopBits(QSerialPort::StopBits(stopBitsSpinBox->value()));
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
      QString paritySetting;
      switch (serial->parity()) {
        case QSerialPort::NoParity:
          paritySetting = "N";
          break;
        case QSerialPort::EvenParity:
          paritySetting = "E";
          break;
        case QSerialPort::OddParity:
          paritySetting = "O";
          break;
        default:
          paritySetting = "ERROR";
          break;
      };

      if (verbose_output) {
        msg = tr("Port: %1").arg(serial->portName());
        this->textArea->append(msg);

        msg = tr("Baud Rate: %1").arg(serial->baudRate());
        this->textArea->append(msg);

        msg = tr("Data bits: %1").arg(serial->dataBits());
        this->textArea->append(msg);

        msg = tr("Parity: %1").arg(paritySetting);
        this->textArea->append(msg);

        msg = tr("Stop bits: %1").arg(serial->stopBits());
        this->textArea->append(msg);
      }

      status = tr("Connected to %1 : %2 %3-%4-%5").arg(serial->portName())
                                                .arg(serial->baudRate())
                                                .arg(serial->dataBits())
               .arg(paritySetting).arg(serial->stopBits());

      connectButton->setText("Close");
      portCb->setEnabled(false);
      sendButton->setEnabled(true);
      portSettingsTab->setEnabled(false);

    } else {
      this->textArea->append(tr("Error: %1").arg(serial->errorString()));
      if (QSerialPortInfo(portName).isBusy()) {
        status = tr("%1 is busy").arg(portName);
      }
    }
  }
  else {
    closeSerialPort();
    portCb->setEnabled(true);
    sendButton->setEnabled(false);
    connectButton->setText("Connect");
    portSettingsTab->setEnabled(true);
    status = tr("Disconnected");
  }

  if (verbose_output) {
    msg = status;
  }

  if (msg.length()>0) {
    this->textArea->append(msg);
  }
  statusBar->showMessage(status, 5000);

}

void MainWindow::writeData(const QByteArray &data)
{
  serial->write(data);
}

void MainWindow::readData()
{
  QByteArray data = serial->readAll();
  buffer.append(data);
  if (buffer.endsWith('\n') == true) {
    this->textArea->append(tr("RX: %1").arg(buffer.trimmed()));
    buffer.clear();
  }
}

void MainWindow::setDebug() {
  QString msg = tr("Verbose logging %1");
  if(debugAct->isChecked()) {
      verbose_output = true;
      msg = msg.arg(tr("enabled"));
  }
  else {
    verbose_output = false;
    msg = msg.arg(tr("disabled"));
  }
  textArea->append(msg);
}
