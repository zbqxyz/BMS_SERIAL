#include "mainbmswindow.h"
#include "ui_mainbmswindow.h"


#define  TIMEOUT_ON
#undef TIMEOUT_ON

int MainBMSWindow::oldvalue = 255;


MainBMSWindow::MainBMSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainBMSWindow)
{
    ui->setupUi(this);
    ui->pushButton_discon->setVisible(false);

    //ui->groupBox_warning->setVisible(false);
    //ui->toolBox_stage->setVisible(false);
#if 0
    ui->groupBox_send->setVisible(false);
    ui->groupBox_receive->setVisible(false);
#endif
    ui->label_12->setVisible(false);
    ui->lineEdit_spn2829->setVisible(false);

    ui->lineEdit_spn2560->setVisible(false);
    ui->label_9->setVisible(false);
    ui->lineEdit_spn2830->setVisible(false);
    ui->lineEdit_spn3929->setVisible(false);

    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(&can_timer,SIGNAL(timeout()),this,SLOT(slot_cantimer()));//
    can_timer.start(100);

    //connect(&get_ip_timer,SIGNAL(timeout()),this,SLOT(slot_getip()));//
    //get_ip_timer.start(1000);

    mReceiveModel = new CanMessageModel(CanMessageModel::Receive, this);
    mReceiveModel->setShowTrigger(false);
    ui->tableViewReceive->setModel(mReceiveModel);
    ui->tableViewReceive->horizontalHeader()->resizeSection(2, 200);
    ui->tableViewReceive->horizontalHeader()->resizeSection(0, 100);
    ui->tableViewReceive->horizontalHeader()->resizeSection(1, 60);
    ui->tableViewReceive->horizontalHeader()->resizeSection(3, 60);
    ui->tableViewReceive->horizontalHeader()->resizeSection(4, 60);
    ui->tableViewReceive->horizontalHeader()->resizeSection(5, 60);

    mSendModel = new CanMessageModel(CanMessageModel::Transmit, this);
    ui->tableViewSend->setModel(mSendModel);
    ui->tableViewSend->horizontalHeader()->resizeSection(2, 200);
    ui->tableViewSend->horizontalHeader()->resizeSection(0, 100);
    ui->tableViewSend->horizontalHeader()->resizeSection(1, 60);
    ui->tableViewSend->horizontalHeader()->resizeSection(3, 60);
    ui->tableViewSend->horizontalHeader()->resizeSection(4, 60);
    ui->tableViewSend->horizontalHeader()->resizeSection(5, 60);

    QItemSelectionModel *selectionModel = new QItemSelectionModel(mSendModel);
    ui->tableViewSend->setSelectionModel(selectionModel);

    mScheduler = new CanSendScheduler(this);
    connect(mScheduler, SIGNAL(jobScheduled(QCanMessage&)), this, SLOT(sendMessage(QCanMessage&)));


    my_tooltip();
    set_data_pgn();
    //show_data_pgn();


    connect(this,SIGNAL(ValueChanged(int)),this,SLOT(ChangeValue(int)));
    connect(this,SIGNAL(ErrValueChanged(int)),this,SLOT(ErrChangeValue(int)));

    connect(&bst_timer,SIGNAL(timeout()),this,SLOT(slot_statustimer()));//充电状态
    //bst_timer.start(100);
    MainBMSWindow::oldvalue = 255;

    //ui->statusBar->showMessage(tr("BMS 模拟软件!!!"));
    my_progressbar = new QProgressBar;
    my_name_label = new QLabel;
    my_label = new QLabel;
    my_name_label->setText("BMS 模拟软件!!!");
    ui->statusBar->addPermanentWidget(my_name_label);
    ui->statusBar->addWidget(my_label);
    ui->statusBar->addWidget(my_progressbar);
    my_label->setText(tr("BMS 准备充电握手!!!"));
    my_progressbar->setValue(5);

    bool ok;
   QByteArray x("ffffffff");
    //QString s = x;
    //s.prepend(x);
    int j = x.toUInt(&ok,16);
    QString s = QString::number(x.toUInt(&ok,16));
    my_progressbar->setValue(5);
}

MainBMSWindow::~MainBMSWindow()
{
    delete ui;
}

#if 0 //可设置为全部显示注释
bool MainBMSWindow::event(QEvent *event)
 {
    //QGraphicsScene scene;
    //QTransform transform;
    //transform.rotate(+0.0);

     if (event->type() == QEvent::ToolTip) {
         QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
         //int index = itemAt(helpEvent->pos());
         //if (index != -1) {
         ui->groupBox_28->setToolTip("333333");
             QToolTip::showText(helpEvent->globalPos(),ui->groupBox_28->toolTip());
//         } else {
//             QToolTip::hideText();
//             event->ignore();
//         }

         return true;
     }
     return QWidget::event(event);
 }
#endif

void MainBMSWindow::my_tooltip()
{
    //辨识阶段
    ui->groupBox_SPN2560->setToolTip("辨识结果");
    ui->groupBox_SPN2561->setToolTip("充电机编号");
    ui->groupBox_SPN2562->setToolTip("区域编码");

    ui->groupBox_SPN2565->setToolTip("BMS通讯协议版本号");
    ui->groupBox_SPN2566->setToolTip("电池类型");
    ui->groupBox_SPN2567->setToolTip("蓄电池系统额定容量/蓄电池系统额定总电压");

    //参数配置阶段
    ui->lineEdit_spn2816->setToolTip("单体动力蓄电池最高允许充电电压");
    ui->lineEdit_spn2817->setToolTip("最高允许充电电流");
    ui->lineEdit_spn2818->setToolTip("动力蓄电池标称总能量");
    ui->lineEdit_spn2819->setToolTip("最高允许充电总电压");
    ui->lineEdit_spn2820->setToolTip("最高允许温度");
    ui->lineEdit_spn2821->setToolTip("整车动力蓄电池荷电状态");
    ui->lineEdit_spn2822->setToolTip("整车动力蓄电池当前电池电压");

    ui->groupBox_CML_V->setToolTip("最高/低输出电压");
    ui->groupBox_CML_A->setToolTip("最大/小输出电流");

    //充电阶段
    ui->groupBox_SPN3072->setToolTip("需求电压");
    ui->groupBox_SPN3073->setToolTip("需求电流");
    ui->groupBox_SPN3074->setToolTip("充电模式");
    ui->groupBox_SPN3081->setToolTip("电压/电流输出");
    ui->groupBox_SPN3083->setToolTip("累计充电时间");
    ui->groupBox_SPN3929->setToolTip("充电允许/暂停");

    ui->groupBox_SPN3075->setToolTip("充电电压/电流测量值");
    ui->groupBox_SPN3077->setToolTip("最高单体动力蓄电池电压及组号");
    ui->groupBox_SPN3078->setToolTip("当前荷电状态");
    ui->groupBox_SPN3079->setToolTip("剩余充电时间");

    ui->lineEdit_spn3085->setToolTip("最高单体动力蓄电池电压所在编号");
    ui->lineEdit_spn3086->setToolTip("最高动力蓄电池温度");
    ui->lineEdit_spn3087->setToolTip("最高温度检测点编号");
    ui->lineEdit_spn3088->setToolTip("最低动力蓄电池温度");
    ui->lineEdit_spn3089->setToolTip("最低动力蓄电池温度检测点编号");

    ui->comboBox_spn3090->setToolTip("单体动力蓄电池电压过高/过低");
    ui->comboBox_spn3091->setToolTip("整车动力蓄电池荷电状态SOC过高/过低");
    ui->comboBox_spn3092->setToolTip("动力蓄电池充电过电流");
    ui->comboBox_spn3093->setToolTip("动力蓄电池温度过高");
    ui->comboBox_spn3094->setToolTip("动力蓄电池绝缘状态");
    ui->comboBox_spn3095->setToolTip("动力蓄电池输出连接器连接状态");
    ui->comboBox_spn3096->setToolTip("充电允许/禁止");

    //充电结束阶段
    ui->lineEdit_spn3601->setToolTip("中止荷电状态");
    ui->lineEdit_spn3602->setToolTip("动力蓄电池单体最低电压");
    ui->lineEdit_spn3603->setToolTip("动力蓄电池单体最高电压");
    ui->lineEdit_spn3604->setToolTip("动力蓄电池最低温度");
    ui->lineEdit_spn3605->setToolTip("动力蓄电池最高温度");

    ui->lineEdit_spn3611->setToolTip("累计充电时间");
    ui->lineEdit_spn3612->setToolTip("输出能量kW.h");
    ui->lineEdit_spn3613->setToolTip("充电机编号");

    //错误报文
    ui->comboBox_spn3901->setToolTip("接收SPN2560==0x00的充电辨识报文超时");
    ui->comboBox_spn3902->setToolTip("接收SPN2560==0xAA的充电辨识报文超时");
    ui->comboBox_spn3903->setToolTip("接收CTS和CML报文超时");
    ui->comboBox_spn3904->setToolTip("接收CRO报文超时");
    ui->comboBox_spn3905->setToolTip("接收CCS报文超时");
    ui->comboBox_spn3906->setToolTip("接收CST报文超时");
    ui->comboBox_spn3907->setToolTip("接收CSD报文超时");

    ui->comboBox_spn3921->setToolTip("接收BMS辨识报文超时");
    ui->comboBox_spn3922->setToolTip("接收BCP报文超时");
    ui->comboBox_spn3923->setToolTip("接收BRO报文超时");
    ui->comboBox_spn3924->setToolTip("接收BCS报文超时");
    ui->comboBox_spn3925->setToolTip("接收BCL报文超时");
    ui->comboBox_spn3926->setToolTip("接收BST报文超时");
    ui->comboBox_spn3927->setToolTip("接收BSD报文超时");
}

