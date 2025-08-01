#include "mainwindow.h"

#include <qsettings.h>

#include <QComboBox>
#include <QMessageBox>
#include <QSpinBox>
#include <iostream>  // 包含输入输出头文件
#include <vector>    // 包含 vector 容器头文件

#include "ui_mainwindow.h"
#include "vplaintextedit.h"
#include "vqcustomplot.h"

/*-----------界面刷新相关的定时器-----------*/
const qint32 vTimerRxPlainText = 100;
const QString CfgPath = "/Config";

// #define ROWS 70
// #define COLS 56

// 用于win设备检测
#pragma comment(lib, "user32.lib")

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 0. 配置相关地址，应包含所有地址初始化操作
    vDependenceAddr();
    // 1.串口检测相关的初始化,检测串口设备
    vInitDetection();
    // 2.串口设置内容初始化，波特率等
    vUpdateComInfo();
    // 3.初始化串口设备更新支持
    vUpdateSerial();
    // 4.串口信息更新，只需要初始化一次
    vInfoChangedInit();
    // 5.加载信息接收配置
    vInitSerialRx();
    // 6.初始化压力图
    vInitHeatMap();
    // 7.初始化表格显示
    vInitTableView();
    // 10.配置刷新相关的定时器
    vShowTimerCfg();

    vReadSettings();

    vUpdateShow();

    qDebug() << "main tid:MainWindow" << QThread::currentThreadId();
    // SerialOpen()
}

