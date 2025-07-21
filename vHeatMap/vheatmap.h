/**
 * @brief Represents a heat map visualization widget with advanced pressure mapping and analysis capabilities
 *
 * This class provides functionality for:
 * - Serial port communication
 * - Pressure data visualization
 * - Spatial interpolation methods
 * - Activity segment recording and playback
 * - Foot region and contour analysis
 *
 * Supports multiple interpolation methods including:
 * - Inverse Distance Weighting (IDW)
 * - Modified Shepard
 * - Radial Basis Function (RBF)
 * - Kriging
 * - Natural Neighbor Interpolation (NNI)
 */
#ifndef VHEATMAP_H
#define VHEATMAP_H

#include <vserialport.h>

#include <QDebug>
#include <QHash>
#include <QPointF>
#include <QSerialPort>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <algorithm>
#include <utility>  // for std::pair
#include <vector>   // for std::vector

#include "qcustomplot.h"
#include "ui_mainwindow.h"
#define ROWS 70.0f
#define COLS 56.0f
#define maptoplot 24
#define NX (ROWS / 14 * (maptoplot + 14))
#define NY (COLS / 14 * (maptoplot + 14))

#define maptoplotX (NX / ROWS)
#define maptoplotY (NY / COLS)
using namespace std;
#define GAP_ELECTORDES 1.0f   // 电极间距，单位：mm
#define WIDTH_ELECTORDE 5.0f  // 电极宽度，单位：mm

/**
 * @brief 脚的结构体定义
 */
typedef struct
{
    QPointF centroid;                                         // 质心
    QVector2D footOrientationVec;                             // 脚的朝向向量
    QVector2D Sec_Axis;                                       // 副轴，脚的向量向右旋转90
    bool is_left_foot;                                        // 是否是左脚
    double extreme_lenth[4];                                  // 四个极点，上下左右
    QPointF extreme_point[4];                                 // 四个端点，右上开始
    vector<vector<bool>> binary;                              //
    vector<qint64> Timestamp;                                 // 原始时间，
    double footLength;                                        // 脚长
    double footWidth;                                         // 脚宽
    double shoeSize;                                          // 脚码
    std::vector<std::vector<std::vector<double>>> snapshots;  //
    std::vector<qint64> time_gaps;
    vector<QPointF> PressureCenterTrajectory;  // 压力中心
} Foot;
QPointF calculatePressureCenterTrajectory(const std::vector<std::vector<double>> &pressureMatrix);
class vHeatMap : public QWidget
{
    Q_OBJECT

   public:
    // 空间插值方法：https://en.wikipedia.org/wiki/Multivariate_interpolation#Irregular_grid_.28scattered_data.29
    enum InterpolationMethod
    {
        IDW = 0,              // 距离反比(Inverse distance weighting)
        ModifiedShepard = 1,  // IDW的改进方法
        RBF = 2,              // 径向基函数(RadialBasisFunction)
        Kriging = 3,          // 克里金
        NNI = 4,              // 自然邻点插值(NaturalNeighbor)
    };
    Q_ENUM(InterpolationMethod)

    vHeatMap(QWidget *parent = nullptr);
    ~vHeatMap();
    /*------------串口基础类，必须初始化------------*/
    void vConnect(void);
    void vDisConnect(void);

    vSerialPort *vSerial = nullptr;
    void vSerialAddrSet(vSerialPort *vSerial_t) { vSerial = vSerial_t; }
    QByteArray vRxBuff;  // 数据处理缓冲，中间量
    uint8_t *vRxU8Buff;
    uint16_t *vRxU16Buff;

    std::vector<double> pressData;
    std::vector<double> pressMaxData;  // 初始化中，取出最大值

    QTimer vQTimer;
    QTimer timer;
    qint32 timerCntSet = 100;
    bool vQTimerEnable;