void MainBMSWindow::on_pushButton_connect_clicked()
{
    VCI_INIT_CONFIG init_config;

    init_config.AccCode=0x00000000;
    init_config.AccMask=0xffffffff;
    init_config.Filter=0x00;
    init_config.Mode=0x00;
    init_config.Timing0=Timing0;//0x01
    init_config.Timing1=Timing1;

#if 1
    if(VCI_OpenDevice(m_devtype,m_devind,0)!=STATUS_OK)
    {
        QMessageBox::warning(this,"打开设备失败!","警告",QMessageBox::Ok);
        ui->actionDisconnect->setEnabled(false);
        ui->actionConnect->setEnabled(true);
        return;
    }
    if(VCI_InitCAN(m_devtype,m_devind,m_cannum,&init_config)!=STATUS_OK)
    {
        QMessageBox::warning(this,"初始化CAN失败!","警告",QMessageBox::Ok);
        ui->actionDisconnect->setEnabled(false);
        ui->actionConnect->setEnabled(true);
        VCI_CloseDevice(m_devtype,m_devind);
        return;
    }

    if(VCI_StartCAN(m_devtype,m_devind,m_cannum)==STATUS_OK)
    {
        //QMessageBox::information(this,"connect","启动成功",QMessageBox::Ok);
        ui->pushButton_connect->setVisible(false);
        ui->pushButton_discon->setVisible(true);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConnect->setEnabled(false);
        mythread_can.start(); //bms_canbus();
        bst_timer.start(100);
    }
    else
    {
        QMessageBox::information(this,"connect","启动失败",QMessageBox::Ok);
        ui->actionDisconnect->setEnabled(false);
        ui->actionConnect->setEnabled(true);
    }
#else //debug
    QMessageBox::information(this,"connect","启动成功",QMessageBox::Ok);
    ui->pushButton_connect->setVisible(false);
    ui->pushButton_discon->setVisible(true);
    ui->actionDisconnect->setEnabled(true);
    ui->actionConnect->setEnabled(false);
    mythread_can.start(); //bms_canbus();
    bst_timer.start(100);
#endif
}

void MainBMSWindow::on_comboBox_bps_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        Timing0 = 0x00;
        Timing1 = 0x14;
        break;
    case 1:
        Timing0 = 0x00;
        Timing1 = 0x16;
        break;
    case 2:
        Timing0 = 0x00;
        Timing1 = 0x1C;
        break;
    case 3:
        Timing0 = 0x01;
        Timing1 = 0x1C;
        break;
    case 4:
        Timing0 = 0x03;
        Timing1 = 0x1C;
        break;
    case 5:
        Timing0 = 0x04;
        Timing1 = 0x1C;
        break;
    case 6:
        Timing0 = 0x09;
        Timing1 = 0x1C;
        break;
    case 7:
        Timing0 = 0x18;
        Timing1 = 0x1C;
        break;
    case 8:
        Timing0 = 0x31;
        Timing1 = 0x1C;
        break;
    case 9:
        Timing0 = 0xBF;
        Timing1 = 0xFF;
        break;
    default:
        break;
    }

}

void MainBMSWindow::on_pushButton_discon_clicked()
{
    VCI_CloseDevice(m_devtype,m_devind);
    mythread_can.stop(); //bms_canbus();
    bst_timer.stop();
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->pushButton_connect->setVisible(true);
    ui->pushButton_discon->setVisible(false);

    ui->warning_msg->setText("警示信息");
}

void MainBMSWindow::on_checkBox_clicked(bool checked)
{
    if(checked == TRUE){
        QMessageBox::information(this,"ready","准备握手",QMessageBox::Ok);
    }else{

    }
}

void MainBMSWindow::on_actionConnect_triggered()
{
    on_pushButton_connect_clicked();
//    ui->actionDisconnect->setEnabled(true);
//    ui->actionConnect->setEnabled(false);
#if 0
    ConnectDlg	dlg;

    if (!mConnection.device.isEmpty()) {
        dlg.setInterface(mConnection.device);
    }
    if (mConnection.appFilter) {
        dlg.setFilter(mConnection.filter);
    }
    if (mConnection.errorMask) {
        dlg.setErrorFrameMask(mConnection.errorMask);
    }
    if (dlg.exec() == QDialog::Accepted) {
        QString iface = dlg.interface();
        if (!iface.isEmpty()) {
            mSocket.close();
            if (mSocket.bind(iface, QIODevice::ReadWrite)) {
                ui.actionDisconnect->setEnabled(true);
                ui.labelBusError->setText("Ok");
                mConnection.device = iface;
                ui.labelDevice->setText(iface);
                if (dlg.filterMessages()) {
                    QPair<quint32, quint32> filter = dlg.filter();
                    mSocket.setMessageFilter(filter);
                    mConnection.filter = filter;
                    mConnection.appFilter = true;
                } else {
                    mConnection.filter = QPair<quint32, quint32>(0, 0);
                    mConnection.appFilter = false;
                }
                if (dlg.generateErrorFrames()) {
                    mSocket.setErrorFilter(dlg.errorFrameMask());
                    mConnection.errorMask = dlg.errorFrameMask();
                } else {
                    mConnection.errorMask = 0;
                }

            }
        }

    }
#endif
}

void MainBMSWindow::on_actionDisconnect_triggered()
{
    //mSocket.close();
    //ui.labelBusError->setText("offline");
    on_pushButton_discon_clicked();
//    ui->actionDisconnect->setEnabled(false);
//    ui->actionConnect->setEnabled(true);
}

void MainBMSWindow::sendMessage(QCanMessage& msg)
{
//    mSocket.write(msg.frame.frame(), msg.frame.size());
    msg.count++;
    mSendModel->addMessage(msg);
}

void MainBMSWindow::on_actionSend_triggered()
{
    QModelIndex idx = ui->tableViewSend->selectionModel()->currentIndex();
    if (!idx.isValid()) return;

    QByteArray ba = idx.data(Qt::UserRole).toByteArray();
    QCanMessage *msg = reinterpret_cast<QCanMessage*>(ba.data());
    sendMessage(*msg);
}

void MainBMSWindow::on_actionNew_triggered()
{
    CanMessageDlg dlg;
    if (dlg.exec() == QDialog::Accepted) {
        QCanMessage msg = dlg.getMessage();
        mSendModel->addMessage(msg, false);
        mScheduler->addJob(msg);
    }
}

void MainBMSWindow::on_actionDelete_triggered()
{
    QModelIndex idx = ui->tableViewSend->selectionModel()->currentIndex();
    if (!idx.isValid()) return;

    QByteArray ba = idx.data(Qt::UserRole).toByteArray();
    QCanMessage *msg = reinterpret_cast<QCanMessage*>(ba.data());
    mSendModel->deleteMessage(*msg);
    mScheduler->removeJob(*msg);
}

void MainBMSWindow::on_actionClearAll_triggered()
{
    mSendModel->deleteAll();
    mScheduler->clearAll();
}

void MainBMSWindow::on_actionEdit_triggered()
{
    QModelIndex idx = ui->tableViewSend->selectionModel()->currentIndex();

    on_tableViewSend_doubleClicked(idx);
}