MainWindow::~MainWindow() { delete ui; }
// 退出事件->读取存储配置
void MainWindow::closeEvent(QCloseEvent *event)
{
    vWriteSettings();
    event->accept();
}
// 0. 配置相关地址，应包含所有地址初始化操作
void MainWindow::vDependenceAddr(void) {}
/******************************************************************************************************/
/**********************************************串口插拔检测**********************************************/
/******************************************************************************************************/
// 1.0 串口设备初始化操作
void MainWindow::vInitDetection(void)
{
    // USB插拔事件   Windown 版本
    vWindownInit();
    // 打开串口
    connect(ui->pushButtonOpen, &QPushButton::released, this, &MainWindow::SerialOpen);
    // 打开串口时阻塞主线程，等待返回是否打开成功
    connect(this, &MainWindow::vOpenSerial, &this->vSerialCtr.vSerial, &vSerialPort::vSerialOpen,
            Qt::BlockingQueuedConnection);

    connect(this, &MainWindow::vCloseSerial, &this->vSerialCtr.vSerial, &vSerialPort::vSerialClose,
            Qt::QueuedConnection);
}
/*************此部分代码调用了windows控件，不适用于Linux，如需移植，可以尝试替换************************/
// 注册插拔事件,调用了Windown控件
// 1.1 串口设备初始化操作
void MainWindow::vWindownInit(void)
{
    static const GUID GUID_DEVINTERFACE_LIST[] = {
        // GUID_DEVINTERFACE_USB_DEVICE
        {0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}},
        //    // GUID_DEVINTERFACE_DISK
        //    { 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9,
        //    0x1e, 0xfb, 0x8b } },
        //    // GUID_DEVINTERFACE_HID,
        //    { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11,
        //    0x00, 0x00, 0x30 } },
        //    // GUID_NDIS_LAN_CLASS
        //    { 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f,
        //    0xc3, 0x35, 0x8c } },
        //    // GUID_DEVINTERFACE_COMPORT
        //    { 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e,
        //    0x30, 0x1f, 0x73 } },
        //    // GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
        //    { 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B,
        //    0xE1, 0x03, 0x18 } },
        //    // GUID_DEVINTERFACE_PARALLEL
        //    { 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8,
        //    0x00, 0x84, 0x5C } },
        //    // GUID_DEVINTERFACE_PARCLASS
        //    { 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8,
        //    0x75, 0x3E, 0xD1 } },
    };
    // 注册插拔事件
    HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
    ZeroMemory(&NotifacationFiler, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
    {
        NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
        // GetCurrentUSBGUID();
        hDevNotify = RegisterDeviceNotification(HANDLE(this->winId()), &NotifacationFiler, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!hDevNotify)
        {
            GetLastError();
        }
    }
}
/*USB设备插入触发事件*/
// 1.2 串口设备依赖
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG *msg = reinterpret_cast<MSG *>(message);  // 第一层解算
    UINT msgType = msg->message;
    if (msgType == WM_DEVICECHANGE)
    {
        PDEV_BROADCAST_HDR lpdb = PDEV_BROADCAST_HDR(msg->lParam);  // 第二层解算
        switch (msg->wParam)
        {
            case DBT_DEVICEARRIVAL:
                if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                {
                    PDEV_BROADCAST_DEVICEINTERFACE pDevInf = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);
                    //                QString device_name =
                    //                QString::fromLocal8Bit("插入设备(name)：")+QString::fromWCharArray(pDevInf->dbcc_name,int(pDevInf->dbcc_size)).toUtf8();
                    //                qDebug()<< device_name;
                    // 插入设备重新扫描
                    vUpdateSerial();
                }
                break;
            case DBT_DEVICEREMOVECOMPLETE:
                if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                {
                    PDEV_BROADCAST_DEVICEINTERFACE pDevInf = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);
                    //                QString device_name =
                    //                QString::fromLocal8Bit("移除设备(name):")+QString::fromWCharArray(pDevInf->dbcc_name,int(pDevInf->dbcc_size)).toUtf8();
                    //                qDebug()<< device_name;
                    // 移除设备重新扫描
                    vUpdateSerial();
                }
                break;
        }
    }
    return false;
}
// 2.串口设置内容初始化，波特率等
void MainWindow::vUpdateComInfo(void)
{
    /*comboBoxCom1相关字符需要从设备读取*/

    /*comboBox配置波特率*/
    ui->comboBoxCom2->addItem("2000000", int(2000000));
    ui->comboBoxCom2->addItem("1382400", int(1382400));
    ui->comboBoxCom2->addItem("921600", int(921600));
    ui->comboBoxCom2->addItem("460800", int(460800));
    ui->comboBoxCom2->addItem("256000", int(256000));
    ui->comboBoxCom2->addItem("230400", int(230400));
    ui->comboBoxCom2->addItem("128000", int(128000));
    ui->comboBoxCom2->addItem("115200", int(115200));
    ui->comboBoxCom2->addItem("76800", int(76800));
    ui->comboBoxCom2->addItem("57600", int(57600));
    ui->comboBoxCom2->addItem("43000", int(43000));
    ui->comboBoxCom2->addItem("38400", int(38400));
    ui->comboBoxCom2->addItem("19200", int(19200));
    ui->comboBoxCom2->addItem("14400", int(14400));
    ui->comboBoxCom2->addItem("9600", int(9600));
    ui->comboBoxCom2->addItem("4800", int(4800));
    ui->comboBoxCom2->addItem("2400", int(2400));
    ui->comboBoxCom2->addItem("1200", int(1200));
    // 默认选择115200
    ui->comboBoxCom2->setCurrentText("115200");

    /*comboBox配置停止位*/
    ui->comboBoxCom3->addItem("1", QSerialPort::StopBits(QSerialPort::OneStop));
    ui->comboBoxCom3->addItem("1.5", QSerialPort::StopBits(QSerialPort::OneAndHalfStop));
    ui->comboBoxCom3->addItem("2", QSerialPort::StopBits(QSerialPort::TwoStop));

    /*comboBox配置数据位*/
    ui->comboBoxCom4->addItem("8", QSerialPort::DataBits(QSerialPort::Data8));
    ui->comboBoxCom4->addItem("7", QSerialPort::DataBits(QSerialPort::Data7));
    ui->comboBoxCom4->addItem("6", QSerialPort::DataBits(QSerialPort::Data6));
    ui->comboBoxCom4->addItem("5", QSerialPort::DataBits(QSerialPort::Data5));

    /*comboBox配置奇偶校验*/
    ui->comboBoxCom5->addItem("无", QSerialPort::Parity(QSerialPort::NoParity));
    ui->comboBoxCom5->addItem("偶校验", QSerialPort::Parity(QSerialPort::EvenParity));
    ui->comboBoxCom5->addItem("无奇校验", QSerialPort::Parity(QSerialPort::OddParity));

    /*配置编码格式*/
    ui->comboBoxCom6->addItem("NoCode", QSerialPort::Parity(SerialCodeNo));
    ui->comboBoxCom6->addItem("UTF8", QSerialPort::Parity(SerialCodeUtf8));
    ui->comboBoxCom6->addItem("UTF16", QSerialPort::Parity(SerialCodeUtf16));
    ui->comboBoxCom6->addItem("GB18030", QSerialPort::Parity(SerialCodeGB18030));
    ui->comboBoxCom6->addItem("ISO8859", QSerialPort::Parity(SerialCodeISO8859));
    ui->comboBoxCom6->addItem("BIG5", QSerialPort::Parity(SerialCodeBig5));
    ui->comboBoxCom6->addItem("Shift-JIS", QSerialPort::Parity(SerialShiftJIS));

    /*设置数据流控制*/
    ui->comboBoxCom7->addItem("NoFlowControl", QSerialPort::Parity(QSerialPort::NoFlowControl));
    ui->comboBoxCom7->addItem("RTS/CTS", QSerialPort::Parity(QSerialPort::HardwareControl));
    ui->comboBoxCom7->addItem("XON/XOFF", QSerialPort::Parity(QSerialPort::SoftwareControl));
};

