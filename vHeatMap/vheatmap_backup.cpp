#include <QLineEdit>
#include <QPushButton>
#include <QTime>

#include "kriginginterpolation.h"
#include "radialbasisfunction.h"
#include "vheatmap.h"
// #include <opencv2/opencv.hpp>
//[nx, ny] 实际color映射
//[datax, datay] 预计算大小，然后双线性插值到 [nx, ny]
// const int nx = 200;
// const int ny = 200;
const int datax = 40;
const int datay = 40;
#define PROTOCOL_DATA_OFFSET 3

const int SENSOR_ROW_MAX = 70;
const int SENSOR_COL_MAX = 56;

int ADCTOMEM[SENSOR_COL_MAX][SENSOR_ROW_MAX];

#define PROTOCOL_DATA_LENGTH (SENSOR_ROW_MAX * SENSOR_COL_MAX)
#define CALIBRATION_LEN 50  // 校准数据长度（平均值）
qint8 initFlag = 0;
QPointF midpoint(const QPointF &p1, const QPointF &p2);

vHeatMap::vHeatMap(QWidget *parent)
{
    initWindow();
    m_method = InterpolationMethod::IDW;
    setCoordinatePoints();
    drawElements();
    precompute();
    showChannelName(false);

    std::vector<std::vector<bool>> binarizedSole = std::vector<std::vector<bool>>(NX, std::vector<bool>(NY, false));

    threshold_filter = 30;
    connect(this, &vHeatMap::sendData, this, &vHeatMap::setData);

    qDebug() << "interpolation method : " << QMetaEnum::fromType<vHeatMap::InterpolationMethod>().valueToKey(m_method);

    for (row = 0; row < SENSOR_ROW_MAX; row++)
    {
        ADCTOMEM[0][row] = row * SENSOR_COL_MAX + 0;
        ADCTOMEM[8][row] = row * SENSOR_COL_MAX + 1;
        ADCTOMEM[16][row] = row * SENSOR_COL_MAX + 2;
        ADCTOMEM[24][row] = row * SENSOR_COL_MAX + 3;
        ADCTOMEM[32][row] = row * SENSOR_COL_MAX + 4;
        ADCTOMEM[40][row] = row * SENSOR_COL_MAX + 5;
        ADCTOMEM[48][row] = row * SENSOR_COL_MAX + 6;

        for (col = 1; col < 8; col++)
        {
            ADCTOMEM[0 + col][row] = ADCTOMEM[0][row] + col * 7;
            ADCTOMEM[8 + col][row] = ADCTOMEM[8][row] + col * 7;
            ADCTOMEM[16 + col][row] = ADCTOMEM[16][row] + col * 7;
            ADCTOMEM[24 + col][row] = ADCTOMEM[24][row] + col * 7;
            ADCTOMEM[32 + col][row] = ADCTOMEM[32][row] + col * 7;
            ADCTOMEM[40 + col][row] = ADCTOMEM[40][row] + col * 7;
            ADCTOMEM[48 + col][row] = ADCTOMEM[48][row] + col * 7;
        }
    }
}

vHeatMap::~vHeatMap() {}

void vHeatMap::vConnect(void)
{
    connect(this->vSerial->qSerial, &QSerialPort::readyRead, this, &vHeatMap::vSerialRxIRQ, Qt::QueuedConnection);
}
void vHeatMap::vDisConnect(void)
{
    disconnect(this->vSerial->qSerial, &QSerialPort::readyRead, this, &vHeatMap::vSerialRxIRQ);
}

