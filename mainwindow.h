#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vserialcom.h>
#include <QCloseEvent>

#include <windows.h>
#include <dbt.h>
#include "vheatmap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


//校准时用的参数
struct CalibrationData{
    double x1;
    double x2;
    double y1;
    double y2;
    double k;
    double b;
    quint8 flag = 0; //第一次校准的时候用到
};
//数据存储
struct SaveSettingStruct{
    CalibrationData calData;
    quint8  tabWidgetIndex;
    quint16 spinBoxColorScaleValue;
};

struct PressureSnapshot {
    QDateTime    timestamp;     // 记录快照时间
    std::vector<double> data;   // 压力数据副本
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //基础功能的串口类
    vSerialCom    vSerialCtr;
    bool          vMapMode = false;

    /*----------------串口插拔检测-----------------*/
    void vWindownInit(void);
    /*-------------------------------------------*/
    /*初始化操作主要包含信号与槽的建立，以及信号与槽的逻辑*/

    void vDependenceAddr(void); //0. 配置相关地址，应包含所有地址初始化操作
    void vInitDetection(void);  //1.串口检测相关的初始化,检测串口设备
    void vUpdateComInfo(void);  //2.串口设置内容初始化，波特率等
    void vUpdateSerial(void);   //3.初始化串口设备更新支持
    void vInfoChangedInit(void);//4.串口信息更新，只需要初始化一次
    void vInitSerialRx(void);   //5.加载信息接收配置
    void vInitHeatMap(void);    //6.初始化压力图
    void vInitTableView();

    void vShowTimerCfg(void);   //10.配置刷新相关的定时器

    void vUpdateShow(void);
    void readSerialChange(void);//读取选中串口
    void SerialOpen(void);      //打开串口
    void SerialClose(void);     //关闭串口
    QPointF calculatePressureCenterTrajectory(const std::vector<QPointF>& Point, const std::vector<double>& FData);

    /*--------------界面配置恢复相关----------------*/
    void vReadSettings(void);       //启动时读取设置
    void vWriteSettings(void);      //关闭时保存设置

    void startSaveData(void);
    void stopSaveData(void);

protected:
    void closeEvent(QCloseEvent *event);
    void doCritical(const QString &str);
    void doWarning(const QString &str);
    void doHelp(void);
    void doAbout(void);
    /*----------------串口插拔检测-----------------*/
    bool nativeEvent(const QByteArray & eventType, void * message, long*result);
public slots:
    void vRxSlotChanged(void);
    void vRxHexEnableCfg(void);
    void updateFTableData(std::vector<double> &channels);
    void getADCData(std::vector<double> &channels);
    void saveData();
    void saveDataing();
private:
    QFile m_file;
    QString m_filePath;
    SaveSettingStruct vSaveSetting;
    std::vector<double> pressFData;
    std::vector<double> ADCData;
    QTimer *saveTimer;
    quint8 saveFlag = 0;
    Ui::MainWindow *ui;
    /*--------------快照保存相关----------------*/
    // 用于存储所有快照
    QVector<PressureSnapshot> m_snapshots;

    // 高峰时刻检测用
    double m_prevMaxPressVal = 0.0;
    std::vector<double> m_prevPressFData;
    bool m_snapshotTaken = false;

    // 抓取并内存保存快照（不外存）
    void takePressureSnapshot(const std::vector<double>& data);
signals:
    void vOpenSerial(bool & isOpen);
    void vCloseSerial(void);
    void plaintextEditShowOne(void);
    void sendDoWarning(const QString &str);
};
#endif // MAINWINDOW_H