// 3.初始化串口设备更新支持
void MainWindow::vUpdateSerial(void)
{
    bool haveIt = false;
    static qint16 index;  // 记录上次打开选中
    this->vSerialCtr.vSerial.vSerialComScanf();
    ui->comboBoxCom1->clear();  // 先清除之前选中的串口端口号
    haveIt = false;
    index = 0;
    if (this->vSerialCtr.vSerial.vSerialPortCnt != 0)
    {
        for (qint16 i = 0; i < this->vSerialCtr.vSerial.vSerialPortCnt; i++)
        {
            QString str = "(" + this->vSerialCtr.vSerial.vSerialComInfo[i].SerialName + ")" +
                          this->vSerialCtr.vSerial.vSerialComInfo[i].DescribeStr;
            ui->comboBoxCom1->addItem(str, this->vSerialCtr.vSerial.vSerialComInfo[i].SerialName);

            if (this->vSerialCtr.vSerial.vSerialComInfo[i].SerialName ==
                this->vSerialCtr.vSerial.vSerialConfig->vSerialPortName)
            {
                haveIt = true;
                index = i;
            }
        }
        // 检测上次打开的串口是否存在
        if (haveIt != false)
        {
            ui->comboBoxCom1->setCurrentIndex(index);
        }
        else
        {
            // 如果之前串口为打开状态，在串口拔出时，串口应变为关闭，因此按钮需要同步
            this->SerialClose();
            this->vSerialCtr.vSerial.vSerialConfig->vSerialPortName = ui->comboBoxCom1->currentData().toString();
        }
    }
    else
    {
        this->SerialClose();
        ui->comboBoxCom1->clear();
        this->vSerialCtr.vSerial.vSerialConfig->vSerialPortName = " ";
    }
};
// 4.串口信息更新,只需要初始化一次
void MainWindow::vInfoChangedInit(void)
{
    // 串口波特率
    this->vSerialCtr.vSerial.vSerialConfig->vSerialBaudRate = ui->comboBoxCom2->currentData().toInt();
    connect(ui->comboBoxCom2, &QComboBox::currentTextChanged, [=]()
            { this->vSerialCtr.vSerial.vSerialConfig->vSerialBaudRate = ui->comboBoxCom2->currentData().toInt(); });
    // 串口数据位
    this->vSerialCtr.vSerial.vSerialConfig->vSerialStopBits =
        QSerialPort::StopBits(ui->comboBoxCom3->currentData().toInt());
    connect(ui->comboBoxCom3, &QComboBox::currentTextChanged,
            [=]()
            {
                this->vSerialCtr.vSerial.vSerialConfig->vSerialStopBits =
                    QSerialPort::StopBits(ui->comboBoxCom3->currentData().toInt());
            });
    this->vSerialCtr.vSerial.vSerialConfig->vSerialDataBits =
        QSerialPort::DataBits(ui->comboBoxCom4->currentData().toInt());
    connect(ui->comboBoxCom4, &QComboBox::currentTextChanged,
            [=]()
            {
                this->vSerialCtr.vSerial.vSerialConfig->vSerialDataBits =
                    QSerialPort::DataBits(ui->comboBoxCom4->currentData().toInt());
            });
    // 串口极性
    this->vSerialCtr.vSerial.vSerialConfig->vSerialParrity =
        QSerialPort::Parity(ui->comboBoxCom5->currentData().toInt());
    connect(ui->comboBoxCom5, &QComboBox::currentTextChanged,
            [=]()
            {
                this->vSerialCtr.vSerial.vSerialConfig->vSerialParrity =
                    QSerialPort::Parity(ui->comboBoxCom5->currentData().toInt());
            });
    // 串口编码格式
    *this->vSerialCtr.vSerial.vCodeConver = vSerialCodeModeEnum(ui->comboBoxCom6->currentData().toInt());
    connect(ui->comboBoxCom6, &QComboBox::currentTextChanged, [=]()
            { *this->vSerialCtr.vSerial.vCodeConver = vSerialCodeModeEnum(ui->comboBoxCom6->currentData().toInt()); });
    // 串口流控
    this->vSerialCtr.vSerial.vSerialConfig->vSerialFlowControl =
        QSerialPort::FlowControl(ui->comboBoxCom7->currentData().toInt());
    connect(ui->comboBoxCom7, &QComboBox::currentTextChanged,
            [=]()
            {
                this->vSerialCtr.vSerial.vSerialConfig->vSerialFlowControl =
                    QSerialPort::FlowControl(ui->comboBoxCom7->currentData().toInt());
            });
}

// 5.加载信息接收配置
void MainWindow::vInitSerialRx(void)
{
    // 切换数据接收函数
    connect(ui->pushButtonOpen, &QPushButton::released, this, &MainWindow::vRxSlotChanged);
}