void MainBMSWindow::on_actionClear_triggered()
{
    mReceiveModel->deleteAll();
}
void MainBMSWindow::on_actionExit_triggered()
{
    exit(0);
}

void MainBMSWindow::onCanbusError(quint32 error)
{
//    QString s;
//    if (error) {
//        if (error & CAN_ERR_TX_TIMEOUT)
//            s += "TX timeout ";
//        if (error & CAN_ERR_LOSTARB )
//            s += "| lost arb. ";
//        if (error & CAN_ERR_CRTL)
//            s += "| ctrl err. ";
//        if (error & CAN_ERR_PROT)
//            s += "| proto. viol. ";
//        if (error & CAN_ERR_TRX)
//            s += "| transceiver status ";
//        if (error & CAN_ERR_ACK)
//            s += "| no ACK ";
//        if (error & CAN_ERR_BUSOFF)
//            s += "| bus off ";
//        if (error & CAN_ERR_BUSERROR)
//            s += "| bus error ";
//        if (error & CAN_ERR_RESTARTED)
//            s += "| contrl. restarted";
//        if (s.startsWith('|'))
//            ui.labelBusError->setText(s.mid(2));
//        else
//            ui.labelBusError->setText(s);
//    } else {
//        ui.statusbar->showMessage(mSocket.errorString(), 3000);
//    }
}

void MainBMSWindow::on_tableViewSend_doubleClicked(const QModelIndex& index)
{

    if (index.isValid()) {
        QByteArray ba = index.data(Qt::UserRole).toByteArray();
        QCanMessage *msg = reinterpret_cast<QCanMessage*>(ba.data());
        if (index.column() == 0) {
            sendMessage(*msg);
        } else {
            CanMessageDlg dlg(*msg);
            if (dlg.exec() == QDialog::Accepted) {
                QCanMessage msg1 = dlg.getMessage();
                mSendModel->addMessage(msg1, false);
                mScheduler->addJob(msg1);
            }
        }
    }
}

void MainBMSWindow::on_actionAbout_triggered()
{
    QString versionInfo =   "<B>BMS_SERIAL</B><p>"
                            "Author: andy   <a href=mailto:andy.zhao@serialsystem.com>andy.zhao@serialsystem.com</a><p>";
#ifdef GITHASH
    versionInfo += "<p>Commit: <b>" XSTR(GITHASH) "</b>";
#endif
    QMessageBox::about ( this,
                    "About BMS_SERIAL", versionInfo);


}

void MainBMSWindow::slot_cantimer()
{
    //can_timer.stop();
    //mythread_can.start(); //bms_canbus();

    QCanMessage msg;

    msg.frame = receive_frame;
    memset(&receive_frame,0,sizeof(receive_frame));//清零以确定接收报文个数
    if(msg.frame.ID != 0x00){
        QDateTime nowreceive = QDateTime::currentDateTimeUtc();
        msg.time = nowreceive.toMSecsSinceEpoch();
        mReceiveModel->addMessage(msg);
    }
    QString trace = QString("%1.%2      %3    %4    ").arg(msg.time / 1000, 16, 10, QLatin1Char(' ')).arg(msg.time % 1000, 3, 10, QLatin1Char('0')).arg(msg.frame.ID, 8, 16).arg(msg.frame.DataLen);
    for (int i = 0; i < msg.frame.DataLen; i++) {
        trace += QString("%1  ").arg(msg.frame.Data[i], 2, 16, QLatin1Char('0'));
    }

    msg.frame = send_frame;
    memset(&send_frame,0,sizeof(send_frame));//清零以确定发送报文个数
    if(msg.frame.ID != 0x00){
        QDateTime nowsend = QDateTime::currentDateTimeUtc();
        msg.time = nowsend.toMSecsSinceEpoch();
        mSendModel->addMessage(msg);
    }
}

void MainBMSWindow::slot_getip()
{
    QString name = qgetenv("USERNAME");
    qDebug() << "User Name" << name;
    //QHostInfo类作用，获取主机名，也可以通过主机名来查找IP地址，或者通过IP地址来反向查找主机名。
    QString localHostName = QHostInfo::localHostName();
    qDebug() << "LocalHostName:" << localHostName;

    //获取IP地址
    QHostInfo info = QHostInfo::fromName(localHostName);
    qDebug() << "IP Address:" << info.addresses();

    foreach(QHostAddress address, info.addresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            qDebug() << "IPv4 Address:" << address.toString();
    }

    foreach (QHostAddress address, QNetworkInterface::allAddresses())
    {
        qDebug() << "Address:" << address;
    }
}

void MainBMSWindow::set_data_bms_PGN9984(struct charge_task * thiz)
{
    memset(&thiz->bms_handshake, INIT, sizeof(struct pgn9984_BHM));
    thiz->bms_handshake.spn2601_bms_max_vol = ui->lineEdit_spn2601->text().toFloat()*10;
    //ui->comboBox_T_HM->currentText().toInt();
}

int MainBMSWindow::get_spn2566_battery_type(int index)
{
    int battery_type = 0x00;
    switch (index) {
    case 0:
        battery_type = battery_type1;
        break;
    case 1:
        battery_type = battery_type2;
        break;
    case 2:
        battery_type = battery_type3;
        break;
    case 3:
        battery_type = battery_type4;
        break;
    case 4:
        battery_type = battery_type5;
        break;
    case 5:
        battery_type = battery_type6;
        break;
    case 6:
        battery_type = battery_type7;
        break;
    case 7:
        battery_type = battery_type8;
        break;
    default:
        battery_type = battery_type9;
        break;
    }
    return battery_type;
}
void MainBMSWindow::set_data_bms_PGN512(struct charge_task * thiz)
{
    memset(&thiz->vehicle_info, INIT, sizeof(struct pgn512_BRM));
    thiz->vehicle_info.spn2565_bms_version[0] = ui->lineEdit_spn2565_1->text().toInt();
    thiz->vehicle_info.spn2565_bms_version[1] = ui->lineEdit_spn2565_2->text().toInt();
    thiz->vehicle_info.spn2565_bms_version[2] = ui->lineEdit_spn2565_3->text().toInt();
    thiz->vehicle_info.spn2566_battery_type = get_spn2566_battery_type(ui->comboBox_spn2566->currentIndex());
    thiz->vehicle_info.spn2567_capacity = ui->lineEdit_spn2567->text().toFloat()*10;
    thiz->vehicle_info.spn2568_volatage = ui->lineEdit_spn2568->text().toFloat()*10;
}

void MainBMSWindow::set_data_bms_PGN1536(struct charge_task *thiz)
{
    memset(&thiz->bms_config_info, INIT, sizeof(struct pgn1536_BCP));
    thiz->bms_config_info.spn2816_max_charge_volatage_single_battery = ui->lineEdit_spn2816->text().toFloat()*100;
    thiz->bms_config_info.spn2817_max_charge_current = ui->lineEdit_spn2817->text().toFloat()*10 + OFFSET_CUR;
    thiz->bms_config_info.spn2818_total_energy = ui->lineEdit_spn2818->text().toFloat()*10;
    thiz->bms_config_info.spn2819_max_charge_voltage = ui->lineEdit_spn2819->text().toFloat()*10;
    thiz->bms_config_info.spn2820_max_temprature = ui->lineEdit_spn2820->text().toInt() + OFFSET_TEMP;
    thiz->bms_config_info.spn2821_soc = ui->lineEdit_spn2821->text().toFloat()*10;
    thiz->bms_config_info.spn2822_total_voltage = ui->lineEdit_spn2822->text().toFloat()*10;
}

int MainBMSWindow::set_combobox_value(int index)
{
    int value =0;
    switch (index) {
    case 0:
        value = 0x00;
        break;
    case 1:
        value = 0xAA;
        break;
    default:
        value = 0xFF;
        break;
    }
    return value;
//    BMS_NOT_READY_FOR_CHARGE =  0x00, // 没有准备好
//    BMS_READY_FOR_CHARGE     =  0xAA, // 已准备好
//    BMS_INVALID              =  0xFF // 无效
}
void MainBMSWindow::set_data_bms_PGN2304(struct charge_task * thiz)
{
    memset(&thiz->bms_bro, INIT, sizeof(struct pgn2304_BRO));
    thiz->bms_bro.spn2829_bms_ready_for_charge = set_combobox_value(ui->comboBox_spn2829->currentIndex());
    if(bit_read(thiz, F_CHARGER_CTS ) && bit_read(thiz, F_CHARGER_CML)){
        thiz->bms_bro.spn2829_bms_ready_for_charge = BMS_READY_FOR_CHARGE;
    }else{
        thiz->bms_bro.spn2829_bms_ready_for_charge = BMS_NOT_READY_FOR_CHARGE;
    }
}

