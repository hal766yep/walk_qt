#ifndef MYTRACER_H
#define MYTRACER_H

#include <QObject>
#include <qcustomplot.h>

/*
 **********************************************
 ************myQCustomPlot的移动光标************
 **********************************************
 */

enum TracerType
{
    XAxisTracer,
    YAxisTracer,
    DataTracer
};

class vTracer : public QObject
{
    Q_OBJECT

public:
    explicit vTracer(QCustomPlot *_plot, TracerType _type); //这里与原贴不同，按照原贴构造总是过不去
    ~vTracer();

    void setLabelFont(QFont font);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setText(const QString &text);
    void setLabelPen(const QPen &pen);
    void updatePosition_Interpolation(double x, QCPGraph *mGraph); //用库里自带的插值函数求y值
    void updatePosition_Dichotomy(double xValue, double yValue); //用自己写的二分法求y值
    void setVisible(bool visible);



protected:
    QCustomPlot *plot ;	     // 传入实例化的QcustomPlot
    QCPItemTracer *tracer;   // 游标
    QCPItemText *label;   	 // 游标说明
    QCPItemLine *arrow;  	 // 箭头

    TracerType type;
    bool visible;

signals:

public slots:

};

#endif // MYTRACER_H