    // 设置要显示的通道
    void setEnabledChannels(QStringList &channels);
    void setEnabledChannels(std::vector<int> &channels);
    // 更新每个通道的数据
    void setData(std::vector<double> &channels);
    // 设置颜色的显示范围
    void setColorRange(double lower, double upper);
    // 是否显示通道名
    void showChannelName(bool flag);
    // 是否显示通道圆
    void showChannelCircle(bool flag);

    void initWindow();
    void readChannelAxisFile(QString path);
    void drawElements();
    void test();
    void handleTimeout();

    void setTimer(qint32 Cnt) { timerCntSet = Cnt; }

    void timerStart(void);
    void timerStop(void);
    void setCoordinatePoints();

    // 根据每个点的位置计算矩阵值
    void calMatrix(std::vector<double> &channels);  // DoubleLinear方式
    double doubleLinear(int m, int n, int X, int Y, double V1, double V2, double V3, double V4);
    double singleLinear(int m, int X, double V1, double V2);
    // 提前计算
    void precompute();

    // 绘图
    QCustomPlot *m_customPlot;
    QCPColorMap *m_colorMap;
    QCPColorScale *m_colorScale;

    std::vector<QPointF> m_channelAxis;
    QHash<QString, std::vector<QPointF>> m_channelPoint;

    QHash<QString, int> m_channelIndex;

    std::vector<QCPItemEllipse *> m_channelCircle;  // 通道圆
    std::vector<QCPItemText *> m_channelName;       // 通道名称
    // 实际显示的通道索引
    std::vector<double> m_usedChannels;

    // 预计算
    std::vector<std::vector<std::vector<double>>> m_channelWeight;  // 64个通道在每个坐标点的权重系数 [nx][ny][channel]
    std::vector<std::vector<bool>> m_inCircle;                      // nx * ny 矩阵 的点是否在圆内

    std::vector<std::vector<double>> m_matrix;
    std::vector<std::vector<double>> m_matrix1;
    std::vector<std::vector<double>> last_m_matrix;
    double threshold_filter;
    //    std::vector<double> matrixSum;

    int m_contourCnt;  // 轮廓线的个数

    InterpolationMethod m_method;  // 空间插值方法

    int colorScaleValue;
    int row, col;
    int recordkey;  // 轮廓线的个数

    void drawLine(double x1, double y1, double x2, double y2, QColor color = Qt::red);
    void clearAllLines();
    Ui::MainWindow *ui = nullptr;
    /*---回放功能---*/
    struct ActivitySegment
    {
        std::vector<qint64> time_gaps;
        vector<qint64> Timestamp;
        std::vector<std::vector<std::vector<double>>> snapshots;  //
        std::vector<QPointF> PressureCenterTrajectory;            // 压力中心
        std::vector<std::vector<double>> overlay;                 // 用来叠加的足
        std::vector<std::vector<double>> fusedMaxMap;             // 每一个点都取最大值
        quint32 max = 0;
        quint32 maxArea = 0;
        quint32 maxAreaValue = 0;
        double maxValue = 0.0f;  // 最大 matrixSum 的值
        vector<Foot> foot;
        quint32 num_Foot;
    };
    QTimer *playbackTimer = nullptr;  // 用于时间回放
    std::vector<ActivitySegment> all_segments;
    // QPointF PRE_PressureCenterTrajectory;
    uint32_t playbackFrameIndex;             // 当前播放到哪一帧
    ActivitySegment currentPlaybackSegment;  // 当前播放段
    ActivitySegment currentSegment;          // 当前段
    bool isPlaybackActive;                   // 是否进入了播放状态
    void startTimedPlayback();
    void playbackNextFrameTimed();
    void playbackNextFrameTimedkey();
    void processActivitySegment(double matrixSum, qint64 now);
    // std::vector<std::vector<double>> resizeBilinear2D(
    // const std::vector<double> &src, int src_w, int src_h,
    // std::vector<std::vector<double>> &dst, int new_w, int new_h);
    QVector<QPointF> getActivePressurePoints(double pointThreshold);
    double bilinearInterpolateAndFillColorMap();
    void calculateWeightedMatrix(std::vector<double> &channels);
    QVector<QPointF> computeConvexHull(const QVector<QPointF> &points);
    void drawConvexHull(const QVector<QPointF> &convexHull);
    QVector<QCPItemLine *> playbackLines;  // 线条容器
    qint64 startKeepTimestamp = 0;
    std::vector<Foot *> foots;  // foot指针容器
    // void splitForeAndHindByWidth(const QVector2D &axis, const QPointF &centroid, Foot &foot);
    /*---回放功能end---*/