void MainWindow::vInitHeatMap()
{
    vSerialCtr.vMapCtr.timerStart();
    ui->verticalLayout_heatmap->addWidget(vSerialCtr.vMapCtr.m_customPlot);

    // spinBox默认设置
    ui->spinBox->setValue(3000);
    ui->spinBox->setRange(0, 10000);
    ui->spinBox->setSingleStep(100);
    ui->spinBox->setWrapping(true);  // 开启循环

    // 调节色条阈值
    connect(ui->spinBox, SIGNAL(valueChanged(int)), &this->vSerialCtr.vMapCtr, SLOT(recvColorScale(int)));

    // checkBox_showchannel默认设置
    ui->checkBox_showChannel->setChecked(false);
    // 通道名字是否显示
    connect(ui->checkBox_showChannel, &QCheckBox::clicked, &this->vSerialCtr.vMapCtr, &vHeatMap::showChannelName);

    connect(&this->vSerialCtr.vMapCtr, &vHeatMap::sendData, this, &MainWindow::getADCData);

    saveTimer = new QTimer;
    connect(ui->pushButtonExportData, &QPushButton::clicked, this, &MainWindow::saveData);

    connect(saveTimer, &QTimer::timeout, this, &MainWindow::saveDataing);

    connect(ui->pushButtonCalibration1, &QPushButton::clicked,
            [&]()
            {
                vSaveSetting.calData.y1 = ui->lineEditCalibration1->text().toDouble();
                vSaveSetting.calData.x1 = ADCData[5 * COLS + 5];  // 选取校准的点
                qDebug() << "vSaveSetting.calData.x1" << vSaveSetting.calData.x1;
            });
    // 回放按键
    // connect(ui->btnStartPlay, &QPushButton::clicked, &this->vSerialCtr.vMapCtr, &vHeatMap::startTimedPlayback);
    connect(ui->btnStartPlay, &QPushButton::clicked,
               &this->vSerialCtr.vMapCtr,   // receiver：vHeatMap 对象
               [this]() {                   // 捕获 this，才能访问成员
           auto &map = this->vSerialCtr.vMapCtr;
           if (map.all_segments.empty()) return;
           map.startTimedPlayback(map.all_segments.back());
       });

    connect(ui->btnNextFrame, &QPushButton::clicked, &this->vSerialCtr.vMapCtr, &vHeatMap::playbackNextFrameTimedkey);
    connect(ui->pushButton_static, &QPushButton::clicked, this,
            [this]()
            {
                staticTimer = QTime::currentTime();
                qDebug() << "采集静态" << staticTimer.toString();
            });
    //    connect(ui->pushButton_dynamic, &QPushButton::clicked, &this->vSerialCtr.vMapCtr, &vHeatMap::coldynamic);

    this->vSerialCtr.vMapCtr.ui = ui;
    connect(ui->tabWidget, &QTabWidget::currentChanged, this,
            [=](int index)
            {
                qDebug() << "当前页索引：" << index;

                if (index == 1)
                {
                    // 第 0 页：显示热力图
                    // ui->verticalLayout_heatmap->addWidget(vSerialCtr.vMapCtr.m_customPlot);
                    ui->verticalLayout_heatmap->addWidget(vSerialCtr.vMapCtr.m_customPlot);
                }
                else if (index == 2)
                {
                    // 第 1 页：显示数据表格
                    // 可选：更新表格内容
                    // ui->tableWidget->setItem(...);
                    ui->verticalLayout_heatmap_3->addWidget(vSerialCtr.vMapCtr.m_customPlot);
                }
            });

    connect(ui->pushButtonCalibration2, &QPushButton::clicked,
            [&]()
            {
                vSaveSetting.calData.y2 = ui->lineEditCalibration2->text().toDouble();
                if (vSaveSetting.calData.y1 < vSaveSetting.calData.y2)
                {
                    vSaveSetting.calData.x2 = ADCData[5 * COLS + 5];  // 选取校准的点
                    if (vSaveSetting.calData.x2 != vSaveSetting.calData.x1)
                    {
                        vSaveSetting.calData.k = (vSaveSetting.calData.y2 - vSaveSetting.calData.y1) /
                                                 (vSaveSetting.calData.x2 - vSaveSetting.calData.x1);
                        vSaveSetting.calData.b =
                            vSaveSetting.calData.y1 - vSaveSetting.calData.k * vSaveSetting.calData.x1;
                        vSaveSetting.calData.flag = 1;
                        qDebug() << "vSaveSetting.calData.k" << vSaveSetting.calData.k;
                        qDebug() << "vSaveSetting.calData.b" << vSaveSetting.calData.b;
                    }
                    else
                    {
                        sendDoWarning("第二次校准的ADC值与第一次的一样！");
                    }
                    qDebug() << "vSaveSetting.calData.x2" << vSaveSetting.calData.x2;
                }
                else
                {
                    sendDoWarning("请输入比Value1大的值！");
                }
            });
    connect(ui->tabWidget, &QTabWidget::currentChanged,
            [&]() { vSaveSetting.tabWidgetIndex = ui->tabWidget->currentIndex(); });

    connect(this, &MainWindow::sendDoWarning, this, &MainWindow::doWarning);
}
void MainWindow::vInitTableView()
{
    /*F*/
    ui->tableWidgetF->setRowCount(static_cast<int>(COLS));
    ui->tableWidgetF->setColumnCount(static_cast<int>(ROWS));
    // 自适应所有列和行，布满整个空间
    ui->tableWidgetF->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetF->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 隐藏表头
    ui->tableWidgetF->horizontalHeader()->setVisible(false);
    ui->tableWidgetF->verticalHeader()->setVisible(false);
    // 列宽自动调整选项
    // ui->tableWidgetF->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);       // 用户可调整
    // ui->tableWidgetF->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);             // 固定大小
    // ui->tableWidgetF->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);           // 拉伸填充
    // ui->tableWidgetF->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);  // 根据内容调整

    // // 行高自动调整选项
    // ui->tableWidgetF->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    // ui->tableWidgetF->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    // ui->tableWidgetF->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // ui->tableWidgetF->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableWidgetF->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // // 如果需要立即生效
    // ui->tableWidgetF->updateGeometry();
    ui->tableWidgetF->setFixedSize(3700, 1500);
    connect(ui->tableWidgetF, &QTableWidget::cellClicked,
            [=](int row, int column) { qDebug() << "Clicked cell at row:" << row << ", column:" << column; });
    connect(&this->vSerialCtr.vMapCtr, &vHeatMap::sendData, this, &MainWindow::updateFTableData);
}
// QPointF calculatePressureCenterTrajectory(
//     const std::vector<QPointF> &Point, const std::vector<double> &FData) {
//     double weightedSumX = 0.0, weightedSumY = 0.0;
//     QPointF Center;