int MainBMSWindow::set_charge_mode(int index)
{
    int charge_mode = 0;
    switch (index) {
    case 0:
        charge_mode = CHARGE_WITH_CONST_VOLTAGE;
        break;
    case 1:
        charge_mode = CHARGE_WITH_CONST_CURRENT;
        break;
    default:
        break;
    }
    return charge_mode;
}
void MainBMSWindow::set_data_bms_PGN4096(struct charge_task * thiz)
{
    memset(&thiz->bms_bcl, INIT, sizeof(struct pgn4096_BCL));
    thiz->bms_bcl.spn3072_need_voltage = ui->lineEdit_spn3072->text().toFloat()*10;
    thiz->bms_bcl.spn3073_need_current = ui->lineEdit_spn3073->text().toFloat()*10 + OFFSET_CUR;
    thiz->bms_bcl.spn3074_charge_mode = set_charge_mode(ui->comboBox_spn3074->currentIndex());

}
void MainBMSWindow::set_data_bms_PGN4352(struct charge_task * thiz)
{
    memset(&thiz->bms_bcs, INIT, sizeof(struct pgn4352_BCS));
    thiz->bms_bcs.spn3075_charge_voltage = ui->lineEdit_spn3075->text().toFloat()*10;
    thiz->bms_bcs.spn3076_charge_current = ui->lineEdit_spn3076->text().toFloat()*10 + OFFSET_CUR;
    thiz->bms_bcs.spn3077_max_v_g_number = (int)(ui->lineEdit_spn3077_1->text().toFloat()*100) | (ui->lineEdit_spn3077_2->text().toUShort() << 12);
    thiz->bms_bcs.spn3078_soc = ui->lineEdit_spn3078->text().toInt();
    thiz->bms_bcs.spn3079_need_time = ui->lineEdit_spn3079->text().toUShort();
}

int MainBMSWindow::analysis_data_bms_PGN4864(struct charge_task * thiz)
{
    if(GENERAL_NORMAL == set_combobox_data(ui->comboBox_spn3090->currentIndex())){

    }else if(GENERAL_UN_NORMAL == set_combobox_data(ui->comboBox_spn3090->currentIndex())){

    }else{

    }



}
int MainBMSWindow::get_data_bms_PGN4864(struct charge_task * thiz)
{
    unsigned short remote_single = 0;
    remote_single = set_combobox_data(ui->comboBox_spn3090->currentIndex());
    remote_single = remote_single | ((set_combobox_data(ui->comboBox_spn3091->currentIndex()))<<2);
    remote_single = remote_single | ((set_combobox_data(ui->comboBox_spn3092->currentIndex()))<<4);
    remote_single = remote_single | ((set_combobox_data(ui->comboBox_spn3093->currentIndex()))<<6);
    remote_single = remote_single | ((set_combobox_data(ui->comboBox_spn3094->currentIndex()))<<8);
    remote_single = remote_single | ((set_combobox_data(ui->comboBox_spn3095->currentIndex()))<<10);
    remote_single = remote_single | ((set_combobox_data(ui->comboBox_spn3096->currentIndex()))<<12);
    return remote_single;
}
int MainBMSWindow::set_combobox_data(int index)
{
    int charge_data = 0;
    switch (index) {
    case 0:
        charge_data = GENERAL_NORMAL;
        break;
    case 1:
        charge_data = GENERAL_UN_NORMAL;
        break;
    case 2:
        charge_data = GENERAL_INVALID;
        break;
    default:
        break;
    }
    return charge_data;
}
void MainBMSWindow::set_data_bms_PGN4864(struct charge_task * thiz)
{
    memset(&thiz->bms_bsm, INIT, sizeof(struct pgn4864_BSM));
    thiz->bms_bsm.sn_of_max_voltage_battery = ui->lineEdit_spn3085->text().toInt();
    thiz->bms_bsm.max_temperature_of_battery = ui->lineEdit_spn3086->text().toInt() + OFFSET_TEMP;
    thiz->bms_bsm.sn_of_max_temperature_point = ui->lineEdit_spn3087->text().toInt();
    thiz->bms_bsm.min_temperature_of_battery = ui->lineEdit_spn3088->text().toInt() + OFFSET_TEMP;
    thiz->bms_bsm.sn_of_min_temperature_point = ui->lineEdit_spn3089->text().toInt();
    thiz->bms_bsm.remote_single = get_data_bms_PGN4864(thiz);
}
void MainBMSWindow::set_data_bms_PGN5376(struct charge_task * thiz)
{
//BMV
}
void MainBMSWindow::set_data_bms_PGN5632(struct charge_task * thiz)
{
//BMT
}
void MainBMSWindow::set_data_bms_PGN5888(struct charge_task * thiz)
{
//BSP
}


int MainBMSWindow::get_data_bms_PGN6400_reason(struct charge_task * thiz)
{
    int reason = 0;
    reason = set_combobox_data(ui->comboBox_spn3511_1->currentIndex());
    reason = reason | ((set_combobox_data(ui->comboBox_spn3511_2->currentIndex()))<<2);
    reason = reason | ((set_combobox_data(ui->comboBox_spn3511_3->currentIndex()))<<4);
    reason = reason | ((set_combobox_data(ui->comboBox_spn3511_4->currentIndex()))<<6);
    return reason;
}
int MainBMSWindow::get_data_bms_PGN6400_fault(struct charge_task * thiz)
{
    unsigned short fault = 0;
    fault = set_combobox_data(ui->comboBox_spn3512_1->currentIndex());
    fault = fault | ((set_combobox_data(ui->comboBox_spn3512_2->currentIndex()))<<2);
    fault = fault | ((set_combobox_data(ui->comboBox_spn3512_3->currentIndex()))<<4);
    fault = fault | ((set_combobox_data(ui->comboBox_spn3512_4->currentIndex()))<<6);
    fault = fault | ((set_combobox_data(ui->comboBox_spn3512_5->currentIndex()))<<8);
    fault = fault | ((set_combobox_data(ui->comboBox_spn3512_6->currentIndex()))<<10);
    fault = fault | ((set_combobox_data(ui->comboBox_spn3512_7->currentIndex()))<<12);
    fault = fault | ((set_combobox_data(ui->comboBox_spn3512_8->currentIndex()))<<14);
    return fault;
}
int MainBMSWindow::get_data_bms_PGN6400_error(struct charge_task * thiz)
{
    int error = 0;
    error = set_combobox_data(ui->comboBox_spn3513_1->currentIndex());
    error = error | ((set_combobox_data(ui->comboBox_spn3513_2->currentIndex()))<<2);
    return error;
}
void MainBMSWindow::set_data_bms_PGN6400(struct charge_task * thiz)
{
    memset(&thiz->bms_bst, INIT, sizeof(struct pgn6400_BST));
    thiz->bms_bst.reason = get_data_bms_PGN6400_reason(thiz);
    thiz->bms_bst.fault = get_data_bms_PGN6400_fault(thiz);
    thiz->bms_bst.error = get_data_bms_PGN6400_error(thiz);
}
void MainBMSWindow::set_data_bms_PGN7168(struct charge_task * thiz)
{
    memset(&thiz->bms_bsd, INIT, sizeof(struct pgn7168_BSD));
    thiz->bms_bsd.spn3601_stop_soc_status = ui->lineEdit_spn3601->text().toInt();
    thiz->bms_bsd.spn3602_singal_battery_min_vol = ui->lineEdit_spn3602->text().toFloat()*100;
    thiz->bms_bsd.spn3603_singal_battery_max_vol = ui->lineEdit_spn3603->text().toFloat()*100;
    thiz->bms_bsd.spn3604_battery_min_temp = ui->lineEdit_spn3604->text().toInt() + OFFSET_TEMP;
    thiz->bms_bsd.spn3605_battery_max_temp = ui->lineEdit_spn3605->text().toInt() + OFFSET_TEMP;
}


void MainBMSWindow::set_data_pgn()
{
    set_data_bms_PGN9984(task);
    set_data_bms_PGN512(task);
    set_data_bms_PGN1536(task);
    set_data_bms_PGN2304(task);
    set_data_bms_PGN4096(task);
    set_data_bms_PGN4352(task);
    set_data_bms_PGN4864(task);
    set_data_bms_PGN6400(task);
    set_data_bms_PGN7168(task);
}

