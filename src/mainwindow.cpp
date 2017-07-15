#include "mainwindow.h"
#include <QVBoxLayout>

#include <QtSerialPort/QSerialPortInfo>



MainWindow::MainWindow(QWidget *parent)
    : QFrame(parent)
{
    createUi();
    getAvailablePorts();

    serial = new QSerialPort(this);

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    statusBar->showMessage("Disconnected");

    loadCommands();
    connect(sendButton,&QPushButton::clicked,this, &MainWindow::sendCommand);

    cmdByte[0] = 'r';
    cmdByte[1] = 't';
}

MainWindow::~MainWindow()
{

}
void MainWindow::closeEvent(QCloseEvent *event)
{
  QString portName = portCb->currentText();
  QString status;
  if (serial->isOpen()) {
      this->textArea->append(tr("Closing port: %1").arg(portName));
      serial->close();
      status = tr("Disconnected...");
      portCb->setEnabled(true);
      sendButton->setEnabled(false);
  }
  event->accept();
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
    QHBoxLayout *wrapLayout = new QHBoxLayout;
    wrapLayout->addStretch(1);
    wrapLayout->addWidget(portGroupBox);
    //mainLayout->addWidget(portGroupBox);
    mainLayout->addLayout(wrapLayout);

    mainLayout->addWidget(textArea);
    mainLayout->addLayout(commandLayout);
    mainLayout->addWidget(statusBar);
    setLayout(mainLayout);

    connect(connectButton,SIGNAL(clicked()), this, SLOT(openSerialPort()));

    setWindowTitle(APPLICATION_TITLE " v" MSCS_VERSION);
}

void MainWindow::createPortGroupBox()
{
    portGroupBox = new QGroupBox(tr("Port control"), this);

    portCb = new QComboBox(portGroupBox);

    connectButton = new QPushButton("Connect", portGroupBox);
    connectButton->setToolTip("Connect to target");
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(5);
    layout->setSpacing(5);

    layout->addWidget(portCb);
    //layout->addStretch(1);
    layout->addWidget(connectButton);
    portGroupBox->setLayout(layout);
    portGroupBox->adjustSize();

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

void MainWindow::openSerialPort()
{
    QString portName = portCb->currentText();
    QString status;
    if (connectButton->text().compare("Connect") == 0) {

        this->textArea->append(tr("Opening port: %1").arg(portName));
        const auto serialPortInfos = QSerialPortInfo::availablePorts();
        QString l = "Total ports found: %1";

        textArea->append(l.arg(serialPortInfos.count()));

        /*
    const QString blankString = QObject::tr("N/A");
    QString description;
    QString manufacturer;
    QString serialNumber;

    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        description = serialPortInfo.description();
        manufacturer = serialPortInfo.manufacturer();
        serialNumber = serialPortInfo.serialNumber();
        l = "Port: %1";
        textArea->append(l.arg(serialPortInfo.portName()));

        //            << QObject::tr("Port: ") << serialPortInfo.portName() << endl
        //            << QObject::tr("Location: ") << serialPortInfo.systemLocation() << endl
//            << QObject::tr("Description: ") << (!description.isEmpty() ? description : blankString) << endl
//            << QObject::tr("Manufacturer: ") << (!manufacturer.isEmpty() ? manufacturer : blankString) << endl
//            << QObject::tr("Serial number: ") << (!serialNumber.isEmpty() ? serialNumber : blankString) << endl
//            << QObject::tr("Vendor Identifier: ") << (serialPortInfo.hasVendorIdentifier() ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16) : blankString) << endl
//            << QObject::tr("Product Identifier: ") << (serialPortInfo.hasProductIdentifier() ? QByteArray::number(serialPortInfo.productIdentifier(), 16) : blankString) << endl
//            << QObject::tr("Busy: ") << (serialPortInfo.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) << endl;
    }*/

        serial->setPortName(portName);
        // For now, we'll hardcode the port configuration parameters.
        // TODO: Add a dialog to the some of the parameters
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        if (serial->open(QIODevice::ReadWrite)) {
            status = tr("Connected to %1 : 9600 8-N-1").arg(portName);
            connectButton->setText("Close");
            portCb->setEnabled(false);
            sendButton->setEnabled(true);
        } else {
            this->textArea->append(tr("Error: %1").arg(serial->errorString()));
            if (QSerialPortInfo(portName).isBusy()) {
              status = tr("%1 is busy").arg(portName);
            }
        }
    }
    else {
        if (serial->isOpen()) {
            this->textArea->append(tr("Closing port: %1").arg(portName));
            serial->close();
            status = tr("Disconnected...");
            portCb->setEnabled(true);
            sendButton->setEnabled(false);
        }
        connectButton->setText("Connect");
    }
    this->textArea->append(status);
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