//     if (Point.size() != FData.size() || FData.empty()) {
//         throw std::invalid_argument(
//             "Vectors must be of the same size and not empty.");
//     }
//     for (size_t i = 0; i < Point.size(); ++i) {
//         weightedSumX += Point[i].x() * FData[i];
//         weightedSumY += Point[i].y() * FData[i];
//     }
//     double sumPressFData = std::accumulate(FData.begin(), FData.end(), 0.0);
//     Center =
//         QPointF(weightedSumX / sumPressFData, weightedSumY / sumPressFData);
//     return Center;
// }
QPointF PressureCenterTrajectory;  // 压力中心
void MainWindow::updateFTableData(std::vector<double> &channels)
{
    static std::vector<double> maxpressFData;
    pressFData.resize(3920, 0);
    maxpressFData.resize(3920, 0);
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            // double ADCData =
            //     double(channels[row * COLS + col] * vSaveSetting.calData.k +
            //            vSaveSetting.calData.b);
            // double ADCData = double(channels[row * COLS + col]);          //* 1323 / 74000);
            // pressFData[row * COLS + col] = round(ADCData * 10.0) / 10.0;  // 保留一位小数
            pressFData[row * COLS + col] = channels[row * COLS + col];  // 保留一位小数
            // 用于测试
            if (pressFData[row * COLS + col] > 0) maxpressFData[row * COLS + col] = pressFData[row * COLS + col];
            // if (pressFData[row * COLS + col] < 0.15)
            // {
            //     pressFData[row * COLS + col] = 0;
            // }
            //            QTableWidgetItem *item = new QTableWidgetItem(QString::number(pressFData[row * COLS + col]));
            QTableWidgetItem *item =
                new QTableWidgetItem(QTableWidgetItem(QString::number(qRound(pressFData[row * COLS + col]))));
            // QTableWidgetItem *item = new QTableWidgetItem(QString::number(maxpressFData[row * COLS + col]));
            ui->tableWidgetF->setItem(COLS - col - 1, row, item);  // 行上下颠倒
            // if (COLS - col - 1 == 0)
            // {
            //     qDebug() << "row:" << row << "col:" << COLS - col - 1
            //              << "pressFData:" << maxpressFData[row * COLS + col];
            // }
        }
    }
    /*最大压力点*/
    auto MaxIter = std::max_element(pressFData.begin(), pressFData.end());
    double MaxPressVal = *MaxIter;
    int MaxPressIndex = std::distance(pressFData.begin(), MaxIter);
    bool isAllZero = std::all_of(pressFData.begin(), pressFData.end(), [](double val) { return val == 0.0; });
    if (isAllZero)
    {
        MaxPressVal = 0;
        MaxPressIndex = -1;
    }
    ui->lineEditMaxPressPoint->setText(QString::number(MaxPressIndex + 1));
    ui->lineEditMaxPressValue->setText(QString::number(MaxPressVal));
    // —— 拐点检测逻辑 ——
    //     if (!m_snapshotTaken && (MaxPressVal > m_prevMaxPressVal)) {
    //         m_prevMaxPressVal  = MaxPressVal;
    //         m_prevPressFData   = pressFData;
    //     }
    //     else if (!m_snapshotTaken && MaxPressVal < m_prevMaxPressVal) {
    //         // 抓取上一次记录的峰值数据
    //         takePressureSnapshot(m_prevPressFData);
    //         m_snapshotTaken = true;
    //         // 显示捕获的画面
    //         for (int row = 0; row < ROWS; row++)
    //         {
    //             for (int col = 0; col < COLS; col++)
    //             {
    //                 // double ADCData = double(channels[row * COLS + col] *
    //                 vSaveSetting.calData.k + vSaveSetting.calData.b);
    //                 // pressFData[row * COLS + col] = round(ADCData * 10.0)
    //                 / 10.0; //保留一位小数
    //                 // if(pressFData[row * COLS + col] < 0.15)
    //                 // {pressFData[row * COLS + col] = 0;}
    // //                QTableWidgetItem *item = new
    // QTableWidgetItem(QString::number(m_prevPressFData[row * COLS + col]));
    //                 // ui->tableWidgetF->setItem(col, row, item);
    //                 //行上下颠倒
    //             }
    //         }
    //     }
    // —— 结束 ——

    /*平均压力*/
    double sumValue = std::accumulate(pressFData.begin(), pressFData.end(), 0.0);
    double avgValue = sumValue / std::distance(pressFData.begin(), pressFData.end());
    ui->lineEditAveragePress->setText(QString::number(round(avgValue * 100.0) / 100.0));
    ui->lineEditTotalgePress->setText(QString::number(round(sumValue * 100.0) / 100.0));

    // 接触面积 按图上坐标比例来 15 * 2.8一个鞋垫面积
    double Area = 400 * 600 / 3924;  // 传输的数据是3924个点
    double AreaNumber = std::count_if(
        pressFData.begin(), pressFData.end(), [&](double value)
        { return value > vSaveSetting.calData.y1 + 1; });  // vSaveSetting.calData.y2-vSaveSetting.calData.y1;
    //                qDebug() << "leftAreaNumber" << leftAreaNumber;
    double Area1 = round(Area * AreaNumber * 100.0) / 100.0;
    //                qDebug() << "leftArea" << leftArea;
    ui->lineEditArea->setText(QString::number(Area1));

    /*压力中心轨迹*/
    //    std::vector<QPointF>
    //    m_channelAxis1(this->vSerialCtr.vMapCtr.m_channelAxis.begin(),
    //    this->vSerialCtr.vMapCtr.m_channelAxis.end()); std::vector<double>
    //    pressFData1(pressFData.begin(), pressFData.end());
    // PressureCenterTrajectory =
    // calculatePressureCenterTrajectory(this->vSerialCtr.vMapCtr.m_channelAxis,
    // pressFData);

    // ui->lineEditPressureCenterTrajectory->setText(QString("(%1,
    // %2)").arg(PressureCenterTrajectory.x(),0,'f',2).arg(PressureCenterTrajectory.y(),0,'f',2));
}
void MainWindow::getADCData(std::vector<double> &channels)
{
    ADCData.assign(channels.begin(),
                   channels.end());  // assign函数将A的元素复制到B
    saveFlag = 1;
    //    qDebug() << "36个点的数值" << ADCData[5*COLS+5];
}
// 10.定时器初始化
void MainWindow::vShowTimerCfg(void)
{
    //    ui->plainTextRx->TimerStop();

    //    ui->plainTextRx->setTimerCfg(vTimerRxPlainText);

    //    ui->plainTextRx->TimerStart();
}
// 同步窗口和变量数据
void MainWindow::vUpdateShow(void)
{
    readSerialChange();
    // 以下函数根据参数变化更新显示
    vRxSlotChanged();  // 刷新接收槽函数连接
    //    vTxSlotChanged();        //刷新发送槽函数连接
    vRxHexEnableCfg();  // 刷新RxHexEnable
    //    vTxHexEnableCfg();       //刷新TxHexEnable
    //    vTxModeCfg();            //发送模式切换处理
    //    vTxStampCfg();           //发送换行符控制
    //    vTxModeTimerCfg();       //发送的定时器控制
}
// 读取串口配置
void MainWindow::readSerialChange(void)
{
    this->vSerialCtr.vSerial.vSerialConfig->vSerialPortName = ui->comboBoxCom1->currentData().toString();
    this->vSerialCtr.vSerial.vSerialConfig->vSerialBaudRate = ui->comboBoxCom2->currentData().toInt();
    this->vSerialCtr.vSerial.vSerialConfig->vSerialStopBits =
        QSerialPort::StopBits(ui->comboBoxCom3->currentData().toInt());
    this->vSerialCtr.vSerial.vSerialConfig->vSerialDataBits =
        QSerialPort::DataBits(ui->comboBoxCom4->currentData().toInt());
    this->vSerialCtr.vSerial.vSerialConfig->vSerialParrity =
        QSerialPort::Parity(ui->comboBoxCom5->currentData().toInt());
    this->vSerialCtr.vSerial.vSerialConfig->vSerialFlowControl =
        QSerialPort::FlowControl(ui->comboBoxCom7->currentData().toInt());
}

