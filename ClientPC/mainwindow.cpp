#include "mainwindow.h"
#include "./ui_mainwindow.h"
const int BufferSize = 14096;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyle();
    readSettings();

    preparePhone();

    startTCP();

    ui->pushButtonAnswer->setDisabled(true);
    ui->pushButtonMute->setDisabled(true);

    createTrayIcon();
    preparePopUp();
}

MainWindow::~MainWindow()
{
    delete phone;
    delete pop;
    delete ui;
}
/* Метод, который обрабатывает событие закрытия окна приложения
 * */
void MainWindow::closeEvent(QCloseEvent * event)
{
    /* Если окно видимо и чекбокс отмечен, то завершение приложения
     * игнорируется, а окно просто скрывается, что сопровождается
     * соответствующим всплывающим сообщением
     */
    if(this->isVisible()){
        event->ignore();
        this->hide();
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);

        trayIcon->showMessage("Домофон",
                              ("Я тут! Развернусь при вызове!"),
                              icon,
                              500);
    }
}
/* Метод, который обрабатывает нажатие на иконку приложения в трее
 * */
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:
        showHidePopUp();
        break;
    default:
        break;
    }
}

void MainWindow::toLog(QString _log)
{
    qDebug() << _log;
    ui->textBrowser->append(_log);
}

void MainWindow::setStyle()
{
    toLog("Set style");
    QFile styleF;
    styleF.setFileName("://qss/mainStyle.css");
    styleF.open(QFile::ReadOnly);
    QString qssStr = styleF.readAll();
    this->setStyleSheet(qssStr);
}

// Network
void MainWindow::startTCP()
{
    if(socket == nullptr)
    {
        toLog("Trying connect to TCP server");

        socket = new QTcpSocket(this);
        toLog("New socket");
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconected()));
        connect(socket, &QTcpSocket::connected, this, &MainWindow::socketConnected);
        toLog("Socket signals connected");

        socket->connectToHost(ipAdr, portTCP);
        toLog("Connecting to host");
        QTimer::singleShot(timeoutTime, this, &MainWindow::connectionTimeout);
        tcpBusy = true;
        toLog("Timer started");
    }
    else
    {
        toLog("TCP is busy!");
    }
}

void MainWindow::callAnswer()
{
    toLog("Call answer");
    if(ui->pushButtonAnswer->isChecked())
    {
        isAnswered = true;
        toLog("Start talk");
        phone->start();
        sendCommand(ANSWER);
        ui->pushButtonMute->setEnabled(true);
    }
    else
    {
        toLog("Stop talk");
        phone->stop();
        if(isAnswered)
        {
            sendCommand(END_CALL);
            isAnswered = false;
        }
        ui->pushButtonAnswer->setChecked(false);
        ui->pushButtonAnswer->setDisabled(true);
        ui->pushButtonMute->setDisabled(true);
        showHidePopUp();
    }
}

void MainWindow::applyCommand(int _com)
{
    toLog("Apply command: " + QString::number(_com));
    if(_com & INCOMMING_CALL)
    {
            showHidePopUp();
        ui->pushButtonAnswer->setEnabled(true);
    }
    if(_com & END_CALL || _com & ANSWER)
    {
        if(!ui->pushButtonAnswer->isChecked())
        {
            endCall();
        }
    }
    if(_com & DISCONNECT)
    {
        endCall();
    }
    if(_com & DOOR_1_IS_CLOSED)
    {
        ui->pushButtonDoor1->setChecked(false);
    }
    if(_com & DOOR_2_IS_CLOSED)
    {
        ui->pushButtonDoor2->setChecked(false);
    }
    if(_com & PING)
    {
        toLog("Ping recived, send ping");
        sendCommand(PING);
    }
}

void MainWindow::sendCommand(int _com)
{
    QString command = "&" + QString::number(_com);
    toLog("Send command: " + QString::number(_com));

    if(connected & socket->isWritable())
    {
        socket->write(command.toUtf8());
        toLog("Command sended");
    }
    else toLog("Not connected");
}

void MainWindow::endCall()
{
    toLog("End call");
    ui->pushButtonAnswer->setChecked(false);
    ui->pushButtonAnswer->setDisabled(true);
    callAnswer();
}

void MainWindow::preparePhone()
{
    phone = new UDPPhone(portUDP);
    phone->initAudio();
    phoneThread = new QThread();
    connect(phoneThread, &QThread::started, phone, &UDPPhone::initAudio);
    connect(phone, &UDPPhone::signalLog, this, &MainWindow::toLog);
    phone->moveToThread(phoneThread);
    phoneThread->start(QThread::TimeCriticalPriority);
}

void MainWindow::socketReady()
{
    QString data = socket->readAll();
    toLog("TCP data recived: " + data);
    if(data[0] == '&')
    {
        toLog("Is command");
        applyCommand(data.midRef(1, data.size() - 1).toInt());
    }

}

void MainWindow::socketConnected()
{
    toLog("TCP connected");
    connected = true;
}