void vHeatMap::vSerialRxIRQ(void)
{
    //    static qint16 Flag = 0;
    QByteArray vRxSerialBuff;
    qint16 dataLength = 0;
    std::vector<double> pressData(4000, 0);
    std::vector<double> pressData2(4000, 0);
    //    vRxIntBuff.resize(40);
    QVector<uint8_t> vRxU8Buff(4000, 0);
    //    vRxU16Buff = new uint16_t[20];
    QVector<uint16_t> vRxU16Buff(5000, 0);

    pressMaxData.resize(PROTOCOL_DATA_LENGTH, 0);  // 初始化中，取出最大值
    std::vector<std::vector<double>> initPressData;
    initPressData = std::vector<std::vector<double>>(CALIBRATION_LEN,
                                                     std::vector<double>(PROTOCOL_DATA_LENGTH));  // 初始化中的数组

    if (this->vSerial != nullptr)
    {
        if (this->vSerial->qSerial->isOpen())
        {
            vRxSerialBuff = this->vSerial->qSerial->readAll();
            //            qDebug() << "vRxSerialBuff" << vRxSerialBuff;
        }
        if (vRxSerialBuff.isEmpty())
        {
            return;
        }

        // 检验数据帧头,帧头固定为(0XAA),同时务必确认帧头
        // vRxBuff中只存满足帧头符合的数据串
        if ((!vRxSerialBuff.isEmpty()) && (vRxSerialBuff.at(0) == char(0XAA)))
        {
            vRxBuff = vRxSerialBuff;
        }
        // 一帧数据可能收不全，这句话就是上次收到的数据满足帧头是A5但是本次数据虽然不带帧头但其实是一帧数据没收全，然后加入到一帧数据中
        else if ((!vRxBuff.isEmpty()) && (vRxBuff.at(0) == char(0XAA)))
        {
            vRxBuff.append(vRxSerialBuff);
        }
        else
        {
            return;
        }
        if ((vRxBuff.at(0) == char(0XAA)) &&
            (vRxBuff.length() >= (PROTOCOL_DATA_OFFSET + 1)))  // 判断帧头和接收数据长度
        {
            // qDebug() << "vRxBuff.length()" << vRxBuff.length();
            if (vRxBuff.length() < 4000)
            {
                memcpy(&vRxU8Buff[0], &((uint8_t *)(vRxBuff.data()))[0], vRxBuff.length());
            }
            dataLength = vRxU8Buff[1] << 8 | vRxU8Buff[2];

            //            qDebug() << "length" << dataLength;

            if (vRxBuff.length() ==
                dataLength + PROTOCOL_DATA_OFFSET + 1)  // 判断接收数据长度是否等于数据长度加上帧头、数据长度和帧尾
            {
                //               qDebug() << "length" << vRxBuff.length() <<
                //               dataLength;
                if (vRxBuff.at(PROTOCOL_DATA_OFFSET + dataLength) == char(0XBB))  // 判断帧尾
                {
                    //                   qDebug() << "length" <<
                    //                   (vRxU8Buff[1]<<8 | vRxU8Buff[2]);

                    for (quint16 i = PROTOCOL_DATA_OFFSET; i < dataLength + PROTOCOL_DATA_OFFSET;
                         i++)  // 取50次数据的平均值
                    {
                        pressData2[i - PROTOCOL_DATA_OFFSET] = vRxU8Buff[i];
                    }

                    for (row = 0; row < SENSOR_ROW_MAX; row++)
                    {
                        for (col = 0; col < SENSOR_COL_MAX; col++)
                        {
                            // 使用新的索引方式填充为一维顺序
                            pressData[row * SENSOR_COL_MAX + col] = pressData2[ADCTOMEM[col][row]];
                        }
                    }
                    //                    for (col = 0; col < SENSOR_COL_MAX;
                    //                    col++)
                    //                    {
                    //                        for (row = 0; row <
                    //                        SENSOR_ROW_MAX; row++)
                    //                        {
                    //                            qDebug() <<
                    //                            ADCTOMEM[col][row];
                    //                        }
                    //                    }

                    //                    qDebug() << "pressData" << pressData;
                    emit sendData(pressData);
                }
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }
    }
}

void vHeatMap::setCoordinatePoints()
{
    quint32 pointNum;

    // 1到16个点
    //    m_channelAxis = {QPointF(1.4,4.8), QPointF(0.1,3.6), QPointF(0.7,4),
    //    QPointF(2,3.8),
    //                     QPointF(0.4,2), QPointF(0.6,0.5), QPointF(1.1,0),
    //                     QPointF(1.7,0.5), QPointF(3.5,4.8), QPointF(5,3.6),
    //                     QPointF(4.2,4), QPointF(3.1,3.8), QPointF(4.6,2),
    //                     QPointF(4.4,0.5), QPointF(3.8,0), QPointF(3.4,0.5)};
    m_channelAxis.resize(SENSOR_ROW_MAX * SENSOR_COL_MAX);  // 直接调整大小

    for (int i = 0; i < SENSOR_ROW_MAX; ++i)
    {
        for (int j = 0; j < SENSOR_COL_MAX; ++j)
        {
            m_channelAxis[i * SENSOR_COL_MAX + j] = QPointF(i, j);
        }
    }

    m_usedChannels.resize(m_channelAxis.size());
    for (pointNum = 0; pointNum < m_channelAxis.size(); pointNum++)
    {
        QString key = QString::number(pointNum + 1);  // 键从 "1" 开始
        m_channelPoint[key] = {m_channelAxis[pointNum]};
        m_channelIndex[key] = pointNum;
        m_usedChannels[pointNum] = pointNum;
    }
    //    qDebug() << "m_channelAxis" << m_channelAxis.size();
    //    qDebug() << "m_channelPoint" << m_channelPoint;
    //    qDebug() << "m_channelIndex" << m_channelIndex;
}

void vHeatMap::recvColorScale(int value)
{
    colorScaleValue = value;
    qDebug() << "colorScaleValue" << colorScaleValue;
    m_colorScale->axis()->setRange(0.0,
                                   colorScaleValue);  // 设置颜色比例轴的范围
    m_customPlot->replot();
}

void vHeatMap::savePng()
{
    m_colorScale->setVisible(false);
    m_customPlot->replot();
    qDebug() << "保存图像";
    QPixmap pixmap = m_customPlot->toPixmap();
    // 指定保存路径和文件名
    QString dirpath = QFileDialog::getSaveFileName(this, QStringLiteral("保存为png"), qApp->applicationDirPath(),
                                                   QString(tr("File (*.png)")));
    if (dirpath != NULL)
    {
        // 保存图像
        pixmap.save(dirpath);
        m_colorScale->setVisible(true);
        m_customPlot->replot();
    }
}

void vHeatMap::setData(std::vector<double> &channels)
{
    calMatrix(channels);
    m_customPlot->replot();
}

void vHeatMap::setColorRange(double lower, double upper) { m_colorScale->axis()->setRange(lower, upper); }

void vHeatMap::readChannelAxisFile(QString path)
{
    // 读csv文件
    QFile file(path);
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    // 读取状态，1 通道数， 2 鼻尖坐标
    //    int flag = 0;
    // 通道索引
    int index = 0;
    QTextStream in(&file);
    while (!in.atEnd())
    {
        // 按行读
        QString line = in.readLine();
        QStringList list = line.split(",");  // 一行的表格数据都用"，"分开
        // 通道数 每一行的list[0]相当于行头
        if (list[0].contains("POS"))
        {  // 找"POS"字符串
            continue;
        }
        // 坐标
        m_channelIndex[list[0]] = index++;  // list[0]是第一列的名字 m_channelIndex是map
        m_channelAxis.push_back(QPointF(list[1].toFloat(), list[2].toFloat()));
        // 坐标ID和
        m_channelPoint[list[0]].push_back(QPointF(list[1].toFloat(), list[2].toFloat()));
    }
    qDebug() << m_channelAxis;
    qDebug() << m_channelPoint;
}
/*
 * plot::drawChannelAxis 画出各个通道的位置，以点的形式
 */
void vHeatMap::drawElements()
{
    m_channelCircle.resize(m_channelAxis.size());
    m_channelName.resize(m_channelAxis.size());
    // 通道，这部分就是在点上画圆，其实没啥用
    for (auto it = m_channelIndex.begin(); it != m_channelIndex.end(); ++it)
    {
        int index = it.value();
        QString name = it.key();
        QPointF point = m_channelAxis[index];

        // 每个通道的名字
        m_channelName[index] = new QCPItemText(m_customPlot);
        m_channelName[index]->setText(name);
        m_channelName[index]->position->setCoords(point.x(), point.y());
        m_channelName[index]->setColor(Qt::white);
    }
}

void vHeatMap::showChannelName(bool flag)
{
    for (quint32 i = 0; i < m_channelName.size(); ++i)
    {
        if (m_channelName[i] == nullptr) continue;
        m_channelName[i]->setVisible(flag);
    }
    m_customPlot->replot();
}

void vHeatMap::showChannelCircle(bool flag)
{
    for (quint32 i = 0; i < m_channelCircle.size(); ++i)
    {
        if (m_channelCircle[i] == nullptr) continue;
        m_channelCircle[i]->setVisible(flag);
    }
}

void vHeatMap::initWindow()
{
    m_customPlot = new QCustomPlot(this);

    //    // 检查 OpenGL 支持性（兼容旧版 Qt 的写法）
    //    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    //    {
    //      // 设置全局共享上下文格式（可选）
    //      QSurfaceFormat format;
    //      format.setRenderableType(QSurfaceFormat::OpenGL);
    //      format.setProfile(QSurfaceFormat::CompatibilityProfile); //
    //      更兼容的配置 format.setVersion(2, 1); // 降低 OpenGL 版本要求
    //      QOpenGLContext::globalShareContext()->setFormat(format);

    // 启用 OpenGL
    m_customPlot->setOpenGl(true);
    qDebug() << m_customPlot->openGl();
    //      qDebug() << "OpenGL acceleration enabled";
    //    }
    //    else
    //    {
    //      qWarning() << "OpenGL not supported, using software rendering";
    //    }

    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_customPlot->axisRect()->setupFullAxesBox(true);

    m_customPlot->xAxis->setVisible(false);
    m_customPlot->yAxis->setVisible(false);
    m_customPlot->xAxis2->setVisible(false);
    m_customPlot->yAxis2->setVisible(false);
    m_customPlot->xAxis->grid()->setVisible(false);
    m_customPlot->yAxis->grid()->setVisible(false);

    m_colorMap = new QCPColorMap(m_customPlot->xAxis, m_customPlot->yAxis);
    m_colorMap->setAntialiased(true);
    m_colorMap->data()->setSize(NX, NY);
    m_colorMap->data()->setRange(QCPRange(-0.5, SENSOR_ROW_MAX - 0.5), QCPRange(-0.5, SENSOR_COL_MAX - 0.5));
    m_colorScale = new QCPColorScale(m_customPlot);
    m_customPlot->plotLayout()->addElement(0, 1, m_colorScale);
    m_colorScale->setType(QCPAxis::atRight);
    m_colorMap->setColorScale(m_colorScale);
    m_colorScale->axis()->setRange(0.0, 3000.0);

    m_colorMap->setGradient(QCPColorGradient::gpJet);
    m_colorMap->rescaleDataRange();

    QCPMarginGroup *marginGroup = new QCPMarginGroup(m_customPlot);
    m_customPlot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
    m_colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
    // double ratio = static_cast<double>(SENSOR_ROW_MAX) / SENSOR_COL_MAX;
    // double viewHeight = SENSOR_COL_MAX;     // y 轴长度
    // double viewWidth = ratio * viewHeight;  // x 轴应对应的宽度

    // m_customPlot->xAxis->setRange(-0.5, viewWidth - 0.5);
    // m_customPlot->yAxis->setRange(-0.5, viewHeight - 0.5);
    m_customPlot->rescaleAxes();
}

void vHeatMap::test()
{
    vQTimer.setInterval(100);
    vQTimer.start();
}

void vHeatMap::handleTimeout()
{
    std::vector<double> tmp(64, 0);
    for (int i = 0; i < 64; ++i)
    {
        tmp[i] = qrand() * 1.0 / 10000;
    }
    //    tmp[3] = 3;

    //    setData(tmp); //给每个点赋值
}

void vHeatMap::timerStart()
{
    vQTimerEnable = true;
    if (!this->vQTimer.isActive())
    {
        this->vQTimer.start(this->timerCntSet);
    }
}

void vHeatMap::timerStop(void)
{
    vQTimerEnable = false;
    if (this->vQTimer.isActive())
    {
        this->vQTimer.stop();
    }
}

/*
 * vHeatMap::calMatrix 计算方法
 * 先缩小矩阵为80 * 80，此时由距离反比插值计算各坐标
 * 放大至240 *
 * 240，采用双线性插值，注意整体网格的大小是一样的，只是网格的数量不一样，就是网格细化的感觉
 * 目的：减少计算量，如果上来用240 * 240的网格计算，那么计算量为240 * 240 * 64
 *      先用80 * 80的网格，，然后放大至240 * 240，计算量为80 * 80 * 64 + 240 *
 * 240
 */
void vHeatMap::calMatrix(std::vector<double> &channels)
{
    if (isPlaybackActive) return;
    threshold_filter = 10;
    // 第一步：根据通道数据，计算权重矩阵 m_matrix1（原始网格维度）
    // calculateWeightedMatrix(channels);
    erodeAndDilate(channels);                        // 腐蚀膨胀
    applyHeatMapFilter(channels, threshold_filter);  // 历史滤波
    // 第二步：对 m_matrix1 做双线性插值，生成细分后的
    // m_matrix，并更新图像数据（若非回放） 同时返回总压力值（矩阵总和） double
    // matrixSum = bilinearInterpolateAndFillColorMap();
    // 直接双线性插值，
    double matrixSum = resizeBilinear2D(channels, 56, 70, m_matrix, NY, NX);
    // 数据滤波结束，开始分析
    //  第三步：提取有效压力点（值大于阈值的坐标），用于轮廓提取
    //  QVector<QPointF> activePoints = getActivePressurePoints(500.0);
    if (!isPlaybackActive) analyzeFeetContours(m_matrix, 0, shoeSizeToArea(36));
    // 第四步：基于有效点计算凸包（即该帧压力的几何边界轮廓）
    //    QVector<QPointF> convexHull = computeConvexHull(activePoints);
    //    drawConvexHull(convexHull);
    // 计算压力中心
    //    PressureCenterTrajectory =
    //    calculatePressureCenterTrajectory(this->vSerialCtr.vMapCtr.m_channelAxis,
    //    pressFData);
    //    ui->lineEditPressureCenterTrajectory->setText(QString("(%1,
    //    %2)").arg(PressureCenterTrajectory.x(),0,'f',2).arg(PressureCenterTrajectory.y(),0,'f',2));

    // 第五步：记录当前时间戳，用于判断动作段的起止//显示帧率
    QTime time = QTime::currentTime();
    qint64 now = time.msec() + time.second() * 1000 + time.minute() * 60000 + time.hour() * 3600000;
    static qint64 now1 = now;
    double fps = 1000.0f / (now - now1);
    now1 = now;

    ui->label_FPS->setText(QString("%1").arg(fps, 0, 'f', 2));

    // 第六步：判断是否触发新动作段，或记录当前段信息（如最大值、间隔、快照等）
    processActivitySegment(matrixSum, now);
    m_colorMap->data()->setCell(NX - 1, NY - 1, 66);
}
//--时间间隔播放--
//--开始播放按钮--
void vHeatMap::startTimedPlayback()
{
    if (!playbackTimer)
    {
        playbackTimer = new QTimer(this);
        connect(playbackTimer, &QTimer::timeout, this, &vHeatMap::playbackNextFrameTimed);
    }

    QString currentText = ui->btnStartPlay->text();
    // 点击“开始播放” → 初始化并播放第一帧
    if (currentText == "开始播放")
    {
        if (all_segments.empty())
        {
            qDebug() << "No segments recorded.";
            return;
        }
        currentPlaybackSegment = all_segments.back();

        if (currentPlaybackSegment.snapshots.empty())
        {
            qDebug() << "No frames in last segment.";
            return;
        }

        playbackFrameIndex = 0;
        isPlaybackActive = true;
        ui->btnStartPlay->setText("暂停播放");
        ui->btnNextFrame->setText("退出播放");
        // ------标注脚掌
        annotateFoot(currentPlaybackSegment);

        playbackNextFrameTimed();  // 显示第一帧

        qDebug() << "Timed playback started. Total frames:" << currentPlaybackSegment.snapshots.size();
        //  playbackTimer->start(100); // 启动下一帧的定时器
    }
    // 点击“暂停播放” → 停止定时器，不改 playbackIndex
    else if (currentText == "暂停播放")
    {
        playbackTimer->stop();
        ui->btnStartPlay->setText("继续播放");
        ui->btnNextFrame->setText("下一帧");
        qDebug() << "Playback paused.";
    }
    // 点击“继续播放” → 继续定时器
    else if (currentText == "继续播放")
    {
        ui->btnStartPlay->setText("暂停播放");
        ui->btnNextFrame->setText("退出播放");

        qDebug() << "Playback resumed.";
        playbackNextFrameTimed();  // 从当前帧继续
    }
    else if (currentText == "结束播放")
    {
        ui->btnStartPlay->setText("开始播放");
        ui->btnNextFrame->setText("退出播放");
        isPlaybackActive = false;
        playbackTimer->stop();
        clearAllLines();
        qDebug() << "Playback end.";
    }
}

void vHeatMap::playbackNextFrameTimed()
{
    if (playbackFrameIndex >= currentPlaybackSegment.snapshots.size())
    {
        ui->btnStartPlay->setText("结束播放");
        ui->btnNextFrame->setText("退出播放");
        // 选择最后显示的画面,现在是压力最大的画面
        // m_matrix = currentPlaybackSegment.snapshots[currentPlaybackSegment.max];
        // 改为合并的画面
        // m_matrix = currentPlaybackSegment.overlay;
        // 改为融合最大的画面
        m_matrix = currentPlaybackSegment.fusedMaxMap;
    }
    else
    {
        // ui->btnNextFrame->setText("结束播放");
        m_matrix = currentPlaybackSegment.snapshots[playbackFrameIndex];
    }
    for (size_t i = 0; i < m_matrix.size(); ++i) q
        {
            for (size_t j = 0; j < m_matrix[i].size(); ++j)
            {
                m_colorMap->data()->setCell(i, j, m_matrix[i][j]);
            }
        }
    m_customPlot->replot();
    // qDebug() << "Displayed frame" << playbackFrameIndex;

    int interval = 110;  // 默认
    if (playbackFrameIndex < currentPlaybackSegment.time_gaps.size())
    {
        interval = currentPlaybackSegment.time_gaps[playbackFrameIndex];
        if (playbackFrameIndex != 0)
            drawLine(currentPlaybackSegment.PressureCenterTrajectory[playbackFrameIndex - 1].x(),
                     currentPlaybackSegment.PressureCenterTrajectory[playbackFrameIndex - 1].y(),
                     currentPlaybackSegment.PressureCenterTrajectory[playbackFrameIndex].x(),
                     currentPlaybackSegment.PressureCenterTrajectory[playbackFrameIndex].y(), Qt::red);
    }
    if (playbackFrameIndex < currentPlaybackSegment.snapshots.size() && ui->btnStartPlay->text() == "暂停播放")
    {
        playbackTimer->start(interval);  // 启动下一帧的定时器
    }
    else
    {
        playbackTimer->stop();
    }
    playbackFrameIndex++;
}
//---播放下一帧按钮--//
void vHeatMap::playbackNextFrameTimedkey()
{
    QString currentText = ui->btnNextFrame->text();
    if (currentText == "下一帧")
    {
        playbackNextFrameTimed();
    }
    else if (currentText == "退出播放")
    {
        isPlaybackActive = false;
        playbackTimer->stop();
        ui->btnStartPlay->setText("开始播放");
        clearAllLines();
        qDebug() << "Timed playback finished.";  // 显示最大图
        return;
    }
}
// 时间间隔播放--ed

// vHeatMap::precompute 计算每个电极点的权重, 提前计算，以减少复杂度
void vHeatMap::precompute()
{
    // 初始化缓存矩阵
    m_matrix1 = std::vector<std::vector<double>>(datax, std::vector<double>(datay));
    m_matrix = std::vector<std::vector<double>>(NX, std::vector<double>(NY));
    last_m_matrix = std::vector<std::vector<double>>(NX, std::vector<double>(NY));
    // 初始化权重
    m_channelWeight = std::vector<std::vector<std::vector<double>>>(
        datax, std::vector<std::vector<double>>(datay, std::vector<double>(m_channelAxis.size())));

    // 开始计算
    double x, y;
    // 缩放系数
    int ratex = NX / datax, ratey = NY / datay;
    for (int i = 0; i < datax; ++i)
    {
        for (int j = 0; j < datay; ++j)
        {
            //(x, y) 为 (i, j) 在 colormap坐标系下的映射，x、y的范围是(-0.6,
            // 0.6)
            m_colorMap->data()->cellToCoord(i * ratex, j * ratey, &x, &y);
            // 计算每个通道相对于80*80的网格的权重值
            double sum_weight = 0.0;
            for (size_t k = 0; k < m_channelAxis.size(); ++k)
            {
                auto &point = m_channelAxis[k];
                // d都是小于1的
                double d = sqrt((point.x() - x) * (point.x() - x) + (point.y() - y) * (point.y() - y));
                // 插值方法不同，计算权重公式不同
                double weight = 0.0, R = 0.2;
                switch (m_method)
                {
                    case InterpolationMethod::IDW:
                        weight = 1 / (d * d);
                        break;
                    case InterpolationMethod::ModifiedShepard:
                        weight = qMax(0.0, R - d) / (R * d);
                        weight = weight * weight;
                        break;
                    default:
                        break;
                }
                sum_weight += weight;
                m_channelWeight[i][j][k] = weight;
            }

            for (quint32 k = 0; k < m_channelAxis.size() && sum_weight > 0.0; ++k)
            {
                m_channelWeight[i][j][k] /= sum_weight;
            }
        }
    }
}

double vHeatMap::doubleLinear(int m, int n, int X, int Y, double V1, double V2, double V3, double V4)
{
    // 用下面单线性插值的方法带入，化简得到
    return (m * n * (V3 - V4 - V2 + V1) + X * n * (V4 - V1) + m * Y * (V2 - V1)) / (X * Y) + V1;
}
double vHeatMap::singleLinear(int m, int X, double V1, double V2)
{
    // y = ((x - x1)(y2 - y1))/(x2 - x1) + y1
    return m * (V2 - V1) / X + V1;  // m是放大的系数，X是放大的倍数，理解成y1的基础上慢慢增大(x
                                    // - x1)得到中间插值的几个点
}
vHeatMapThread::vHeatMapThread(QObject *parent) {}
//--画直线
void vHeatMap::drawLine(double x1, double y1, double x2, double y2, QColor color)
{
    QCPItemLine *line = new QCPItemLine(m_customPlot);
    line->start->setCoords(x1, y1);
    line->end->setCoords(x2, y2);
    line->setPen(QPen(color, 2));
    // qDebug() << "Y1" << y1 << "X1" << x1;
    // qDebug() << "Y2" << y2 << "X2" << x2;

    playbackLines.append(line);  // <--- 关键

    m_customPlot->replot();
}
void vHeatMap::clearAllLines()
{
    for (auto *line : playbackLines)
    {
        m_customPlot->removeItem(line);
    }
    playbackLines.clear();
    m_customPlot->replot();
}
/**
 * @brief 计算给定点集的凸包（Convex Hull）
 *
 * 使用 Andrew’s monotone chain 算法（O(n log n)）计算二维平面上一组点的凸包。
 * 凸包是包含所有点的最小凸多边形。
 *
 * @param points 输入的点集，QVector<QPointF>，点的顺序无要求
 * @return QVector<QPointF>
 * 返回凸包上的点，按顺时针或逆时针排序（闭合多边形的顶点序列）
 *
 * 算法流程：
 * 1. 按 x 坐标（x相同时按 y 坐标）对点集排序
 * 2. 计算下凸包（从左到右遍历）
 * 3. 计算上凸包（从右到左遍历）
 * 4. 合并上下凸包，去除重复点，得到最终凸包
 *
 * 特殊情况：
 * - 点数小于等于3时，直接返回原点集（所有点均为凸包顶点）
 */
QVector<QPointF> vHeatMap::computeConvexHull(const QVector<QPointF> &points)
{
    int n = points.size(), k = 0;
    if (n <= 3) return points;

    QVector<QPointF> pts = points;
    std::sort(pts.begin(), pts.end(),
              [](const QPointF &a, const QPointF &b) { return a.x() < b.x() || (a.x() == b.x() && a.y() < b.y()); });

    QVector<QPointF> hull(2 * n);

    // Lower hull
    for (int i = 0; i < n; ++i)
    {
        while (k >= 2 && (hull[k - 2].x() - hull[k - 1].x()) * (pts[i].y() - hull[k - 1].y()) -
                                 (hull[k - 2].y() - hull[k - 1].y()) * (pts[i].x() - hull[k - 1].x()) <=
                             0)
            k--;
        hull[k++] = pts[i];
    }

    // Upper hull
    for (int i = n - 2, t = k + 1; i >= 0; --i)
    {
        while (k >= t && (hull[k - 2].x() - hull[k - 1].x()) * (pts[i].y() - hull[k - 1].y()) -
                                 (hull[k - 2].y() - hull[k - 1].y()) * (pts[i].x() - hull[k - 1].x()) <=
                             0)
            k--;
        hull[k++] = pts[i];
    }

    hull.resize(k - 1);
    return hull;
}
// 计算基础加权矩阵
void vHeatMap::calculateWeightedMatrix(std::vector<double> &channels)
{
    //
    int size = qMin(m_channelAxis.size(), channels.size());
    int ratex = NX / datax, ratey = NY / datay;
    double x, y;

    for (int i = 0; i < datax; ++i)
    {
        for (int j = 0; j < datay; ++j)
        {
            m_colorMap->data()->cellToCoord(i * ratex, j * ratey, &x, &y);
            double sum = 0.0;
            for (int k = 0; k < size; ++k)
            {
                sum += m_channelWeight[i][j][k] * channels[k];
            }
            m_matrix1[i][j] = sum;
        }
    }
}

// 对基础矩阵进行双线性插值，生成细网格矩阵，并更新颜色图（非回放状态下）
double vHeatMap::bilinearInterpolateAndFillColorMap()
{
    int ratex = NX / datax, ratey = NY / datay;
    double matrixSum = 0;

    for (int i = 0; i < datax - 1; i++)
    {
        for (int j = 0; j < datay - 1; j++)
        {
            double V1 = m_matrix1[i][j];
            double V2 = m_matrix1[i + 1][j];
            double V3 = m_matrix1[i + 1][j + 1];
            double V4 = m_matrix1[i][j + 1];

            for (int m = 0; m < ratex; m++)
            {
                for (int n = 0; n < ratey; n++)
                {
                    int x = i * ratex + m, y = j * ratey + n;
                    m_matrix[x][y] = doubleLinear(m, n, ratex, ratey, V1, V2, V3, V4);
                    if (!isPlaybackActive) m_colorMap->data()->setCell(x, y, m_matrix[x][y]);
                }
            }
        }
    }

    return matrixSum;
}

// 处理动作段逻辑：开始、记录、结束
void vHeatMap::processActivitySegment(double matrixSum, qint64 now)
{
    static int currentSegmentIndex = -1;
    double threshold = 5000;

    extern QPointF PressureCenterTrajectory;

    if (matrixSum > threshold)
    {
        // qDebug() << "matrixSum =" << matrixSum;
        if (startKeepTimestamp == 0)  // 新动作段开始
        {
            startKeepTimestamp = now;
            ActivitySegment new_segment;
            all_segments.push_back(new_segment);
            qDebug() << "新的一帧开始";
            currentSegmentIndex++;
            all_segments[currentSegmentIndex].max = 0;
            all_segments[currentSegmentIndex].maxValue = 0;
        }

        all_segments[currentSegmentIndex].Timestamp.push_back(now);
        qint64 gap = now - startKeepTimestamp;
        startKeepTimestamp = now;
        // qDebug() << "startKeepTimestamp=" << startKeepTimestamp;

        all_segments[currentSegmentIndex].time_gaps.push_back(gap);
        all_segments[currentSegmentIndex].snapshots.push_back(m_matrix);

        if (matrixSum > all_segments[currentSegmentIndex].maxValue)
        {
            all_segments[currentSegmentIndex].max = currentSegmentIndex;
            all_segments[currentSegmentIndex].maxValue = matrixSum;
        }
        // 计算重心
        PressureCenterTrajectory = calculatePressureCenterTrajectory(m_matrix);
        all_segments[currentSegmentIndex].PressureCenterTrajectory.push_back(PressureCenterTrajectory);

        qDebug() << "Segment" << currentSegmentIndex << "Step" << all_segments[currentSegmentIndex].time_gaps.size()
                 << "Gap:" << gap << "MatrixSum:" << matrixSum;
    }
    else
    {
        if (startKeepTimestamp != 0)
        {
            // 动作段结束，进行处理
            //  1、叠加足印
            startKeepTimestamp = 0;  // 动作段结束
            const auto &snapshots = all_segments[currentSegmentIndex].snapshots;
            qDebug() << "处理数据";

            size_t h = snapshots[0].size();
            size_t w = snapshots[0][0].size();
            size_t frameCount = snapshots.size();
            all_segments[currentSegmentIndex].overlay.assign(h, std::vector<double>(w, 0.0));
            all_segments[currentSegmentIndex].fusedMaxMap.assign(h, std::vector<double>(w, 0.0));
            // 叠加所有帧

            for (const auto &frame : snapshots)
            {
                for (size_t j = 0; j < h; ++j)
                {
                    for (size_t k = 0; k < w; ++k)
                    {
                        all_segments[currentSegmentIndex].overlay[j][k] += frame[j][k];
                    }
                }
            }

            // 归一化叠加后的足迹
            // for (size_t j = 0; j < h; ++j) {
            //     for (size_t k = 0; k < w; ++k) {
            //         all_segments[currentSegmentIndex].overlay[j][k] /= frameCount;
            //     }
            // }

            // 2、过滤非足印
            // 二值化
            auto binary = thresholdBinarize(all_segments[currentSegmentIndex].overlay, 0);
            auto largestRegion = extractFootRegions(binary, 600);
            all_segments[currentSegmentIndex].num_Foot = largestRegion.size();

            // 初始化为两个 200x200 的布尔矩阵
            // 标记区域
            if (all_segments[currentSegmentIndex].num_Foot == 0)
            {
                qDebug() << "没有足迹";
                all_segments.pop_back();
                currentSegmentIndex--;
                qDebug() << "删除这一帧";
                return;
            }
            else
            {
                markRegionOnBinary(largestRegion[0], all_segments[currentSegmentIndex].foot[0].binary);
                qDebug() << "一个足迹";
            }
            if (all_segments[currentSegmentIndex].num_Foot == 2)
            {
                markRegionOnBinary(largestRegion[1], (all_segments[currentSegmentIndex].foot[1].binary));
                qDebug() << "两个足迹";
            }
            all_segments[currentSegmentIndex].foot.resize(2);  // 假设有两个脚
            for (auto &f : all_segments[currentSegmentIndex].foot)
            {
                f.binary.resize(NX, std::vector<bool>(NY, false));  // 初始化每个foot的binary为200x200
            }
            classifyLeftRight(all_segments[currentSegmentIndex]);
            if (all_segments[currentSegmentIndex].num_Foot == 2)
            {
                // 计算脚间距
                // double stepWidth =
                // [all_segments[currentSegmentIndex].foot[0].centroid.y()-all_segments[currentSegmentIndex].foot[1].centroid.y()]
                qreal distance = QLineF(all_segments[currentSegmentIndex].foot[0].centroid,
                                        all_segments[currentSegmentIndex].foot[1].centroid)
                                     .length();
                qDebug() << "stepwidth" << distance;
            }

            qDebug() << "过滤非足迹";
            bool keep_key[2];
            for (size_t i = 0; i < all_segments[currentSegmentIndex].snapshots.size(); ++i)
            {
                keep_key[0] = 0;
                keep_key[1] = 0;
                for (size_t j = 0; j < all_segments[currentSegmentIndex].snapshots[i].size(); ++j)
                {
                    for (size_t k = 0; k < all_segments[currentSegmentIndex].snapshots[i][j].size(); ++k)
                    {
                        if (!all_segments[currentSegmentIndex].foot[0].binary[j][k] &&
                            !all_segments[currentSegmentIndex].foot[1].binary[j][k])
                        {
                            all_segments[currentSegmentIndex].snapshots[i][j][k] = 0;
                            all_segments[currentSegmentIndex].overlay[j][k] = 0;
                        }
                        else if (all_segments[currentSegmentIndex].snapshots[i][j][k] > 0)
                        {
                            if (all_segments[currentSegmentIndex].foot[0].binary[j][k])
                            {
                                keep_key[0] = 1;
                            }
                            else if (all_segments[currentSegmentIndex].foot[1].binary[j][k])
                            {
                                keep_key[1] = 1;
                            }
                            //
                            all_segments[currentSegmentIndex].fusedMaxMap[j][k] =
                                max(all_segments[currentSegmentIndex].fusedMaxMap[j][k],
                                    all_segments[currentSegmentIndex].snapshots[i][j][k]);
                        }
                    }
                }
                if (keep_key[0])
                {
                    all_segments[currentSegmentIndex].foot[0].Timestamp.push_back(
                        all_segments[currentSegmentIndex].Timestamp[i]);
                }
                if (keep_key[1])
                {
                    all_segments[currentSegmentIndex].foot[1].Timestamp.push_back(
                        all_segments[currentSegmentIndex].Timestamp[i]);
                }
            }
            // 3、计算压力重心

            // ----排序全部foot
            if (all_segments[currentSegmentIndex].num_Foot == 1)
            {
                foots.push_back(&all_segments[currentSegmentIndex].foot[0]);
            }
            else if (all_segments[currentSegmentIndex].num_Foot == 2)
            {
                if (all_segments[currentSegmentIndex].foot[0].Timestamp.front() >
                    all_segments[currentSegmentIndex].foot[1].Timestamp.front())
                {
                    foots.push_back(&all_segments[currentSegmentIndex].foot[1]);
                    foots.push_back(&all_segments[currentSegmentIndex].foot[0]);
                }
                else
                {
                    foots.push_back(&all_segments[currentSegmentIndex].foot[0]);
                    foots.push_back(&all_segments[currentSegmentIndex].foot[1]);
                }
            }
            if (foots.size() >= 2)
            {
                if (foots[foots.size() - 2]->Timestamp.back() >
                    foots[foots.size() - 1]->Timestamp.front())  // 判定为一次
                {
                    // 计算步态周期，一只脚从落地到着一只脚下一次落地//有问题
                    auto step_gap =
                        (abs(foots[foots.size() - 1]->Timestamp.front() - foots[foots.size() - 2]->Timestamp.front()) +
                         abs(foots[foots.size() - 1]->Timestamp.back() - foots[foots.size() - 2]->Timestamp.back()));
                    // 计算步频
                    auto cadence = 60000 / step_gap;
                    ui->label_stepFrequency->setText(QString("%1 steps/min").arg(cadence, 0, 'f', 2));
                    qDebug() << "step time"
                             << (abs(foots[foots.size() - 1]->Timestamp.front() -
                                     foots[foots.size() - 2]->Timestamp.front()) +
                                 abs(foots[foots.size() - 1]->Timestamp.back() -
                                     foots[foots.size() - 2]->Timestamp.back())) /
                                    2;
                    // qDebug() << "时间间隔1" << foots[foots.size() - 2]->Timestamp.back() << "ms";
                    // qDebug() << "时间间隔2" << all_segments[currentSegmentIndex].foot[1].Timestamp.front()
                    //          << all_segments[currentSegmentIndex].foot[1].Timestamp.back() << "ms";
                    // 计算步长  //正确的应该是同一只脚来计算位置
                    QPointF midfront =
                        midpoint(foots[foots.size() - 2]->extreme_point[0], foots[foots.size() - 1]->extreme_point[0]);
                    QPointF midback =
                        midpoint(foots[foots.size() - 2]->extreme_point[1], foots[foots.size() - 1]->extreme_point[1]);
                    auto step_lenth = QLineF(midfront, midback).length();
                    ui->label_steplenth->setText(QString("%1").arg(step_lenth, 0, 'f', 2));
                    qDebug() << "step_lenth" << step_lenth;
                    // 计算步速
                    auto walkspeed = step_lenth * cadence;
                    ui->label_walkspeed->setText(QString("%1").arg(walkspeed, 0, 'f', 2));
                    // 求步宽
                    QVector2D diff = QVector2D(foots[foots.size() - 1]->centroid - foots[foots.size() - 2]->centroid);
                    QVector2D walkDir = QVector2D(midback - midfront);  // 假设Y为主轴
                    walkDir = walkDir.normalized();
                    QVector2D perp(-walkDir.y(), walkDir.x());  // 垂直方向

                    double stepWidth = std::abs(QVector2D::dotProduct(diff, perp.normalized()));
                    ui->label_stepwidth->setText(QString("%1").arg(stepWidth, 0, 'f', 2));
                }
            }
            //--支撑时间
            for (quint32 i = 0; i < all_segments[currentSegmentIndex].num_Foot; i++)
            {
                auto stance_time = all_segments[currentSegmentIndex].foot[i].Timestamp.back() -
                                   all_segments[currentSegmentIndex].foot[i].Timestamp.front();
                if (all_segments[currentSegmentIndex].foot[i].is_left_foot)
                {
                    ui->label_stancetimeL->setText(QString("%1 ms").arg(stance_time));
                }
                else
                    ui->label_stancetimeR->setText(QString("%1 ms").arg(stance_time));
            }
        }
    }
}
// 绘制轮廓线（凸包）
void vHeatMap::drawConvexHull(const QVector<QPointF> &convexHull)
{
    //    // 删除旧的轮廓线图层（防止叠加）
    //    if (m_hullGraph) {
    //        m_colorMap->parentPlot()->removePlottable(m_hullGraph);
    //        m_hullGraph = nullptr;
    //    }

    //    // 新建一个 graph 用于绘制凸包
    //    m_hullGraph = m_colorMap->parentPlot()->addGraph();
    //    QVector<double> xData, yData;

    //    for (const QPointF &pt : convexHull) {
    //        xData.append(pt.x());
    //        yData.append(pt.y());
    //    }

    //    // 补上起点，形成闭合曲线
    //    if (!xData.isEmpty()) {
    //        xData.append(xData[0]);
    //        yData.append(yData[0]);
    //    }

    //    m_hullGraph->setData(xData, yData);
    //    m_hullGraph->setPen(QPen(Qt::red, 2));  // 红色线，粗细为2
    //    m_hullGraph->setLineStyle(QCPGraph::lsLine);

    //    m_colorMap->parentPlot()->replot();
}
/**
 * @brief 双线性插值缩放一维输入矩阵为二维热力图，并写入 dst。
 * @param src 一维源数据，按行优先存储
 * @param src_w 原图宽度
 * @param src_h 原图高度
 * @param dst 输出的二维矩阵
 * @param new_w 插值后的宽度
 * @param new_h 插值后的高度
 * @return 插值后矩阵的总和（总压力）
 */
double vHeatMap::resizeBilinear2D(const vector<double> &src, int src_w, int src_h, vector<vector<double>> &dst,
                                  int new_w, int new_h)
{
    dst.assign(new_h, std::vector<double>(new_w, 0.0));

    double scaleX = static_cast<double>(src_w) / new_w;
    double scaleY = static_cast<double>(src_h) / new_h;
    double matrixSum = 0.0;

    for (int i = 0; i < new_h; ++i)
    {
        double srcY = i * scaleY;
        int y0 = std::min(static_cast<int>(std::floor(srcY)), src_h - 1);
        int y1 = std::min(y0 + 1, src_h - 1);
        double dy = srcY - y0;

        for (int j = 0; j < new_w; ++j)
        {
            double srcX = j * scaleX;
            int x0 = std::min(static_cast<int>(std::floor(srcX)), src_w - 1);
            int x1 = std::min(x0 + 1, src_w - 1);
            double dx = srcX - x0;

            double f00 = src[y0 * src_w + x0];
            double f01 = src[y0 * src_w + x1];
            double f10 = src[y1 * src_w + x0];
            double f11 = src[y1 * src_w + x1];

            double r0 = f00 + (f01 - f00) * dx;
            double r1 = f10 + (f11 - f10) * dx;
            double value = r0 + (r1 - r0) * dy;

            dst[i][j] = value;
            matrixSum += value;
            if (!isPlaybackActive && m_colorMap) m_colorMap->data()->setCell(i, j, value);

            QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName("layout_output.pdf");

            QPainter painter;
            if (painter.begin(&printer))
            {
                page->render(&painter);
                painter.end();
            }
        }
    }
    // erodeAndDilate(dst);  // 腐蚀膨胀
    // for (int i = 0; i < new_h; ++i)
    //     for (int j = 0; j < new_w; ++j)

    return matrixSum;
}
QPointF midpoint(const QPointF &p1, const QPointF &p2)
{
    return QPointF((p1.x() + p2.x()) / 2.0, (p1.y() + p2.y()) / 2.0);
}