// 打开串口设备
void MainWindow::SerialOpen(void)
{
    if (ui->pushButtonOpen->text() == QString("打开串口"))
    {
        // 读取选择的串口配置
        readSerialChange();
        bool isOpen;
        emit vOpenSerial(isOpen);
        if (isOpen)
        {
            ui->pushButtonOpen->setText("关闭串口");
            ui->comboBoxCom1->setEnabled(false);
            ui->comboBoxCom2->setEnabled(false);
            ui->comboBoxCom3->setEnabled(false);
            ui->comboBoxCom4->setEnabled(false);
            ui->comboBoxCom5->setEnabled(false);
            ui->comboBoxCom6->setEnabled(false);
            ui->comboBoxCom7->setEnabled(false);
        }
        else
        {
            // 打开串口失败
            this->doWarning("打开串口失败，请检查串口是否插入！");
            this->SerialClose();
        }
    }
    else
    {
        this->SerialClose();
        ui->pushButtonOpen->setText("打开串口");
        ui->comboBoxCom1->setEnabled(true);
        ui->comboBoxCom2->setEnabled(true);
        ui->comboBoxCom3->setEnabled(true);
        ui->comboBoxCom4->setEnabled(true);
        ui->comboBoxCom5->setEnabled(true);
        ui->comboBoxCom6->setEnabled(true);
        ui->comboBoxCom7->setEnabled(true);
    }
}

