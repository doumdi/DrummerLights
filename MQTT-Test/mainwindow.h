#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMqtt/QtMqtt>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void clientConnected();
    void timeout();
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic = QMqttTopicName());
    void colorButtonClicked();
    void colorValueChanged(int);

private:
    Ui::MainWindow *ui;
    QMqttClient *m_mqttClient;
    QTimer *m_timer;
};

#endif // MAINWINDOW_H
