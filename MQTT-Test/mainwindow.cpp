#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonArray>
#include <QTextStream>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_mqttClient(nullptr),
    m_serialPort(nullptr)
{
    ui->setupUi(this);

    connect(ui->colorButton,&QPushButton::clicked, this, &MainWindow::colorButtonClicked);

    //Setup MQTT Client
#if 0
    m_mqttClient = new QMqttClient(this);
    m_mqttClient->setClientId("qtgui");
    m_mqttClient->setHostname("localhost");
    m_mqttClient->setPort(1883);

    connect(m_mqttClient, &QMqttClient::connected, this, &MainWindow::clientConnected);
    connect(m_mqttClient, &QMqttClient::messageReceived, this, &MainWindow::messageReceived);

    m_mqttClient->connectToHost();

 #endif


    m_serialPort = new QSerialPort(this);
    m_serialPort->setParity(QSerialPort::Parity::NoParity);
    m_serialPort->setBaudRate(QSerialPort::Baud38400);
    m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
    m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
    m_serialPort->setPortName("/dev/cu.usbmodem14201");

    //Signals
    connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::readyReadSerialPort);

    if (m_serialPort->open(QIODevice::ReadWrite))
        ui->textEdit->append("Serial port opened.");

    QThread::sleep(3);

    connect(ui->redVerticalSlider, &QSlider::valueChanged, this, &MainWindow::colorValueChanged);
    connect(ui->greenVerticalSlider, &QSlider::valueChanged, this, &MainWindow::colorValueChanged);
    connect(ui->blueVerticalSlider, &QSlider::valueChanged, this, &MainWindow::colorValueChanged);

    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout, this, &MainWindow::timeout);
    m_timer->start(100);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clientConnected()
{
    ui->textEdit->append("Connected!");
}

void MainWindow::timeout()
{

    int red = ui->redVerticalSlider->value();
    int green = ui->greenVerticalSlider->value();
    int blue = ui->blueVerticalSlider->value();

    QString data;
    QTextStream myStream(&data);
    myStream << red <<" " << green <<" " << blue << "\n";

    if (m_mqttClient != nullptr && m_mqttClient->state() == QMqttClient::Connected)
    {
        ui->textEdit->append(QString("Writing MQTT") + data);
        QMqttTopicName topic("/arduino/rgb");
        m_mqttClient->publish(topic, data.toUtf8());
    }

    if (m_serialPort)
    {
        m_serialPort->write(data.toUtf8());
    }


}

void MainWindow::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    Q_UNUSED(message);
    Q_UNUSED(topic);

}

void MainWindow::colorButtonClicked()
{
    QColorDialog dialog(this);
    QColor color = dialog.getColor();
    ui->redVerticalSlider->setValue(color.red());
    ui->greenVerticalSlider->setValue(color.green());
    ui->blueVerticalSlider->setValue(color.blue());
    QPalette palette = ui->colorLabel->palette();
    palette.setColor(ui->colorLabel->backgroundRole(),color);
    palette.setColor(ui->colorLabel->foregroundRole(), color);
    ui->colorLabel->setPalette(palette);
    ui->colorLabel->setAutoFillBackground(true);
}

void MainWindow::colorValueChanged(int)
{
    QColor color;

    color.setRed(ui->redVerticalSlider->value());
    color.setGreen(ui->greenVerticalSlider->value());
    color.setBlue(ui->blueVerticalSlider->value());

    QPalette palette = ui->colorLabel->palette();
    palette.setColor(ui->colorLabel->backgroundRole(),color);
    palette.setColor(ui->colorLabel->foregroundRole(), color);
    ui->colorLabel->setPalette(palette);
    ui->colorLabel->setAutoFillBackground(true);
}

void MainWindow::readyReadSerialPort()
{
    if (m_serialPort)
        ui->textEdit->append(QString("[SERIAL]") + m_serialPort->readAll());
}