// 刷新接收槽函数
void MainWindow::vRxSlotChanged(void)
{
    if (ui->pushButtonOpen->isChecked())
    {
        // 先断开所有，避免重复套用了链接
        //        this->vSerialCtr.vQObjectTxCtr.vDisConnectTx();
        this->vSerialCtr.vQObjectRxCtr.vDisConnectRx();
        this->vSerialCtr.vMapCtr.vDisConnect();
        this->vSerialCtr.vMapCtr.vConnect();
    }
    else  // 连接为串口调试助手接收
    {
        //        this->vSerialCtr.vQObjectTxCtr.vDisConnectTx();
        this->vSerialCtr.vQObjectRxCtr.vDisConnectRx();
        this->vSerialCtr.vMapCtr.vDisConnect();
        this->vSerialCtr.vQObjectRxCtr.vConnectRx();
        //        this->vSerialCtr.vQObjectTxCtr.vConnectTx();
    }
}
// 刷新RxHexEnable
void MainWindow::vRxHexEnableCfg(void)
{
    //    if(ui->checkBoxRx1->isChecked())
    //    {
    //        *ui->plainTextRx->hexEnable = true;
    //    }
    //    else
    //    {
    //        *ui->plainTextRx->hexEnable = false;
    //    }
    emit plaintextEditShowOne();
}