void MainBMSWindow::show_data_charger_PGN9728(struct charge_task * thiz)//CHM
{
    ui->lineEdit_spn2600_1->setText(QString(thiz->charger_handshake.spn2600_charger_version[0]));
    ui->lineEdit_spn2600_2->setText(QString(thiz->charger_handshake.spn2600_charger_version[1]));
    ui->lineEdit_spn2600_3->setText(QString(thiz->charger_handshake.spn2600_charger_version[2]));
}
void MainBMSWindow::show_data_charger_PGN256(struct charge_task * thiz)//CRM
{
    if(thiz->charger_info.spn2560_recognize == BMS_NOT_RECOGNIZED){
        ui->comboBox_spn2560->setCurrentIndex(0);
    }else if(thiz->charger_info.spn2560_recognize == BMS_RECOGNIZED){
        ui->comboBox_spn2560->setCurrentIndex(1);
    }
    QString tmp_sn;
    for (int i = 0; i < 4; i++) {
        tmp_sn += QString("%1").arg(thiz->charger_info.spn2561_charger_sn[i], 2, 16, QLatin1Char('0'));
    }

    QString tmp_code;
    tmp_code = QString((char*)thiz->charger_info.spn2562_charger_region_code);
//    for (int i = 0; i < 3; i++) {
//        tmp_code += QString(thiz->charger_info.spn2562_charger_region_code[i]);
//    }
    ui->lineEdit_spn2561->setText(tmp_sn);
    ui->lineEdit_spn2562->setText(tmp_code);
    //ui->lineEdit_spn2561->setText((char*)thiz->charger_info.spn2561_charger_sn);//传输为十六进制无法显示字符需转换 错误
    //ui->lineEdit_spn2562->setText((char*)thiz->charger_info.spn2562_charger_region_code);
}

void MainBMSWindow::show_data_charger_PGN1792(struct charge_task * thiz)//CTS
{

}
void MainBMSWindow::show_data_charger_PGN2048(struct charge_task * thiz)//CML
{
    ui->lineEdit_spn2824->setText(QString::number(thiz->charger_cml.spn2824_max_output_voltage));
    ui->lineEdit_spn2825->setText(QString::number(thiz->charger_cml.spn2825_min_output_voltage));
    ui->lineEdit_spn2826->setText(QString::number(thiz->charger_cml.spn2826_max_output_current));
    ui->lineEdit_spn2827->setText(QString::number(thiz->charger_cml.spn2826_min_output_current));
}
void MainBMSWindow::show_data_charger_PGN2560(struct charge_task * thiz)//CRO
{
    if(thiz->charger_cro.spn2830_charger_ready_for_charge == CHARGER_NOT_READY_FOR_CHARGE){
        ui->comboBox_spn2830->setCurrentIndex(0);
    }else if(thiz->charger_cro.spn2830_charger_ready_for_charge == CHARGER_READY_FOR_CHARGE){
        ui->comboBox_spn2830->setCurrentIndex(1);
    }else{
        ui->comboBox_spn2830->setCurrentIndex(2);
    }
}

void MainBMSWindow::show_data_charger_PGN4608(struct charge_task * thiz)//CCS
{
    ui->lineEdit_spn3081->setText(QString::number(thiz->charger_ccs.spn3081_output_voltage));
    ui->lineEdit_spn3082->setText(QString::number(thiz->charger_ccs.spn3082_outpu_current));
    ui->lineEdit_spn3083->setText(QString::number(thiz->charger_ccs.spn3083_charge_time));
    if(thiz->charger_ccs.spn3929_charger_status == CHARGER_ALLOW){
        ui->comboBox_spn3929->setCurrentIndex(0);
    }else{
        ui->comboBox_spn3929->setCurrentIndex(1);
    }
}