    /*---modelprocess---*/
    void annotateFoot(ActivitySegment &dealSegment);

    std::vector<std::vector<std::pair<int, int>>> extractFootRegions(const std::vector<std::vector<bool>> &binary,
                                                                     size_t minRegionSize);  //
    void markRegionOnBinary(const std::vector<std::pair<int, int>> &region, std::vector<std::vector<bool>> &binary);
    vector<vector<bool>> binarizedSole;  // 储存二值化后的足底”
    double projectionDistance(const QPointF &point, const QPointF &centroid, const QVector2D &axisUnit);
    void splitForeAndHindByWidth(const QVector2D &axis, const QPointF &centroid, Foot &foot);
    void detectAndMarkToeTip(const std::vector<std::vector<bool>> &binary, const QVector2D &axis,
                             const QPointF &centroid);
    void drawAxes(const QVector2D &axis, QPointF &centroid, double length);
    std::vector<std::pair<int, int>> findLargestRegion(const std::vector<std::vector<bool>> &binary,
                                                       size_t minRegionSize);

    void drawAxisLine(QCPGraph *&m_hullGraph, const QPointF &p1, const QPointF &p2);
    std::pair<QPointF, QVector2D> computeMainAxisEndpoints(const std::vector<std::vector<bool>> &binary);
    void drawConnectedRegions(const std::vector<std::vector<std::pair<int, int>>> &regions);
    void erodeAndDilate(std::vector<double> &channels);  // 腐蚀
    void dilate();                                       // 膨胀
    void applyHeatMapFilter(std::vector<double> &currentData,
                            double threshold);  // 对热力图数据进行阈值过滤和历史滤波
    // void analyzeFeetContours(const vector<vector<double>> &pressureData, int threshold, int minRegionSize);
    void dealSegment(ActivitySegment &dealSegment, const vector<vector<double>> &pressureData, int threshold,
                     int minRegionSize);
    vector<vector<bool>> thresholdBinarize(const vector<vector<double>> &pressureData, int threshold);
    vector<pair<int, int>> bfsRegion(const vector<vector<bool>> &binary, int startX, int startY,
                                     vector<vector<bool>> &visited);
    vector<vector<pair<int, int>>> findConnectedRegions(const vector<vector<bool>> &binary, int minRegionSize);
    pair<double, double> getCentroid(const vector<pair<int, int>> &points);
    void classifyLeftRight(ActivitySegment &dealSegment);
    vector<pair<int, int>> extractContour(const vector<pair<int, int>> &regionPoints,
                                          const vector<vector<bool>> &binary);
    double resizeBilinear2D(const vector<double> &src, int src_w, int src_h, vector<std::vector<double>> &dst,
                            int new_w, int new_h);
    double shoeSizeToArea(int size);
    int plotLengthToShoeSize(double footLength_plot);
    int plotWidthToShoeSize(double footWidth_plot);

    /*---modelprocess end---*/

   signals:
    void sendData(std::vector<double> &channels);

   public slots:
    void vSerialRxIRQ(void);
    //    void vHeatMap::vSerialRxIRQ(const QByteArray &str);
    void recvColorScale(int value);
    void savePng();
};

class vHeatMapThread : public QThread
{
    Q_OBJECT
   public:
    explicit vHeatMapThread(QObject *parent = nullptr);

   public:
    void run(void)
    {
        qDebug() << "vHeatMapThread tid:vHeatMapThread run" << QThread::currentThreadId();
        exec();
    }
   signals:
   public slots:
};

#endif  // VHEATMAP_H