// 关闭串口设备
void MainWindow::SerialClose(void) { emit vCloseSerial(); }
// 启动时读取设置
void MainWindow::vReadSettings(void)
{
    // 获取配置路径
    QString path = qApp->applicationDirPath() + CfgPath + "/" + "config.ini";
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("Configuration");
    // 恢复上次的界面位置和大小
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    // 串口配置
    ui->comboBoxCom1->setCurrentIndex(settings.value("PortName", QVariant(0)).toInt());  // QVariant中的值不懂
    ui->comboBoxCom2->setCurrentIndex(settings.value("BaudRate", QVariant(0)).toInt());
    ui->comboBoxCom3->setCurrentIndex(settings.value("StopBits", QVariant(0)).toInt());
    ui->comboBoxCom4->setCurrentIndex(settings.value("DataBits", QVariant(0)).toInt());
    ui->comboBoxCom5->setCurrentIndex(settings.value("Parrity", QVariant(0)).toInt());
    ui->comboBoxCom6->setCurrentIndex(settings.value("CodeMode", QVariant(0)).toInt());
    ui->comboBoxCom7->setCurrentIndex(settings.value("FlowControl", QVariant(0)).toInt());
    //    ui->checkBoxRx1->setChecked(settings.value("rxHexEnable",false).toBool());
    ui->spinBox->setValue(settings.value("colorScale", QVariant(0)).toInt());
    emit ui->spinBox->valueChanged(ui->spinBox->value());

    vSaveSetting.calData.x1 = settings.value("x1", QVariant(0)).toDouble();
    vSaveSetting.calData.y1 = settings.value("y1", QVariant(0)).toDouble();

    ui->lineEditCalibration1->setText(QString::number(vSaveSetting.calData.y1));
    vSaveSetting.calData.x2 = settings.value("x2", QVariant(0)).toDouble();
    vSaveSetting.calData.y2 = settings.value("y2", QVariant(0)).toDouble();
    ui->lineEditCalibration2->setText(QString::number(vSaveSetting.calData.y2));
    vSaveSetting.calData.k = settings.value("k", QVariant(0)).toDouble();
    vSaveSetting.calData.b = settings.value("b", QVariant(0)).toDouble();

    settings.endGroup();
}
// 关闭时保存设置
void MainWindow::vWriteSettings(void)
{
    QString path = qApp->applicationDirPath() + CfgPath + "/" + "config.ini";
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("Configuration");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    // 串口配置
    settings.setValue("PortName", ui->comboBoxCom1->currentIndex());
    settings.setValue("BaudRate", ui->comboBoxCom2->currentIndex());
    settings.setValue("StopBits", ui->comboBoxCom3->currentIndex());
    settings.setValue("DataBits", ui->comboBoxCom4->currentIndex());
    settings.setValue("Parrity", ui->comboBoxCom5->currentIndex());
    settings.setValue("CodeMode", ui->comboBoxCom6->currentIndex());
    settings.setValue("FlowControl", ui->comboBoxCom7->currentIndex());
    //    settings.setValue("rxHexEnable",ui->checkBoxRx1->isChecked());

    settings.setValue("colorScale", ui->spinBox->value());
    settings.setValue("x1", vSaveSetting.calData.x1);
    settings.setValue("y1", vSaveSetting.calData.y1);
    settings.setValue("x2", vSaveSetting.calData.x2);
    settings.setValue("y2", vSaveSetting.calData.y2);
    settings.setValue("k", vSaveSetting.calData.k);
    settings.setValue("b", vSaveSetting.calData.b);
    settings.setValue("b", vSaveSetting.calData.b);
    settings.setValue("flag", vSaveSetting.calData.flag);
    settings.setValue("tabWidgetIndex", vSaveSetting.tabWidgetIndex);
    settings.setValue("spinBoxColorScaleValue", vSaveSetting.spinBoxColorScaleValue);

    settings.endGroup();
}
void MainWindow::startSaveData()
{
    // 获取当前程序运行路径
    QString currentPath = QCoreApplication::applicationDirPath();

    // 构造文件名（格式：data_yyyy-MM-dd_HH-mm-ss.csv）
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString fileName = QString("data_%1.csv").arg(timestamp);
    m_filePath = QDir(currentPath).filePath(fileName);

    // 打开CSV文件
    m_file.setFileName(m_filePath);
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file for writing:" << m_file.errorString();
        return;
    }
    qDebug() << "Saving data to:" << m_filePath;
    // 写入CSV表头
    QTextStream stream(&m_file);
    QStringList headers;
    for (int i = 1; i <= ROWS * COLS; ++i)
    {
        headers.append(QString("FData%1").arg(i));
    }
    stream << headers.join(',') << Qt::endl;

    // 启动定时器，开始记录数据
    saveTimer->start(30);  // 每 30ms 保存一次数据
}

void MainWindow::stopSaveData()
{
    // 关闭文件
    m_file.close();
}
void MainWindow::saveDataing()
{
    // 写入当前数据到CSV文件
    if (m_file.isOpen() && m_file.isWritable())
    {
        QTextStream stream(&m_file);                            // ⚠️ 这句要放在前面，不能在它之后再使用 stream
        qint64 timestamp = QDateTime::currentSecsSinceEpoch();  // 获取当前Unix时间戳（秒）

        stream << timestamp << ",";  // ✅ 把时间戳写入文件

        for (int i = 0; i < ROWS * COLS; ++i)
        {
            stream << pressFData[i];
            if (i != ROWS * COLS - 1) stream << ",";  // 不在末尾加多余逗号
        }

        stream << endl;  // 换行
    }
}

void MainWindow::saveData()
{
    if (saveFlag)  // 等待初始化完毕
    {
        if (ui->pushButtonExportData->text() == "导出数据")
        {
            startSaveData();
            doWarning("Start Save");
            ui->pushButtonExportData->setText("停止导出");
        }
        else
        {
            ui->pushButtonExportData->setText("导出数据");
            stopSaveData();
            doWarning("Stop Save");
        }
    }
    else
    {
        doWarning("请等待初始化完毕!!");
    }
}
/*---------------------------提示窗口-------------------------------*/
/*错误*/
void MainWindow::doCritical(const QString &str) { QMessageBox::critical(NULL, "错误", str); }
/*警告*/
void MainWindow::doWarning(const QString &str) { QMessageBox::warning(this, "警告", str); }
/*帮助*/
void MainWindow::doHelp(void) { QMessageBox::information(this, "帮助", ""); }

/*--------------快照保存相关----------------*/
void MainWindow::takePressureSnapshot(const std::vector<double> &data)
{
    PressureSnapshot snap;
    snap.timestamp = QDateTime::currentDateTime();
    snap.data = data;  // 拷贝一份数据

    m_snapshots.append(std::move(snap));

    // （可选）在 UI 上显示一下快照总数
    //    ui->labelSnapshotCount->setText(
    //        QString::number(m_snapshots.count()));
}
