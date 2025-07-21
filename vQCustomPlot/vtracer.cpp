#include "vtracer.h"
/*
 **********************************************
 ************myQCustomPlot的移动光标************
 **********************************************
 */
vTracer::vTracer(QCustomPlot *_plot, TracerType _type) : plot(_plot),
    type(_type),
    visible(false)

{
    if (plot)
    {
        // 游标
        tracer = new QCPItemTracer(plot); // 生成游标
        tracer->setStyle(QCPItemTracer::tsPlus); //可以选择设置追踪光标的样式，这个是小十字，还有大十字，圆点等样式
        tracer->setPen(QPen(Qt::blue)); //设置tracer轮廓颜色
        tracer->setBrush(Qt::blue); //设置tracer轮廓内颜色
        tracer->setSize(10); //设置大小

        // 游标说明
        label = new QCPItemText(plot); //生成游标说明
        label->setLayer("overlay"); //设置图层为overlay，因为需要频繁刷新
        label->setClipToAxisRect(false);
        label->setPadding(QMargins(4, 2, 2, 4)); //设置文本矩形边框与文本之间的距离

        label->position->setParentAnchor(tracer->position); //将游标说明锚固在tracer位置处，实现自动跟随
        label->setFont(QFont("宋体", 10)); //设置字体

        // 箭头
        arrow = new QCPItemLine(plot); //生成箭头
        arrow->setLayer("overlay"); //设置图层为overlay，因为需要频繁刷新
        arrow->setPen(QPen(Qt::blue)); //设置箭头的颜色
        arrow->setClipToAxisRect(false);
        arrow->setHead(QCPLineEnding::esSpikeArrow); //设置头部的线条结束风格，头部对应于末端位置

        switch (type) {
        case XAxisTracer:
        {
            tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            tracer->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
            label->setBrush(QBrush(QColor(244, 244, 244, 100)));
            label->setPen(QPen(Qt::black));

            label->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter); //设置游标说明的相对位置

            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(arrow->end);
            arrow->start->setCoords(20, 0);//偏移量
            break;
        }
        case YAxisTracer:
        {
            tracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
            tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

            label->setBrush(QBrush(QColor(244, 244, 244, 100)));
            label->setPen(QPen(Qt::black));
            label->setPositionAlignment(Qt::AlignRight|Qt::AlignHCenter);

            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(label->position);
            arrow->start->setCoords(-20, 0); //偏移量
            break;
        }
        case DataTracer:
        {
            tracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);

            label->setBrush(QBrush(QColor(255, 255, 255, 190)));
            label->setPen(QPen(Qt::black));
            label->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);

            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(arrow->end);
            arrow->start->setCoords(25, 0); //偏移量
            break;
        }
        }

        setVisible(false);
    }
}

vTracer::~vTracer()
{
    if (tracer)
        plot->removeItem(tracer);
    if (label)
        plot->removeItem(label);
}

void vTracer::setLabelFont(QFont font)
{
    label->setFont(font);
}

void vTracer::setPen(const QPen &pen)
{
    tracer->setPen(pen);
    arrow->setPen(pen);
}

void vTracer::setBrush(const QBrush &brush)
{
    tracer->setBrush(brush);
}

void vTracer::setLabelPen(const QPen &pen)
{
    label->setPen(pen);
}

void vTracer::setText(const QString &text)
{
    label->setText(text);
}

void vTracer::setVisible(bool visible)
{
    tracer->setVisible(visible);
    label->setVisible(visible);
    arrow->setVisible(visible);
}

void vTracer::updatePosition_Interpolation(double x, QCPGraph *mGraph)
{
    if (!visible)
    {
        setVisible(true);
        visible = true;
    }
    tracer->setGraph(mGraph);//将锚点设置到被选中的曲线上
    tracer->setGraphKey(x); //将游标横坐标设置成刚获得的横坐标数据x
    tracer->setInterpolating(true); //游标的纵坐标可以通过曲线数据线性插值自动获得
    tracer->updatePosition(); //使得刚设置游标的横纵坐标位置生效
    label->position->setCoords(25, 0);

    double xValue = tracer->position->key();
    double yValue = tracer->position->value();

    QString text = QString("%1\n x=%2, y=%3")
            .arg(mGraph->name())
            .arg(xValue)
            .arg(yValue);

    setText(text);
}

void vTracer::updatePosition_Dichotomy(double xValue, double yValue)
{
    if (!visible)
    {
        setVisible(true);
        visible = true;
    }
    if (yValue > plot->yAxis->range().upper)
        yValue = plot->yAxis->range().upper;

    switch (type) {
        case XAxisTracer:
        {
            tracer->position->setCoords(xValue, 1);
            label->position->setCoords(0, 15);
            arrow->start->setCoords(0, 15);
            arrow->end->setCoords(0, 0);

            break;
        }
        case YAxisTracer:
        {
            tracer->position->setCoords(1, yValue);
            label->position->setCoords(-20, 0);
            break;
        }
        case DataTracer:
        {
            tracer->position->setCoords(xValue, yValue);
            label->position->setCoords(25, 0);
            break;
        }
    }
}
