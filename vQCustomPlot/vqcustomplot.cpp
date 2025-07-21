#include "vqcustomplot.h"
#include <QDebug>
/*
 ******************************
 ************波形绘制************
 ******************************
 */
const uint8_t KEY_CTRL = 0X01;
const uint8_t KEY_ALT  = 0X02;
const uint8_t KEY_ALT_CTRL=KEY_CTRL|KEY_ALT;

vQCustomPlot::vQCustomPlot(QWidget *parent) : QCustomPlot(parent)
{
    // 开启OpenGL加速渲染，曲线不会卡顿
    this->setOpenGl(true); //qDebug() << openGl(); 测试是否成功开启

//    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
//    timeTicker->setTimeFormat("%h:%m:%s");
//    this->xAxis->setTicker(timeTicker); // 设置轴的刻度位置和标签
//    this->yAxis->setRange(-1.2, 1.2);

    this->xAxis->setRange(0, 300);
    this->yAxis->setRange(0, 1000);

    this->axisRect()->setupFullAxesBox(); // 在没有轴的一侧创建轴
    this->xAxis2->setVisible(true);
    this->xAxis2->setTickLabels(false);
    this->yAxis2->setVisible(true);
    this->yAxis2->setTickLabels(false);

    // 为坐标轴添加标签
    this->xAxis->setLabel("Time");
    this->yAxis->setLabel("Data");

    // 使上下两个X轴的范围总是相等，使左右两个Y轴的范围总是相等:
    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));

    // 支持鼠标拖拽轴的范围、滚动缩放轴的范围，可选择轴，可选择图例，左键点选图层（每条曲线独占一个图层）
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                          QCP::iSelectLegend | QCP::iSelectPlottables);

    // 将选中的graph与相应图例项同步
    /*
     * selectionChangedByUser信号在用户更改QCustomPlot中的选择后发出
     * 要想发出选中图例的信号，setInteractions()中必须设置QCP::iSelectLegend
     */
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    // 绑定定时器信号与槽:
//    connect(&GraphShowTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    connect(&GraphShowTimer, &QTimer::timeout, this, &vQCustomPlot::GraphShowSlot);

    GraphShowTimer.setTimerType(Qt::PreciseTimer); //精确定时器

    //双击修改坐标轴名称
    connect(this, SIGNAL(axisDoubleClick(QCPAxis*, QCPAxis::SelectablePart, QMouseEvent*)),
            this, SLOT(axisLabelDoubleClick(QCPAxis*, QCPAxis::SelectablePart)));
    //双击修改图例名称
    connect(this, SIGNAL(legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)),
            this, SLOT(legendLabelDoubleClick(QCPLegend*, QCPAbstractLegendItem*)));

    //显示图例名称
    this->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    this->legend->setFont(legendFont);
    this->legend->setSelectedFont(legendFont);
    this->legend->setSelectableParts(QCPLegend::spItems);

    //追踪光标
    mxTracer = new vTracer(this, TracerType::DataTracer);

    TimerStop();
    //鼠标右键逻辑
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextMenuRequest(QPoint))); //QPoint是鼠标右击的位置
}

vQCustomPlot::~vQCustomPlot()
{

}
void vQCustomPlot::TimerStart(void)
{
    TimerEnable = true;
    if(!GraphShowTimer.isActive())
    {
        GraphShowTimer.start(GraphShowTimerSet);
    }
}
void vQCustomPlot::TimerStop(void)
{
    TimerEnable = false;
    if(GraphShowTimer.isActive())
    {
        GraphShowTimer.stop();
    }
}

void vQCustomPlot::vSetNameAddr(QString *addr)
{
    this->vName=addr;
}
// 定时器溢出槽函数
void vQCustomPlot::realtimeDataSlot(void)
{
    static QTime timeStart = QTime::currentTime();
    // 计算两个新的数据点:
    key = timeStart.msecsTo(QTime::currentTime())/1000.0; // 开始后经过的时间(秒)
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.002) // 最多每2ms添加一点
    {
        lastPointKey = key;
        graphScope();
    }
    this->yAxis->rescale(true); //每次更新都自动调整Y轴，也可以用AUTO

    // 使键轴范围随数据滚动(恒定范围大小为8):
    this->xAxis->setRange(key, 8, Qt::AlignRight);
    this->replot(QCustomPlot::rpQueuedReplot); //绘制器一定要重绘，刷新界面

    // 计算FPS:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
        fpsFlag = true;
        graphFps = frameCount/(key-lastFpsKey);
        lastFpsKey = key;
        frameCount = 0;
    }

}