void MainWindow::socketDisconected()
{
    toLog("Socket disconnected");
    connected = false;
    disconnect(socket, &QTcpSocket::connected, this, &MainWindow::socketConnected);
    toLog("Socket signals disconnected");

    if(phone->isStarted()) phone->stop();

    ui->pushButtonAnswer->setChecked(false);
    ui->pushButtonAnswer->setDisabled(true);
    ui->pushButtonMute->setChecked(false);
    ui->pushButtonMute->setDisabled(true);

    socket->deleteLater();
    toLog("Socket deleted");
    socket = nullptr;
    toLog("Socket = nullptr");
    toLog("TCP disconnected");

    if(pop->isVisible())
    {
        pop->hideAnimation();
    }
    startTCP();
}

void MainWindow::connectionTimeout()
{
    toLog("---");
    toLog("Connection timeout signal!");

    if(!connected)
    {
        toLog("Not connected");
        socketDisconected();
    }
}

void MainWindow::UDPPhoneStopped()
{
    endCall();
}

void MainWindow::toMainWindow()
{
    toLog("To main window");
    this->show();
}

void MainWindow::popCallClicked()
{
    toLog("Pop call clicked");
    if(ui->pushButtonAnswer->isEnabled())
    {
        if(ui->pushButtonAnswer->isChecked())
        {
            ui->pushButtonAnswer->setChecked(false);
            callAnswer();
        }
        else
        {
            ui->pushButtonAnswer->setChecked(true);
            callAnswer();
        }
    }
}

void MainWindow::popDoor1Clicked()
{
    toLog("Pop door 1 clicked");
    sendCommand(DOOR_1);
}

void MainWindow::popDoor2Clicked()
{
    toLog("Pop door 2 clicked");
    sendCommand(DOOR_2);
}

// Settings

void MainWindow::readSettings()
{
    toLog("Read settings");
    QSettings settings("ShurkSoft", "Door phone client");
    settings.beginGroup("settings");
    ipAdr = settings.value("server IP").toString();
    toLog("IP: " + ipAdr);
    portTCP = settings.value("TCP port").toInt();
    toLog("Port TCP: " + QString::number(portTCP));
    portUDP = settings.value("UDP port").toInt();
    toLog("Port UDP: " + QString::number(portUDP));
    settings.endGroup();
    toLog("OK");
}

void MainWindow::createTrayIcon()
{
    toLog("Create tray icon");
    /* Инициализируем иконку трея, устанавливаем иконку из набора системных иконок,
     * а также задаем всплывающую подсказку
     * */
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(this->style()->standardIcon(QStyle::SP_ComputerIcon));
    trayIcon->setToolTip("Домофон" "\n"
                         "Отображает всплывающее окно при вызове");
    /* После чего создаем контекстное меню из двух пунктов*/
    QMenu * menu = new QMenu(this);
    QAction * viewWindow = new QAction("Развернуть окно", this);
    QAction * quitAction = new QAction("Выход", this);

    /* подключаем сигналы нажатий на пункты меню к соответсвующим слотам.
     * Первый пункт меню разворачивает приложение из трея,
     * а второй пункт меню завершает приложение
     * */
    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    menu->addAction(viewWindow);
    menu->addAction(quitAction);

    /* Устанавливаем контекстное меню на иконку
     * и показываем иконку приложения в трее
     * */
    trayIcon->setContextMenu(menu);
    // trayIcon.s
    trayIcon->show();

    /* Также подключаем сигнал нажатия на иконку к обработчику
     * данного нажатия
     * */
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::showHideWindow()
{
    toLog("Show / hide window");
    if(this->isVisible()){
        this->hide();
    } else {
        this->show();
    }
}

void MainWindow::preparePopUp()
{
    toLog("Prepare PopUp");
    pop = new PopUp;
    connect(pop, &PopUp::toMainWindow, this, &MainWindow::toMainWindow);
    connect(pop, &PopUp::callClicked, this, &MainWindow::popCallClicked);
    connect(pop, &PopUp::door1Clicked, this, &MainWindow::popDoor1Clicked);
    connect(pop, &PopUp::door2Clicked, this, &MainWindow::popDoor2Clicked);
}

void MainWindow::showHidePopUp()
{
    toLog("Show / hide PopUp");
    if(pop->isVisible()){
        pop->hideAnimation();
    } else {
        pop->show();
    }
}

void MainWindow::applySettings()
{
    toLog("Settings window is closed");
    readSettings();
    toLog("Settings applyed");
}

void MainWindow::on_pushButtonSettings_clicked()
{
    SettingsWindow *sw = new SettingsWindow;
    sw->show();
    connect(sw, &SettingsWindow::settingsChanged, this, &MainWindow::applySettings);
}

void MainWindow::on_pushButtonAnswer_clicked()
{
    callAnswer();
}

void MainWindow::on_pushButtonMute_clicked()
{
    toLog("Not muted! Button is not active.");
    // audioInput->setVolume(!ui->pushButtonMute->isChecked());
}

void MainWindow::on_pushButtonDoor1_clicked()
{
    sendCommand(DOOR_1);
}

void MainWindow::on_pushButtonDoor2_clicked()
{
    sendCommand(DOOR_2);
}

