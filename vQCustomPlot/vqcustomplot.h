#ifndef MYQCUSTOMPLOT_H
#define MYQCUSTOMPLOT_H

#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QThread>
#include <qcustomplot.h>
#include <vtracer.h>

/*
 *****************************
 ************波形绘制************
 ******************************
 */
class vQCustomPlot: public QCustomPlot
{
    Q_OBJECT
public:
    explicit vQCustomPlot(QWidget *parent = nullptr);
    ~vQCustomPlot();

    bool xAxisAuto = true;
    double graphFps;
    bool fpsFlag = false;
    QString        *vName = nullptr; //把曲线的名字从主函数传过来
    vTracer        *mxTracer = nullptr; //坐标跟随鼠标.使用时创建
    qint32 GraphShowTimerSet = 0;//波形刷新
    int lenThurs = 0;
    QTimer GraphShowTimer;
    bool TimerEnable;
    bool showEnable=true;
    int graphNum = 0;
    double key = 0; //后面删掉
    double rxThursScope[2]; //后面删掉
    uint8_t keyStatus;


    void vSetNameAddr(QString * addr);
    void TimerStart(void);
    void TimerStop(void);
    void setQCustomPlotTimer(qint32 tim)
    {
        GraphShowTimerSet = tim;
    }
public slots:
    // 绘制动态曲线
    void realtimeDataSlot(void);
    void selectionChanged(void);
    void contextMenuRequest(QPoint pos);
    void moveLegend(void);
    bool ishaveGraphHide(void);
    bool isAllGraphHide(void);
    void axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);
    void legendLabelDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);
    void autoAddGraph(void);
    void addRandomGraph(void);
    void removeAllGraphs(void);
    void rescaleYAxis(void);
    void colorSet(void);
    void hideSelectedGraph(void);
    void hideAllGraph(void);
    void showAllGraph(void);
    void vReadCsvData(void);
    void vSaveCsvData(void);
    void graphScope(void);
    void showCsvData(double *data,int size);
    void ThursScope(const QVariant &rxData);
    void addThursGraph(void);
    void GraphShowSlot(void);

protected:
    //滚轮
    void wheelEvent(QWheelEvent *ev);
    //鼠标点击事件
    void mousePressEvent(QMouseEvent *ev) ;
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *ev) ;
private:
    QCustomPlot *customPlot ;	     //传入实例化的QcustomPlot


};





#endif // MYQCUSTOMPLOT_H