void vQCustomPlot::selectionChanged(void)
{
    // 使上下轴同步选择，并将轴和记号标签作为一个可选对象处理:
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        this->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        this->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // 使左右轴同步选择，并将轴和记号标签作为一个可选对象处理:
    if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        this->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        this->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // 将选中的graph与相应图例项同步:
    for (int i=0; i<this->graphCount(); ++i)
    {
        QCPGraph *graph = this->graph(i);
        QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);//返回与绘图表(例如QCPGraph*)关联的QCPPlotableLegendItem
        if (item->selected() || graph->selected()) //判断哪一条曲线或者图例被选中
        {
            item->setSelected(true); //设置选择此特定图例项
            graph->setSelection(QCPDataSelection(graph->data()->dataRange())); //设置选择绘制该graph的数据范围
        }
    }
}
void vQCustomPlot::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose); // menu接收关闭事件时，使Qt删除此menu
    if(this->legend->selectTest(pos, false) >= 0) // 如果选中图例框中的位置
    {
        menu->addAction("移动到左上角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction("移动到右上角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
        menu->addAction("移动到右下角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
        menu->addAction("移动到左下角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
        menu->addSeparator(); // 创建一个菜单分割线
    }
    if(this->graphCount()==0)
    {
//        menu->addAction("自动添加多条曲线", this, SLOT(autoAddGraph()));
        menu->addAction("自动添加多条曲线", this, SLOT(addThursGraph()));
    }
    else if (this->graphCount() > 0)
    {
        menu->addAction("移除所有曲线", this, SLOT(removeAllGraphs()));
        menu->addSeparator();
        menu->addAction("AUTO", this, SLOT(rescaleYAxis()));
        menu->addAction("实时刷新", this, [=]
        {
            xAxisAuto = true;
        });
        //选择了曲线
        if (this->selectedGraphs().size() > 0)
        {
            menu->addSeparator();
            menu->addAction("设置曲线颜色", this, SLOT(colorSet()));
            //所选曲线是否可见
            if(this->selectedGraphs().first()->visible())
            {
                menu->addAction("隐藏所选曲线", this, SLOT(hideSelectedGraph()));
            }
            else
            {
                menu->addAction("显示所选曲线", this, SLOT(hideSelectedGraph()));
            }
        }
        menu->addSeparator();
        //不是所有曲线被隐藏
        if(!isAllGraphHide())
        {
            menu->addAction("隐藏所有曲线", this, SLOT(hideAllGraph()));
        }
        //有曲线未被显示
        if(ishaveGraphHide())
        {
            menu->addAction("显示所有曲线", this, SLOT(showAllGraph()));
        }
    }
    menu->addSeparator();
    menu->addAction("导出Csv", this, SLOT(vSaveCsvData()));
    menu->addAction("导入Csv", this, SLOT(vReadCsvData()));

    menu->popup(this->mapToGlobal(pos));
}
void vQCustomPlot::addThursGraph(void)
{
    showEnable = false;
    this->clearGraphs();
    for(int i=0;i<lenThurs;i++)
    {
        addRandomGraph();
    }
    showEnable = true;
}
void vQCustomPlot::ThursScope(const QVariant &rxData)
{
    QVector<float> rxThursScope = rxData.value<QVector<float>>();
    lenThurs = 0;
    lenThurs = rxThursScope.size();

    if((showEnable)&&(lenThurs<=24)&&(lenThurs>0))
    {
        for (int i =0;(i<this->graphCount())&&(i<lenThurs);i++)
        {
            this->graph(i)->addData(this->graph(0)->dataCount(), (double)rxThursScope[i]);
        }
    }
}
void vQCustomPlot::GraphShowSlot(void)
{
    static int upper,lower,length,last_upper;
    upper = this->xAxis->range().upper;
    lower = this->xAxis->range().lower;
    length = upper - lower;

    if(last_upper>upper)
    {
        xAxisAuto=false;
    }
    else if(upper>=this->xAxis->range().upper)
    {
        xAxisAuto =true;
    }
    if(xAxisAuto == true)
    {
        this->yAxis->rescale(true);
        this->xAxis->rescale(true);
        upper = this->xAxis->range().upper;
        this->xAxis->setRange(upper,length,Qt::AlignRight);
    }
    last_upper = upper;
    this->replot(QCustomPlot::rpQueuedReplot); //绘制器一定要重绘，刷新界面

    /*计算FPS: */
    static QTime timeStart = QTime::currentTime();
    // 计算两个新的数据点:
    key = timeStart.msecsTo(QTime::currentTime())/1000.0; // 开始后经过的时间(秒)
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.002) // 最多每2ms添加一点
    {
        lastPointKey = key;
    }
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
        fpsFlag = true;
        graphFps = frameCount/(key-lastFpsKey);
        lastFpsKey = key;
        frameCount = 0;
    }

}
void vQCustomPlot::moveLegend(void)
{
    // 确保这个槽真的是由上下文菜单操作调用的，它携带了需要的数据
    // sender()如果在由信号激活的插槽中调用，则返回指向发送信号的对象的指针，contextAction是菜单中选的对象的指针，比如"移动到左上角"
    // 上面setData()给action的内部数据赋值，data()取出数据
    if (QAction* contextAction = qobject_cast<QAction*>(sender()))
    {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)
        {
            this->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt); //设置图例的位置
            this->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}