void MainBMSWindow::show_data_charger_PGN6656(struct charge_task * thiz)//CST
{
    if(REASON_CHARGER_NORMAL == (thiz->charger_cst.spn3521_reason & REASON_CHARGER_NORMAL)){
        ui->comboBox_spn3521_1->setCurrentIndex(0);
    }
    if(REASON_REACH_CHARGER_STOP == (thiz->charger_cst.spn3521_reason & REASON_REACH_CHARGER_STOP)){
        ui->comboBox_spn3521_1->setCurrentIndex(1);
    }
    if(REASON_CHARGER_UNRELIABLE == (thiz->charger_cst.spn3521_reason & REASON_CHARGER_UNRELIABLE)){
        ui->comboBox_spn3521_1->setCurrentIndex(2);
    }

    if(REASON_CHARGER_MANUAL_NORMAL == ((thiz->charger_cst.spn3521_reason >>2) & REASON_CHARGER_MANUAL_NORMAL)){
        ui->comboBox_spn3521_2->setCurrentIndex(0);
    }
    if(REASON_CHARGER_MANUAL_STOP == (thiz->charger_cst.spn3521_reason & REASON_CHARGER_MANUAL_STOP)){
        ui->comboBox_spn3521_2->setCurrentIndex(1);
    }
    if(REASON_CHARGER_MANUAL_UNRELIABLE == (thiz->charger_cst.spn3521_reason & REASON_CHARGER_MANUAL_UNRELIABLE)){
        ui->comboBox_spn3521_2->setCurrentIndex(2);
    }

    if(REASON_CHARGER_NO_ERROR == ((thiz->charger_cst.spn3521_reason >> 4) & REASON_CHARGER_NO_ERROR)){
        ui->comboBox_spn3521_3->setCurrentIndex(0);
    }
    if(REASON_CHARGER_ERROR_STOP == (thiz->charger_cst.spn3521_reason & REASON_CHARGER_ERROR_STOP)){
        ui->comboBox_spn3521_3->setCurrentIndex(1);
    }
    if(REASON_CHARGER_ERROR_UNRELIABLE == (thiz->charger_cst.spn3521_reason & REASON_CHARGER_ERROR_UNRELIABLE)){
        ui->comboBox_spn3521_3->setCurrentIndex(2);
    }

    if(REASON_BMS_NORMAL == ((thiz->charger_cst.spn3521_reason >> 6) & REASON_BMS_NORMAL)){
        ui->comboBox_spn3521_4->setCurrentIndex(0);
    }
    if(REASON_BMS_STOP == (thiz->charger_cst.spn3521_reason & REASON_BMS_STOP)){
        ui->comboBox_spn3521_4->setCurrentIndex(1);
    }
    if(REASON_BMS_UNRELIABLE == (thiz->charger_cst.spn3521_reason & REASON_BMS_UNRELIABLE)){
        ui->comboBox_spn3521_4->setCurrentIndex(2);
    }
////////////////////////////////////////////////////////////////////////////////////////////////

    if(ERROR_CHARGER_TEMP_NORMAL == (thiz->charger_cst.spn3522_error & ERROR_CHARGER_TEMP_NORMAL)){
        ui->comboBox_spn3522_1->setCurrentIndex(0);
    }
    if(ERROR_CHARGER_TEMP == (thiz->charger_cst.spn3522_error & ERROR_CHARGER_TEMP)){
        ui->comboBox_spn3522_1->setCurrentIndex(1);
    }
    if(ERROR_CARGER_TEMP_UNRELIABLE == (thiz->charger_cst.spn3522_error & ERROR_CARGER_TEMP_UNRELIABLE)){
        ui->comboBox_spn3522_1->setCurrentIndex(2);
    }

    if(ERROR_CHARGINGCONNECTOR_TEMP_NORMAL == ((thiz->charger_cst.spn3522_error >> 2) & ERROR_CHARGINGCONNECTOR_TEMP_NORMAL)){
        ui->comboBox_spn3522_2->setCurrentIndex(0);
    }
    if(ERROR_CHARGINGCONNECTOR_TEMP == (thiz->charger_cst.spn3522_error & ERROR_CHARGINGCONNECTOR_TEMP)){
        ui->comboBox_spn3522_2->setCurrentIndex(1);
    }
    if(ERROR_CHARGINGCONNECTOR_TEMP_UNRELIABLE == (thiz->charger_cst.spn3522_error & ERROR_CHARGINGCONNECTOR_TEMP_UNRELIABLE)){
        ui->comboBox_spn3522_2->setCurrentIndex(2);
    }

    if(ERROR_CHARGER_INTEMP_NORMAL == ((thiz->charger_cst.spn3522_error >> 4) & ERROR_CHARGER_INTEMP_NORMAL)){
        ui->comboBox_spn3522_3->setCurrentIndex(0);
    }
    if(ERROR_CHARGER_INTEMP == (thiz->charger_cst.spn3522_error & ERROR_CHARGER_INTEMP)){
        ui->comboBox_spn3522_3->setCurrentIndex(1);
    }
    if(ERROR_CHARGER_INTEMP_UNRELIABLE == (thiz->charger_cst.spn3522_error & ERROR_CHARGER_INTEMP_UNRELIABLE)){
        ui->comboBox_spn3522_3->setCurrentIndex(2);
    }

    if(ERROR_POWER_TRANSFER_NORMAL == ((thiz->charger_cst.spn3522_error >> 6) & ERROR_POWER_TRANSFER_NORMAL)){
        ui->comboBox_spn3522_4->setCurrentIndex(0);
    }
    if(ERROR_POWER_TRANSFER == (thiz->charger_cst.spn3522_error & ERROR_POWER_TRANSFER)){
        ui->comboBox_spn3522_4->setCurrentIndex(1);
    }
    if(ERROR_POWER_TRANSFER_UNRELIABLE == (thiz->charger_cst.spn3522_error & ERROR_POWER_TRANSFER_UNRELIABLE)){
        ui->comboBox_spn3522_4->setCurrentIndex(2);
    }

    if(ERROR_CHARGER_EMERGENCY_STOP_NORMAL == ((thiz->charger_cst.spn3522_error >> 8) & ERROR_CHARGER_EMERGENCY_STOP_NORMAL)){
        ui->comboBox_spn3522_5->setCurrentIndex(0);
    }
    if(ERROR_CHARGER_EMERGENCY_STOP == (thiz->charger_cst.spn3522_error & ERROR_CHARGER_EMERGENCY_STOP)){
        ui->comboBox_spn3522_5->setCurrentIndex(1);
    }
    if(ERROR_CHARGER_EMERGENCY_STOP_UNRELIABLE == (thiz->charger_cst.spn3522_error & ERROR_CHARGER_EMERGENCY_STOP_UNRELIABLE)){
        ui->comboBox_spn3522_5->setCurrentIndex(2);
    }

    if(ERROR_OTHERPGN6656_NORMAL == ((thiz->charger_cst.spn3522_error >> 10) & ERROR_OTHERPGN6656_NORMAL)){
        ui->comboBox_spn3522_6->setCurrentIndex(0);
    }
    if(ERROR_OTHERPGN6656 == (thiz->charger_cst.spn3522_error & ERROR_OTHERPGN6656)){
        ui->comboBox_spn3522_6->setCurrentIndex(1);
    }
    if(ERROR_OTHERPGN6656_UNRELIABLE == (thiz->charger_cst.spn3522_error & ERROR_OTHERPGN6656_UNRELIABLE)){
        ui->comboBox_spn3522_6->setCurrentIndex(2);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if(FAULT_CURRENT_MATCHING_NORMAL == (thiz->charger_cst.spn3523_fault & FAULT_CURRENT_MATCHING_NORMAL)){
        ui->comboBox_spn3523_1->setCurrentIndex(0);
    }
    if(FAULT_CURRENT_UN_MATCHING == (thiz->charger_cst.spn3523_fault & FAULT_CURRENT_UN_MATCHING)){
        ui->comboBox_spn3523_1->setCurrentIndex(1);
    }
    if(FAULT_CURRENT_PGN6656_UNRELIABLE == (thiz->charger_cst.spn3523_fault & FAULT_CURRENT_PGN6656_UNRELIABLE)){
        ui->comboBox_spn3523_1->setCurrentIndex(2);
    }

    if(FAULT_VOL_PGN6656_NORMAL == ((thiz->charger_cst.spn3523_fault >> 2) & FAULT_VOL_PGN6656_NORMAL)){
        ui->comboBox_spn3523_2->setCurrentIndex(0);
    }
    if(FAULT_VOL_PGN6656_UN_NORMAL == (thiz->charger_cst.spn3523_fault & FAULT_VOL_PGN6656_UN_NORMAL)){
        ui->comboBox_spn3523_2->setCurrentIndex(1);
    }
    if(FAULT_VOL_PGN6656_UNRELIABLE == (thiz->charger_cst.spn3523_fault & FAULT_VOL_PGN6656_UNRELIABLE)){
        ui->comboBox_spn3523_2->setCurrentIndex(2);
    }
}
void MainBMSWindow::show_data_charger_PGN7424(struct charge_task * thiz)//CSD
{
    ui->lineEdit_spn3611->setText(QString::number(thiz->charger_csd.spn3611_total_charge_time));
    ui->lineEdit_spn3612->setText(QString::number(thiz->charger_csd.spn3612_output_energy));
    QString tmp_sn;
    for (int i = 0; i < 4; i++) {
        tmp_sn += QString("%1").arg(thiz->charger_csd.spn3613_charger_sn[i], 2, 16, QLatin1Char('0'));
    }
    ui->lineEdit_spn3613->setText(tmp_sn);
}

void MainBMSWindow::show_data_charger_PGN7680(struct charge_task * thiz)//BEM
{
    if(BEM_00_NORMAL == (thiz->bms_bem.bem_crm & BEM_00_NORMAL)){
        ui->comboBox_spn3901->setCurrentIndex(0);
    }
    if(BEM_00_TIMEOUT == (thiz->bms_bem.bem_crm & BEM_00_TIMEOUT)){
        ui->comboBox_spn3901->setCurrentIndex(1);
    }
    if(BEM_00_UNRELIABLE == (thiz->bms_bem.bem_crm & BEM_00_UNRELIABLE)){
        ui->comboBox_spn3901->setCurrentIndex(2);
    }

    if(BEM_AA_NORMAL == ((thiz->bms_bem.bem_crm >> 2) & BEM_AA_NORMAL)){
        ui->comboBox_spn3902->setCurrentIndex(0);
    }
    if(BEM_AA_TIMEOUT == (thiz->bms_bem.bem_crm & BEM_AA_TIMEOUT)){
        ui->comboBox_spn3902->setCurrentIndex(1);
    }
    if(BEM_AA_UNRELIABLE == (thiz->bms_bem.bem_crm & BEM_AA_UNRELIABLE)){
        ui->comboBox_spn3902->setCurrentIndex(2);
    }
//====================================================================================//
    if(BEM_CTS_CML_NORMAL == (thiz->bms_bem.bem_ccp & BEM_CTS_CML_NORMAL)){
        ui->comboBox_spn3903->setCurrentIndex(0);
    }
    if(BEM_CTS_CML_TIMEOUT == (thiz->bms_bem.bem_ccp & BEM_CTS_CML_TIMEOUT)){
        ui->comboBox_spn3903->setCurrentIndex(1);
    }
    if(BEM_CTS_CML_UNRELIABLE == (thiz->bms_bem.bem_ccp & BEM_CTS_CML_UNRELIABLE)){
        ui->comboBox_spn3903->setCurrentIndex(2);
    }

    if(BEM_CRO_NORMAL == ((thiz->bms_bem.bem_ccp >> 2) & BEM_CRO_NORMAL)){
        ui->comboBox_spn3904->setCurrentIndex(0);
    }
    if(BEM_CRO_TIMEOUT == (thiz->bms_bem.bem_ccp & BEM_CRO_TIMEOUT)){
        ui->comboBox_spn3904->setCurrentIndex(1);
    }
    if(BEM_CRO_UNRELIABLE == (thiz->bms_bem.bem_ccp & BEM_CRO_UNRELIABLE)){
        ui->comboBox_spn3904->setCurrentIndex(2);
    }
//====================================================================================//
    if(BEM_CCS_NORMAL == (thiz->bms_bem.bem_cst & BEM_CCS_NORMAL)){
        ui->comboBox_spn3905->setCurrentIndex(0);
    }
    if(BEM_CCS_TIMEOUT == (thiz->bms_bem.bem_cst & BEM_CCS_TIMEOUT)){
        ui->comboBox_spn3905->setCurrentIndex(1);
    }
    if(BEM_CCS_UNRELIABLE == (thiz->bms_bem.bem_cst & BEM_CCS_UNRELIABLE)){
        ui->comboBox_spn3905->setCurrentIndex(2);
    }

    if(BEM_CST_NORMAL == ((thiz->bms_bem.bem_cst >> 2) & BEM_CST_NORMAL)){
        ui->comboBox_spn3906->setCurrentIndex(0);
    }
    if(BEM_CST_TIMEOUT == (thiz->bms_bem.bem_cst & BEM_CST_TIMEOUT)){
        ui->comboBox_spn3906->setCurrentIndex(1);
    }
    if(BEM_CST_UNRELIABLE == (thiz->bms_bem.bem_cst & BEM_CST_UNRELIABLE)){
        ui->comboBox_spn3906->setCurrentIndex(2);
    }
//====================================================================================//
    if(BEM_CSD_NORMAL == (thiz->bms_bem.bem_csd & BEM_CSD_NORMAL)){
        ui->comboBox_spn3906->setCurrentIndex(0);
    }
    if(BEM_CSD_TIMEOUT == (thiz->bms_bem.bem_csd & BEM_CSD_TIMEOUT)){
        ui->comboBox_spn3906->setCurrentIndex(1);
    }
    if(BEM_CSD_UNRELIABLE == (thiz->bms_bem.bem_csd & BEM_CSD_UNRELIABLE)){
        ui->comboBox_spn3906->setCurrentIndex(2);
    }
}
void MainBMSWindow::show_data_charger_PGN7936(struct charge_task * thiz)//CEM
{
    if(CEM_BMS_NORMAL == (thiz->charger_cem.cem_brm & CEM_BMS_NORMAL)){
        ui->comboBox_spn3921->setCurrentIndex(0);
    }
    if(CEM_BMS_TIMEOUT == (thiz->charger_cem.cem_brm & CEM_BMS_TIMEOUT)){
         ui->comboBox_spn3921->setCurrentIndex(1);
    }
    if(CEM_BMS_UNRELIABLE == (thiz->charger_cem.cem_brm & CEM_BMS_UNRELIABLE)){
         ui->comboBox_spn3921->setCurrentIndex(2);
    }
//====================================================================================//
    if(CEM_BCP_NORMAL == (thiz->charger_cem.cem_bro & CEM_BCP_NORMAL)){
        ui->comboBox_spn3922->setCurrentIndex(0);
    }
    if(CEM_BCP_TIMEOUT == (thiz->charger_cem.cem_bro & CEM_BCP_TIMEOUT)){
         ui->comboBox_spn3922->setCurrentIndex(1);
    }
    if(CEM_BCP_UNRELIABLE == (thiz->charger_cem.cem_bro & CEM_BCP_UNRELIABLE)){
         ui->comboBox_spn3922->setCurrentIndex(2);
    }

    if(CEM_BRO_NORMAL == ((thiz->charger_cem.cem_bro >> 2) & CEM_BRO_NORMAL)){
        ui->comboBox_spn3923->setCurrentIndex(0);
    }
    if(CEM_BRO_TIMEOUT == (thiz->charger_cem.cem_bro & CEM_BRO_TIMEOUT)){
         ui->comboBox_spn3923->setCurrentIndex(1);
    }
    if(CEM_BRO_UNRELIABLE == (thiz->charger_cem.cem_bro & CEM_BRO_UNRELIABLE)){
         ui->comboBox_spn3923->setCurrentIndex(2);
    }
//====================================================================================//
    if(CEM_BCS_NORMAL == (thiz->charger_cem.cem_bst & CEM_BCS_NORMAL)){
        ui->comboBox_spn3924->setCurrentIndex(0);
    }
    if(CEM_BCS_TIMEOUT == (thiz->charger_cem.cem_bst & CEM_BCS_TIMEOUT)){
         ui->comboBox_spn3924->setCurrentIndex(1);
    }
    if(CEM_BCS_UNRELIABLE == (thiz->charger_cem.cem_bst & CEM_BCS_UNRELIABLE)){
         ui->comboBox_spn3924->setCurrentIndex(2);
    }

    if(CEM_BCL_NORMAL == ((thiz->charger_cem.cem_bst >> 2) & CEM_BCL_NORMAL)){
        ui->comboBox_spn3925->setCurrentIndex(0);
    }
    if(CEM_BCL_TIMEOUT == (thiz->charger_cem.cem_bst & CEM_BCL_TIMEOUT)){
         ui->comboBox_spn3925->setCurrentIndex(1);
    }
    if(CEM_BCL_UNRELIABLE == (thiz->charger_cem.cem_bst & CEM_BCL_UNRELIABLE)){
         ui->comboBox_spn3925->setCurrentIndex(2);
    }

    if(CEM_BST_NORMAL == ((thiz->charger_cem.cem_bst >> 4) & CEM_BST_NORMAL)){
        ui->comboBox_spn3926->setCurrentIndex(0);
    }
    if(CEM_BST_TIMEOUT == (thiz->charger_cem.cem_bst & CEM_BST_TIMEOUT)){
         ui->comboBox_spn3926->setCurrentIndex(1);
    }
    if(CEM_BST_UNRELIABLE == (thiz->charger_cem.cem_bst & CEM_BST_UNRELIABLE)){
         ui->comboBox_spn3926->setCurrentIndex(2);
    }
//====================================================================================//
    if(CEM_BSD_NORMAL == (thiz->charger_cem.cem_bsd & CEM_BSD_NORMAL)){
        ui->comboBox_spn3927->setCurrentIndex(0);
    }
    if(CEM_BSD_TIMEOUT == (thiz->charger_cem.cem_bsd & CEM_BSD_TIMEOUT)){
         ui->comboBox_spn3927->setCurrentIndex(1);
    }
    if(CEM_BSD_UNRELIABLE == (thiz->charger_cem.cem_bsd & CEM_BSD_UNRELIABLE)){
         ui->comboBox_spn3927->setCurrentIndex(2);
    }
}

void MainBMSWindow::show_data_pgn()
{
    show_data_charger_PGN9728(task);//CHM
    show_data_charger_PGN256(task);//CRM
    show_data_charger_PGN1792(task);//CTS
    show_data_charger_PGN2048(task);//CML
    show_data_charger_PGN2560(task);//CRO
    show_data_charger_PGN4608(task);//CCS
    show_data_charger_PGN6656(task);//CST
    show_data_charger_PGN7424(task);//CSD
    show_data_charger_PGN7936(task);//CEM
}




void MainBMSWindow::on_pushButton_BHM_clicked()
{
    set_data_bms_PGN9984(task);
}

void MainBMSWindow::on_pushButton_BRM_clicked()
{
    set_data_bms_PGN512(task);
}

void MainBMSWindow::on_pushButton_BCP_clicked()
{
    set_data_bms_PGN1536(task);
}

void MainBMSWindow::on_pushButton_BCL_BCS_BSM_clicked()
{
    set_data_bms_PGN2304(task);
    set_data_bms_PGN4096(task);
    set_data_bms_PGN4352(task);
    set_data_bms_PGN4864(task);
    set_data_bms_PGN6400(task);
}

void MainBMSWindow::on_pushButton_BSD_clicked()
{
    set_data_bms_PGN7168(task);
}


void MainBMSWindow::on_pushButton_BST_clicked()
{
    set_data_bms_PGN6400(task);
    bit_set(task,F_BMS_STOP) ;
    //task->bms_stage = CHARGE_STAGE_CHARGING;
}

void MainBMSWindow::on_pushButton_set_BST_clicked()
{
    set_data_bms_PGN6400(task);
    bit_set(task,F_BMS_STOP) ;
}


void MainBMSWindow::on_pushButton_sendBST_clicked()
{
    set_data_bms_PGN6400(task);
    bit_set(task,F_BMS_STOP) ;
}


void MainBMSWindow::ui_exit()
{
    MainBMSWindow::oldvalue = 255;
}

void MainBMSWindow::timeout_frame()
{
    QString warning_msg = ui->warning_msg->text();
    switch(task->bms_err_stage){
    case (CHARGE_STAGE_ERR_TIMEOUT | CHARGE_STAGE_ERR_INVALID):
        warning_msg += "： 握手超时\n";
        if(msgBox.warning(NULL, "Error", "握手超时",QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Retry) == QMessageBox::Retry){
            task->bms_err_stage = CHARGE_STAGE_ERR_TIMEOUT;
            task->bms_stage = CHARGE_STAGE_INVALID;
        }
        break;
    case (CHARGE_STAGE_ERR_TIMEOUT | CHARGE_STAGE_ERR_HANDSHACKING):
        warning_msg += "： 接收0x00辨识超时\n";
        if(msgBox.warning(NULL, "Error", "接收0x00辨识超时",QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Retry) == QMessageBox::Retry){
            task->bms_err_stage = CHARGE_STAGE_ERR_TIMEOUT;
            task->bms_stage = CHARGE_STAGE_HANDSHACKING;
        }
        break;
    case (CHARGE_STAGE_ERR_TIMEOUT | CHARGE_STAGE_ERR_IDENTIFICATION):
        warning_msg += "： 接收0xAA辨识超时\n";
        if(msgBox.warning(NULL, "Error", "接收0xAA辨识超时",QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Retry) == QMessageBox::Retry){
            task->bms_err_stage = CHARGE_STAGE_ERR_TIMEOUT;
            task->bms_stage = CHARGE_STAGE_IDENTIFICATION;
        }
        break;
    case (CHARGE_STAGE_ERR_TIMEOUT | CHARGE_STAGE_ERR_CONFIGURE):
        warning_msg += "： 接收CTS CML CRO超时\n";
        if(msgBox.warning(NULL, "Error", "接收CTS CML CRO超时",QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Retry) == QMessageBox::Retry){
            task->bms_err_stage = CHARGE_STAGE_ERR_TIMEOUT;
            task->bms_stage = CHARGE_STAGE_CONFIGURE;
        }
        break;
    case (CHARGE_STAGE_ERR_TIMEOUT | CHARGE_STAGE_ERR_CHARGING):
        warning_msg += "： 接收CCS CST超时\n";
        if(msgBox.warning(NULL, "Error", "接收CCS CST超时",QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Retry) == QMessageBox::Retry){
            task->bms_err_stage = CHARGE_STAGE_ERR_TIMEOUT;
            task->bms_stage = CHARGE_STAGE_CHARGING;
        }
        break;
    case (CHARGE_STAGE_ERR_TIMEOUT | CHARGE_STAGE_ERR_DONE):
        warning_msg += "： 接收CSD超时\n";
        if(msgBox.warning(NULL, "Error", "接收CSD超时",QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Retry) == QMessageBox::Retry){
            task->bms_err_stage = CHARGE_STAGE_ERR_TIMEOUT;
            task->bms_stage = CHARGE_STAGE_DONE;
        }
        break;
    default:
        break;
    }
    ui->warning_msg->setText(warning_msg);
}

void MainBMSWindow::SetValue(int value)
{
    if(value!=MainBMSWindow::oldvalue)
    {
        MainBMSWindow::oldvalue=value;
        emit ValueChanged(value);
    }//注意这里的if判断,这是避免递归的方式!如果没有if,当出现了循环连接的时候就会产生无限递归
}

void MainBMSWindow::ChangeValue(int value)
{
    switch (value) {
    case CHARGE_STAGE_INVALID:
#ifdef SET_DATA
        set_data_bms_PGN9984(task);
#endif
        //QMessageBox::about(NULL, "Connect", "准备充电握手");
        //ui->statusBar->showMessage(tr("BMS 准备充电握手!!!"));
        my_label->setText(tr("BMS 准备充电握手!!!"));
        my_progressbar->setValue(10);
        break;
    case CHARGE_STAGE_HANDSHACKING:
        //QMessageBox::about(NULL, "Connect", "充电握手完成");
        //ui->statusBar->showMessage(tr("BMS 充电握手完成!!!"));
        my_label->setText(tr("BMS 充电握手完成!!!"));
        my_progressbar->setValue(30);
        break;
    case CHARGE_STAGE_IDENTIFICATION:
#ifdef SET_DATA
        set_data_bms_PGN512(task);
#endif
        //QMessageBox::about(NULL, "Connect", "充电辨识开始");
        my_label->setText(tr("BMS 充电辨识开始!!!"));
        my_progressbar->setValue(50);
        show_data_charger_PGN256(task);
        break;
    case CHARGE_STAGE_CONFIGURE:
        //ui->statusBar->showMessage(tr("BMS 充电辨识完成，充电配置!!!"));
        my_label->setText(tr("BMS 充电辨识完成，充电配置!!!"));
        my_progressbar->setValue(70);
        show_data_charger_PGN256(task);
        //QMessageBox::about(NULL, "Connect", "充电辨识完成，充电配置");
#ifdef SET_DATA
        set_data_bms_PGN1536(task);
#endif
        /*if(bit_read(task,F_CHARGER_CML))*/{
            show_data_charger_PGN2048(task);
        }
#ifdef SET_DATA
        set_data_bms_PGN2304(task);
#endif
        show_data_charger_PGN2560(task);
        break;
    case CHARGE_STAGE_CHARGING:
        show_data_charger_PGN2560(task);
        //ui->statusBar->showMessage(tr("BMS 充电配置完成，开始充电!!!"));
        my_label->setText(tr("BMS 充电配置完成，开始充电!!!"));
        my_progressbar->setValue(80);
        //QMessageBox::about(NULL, "Connect", "充电配置完成，开始充电");
#ifdef SET_DATA
        set_data_bms_PGN4096(task);
        set_data_bms_PGN4352(task);
#endif
        show_data_charger_PGN4608(task);
#ifdef SET_DATA
        set_data_bms_PGN4864(task);
        set_data_bms_PGN6400(task);
#endif
        show_data_charger_PGN6656(task);
        break;
    case CHARGE_STAGE_DONE:
        show_data_charger_PGN6656(task);
        //ui->statusBar->showMessage(tr("BMS 充电完成，结束充电!!!"));
        my_label->setText(tr("BMS 充电完成，结束充电!!!"));
        my_progressbar->setValue(90);
        //QMessageBox::about(NULL, "Connect", "充电完成，结束充电");
#ifdef SET_DATA
        set_data_bms_PGN7168(task);
#endif
        show_data_charger_PGN6656(task);
        show_data_charger_PGN7680(task);
        break;
    case CHARGE_STAGE_ANY:
        my_label->setText(tr("BMS 充电完成，结束充电!!!"));
        my_progressbar->setValue(100);

        show_data_charger_PGN7424(task);
        on_pushButton_discon_clicked();
        break;
    case CHARGE_STAGE_TIMEOUT:
        #ifdef TIMEOUT_ON
        timeout_frame();
        #endif
        break;    
    default:
        //QMessageBox::about(NULL, "Connect", "电动汽车");
        break;
    }
}

void MainBMSWindow::SetErrValue(int value)
{
    if(value!=MainBMSWindow::oldvalue)
    {
        MainBMSWindow::oldvalue=value;
        emit ErrValueChanged(value);
    }//注意这里的if判断,这是避免递归的方式!如果没有if,当出现了循环连接的时候就会产生无限递归
}

void MainBMSWindow::ErrChangeValue(int value)
{
    switch (value) {
    case CHARGE_STAGE_ERR_INVALID:
        break;
    case CHARGE_STAGE_ERR_HANDSHACKING:
        ui->warning_msg->setText("握手失败");
        task->bms_stage = CHARGE_STAGE_INVALID;
        break;
    case CHARGE_STAGE_ERR_IDENTIFICATION:
        ui->warning_msg->setText("辨识失败");
        task->bms_stage = CHARGE_STAGE_INVALID;
        break;
    default:
        //QMessageBox::about(NULL, "Connect", "电动汽车");
        break;
    }
}


void MainBMSWindow::slot_statustimer()
{
//    if(task->bms_stage == CHARGE_STAGE_HANDSHACKING){
//    }
    if(bit_read(task,F_CHARGER_CML) && (task->bms_stage == CHARGE_STAGE_CONFIGURE)){
        MainBMSWindow::oldvalue = 255;
    }
    if(bit_read(task,F_CHARGER_CCS) && (task->bms_stage == CHARGE_STAGE_CHARGING)){
        MainBMSWindow::oldvalue = 255;
    }
//    if(bit_read(task,F_CHARGER_CTS)){
//        oldvalue = CHARGE_STAGE_CONFIGURE;
//    }
    SetValue(task->bms_stage);
    SetErrValue(task->bms_err_stage);
#if 0
    switch (task->bms_stage) {
        case CHARGE_STAGE_INVALID:
            ui->toolBox_stage->setCurrentIndex(0);
            break;
        case CHARGE_STAGE_HANDSHACKING:
            ui->toolBox_stage->setCurrentIndex(0);
            break;
        case CHARGE_STAGE_IDENTIFICATION:
            ui->toolBox_stage->setCurrentIndex(0);
            break;
        case CHARGE_STAGE_CONFIGURE:
            ui->toolBox_stage->setCurrentIndex(1);
            break;
        case CHARGE_STAGE_CHARGING:
            ui->toolBox_stage->setCurrentIndex(2);
            break;
        case CHARGE_STAGE_DONE:
            ui->toolBox_stage->setCurrentIndex(5);
            break;
        default:
            break;
    }
#endif
}