bool vQCustomPlot::ishaveGraphHide(void)
{
    int index = 0;
    for(;index < this->graphCount(); index++)
    {
        if(!this->graph(index)->visible())
        {
            return true;
        }
    }
    return false;
}
bool vQCustomPlot::isAllGraphHide(void)
{
    int index = 0;
    for(;index < this->graphCount(); index++)
    {
        if(this->graph(index)->visible())
        {
            return false;
        }
    }
    return true;
}
void vQCustomPlot::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
    // 双击轴标签，设置轴标签
    if (part == QCPAxis::spAxisLabel)
    {
        bool ok;
        // 用Qt::MSWindowsFixedSizeDialogHint不会报"setGeometry: Unable to set geometry"错误，给对话框一个固定尺寸
        QString newLabel = QInputDialog::getText(this, "更改坐标名称", "新的坐标名称",
                                                 QLineEdit::Normal, axis->label(),
                                                 &ok, Qt::MSWindowsFixedSizeDialogHint);
        if (ok)
        {
            axis->setLabel(newLabel);
            this->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}
void vQCustomPlot::legendLabelDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
    // 通过双击图形的图例项来重命名图形
    Q_UNUSED(legend) //向编译器指示函数体中未使用具有指定名称的参数，这可用于抑制编译器警告

    if (item)
    {
        QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText(this, "更改曲线名称", "新的曲线名称",
                                                QLineEdit::Normal, plItem->plottable()->name(),
                                                &ok, Qt::MSWindowsFixedSizeDialogHint); //MSWindowsFixedSizeDialogHint设置对话框的大小
        if(ok)
        {
            plItem->plottable()->setName(newName);
            this->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}
void vQCustomPlot::autoAddGraph(void)
{
    showEnable = false;
    this->clearGraphs();
    for(int i=0;i<graphNum;i++)
    {
        addRandomGraph();
    }
    showEnable = true;
}

void vQCustomPlot::addRandomGraph()
{
    this->addGraph();
    if(this->vName[this->graphCount()-1]!=nullptr)
    {
        this->graph()->setName(this->vName[this->graphCount()-1]);
    }
    this->graph()->setLineStyle((QCPGraph::LineStyle::lsLine));
//    this->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc,1.325));//数据点
    QPen graphPen;
    graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
    graphPen.setWidthF(2);
    this->graph()->setPen(graphPen);
    this->replot(QCustomPlot::rpQueuedReplot);
}

void vQCustomPlot::removeAllGraphs(void)
{
    this->clearGraphs();
}

void vQCustomPlot::rescaleYAxis(void)
{
    this->yAxis->rescale(true);
    this->replot(QCustomPlot::rpQueuedReplot);
}

void vQCustomPlot::colorSet(void)
{
    QColor color = QColorDialog::getColor(Qt::red, this, "设置曲线颜色",
                                          QColorDialog::ShowAlphaChannel);
    QPen graphPen;
    graphPen.setColor(color);
    graphPen.setWidthF(1.5);
    this->selectedGraphs().first()->setPen(graphPen);
}

// 隐藏和显示功能放在一起了
void vQCustomPlot::hideSelectedGraph(void)
{
    if (this->selectedGraphs().size() > 0)
    {
        //获取图像编号
        int index = 0;
        for(;index < this->graphCount(); index++)
        {
            if(this->graph(index)->name() == this->selectedGraphs().first()->name())
            {
                break;
            }
        }
        //可见性控制
        if(this->selectedGraphs().first()->visible())
        {
            this->selectedGraphs().first()->setVisible(false);
            this->legend->item(index)->setTextColor(Qt::gray);
        }
        else
        {
            this->selectedGraphs().first()->setVisible(true);
            this->legend->item(index)->setTextColor(Qt::black);
        }
        this->replot(QCustomPlot::rpQueuedReplot);
    }
}

void vQCustomPlot::hideAllGraph(void)
{
    int index = 0;
    for(index = 0;index < this->graphCount(); index++){
        this->graph(index)->setVisible(false);
        this->legend->item(index)->setTextColor(Qt::gray);
    }

    this->replot(QCustomPlot::rpQueuedReplot);
}

void vQCustomPlot::showAllGraph(void)
{
    int index = 0;
    for(;index < this->graphCount(); index++){
        this->graph(index)->setVisible(true);
        this->legend->item(index)->setTextColor(Qt::black);
    }
    this->replot(QCustomPlot::rpQueuedReplot);
}

void vQCustomPlot::vReadCsvData(void)
{
    QString dirpath = QFileDialog::getOpenFileName(this, QStringLiteral("导入Csv"),
                                                   qApp->applicationDirPath(), QString(tr("File (*.csv)")));
    if(dirpath!=NULL)
    {
        QFile file(dirpath);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(NULL,
                                  QString::fromLocal8Bit("提示"),
                                  QString::fromLocal8Bit("无法打开该文件"));
            return ;
        }
        else
        {
            QStringList list;
            static double csvData[24];
            list.clear();
            QTextStream csvStream(&file);
            //遍历行
            this->clearGraphs();
            this->replot(QCustomPlot::rpQueuedReplot);

            QString fileLine = csvStream.readLine();
            list =  fileLine.split(",",
                                  QString::SkipEmptyParts);
            for(int j=1;j<list.size();j++)
            {
                this->vName[j-1] = list[j];
                addRandomGraph();
            }
            for(int i = 0;!csvStream.atEnd();i++)
            {
                QString fileLine = csvStream.readLine();
                list =  fileLine.split(",",
                                       QString::KeepEmptyParts);
                //根据","开分隔开每行的列
                for(int j=1;j<list.size();j++)
                {
                    csvData[j-1] = list[j].toDouble();
                }
                this->showCsvData(csvData,
                                  list.size()-1);
            }
            file.close();
        }
    }
}
void vQCustomPlot::showCsvData(double *data,int size)
{
    if(this->graphCount()==size)
    {
        for (int i =0;(i<size);i++)
        {
            this->graph(i)->addData(this->graph(0)->dataCount(), (double)data[i]);
        }
    }
    else
    {
        if(this->graphCount()==0)
        {
            for(int i=0;i<size;i++)
            {
                addRandomGraph();
            }
        }
    }
}
/*鼠标键盘事件*/
void vQCustomPlot::wheelEvent(QWheelEvent *ev)
{
    //如果选择了轴，只允许缩放该轴的方向
    //如果没有选择轴，两个方向都可以缩放
    uint8_t key_val = this->keyStatus;
    if(key_val != KEY_ALT_CTRL)
    {
        if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
            this->axisRect()->setRangeZoom(this->xAxis->orientation());
        else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
            this->axisRect()->setRangeZoom(this->yAxis->orientation());
        else
            this->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    }
    QCustomPlot::wheelEvent(ev);
}

void vQCustomPlot::mousePressEvent(QMouseEvent *ev)
{
    //如果选择了一个轴，只允许拖动该轴的方向
    //如果没有选择轴，两个方向都可以拖动
    uint8_t key_val = this->keyStatus;
    if(key_val != KEY_ALT_CTRL)
    {
        if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
            this->axisRect()->setRangeDrag(this->xAxis->orientation());
        else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
            this->axisRect()->setRangeDrag(this->yAxis->orientation());
        else
            this->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    }
    QCustomPlot::mousePressEvent(ev);
}

/*光标追踪数据点*/
void vQCustomPlot::mouseMoveEvent(QMouseEvent *ev)
{
    if(mxTracer==nullptr)
    {
        return;
    }
    if(this->selectedGraphs().size() <= 0)
    {
        mxTracer->setVisible(false);
        QCustomPlot::mouseMoveEvent(ev);
        return;
    }
    mxTracer->setVisible(true);
    double x = this->xAxis->pixelToCoord(ev->pos().x()); //获得鼠标位置处对应的横坐标数据x

#if 1
    ///用库里自带的差值函数求y值
    //遍历曲线

    for (int i = 0; i < this->graphCount(); ++i)
    {
        //判断哪一条曲线被选中
        if(this->graph(i)->selected())
        {
            mxTracer->updatePosition_Interpolation(x, this->graph(i));
        }
    }

#else
    ///用自己写的二分法求y值
    //获取容器
    QSharedPointer<QCPGraphDataContainer> tmpContainer;
    tmpContainer = this->selectedGraphs().first()->data();
    //获取y轴坐标
    double y = 0;
    {
        //使用二分法快速查找所在点数据
        x = this->xAxis->pixelToCoord(ev->pos().x()); //获得鼠标位置处对应的横坐标数据x

        int low = 0, high = tmpContainer->size();
        while(high > low)
        {
            int middle = (low + high) / 2;
            if(x < tmpContainer->constBegin()->mainKey() ||
                    x > (tmpContainer->constEnd()-1)->mainKey())
                break;

            if(x == (tmpContainer->constBegin() + middle)->mainKey())
            {
                y = (tmpContainer->constBegin() + middle)->mainValue();
                break;
            }
            if(x > (tmpContainer->constBegin() + middle)->mainKey())
            {
                low = middle;
            }
            else if(x < (tmpContainer->constBegin() + middle)->mainKey())
            {
                high = middle;
            }
            if(high - low <= 1)
            {   //差值计算所在位置数据
                y = (tmpContainer->constBegin()+low)->mainValue() + ( (x - (tmpContainer->constBegin() + low)->mainKey()) *
                                                                      ((tmpContainer->constBegin()+high)->mainValue() - (tmpContainer->constBegin()+low)->mainValue()) ) /
                        ((tmpContainer->constBegin()+high)->mainKey() - (tmpContainer->constBegin()+low)->mainKey());
                break;
            }

        }
    }
    //更新Tracer
    QString text = "X:" + QString::number(x, 'g', 6) + " Y:" + QString::number(y, 'g', 6);
    mxTracer->updatePosition_Dichotomy(x, y);

    mxTracer->setText(text);
#endif
    /*重新显示*/
    QCustomPlot::mouseMoveEvent(ev);
}
void vQCustomPlot::vSaveCsvData(void)
{
    QString dirpath = QFileDialog::getSaveFileName(this, QStringLiteral("保存为Csv"),
                                                   qApp->applicationDirPath(),
                                                   QString(tr("File (*.csv)")));
    if(dirpath!=NULL)
    {
        QFile file(dirpath);
        //方式：Append为追加，WriteOnly，ReadOnly
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(NULL,QString::fromLocal8Bit("提示"),
                                  QString::fromLocal8Bit("无法创建文件"));
            return ;
        }
        else
        {
            /*以下仅适用于key为int的数据*/
            int myGraphCount,posCount,maxSize,maxSizePos;
            QString str;
            QVector<double> saveData[24];
            myGraphCount = this->graphCount();
            /*将数据加载到QVector<double>，进行排列*/
            /*寻找数据最多的曲线*/
            maxSize = 0;
            posCount = 0;
            for(posCount = 0;posCount<myGraphCount;posCount++)
            {
                if(maxSize<this->graph(posCount)->dataCount())
                {
                    maxSize = this->graph(posCount)->dataCount();
                    maxSizePos = posCount;
                }
            }
            for(int i=0;i<24;i++)
            {
                /*分配指定大小的内存*/
                saveData[i].resize(maxSize+1);
            }
            for(posCount = 0;posCount<myGraphCount;posCount++)
            {
                for(int i = 0; i <this->graph(posCount)->dataCount(); i++)
                {
                    saveData[posCount][(int)(
                            this->graph(posCount)->data()->at(i)->key)] =
                            this->graph(posCount)->data()->at(i)->value;
                }
            }
            str.clear();
            str.append("time,");
            for(posCount=0;posCount<myGraphCount-1;posCount++)
            {
                str.append(this->graph(posCount)->name()+",");
            }
            str.append(this->graph(posCount)->name()+"\n");
            file.write(str.toStdString().c_str());//每次写入一行文本
            for(int i = 0; i < maxSize; i++)
            {
                str.clear();
                str.append(QString::number(i,10)+",");
                for(posCount=0;posCount<myGraphCount-1;posCount++)
                {
                    str.append(QString::number(saveData[posCount][i], 'f', 6)+",");
                }
                str.append(QString::number(saveData[posCount][i], 'f', 6)+"\n");
                file.write(str.toStdString().c_str());//每次写入一行文本
            }
            file.close();
        }
    }
}

void vQCustomPlot::graphScope()
{
    double rxThursScope[2];

    rxThursScope[0] = qSin(key)+std::rand()/(double)RAND_MAX*1*qSin(key/0.3843);
    rxThursScope[1] = qCos(key)+std::rand()/(double)RAND_MAX*0.5*qSin(key/0.4364);

    graphNum = 2;
    if((showEnable)&&(graphNum<=24)&&(graphNum>0))
    {
        // 添加数据到线上:
        for (int i =0;(i<this->graphCount())&&(i<graphNum);i++)
        {
            this->graph(i)->addData(key, (double)rxThursScope[i]);
        }
    }

}

