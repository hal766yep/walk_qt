// #include <QLineEdit>
// #include <QPushButton>
// #include <QTime>
#include <QDebug>
#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "qcustomplot.h"

// #include "kriginginterpolation.h"
// #include "radialbasisfunction.h"
#include "vheatmap.h"
extern double projectionDistance(const QPointF &point, const QPointF &centroid, const QPointF &axisUnit);

using namespace std;
// extern void markRegionOnBinary(const std::vector<std::pair<int, int>>
// &region,/                                std::vector<std::vector<bool>> &binary);

// 方向数组：上下左右
const int dx[4] = {1, -1, 0, 0};
const int dy[4] = {0, 0, 1, -1};
/**
 * @brief 分析足底压力数据，提取左右脚轮廓
 *
 * @param pressureData 足底压力二维矩阵，pressureData[y][x] 表示第 y 行第 x
 * 列的压力值
 * @param threshold 阈值分割的压力值，大于等于该值的点被视为有效区域
 * @param minRegionSize 连通域最小面积，小于该值的区域将被过滤掉（认为是噪声）
 *
 * 功能流程：
 *  1. 二值化压力图（thresholdBinarize）
 *  2. 提取连通区域（findConnectedRegions）
 *  3. 判断左右脚（classifyLeftRight）
 *  4. 提取轮廓（extractContour）
 */
void vHeatMap::dealSegment(ActivitySegment &dealSegment, const vector<vector<double>> &pressureData, int threshold,
                           int minRegionSize)
{
    if (pressureData.empty() || pressureData[0].empty())
    {
        std::cerr << "Error: input pressure data is empty." << std::endl;
        return;
    }
    // qDebug() << "thresholdBinarize";

    // Step 1: Thresholding...二值化
    auto binary = thresholdBinarize(pressureData, threshold);
    // qDebug() << "extractFootRegions";

    // Step 2: Find connected regions
    auto largestRegion = extractFootRegions(binary, minRegionSize);
    dealSegment.num_Foot = largestRegion.size();

    // 初始化为两个 200x200 的布尔矩阵
    dealSegment.foot.resize(2);  // 假设有两个脚
    // qDebug() << "resize binary" << dealSegment.num_Foot;
    for (auto &f : dealSegment.foot)
    {
        f.binary.resize(NX, std::vector<bool>(NY, false));  // 初始化每个foot的binary为200x200
    }

    // vector<vector<vector<bool>>> binarizedSole(2, vector<vector<bool>>(200, vector<bool>(200, false)));
    // 标记区域
    if (dealSegment.num_Foot == 1)
    {
        qDebug() << "一个足迹";
        markRegionOnBinary(largestRegion[0], dealSegment.foot[0].binary);
        qDebug() << "Area" << largestRegion[0].size() << "鞋码" << areaToShoeSize(largestRegion[0].size());
    }
    else if (dealSegment.num_Foot == 2)
    {
        qDebug() << "两个足迹";
        markRegionOnBinary(largestRegion[0], dealSegment.foot[0].binary);
        markRegionOnBinary(largestRegion[1], (dealSegment.foot[1].binary));
    }
    else if (dealSegment.num_Foot > 2)
    {
        qDebug() << "error,脚的数量" << dealSegment.num_Foot;
        dealSegment.num_Foot = 0;
    }

    // 格式转换
    // markRegionOnBinary(largestRegion[0], binarizedSole);
    // Step 3: Classify left and right foot

    // drawAxes(axis, centroid, 120.0);
}
void vHeatMap::dealSnapshots(ActivitySegment &dealSegment)
{
    bool keep_key[2];
    for (size_t i = 0; i < dealSegment.snapshots.size(); ++i)  // 遍历当前帧的每一张闪照
    {
        keep_key[0] = 0;
        keep_key[1] = 0;
        for (size_t j = 0; j < dealSegment.snapshots[i].size(); ++j)
        {
            for (size_t k = 0; k < dealSegment.snapshots[i][j].size(); ++k)
            {
                if (!dealSegment.foot[0].binary[j][k] && !dealSegment.foot[1].binary[j][k])
                {
                    dealSegment.snapshots[i][j][k] = 0;
                    dealSegment.overlay[j][k] = 0;
                }
                else if (dealSegment.snapshots[i][j][k] > 0)
                {
                    if (dealSegment.foot[0].binary[j][k])
                    {
                        keep_key[0] = 1;
                    }
                    else if (dealSegment.foot[1].binary[j][k])
                    {
                        keep_key[1] = 1;
                    }
                    //
                    dealSegment.fusedMaxMap[j][k] = max(dealSegment.fusedMaxMap[j][k], dealSegment.snapshots[i][j][k]);
                }
            }
        }
        // qDebug() << "Filter non-footprints";  // 用于回放

        for (int f = 0; f < 2; ++f)  // 保存每一张闪照到foot中
        {
            if (keep_key[f])
            {
                // qDebug() << "keep_key" << f;  // 用于回放

                dealSegment.foot[f].Timestamp.push_back(dealSegment.Timestamp[i]);
                dealSegment.foot[f].snapshots.push_back(dealSegment.snapshots[i]);
                for (size_t j = 0; j < dealSegment.snapshots[i].size(); ++j)
                {
                    for (size_t k = 0; k < dealSegment.snapshots[i][j].size(); ++k)
                    {
                        if (dealSegment.foot[f].binary[j][k] != 1) dealSegment.foot[f].snapshots.back()[j][k] = 0;
                    }
                }
                // qDebug() << "PressureCenterTrajectory";

                dealSegment.foot[f].PressureCenterTrajectory.push_back(
                    calculatePressureCenterTrajectory(dealSegment.foot[f].snapshots.back()));
            }
        }
        dealSegment.PressureCenterTrajectory.push_back(
            calculatePressureCenterTrajectory(dealSegment.snapshots[i]));  // 过滤完再计算当前帧的压力中心轨迹
    }
}
// 1. 阈值二值化（返回二值矩阵）
vector<vector<bool>> vHeatMap::thresholdBinarize(const vector<vector<double>> &pressureData, int threshold)
{
    int rows = pressureData.size();
    int cols = pressureData[0].size();
    vector<vector<bool>> binary(rows, vector<bool>(cols, false));
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            binary[i][j] = (pressureData[i][j] > threshold);
        }
    }
    return binary;
}

// 2. 连通域搜索（BFS），返回一个连通域点集
vector<pair<int, int>> vHeatMap::bfsRegion(const vector<vector<bool>> &binary, int startX, int startY,
                                           vector<vector<bool>> &visited)
{
    int rows = binary.size();
    int cols = binary[0].size();
    queue<pair<int, int>> q;
    vector<pair<int, int>> regionPoints;

    q.push({startX, startY});
    visited[startX][startY] = true;

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();
        regionPoints.push_back({x, y});

        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols)
            {
                if (!visited[nx][ny] && binary[nx][ny])
                {
                    visited[nx][ny] = true;
                    q.push({nx, ny});
                }
            }
        }
    }
    return regionPoints;
}

// 3. 找所有连通域
vector<vector<pair<int, int>>> vHeatMap::findConnectedRegions(const vector<vector<bool>> &binary, int minRegionSize)
{
    int rows = binary.size();
    int cols = binary[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    vector<vector<pair<int, int>>> regions;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (binary[i][j] && !visited[i][j])
            {
                vector<pair<int, int>> region = bfsRegion(binary, i, j, visited);
                if ((int)region.size() >= minRegionSize)
                {
                    regions.push_back(region);
                }
            }
        }
    }
    return regions;
}

// 4. 计算连通域质心（平均坐标）
pair<double, double> vHeatMap::getCentroid(const vector<pair<int, int>> &points)
{
    double sumX = 0, sumY = 0;
    for (auto &p : points)
    {
        sumX += p.first;
        sumY += p.second;
    }
    return {sumX / points.size(), sumY / points.size()};
}

// 5. 根据质心判断左右脚，返回左右脚轮廓索引
void vHeatMap::classifyLeftRight(ActivitySegment &dealSegment)
{
    // 1. 质心和脚朝向
    if (dealSegment.num_Foot >= 1)
    {
        auto [centroid, axis] = computeMainAxisEndpoints(dealSegment.foot[0].binary);
        dealSegment.foot[0].centroid = centroid;
        splitForeAndHindByWidth(axis, centroid, dealSegment.foot[0]);
    }
    if (dealSegment.num_Foot == 2)
    {
        auto [centroid1, axis1] = computeMainAxisEndpoints(dealSegment.foot[1].binary);
        dealSegment.foot[1].centroid = centroid1;
        splitForeAndHindByWidth(axis1, centroid1, dealSegment.foot[1]);
    }
}

// 6. 提取连通域轮廓边界点（边界点是相邻4邻居中有背景点的点）
vector<pair<int, int>> vHeatMap::extractContour(const vector<pair<int, int>> &regionPoints,
                                                const vector<vector<bool>> &binary)
{
    int rows = binary.size();
    int cols = binary[0].size();
    vector<pair<int, int>> contour;

    // 把点集合放入哈希集合快速查询
    vector<vector<bool>> isInRegion(rows, vector<bool>(cols, false));
    for (auto &p : regionPoints)
    {
        isInRegion[p.first][p.second] = true;
    }

    for (auto &p : regionPoints)
    {
        int x = p.first;
        int y = p.second;
        // 检查4邻居是否有不在连通域的（背景）
        bool isBoundary = false;
        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            if (nx < 0 || nx >= rows || ny < 0 || ny >= cols || !isInRegion[nx][ny])
            {
                isBoundary = true;
                break;
            }
        }
        if (isBoundary)
        {
            contour.push_back(p);
        }
    }
    return contour;
}
/**
 * @brief 在热力图上绘制所有连通区域
 *
 * 遍历所有连通区域的像素点，并将其在 m_colorMap 上标记为白色。
 * 用于可视化压力图中的所有有效区域。
 *
 * @param regions 连通区域集合，每个元素是一个区域的像素坐标集合 (y, x)
 */
void vHeatMap::drawConnectedRegions(const std::vector<std::vector<std::pair<int, int>>> &regions)
{
    if (!m_colorMap || !m_colorMap->data()) return;

    const double borderValue = 999.0;  // 边界颜色值

    // 获取颜色映射的尺寸（这里假设有width()和height()方法）
    int height = m_colorMap->data()->valueSize();  // 行数，高度
    int width = m_colorMap->data()->keySize();     // 列数，宽度

    for (const auto &region : regions)
    {
        // 用std::unordered_set加速查找，需要自定义哈希（如果用C++11）
        // 这里用std::set保证兼容性
        std::set<std::pair<int, int>> regionPoints(region.begin(), region.end());

        for (const auto &pt : region)
        {
            int y = pt.first;
            int x = pt.second;

            // 检查四个邻居
            bool isBorder = false;
            const std::vector<std::pair<int, int>> neighbors = {{y - 1, x}, {y + 1, x}, {y, x - 1}, {y, x + 1}};

            for (const auto &nb : neighbors)
            {
                int ny = nb.first;
                int nx = nb.second;
                if (ny < 0 || ny >= height || nx < 0 || nx >= width)
                {
                    isBorder = true;
                    break;
                }
                if (regionPoints.find({ny, nx}) == regionPoints.end())
                {
                    isBorder = true;
                    break;
                }
            }

            if (isBorder)
            {
                m_colorMap->data()->setCell(y, x, borderValue);
            }
        }
    }
}

/**
 * @brief 对热力图数据进行和历史滤波
 *
 * 对当前帧的一维热力图数据进行处理：
 * - 如果上一帧对应位置的值为0，且当前值小于阈值，则将当前值置为0；
 * - 同时更新上一帧数据为当前帧数据。
 *
 * @param currentData 当前帧的一维热力图数据
 * @param threshold 阈值，小于该值的元素将被置为0
 */
void vHeatMap::applyHeatMapFilter(std::vector<double> &currentData, double threshold)
{
    static std::vector<double> previousData;

    if (previousData.size() != currentData.size())
    {
        previousData = currentData;  // 初始化上一帧数据
    }

    for (size_t i = 0; i < currentData.size(); ++i)
    {
        if (previousData[i] == 0)
        {
            if (currentData[i] < threshold)
            {
                currentData[i] = 0;
            }
            previousData[i] = currentData[i];
        }
        else
        {
            previousData[i] = currentData[i];
        }
    }
}

// 对通道数据执行3x3腐蚀及膨胀操作，
void vHeatMap::erodeAndDilate(std::vector<double> &channels)
{
    using Image = std::vector<std::vector<double>>;

    int new_h = ROWS + 2;
    int new_w = COLS + 2;

    // 创建padding后的二维图像，边界补零
    Image padded(new_h, std::vector<double>(new_w, 0.0));

    for (int y = 0; y < ROWS; ++y)
    {
        for (int x = 0; x < COLS; ++x)
        {
            padded[y + 1][x + 1] = channels[y * COLS + x];
        }
    }

    // 创建一个临时数组保存腐蚀结果，避免边遍历边修改导致错误
    std::vector<double> erodedChannels = channels;
    double sum = 0;
    // 3x3腐蚀核遍历
    for (int y = 0; y < ROWS; ++y)
    {
        for (int x = 0; x < COLS; ++x)
        {
            // 不腐蚀标记位
            quint32 match = 0;
            sum = 0;
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    sum += padded[y + 1 + dy][x + 1 + dx];
                    if (padded[y + 1 + dy][x + 1 + dx] > threshold_filter)
                    {
                        match++;
                    }
                }
                // if (!match) break;
            }

            if (match > 2)
            {
                erodedChannels[y * COLS + x] /= 2;
                erodedChannels[y * COLS + x] += sum / 9 / 2;
            }
            else
            {
                erodedChannels[y * COLS + x] = 0.0f;
            }
        }
    }

    // 结果回写
    channels = erodedChannels;
}
// 标准腐蚀
// void vHeatMap::erodeAndDilate(std::vector<double> &channels)
// {
//     using Image = std::vector<std::vector<double>>;
//     Image padded(ROWS + 2, std::vector<double>(COLS + 2, 0.0));
//     for (int y = 0; y < ROWS; ++y)
//         for (int x = 0; x < COLS; ++x) padded[y + 1][x + 1] = channels[y * COLS + x];

//     std::vector<double> result = channels;
//     for (int y = 0; y < ROWS; ++y)
//     {
//         for (int x = 0; x < COLS; ++x)
//         {
//             double minVal = padded[y + 1][x + 1];
//             for (int dy = -1; dy <= 1; ++dy)
//                 for (int dx = -1; dx <= 1; ++dx) minVal = std::min(minVal, padded[y + 1 + dy][x + 1 + dx]);

//             result[y * COLS + x] = minVal;
//         }
//     }
//     channels = result;
// }
/**
 * @brief 计算二值图形主轴的两个端点（基于 PCA 主成分分析）
 *
 * 该函数从输入的二值图像中提取所有为 true 的像素点，计算其质心和协方差矩阵，
 * 使用 PCA
 * 分析得到主轴方向，并基于给定的长度，返回质心沿主轴方向前后延伸的两个端点。
 *
 * @param binary 输入的二值图像（true 表示有效点）
 * @return std::pair<QPointF, QPointF>
 * 返回质心和主轴（绘图坐标系下）
 */
std::pair<QPointF, QVector2D> vHeatMap::computeMainAxisEndpoints(const std::vector<std::vector<bool>> &binary)
{
    std::vector<QPointF> pts;
    //    float plotY;
    int rows = binary.size(), cols = rows ? binary[0].size() : 0;
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            if (binary[y][x])
            {
                pts.emplace_back(y, x);
            }
    if (pts.empty()) return {{0, 0}, {0, 0}};

    // 质心
    double cx = 0, cy = 0;
    for (auto &p : pts)
    {
        cx += p.x();
        cy += p.y();
    }
    cx /= pts.size();
    cy /= pts.size();
    QPointF centroid(cx, cy);

    // 协方差
    double varX = 0, varY = 0, covXY = 0;
    for (auto &p : pts)
    {
        double dx = p.x() - cx;
        double dy = p.y() - cy;
        varX += dx * dx;
        varY += dy * dy;
        covXY += dx * dy;
    }
    varX /= pts.size();
    varY /= pts.size();
    covXY /= pts.size();

    // PCA 主轴
    double theta = 0.5 * atan2(2 * covXY, varX - varY);
    QVector2D axis(std::cos(theta), std::sin(theta));
    axis.normalize();

    //    QVector2D halfVec = axis * (length / 2.0);
    //    QPointF p1 = centroid - halfVec.toPointF();
    //    QPointF p2 = centroid + halfVec.toPointF();
    return {centroid, axis};
}

/**
 * @brief 在二值图中查找面积最大的连通区域
 *
 * @param binary 二值化矩阵，binary[y][x] 为 true 表示前景
 * @param minRegionSize 连通区域最小像素数，小于该值的区域将被忽略
 * @return vector<pair<int,int>>
 * 最大连通区域的点集合；若无符合条件的区域，则返回空向量
 */
std::vector<std::pair<int, int>> vHeatMap::findLargestRegion(const std::vector<std::vector<bool>> &binary,
                                                             size_t minRegionSize)
{
    // 先找出所有符合条件的连通区域
    auto regions = findConnectedRegions(binary, minRegionSize - 400);

    // 如果没有区域，则返回空
    if (regions.empty())
    {
        return {};
    }

    // 遍历所有区域，找出 size 最大的
    size_t sec_Size = 0;
    int secIdx = 0;
    size_t maxSize = 0;
    int maxIdx = 0;
    for (int i = 0; i < (int)regions.size(); ++i)
    {
        size_t size = regions[i].size();
        if (size > maxSize)
        {
            sec_Size = maxSize;
            secIdx = maxIdx;
            maxSize = size;
            maxIdx = i;
        }
        else if (size > sec_Size)
        {
            sec_Size = size;
            secIdx = i;
        }
    }
    // 一边没有被识别成两半
    if (maxSize > minRegionSize * 3)
    {
        // qDebug() << "maxSize" << maxSize;
        if (sec_Size > minRegionSize * 3)
        {
            qDebug() << "检测到两个脚";
            qDebug() << "maxSize" << maxSize << "sec_Size" << sec_Size;
        }
    }
    else if (maxSize + sec_Size > minRegionSize * 2)
    {
        regions[maxIdx].insert(regions[maxIdx].end(), regions[secIdx].begin(), regions[secIdx].end());
        // qDebug() << "maxSize" << maxSize;
        // qDebug() << "sec_Size" << sec_Size;
    }
    else
        return {};

    // 返回最大的区 域
    return regions[maxIdx];
}
/**
 * @brief 在二值图中查找最大的两个足底区域，分别返回（不合并），用于左右脚识别。
 *
 * @param binary 二值图，true 表示有效点。
 * @param minRegionSize 最小有效区域大小。
 * @return std::vector<std::vector<std::pair<int, int>>> 返回最大两个区域，每个区域是一组点。
 */
std::vector<std::vector<std::pair<int, int>>> vHeatMap::extractFootRegions(const std::vector<std::vector<bool>> &binary,
                                                                           size_t minRegionSize)
{
    // qDebug() << "findConnectedRegions";

    auto regions = findConnectedRegions(binary, 0);  // 全都找出来
    // qDebug() << "regions size:" << regions.size();

    if (regions.empty())
    {
        return {};
    }

    // 找出前两个最大区域
    //    size_t maxSize = 0, secondSize = 0;
    //    int maxIdx = -1, secondIdx = -1;

    // for (int i = 0; i < static_cast<int>(regions.size()); ++i)
    // {
    //     size_t size = regions[i].size();
    //     if (size > maxSize)
    //     {
    //         secondSize = maxSize;
    //         secondIdx = maxIdx;
    //         maxSize = size;
    //         maxIdx = i;
    //     }
    //     else if (size > secondSize)
    //     {
    //         secondSize = size;
    //         secondIdx = i;
    //     }
    // }
    std::sort(regions.begin(), regions.end(),
              [](const auto &a, const auto &b)
              {
                  return a.size() > b.size();  // 从大到小全排序
              });

    std::vector<std::vector<std::pair<int, int>>> result = {};

    // 情况一：两个区域都够大 => 分别返回两个脚
    if (regions[0].size() > minRegionSize * 0.8 && regions[1].size() > minRegionSize * 0.8 && regions.size() >= 2)
    {
        qDebug() << "检测到两个脚区域：";
        qDebug() << "Max Size:" << regions[0].size() << ", Second Size:" << regions[1].size();
        result.push_back(regions[0]);
        result.push_back(regions[1]);
    }
    // 一个脚
    else if (regions[0].size() > minRegionSize * 0.8)
    {
        result.push_back(regions[0]);
    }
    // 情况二：两个区域加起来还行 => 合并成一个脚
    else if (regions[0].size() + regions[1].size() > minRegionSize * 0.7 && regions.size() >= 2)
    {
        std::vector<std::pair<int, int>> merged = regions[0];

        if (!regions[1].empty())
        {
            merged.insert(merged.end(), regions[1].begin(), regions[1].end());
        }

        result.push_back(merged);
    }

    return result;
}

/**
 * @brief 在二值图像上标记区域点，区域点为 (x, y)，即 x 为列，y 为行。
 *
 * @param region 区域点的集合，每个点是 (x, y) 对
 * @param binary 二值图像，使用 binary[y][x] 访问，y 为行，x 为列
 */
void vHeatMap::markRegionOnBinary(const std::vector<std::pair<int, int>> &region,
                                  std::vector<std::vector<bool>> &binary)
{
    if (region.empty())
    {
        binary.clear();
        return;
    }
    // binary.assign(200, std::vector<bool>(200, false));
    int rows = binary.size();
    if (rows == 0) return;
    int cols = binary[0].size();

    // 清空整个二值图像
    for (int y = 0; y < rows; ++y)
    {
        std::fill(binary[y].begin(), binary[y].end(), false);
    }

    // 标记 region 中的每个点 (x, y)
    for (const auto &pt : region)
    {
        int x = pt.first;   // 列
        int y = pt.second;  // 行
        if (y >= 0 && y < rows && x >= 0 && x < cols)
        {
            binary[x][y] = true;
        }
    }
}

/**
 * @brief 检测二值图中沿主轴方向的脚尖点，并在图中标注。
 *
 * 输入为主轴方向（单位向量）、质心和二值图像，输出为图上红色标记的脚尖点。
 * 不进行坐标翻转，适用于 colorMap 原始坐标系。
 *
 * 使用类中的 m_customPlot 进行绘图，无需外部传入。
 */
void vHeatMap::detectAndMarkToeTip(const std::vector<std::vector<bool>> &binary, const QVector2D &axis,
                                   const QPointF &centroid)
{
    int rows = binary.size();
    int cols = rows > 0 ? binary[0].size() : 0;
    if (rows == 0 || cols == 0 || !m_customPlot) return;

    // 寻找投影最大点（脚尖）
    QPointF toeTip;
    double maxProj = -1e9;

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            if (!binary[y][x]) continue;

            QPointF pt(y, x);
            QVector2D vec(pt - centroid);
            double proj = QVector2D::dotProduct(vec, axis);

            if (proj > maxProj)
            {
                maxProj = proj;
                toeTip = pt;
            }
        }
    }

    toeTip.setX(toeTip.x() / (NX / ROWS) - 0.5);
    toeTip.setY(toeTip.y() / (NY / COLS) - 0.5);
    // toeTip.setX(200 / (200.0 / ROWS) - 0.5);
    // toeTip.setY(200 / (200.0 / COLS) - 0.5);
    std::cout << "toeTip: (" << toeTip.x() << ", " << toeTip.y() << ")" << std::endl;

    static QCPItemEllipse *circle = nullptr;
    static QCPItemText *label = nullptr;
    // 删除旧标注
    if (circle)
    {
        m_customPlot->removeItem(circle);
        // 不删除指针，让父对象自动释放
        circle = nullptr;
    }
    if (label)
    {
        m_customPlot->removeItem(label);
        // 不删除指针，让父对象自动释放
        circle = nullptr;
    }

    // 可视化脚尖点（圆圈 + 文字）
    circle = new QCPItemEllipse(m_customPlot);
    circle->topLeft->setCoords(toeTip.x() - 1.5, toeTip.y() + 1.5);
    circle->bottomRight->setCoords(toeTip.x() + 1.5, toeTip.y() - 1.5);
    circle->setPen(QPen(Qt::red, 2));

    label = new QCPItemText(m_customPlot);
    label->position->setCoords(toeTip.x() + 2, toeTip.y() + 2);
    label->setText("Toe Tip");
    label->setColor(Qt::red);
    label->setFont(QFont(m_customPlot->font().family(), 10));
    label->setPen(QPen(Qt::red));
}
void dividePoint(QPointF &p, int d)
{
    p.setX(p.x() / d);
    p.setY(p.y() / d);
}

/**
 * @brief
 * 根据给定主轴和质心，将二值图像区域按宽度分割为前后部分，并判断左右脚掌方向。
 *
 * 该函数基于输入的二值矩阵（binary）、主轴向量（axis）和区域质心（centroid），
 * 通过计算投影距离和方差，确定前后区域及左右脚掌方向。
 * 并在图形界面（m_customPlot）上以红色文字标注脚尖方向。
 *
 * @param binary 输入的二维布尔矩阵，表示区域的二值图像。
 * @param axis   主轴方向的二维向量，用于分割前后区域。
 * @param centroid 区域的质心点。
 *
 * @note
 * - 使用projectionDistance函数计算点到轴的投影距离。
 * - 通过副轴方向辅助判断脚尖方向。
 * - 使用QCPItemText在绘图控件中显示脚尖标注。
 */
void vHeatMap::splitForeAndHindByWidth(const QVector2D &axis, const QPointF &centroid, Foot &foot)
{
    // if (binary.empty()) return;
    int rows = foot.binary.size(), cols = rows ? foot.binary[0].size() : 0;
    if (rows == 0 || cols == 0) return;
    if (foot.binary.empty() || foot.binary[0].empty()) return;
    // 副轴
    QVector2D Sec_Axis(axis.y(), -axis.x());
    // 求在副轴的投影
    double perp[2] = {0.0, 0.0};
    quint32 numb[2] = {0, 0};
    QPointF point;
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++)
        {
            if (!foot.binary[y][x]) continue;
            point = QPointF(y, x);
            double temp = projectionDistance(point, centroid, Sec_Axis);
            if (projectionDistance(point, centroid, axis) > 0)
            {
                perp[0] += temp * temp;
                numb[0]++;
            }
            else
            {
                perp[1] += temp * temp;
                numb[1]++;
            }
        }
    perp[0] /= numb[0];
    perp[1] /= numb[1];
    if (perp[0] > perp[1])
    {
        foot.footOrientationVec = axis;
    }
    else
    {
        Sec_Axis = -Sec_Axis;
        foot.footOrientationVec = -axis;
        foot.Sec_Axis = Sec_Axis;
    }
    /*---解析左右脚--*/
    perp[0] = 0.0;
    perp[1] = 0.0;
    numb[0] = 0;
    numb[1] = 0;
    qDebug() << "footOrientationVec" << foot.footOrientationVec;
    qDebug() << "Sec_Axis          " << Sec_Axis;
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            if (!foot.binary[y][x]) continue;
            point = QPointF(y, x);
            double temp = projectionDistance(point, centroid, foot.footOrientationVec);
            double temp1 = projectionDistance(point, centroid, Sec_Axis);
            if (temp1 > 0)
            {
                perp[0] += temp * temp;
                numb[0]++;
            }
            else
            {
                perp[1] += temp * temp;
                numb[1]++;
            }
            // 保存四个极点的距离
            foot.extreme_lenth[0] = std::max(foot.extreme_lenth[0], temp);
            foot.extreme_lenth[1] = std::min(foot.extreme_lenth[1], temp);
            foot.extreme_lenth[2] = std::min(foot.extreme_lenth[2], temp1);
            foot.extreme_lenth[3] = std::max(foot.extreme_lenth[3], temp1);
        }
    }
    double lenth = abs(foot.extreme_lenth[0]) + abs(foot.extreme_lenth[1]);
    double wide = abs(foot.extreme_lenth[2]) + abs(foot.extreme_lenth[3]);
    qDebug() << "lenth" << lenth << "鞋码" << plotLengthToShoeSize(lenth);
    qDebug() << "wide" << wide << "鞋码" << plotWidthToShoeSize(wide);

    qDebug() << foot.extreme_lenth[0] << foot.extreme_lenth[1] << foot.extreme_lenth[2] << foot.extreme_lenth[3];
    QVector2D extrem_plenth0 = foot.footOrientationVec * foot.extreme_lenth[0];
    QVector2D extrem_plenth3 = foot.Sec_Axis * foot.extreme_lenth[3];
    QVector2D extrem_plenth1 = foot.footOrientationVec * foot.extreme_lenth[1];
    QVector2D extrem_plenth2 = foot.Sec_Axis * foot.extreme_lenth[2];
    qDebug() << "extreme_lenth";
    qDebug() << extrem_plenth0 << extrem_plenth1 << extrem_plenth2 << extrem_plenth3;

    // 求四个端点
    foot.extreme_point[0] = (extrem_plenth0 + extrem_plenth3).toPointF() + foot.centroid;
    foot.extreme_point[3] = (extrem_plenth3 + extrem_plenth1).toPointF() + foot.centroid;
    foot.extreme_point[1] = (extrem_plenth1 + extrem_plenth2).toPointF() + foot.centroid;
    foot.extreme_point[2] = (extrem_plenth2 + extrem_plenth0).toPointF() + foot.centroid;
    // dividePoint(foot.extreme_point[0], maptoplotX);
    // dividePoint(foot.extreme_point[1], maptoplotX);
    // dividePoint(foot.extreme_point[2], maptoplotX);
    // dividePoint(foot.extreme_point[3], maptoplotX);

    qDebug() << "extreme_point";
    qDebug() << foot.extreme_point[0] << foot.extreme_point[1] << foot.extreme_point[2] << foot.extreme_point[3];

    perp[0] /= numb[0];
    perp[1] /= numb[1];
    if (perp[0] > perp[1])
    {
        foot.is_left_foot = 1;
        qDebug() << "左脚掌";
    }
    else
    {
        foot.is_left_foot = 0;
        qDebug() << "右脚掌";
    }
    /*---解析左右脚end--*/
}

// -------标注脚掌--------
void vHeatMap::annotateFoot(ActivitySegment &dealSegment)
{
    static QCPItemText *label[2] = {nullptr, nullptr};
    m_colorMap->parentPlot()->clearItems();
    m_colorMap->parentPlot()->clearGraphs();
    auto makeLine = [&](const QPointF &a, const QPointF &b)
    {
        auto *line = new QCPItemLine(m_colorMap->parentPlot());
        line->start->setType(QCPItemPosition::ptPlotCoords);
        line->end->setType(QCPItemPosition::ptPlotCoords);
        line->start->setCoords(a.x(), a.y());
        line->end->setCoords(b.x(), b.y());
        line->setPen(QPen(Qt::blue, 2));
        return line;
    };  // 标注脚掌方向

    // 可视化脚尖点（圆圈 + 文字）
    // qDebug() << "标注脚掌数量" << dealSegment.num_Foot;
    for (quint32 i = 0; i < dealSegment.num_Foot; i++)
    {
        label[i] = new QCPItemText(m_customPlot);
        label[i]->position->setCoords(
            dealSegment.foot[i].footOrientationVec.x() * 10 + (dealSegment.foot[i].centroid.x() / NX * 70),
            dealSegment.foot[i].footOrientationVec.y() * 10 + (dealSegment.foot[i].centroid.y() / NY * 56));
        if (dealSegment.foot[i].is_left_foot)
            label[i]->setText("left Toe");
        else
            label[i]->setText("left Toe");
        label[i]->setColor(Qt::red);
        label[i]->setFont(QFont(m_customPlot->font().family(), 10));
        label[i]->setPen(QPen(Qt::red));
    }
    // 画出主轴和框出脚
    //--遍历foot
    for (Foot dealfoot : dealSegment.foot)
    {
        // 4. 画主轴线
        QPointF offset(dealfoot.footOrientationVec.x() * dealfoot.extreme_lenth[0],
                       dealfoot.footOrientationVec.y() * dealfoot.extreme_lenth[0]);
        QPointF p1 = dealfoot.centroid + offset;
        offset.setX(dealfoot.footOrientationVec.x() * dealfoot.extreme_lenth[1]);
        offset.setY(dealfoot.footOrientationVec.y() * dealfoot.extreme_lenth[1]);
        QPointF p2 = dealfoot.centroid + offset;
        p1.setX(p1.x() / maptoplotX);
        p1.setY(p1.y() / maptoplotY);
        p2.setX(p2.x() / maptoplotX);
        p2.setY(p2.y() / maptoplotY);
        makeLine(p1, p2);

        // 副轴线
        offset.setX(dealfoot.Sec_Axis.x() * dealfoot.extreme_lenth[2]);
        offset.setY(dealfoot.Sec_Axis.y() * dealfoot.extreme_lenth[2]);
        p1 = dealfoot.centroid + offset;
        offset.setX(dealfoot.Sec_Axis.x() * dealfoot.extreme_lenth[3]);
        offset.setY(dealfoot.Sec_Axis.y() * dealfoot.extreme_lenth[3]);
        p2 = dealfoot.centroid + offset;
        p1.setX(p1.x() / (NX / ROWS));
        p1.setY(p1.y() / (NY / COLS));
        p2.setX(p2.x() / (NX / ROWS));
        p2.setY(p2.y() / (NY / COLS));
        makeLine(p1, p2);

        //--draw box
        for (int i = 0; i < 4; ++i)
        {
            dealfoot.extreme_point[i].setX(dealfoot.extreme_point[i].x() / maptoplotX);
            dealfoot.extreme_point[i].setY(dealfoot.extreme_point[i].y() / maptoplotY);
        }
        makeLine(dealfoot.extreme_point[0], dealfoot.extreme_point[3]);
        makeLine(dealfoot.extreme_point[3], dealfoot.extreme_point[1]);
        makeLine(dealfoot.extreme_point[1], dealfoot.extreme_point[2]);
        makeLine(dealfoot.extreme_point[2], dealfoot.extreme_point[0]);
    }
}
QRectF computeBoundingBox(const std::vector<std::vector<bool>> &binary)
{
    int rows = binary.size();
    int cols = binary[0].size();
    int minRow = rows, maxRow = -1;
    int minCol = cols, maxCol = -1;

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (binary[i][j])
            {
                minRow = std::min(minRow, i);
                maxRow = std::max(maxRow, i);
                minCol = std::min(minCol, j);
                maxCol = std::max(maxCol, j);
            }
        }
    }

    if (minRow > maxRow || minCol > maxCol)
    {
        return QRectF();  // 返回空矩形，表示没有有效点
    }

    return QRectF(minCol, minRow, maxCol - minCol + 1, maxRow - minRow + 1);
}
/**
 * @brief 计算点到质心沿主轴方向的投影距离
 *
 * 给定一个点、质心和单位主轴向量，计算该点相对于质心在主轴方向上的投影距离。
 * 投影距离为点到质 心的向量在主轴单位向量上的点积。
 *
 * @param point 需要投影的点坐标
 * @param centroid 质心坐标，投影参考点
 * @param axisUnit 已归一化的主轴单位向量
 * @return double 点在主轴方向上的投影距离（标量）
 */
double vHeatMap::projectionDistance(const QPointF &point, const QPointF &centroid, const QVector2D &axisUnit)
{
    // // axisUnit 已归一化为单位向量
    // double vx = point.x() - centroid.x();
    // double vy = point.y() - centroid.y();
    // return vx * axisUnit.x() + vy * axisUnit.y();
    QVector2D v(point - centroid);              // QPointF 差值构造 QVector2D
    return QVector2D::dotProduct(v, axisUnit);  // 点积即为投影长度
}
/**
 * @brief 计算压力中心轨迹点（重心）
 *
 * 此函数用于从二维压力矩阵中计算压力中心的位置（即加权重心）。
 * 输入的 pressureMatrix 是一个二维数组，表示每个点的压力值，索引 (i, j) 表示坐标 (x=j, y=i)。
 * 计算方式是以压力值为权重对所有点坐标进行加权平均。
 *
 * @param pressureMatrix 二维压力值矩阵，尺寸为 [height][width]
 * @return QPointF 压力中心位置（浮点型坐标）
 */
QPointF calculatePressureCenterTrajectory(const std::vector<std::vector<double>> &pressureMatrix)
{
    double weightedSumX = 0.0, weightedSumY = 0.0;
    double totalPressure = 0.0;
    int height = pressureMatrix.size();
    if (height == 0)
    {
        throw std::invalid_argument("Pressure matrix is empty.");
    }
    auto width = pressureMatrix[0].size();
    if (width == 0)
    {
        throw std::invalid_argument("Pressure matrix has empty rows.");
    }
    for (int i = 0; i < height; ++i)
    {
        if (pressureMatrix[i].size() != width)
        {
            throw std::invalid_argument("All rows in the pressure matrix must have the same width.");
        }
        for (unsigned int j = 0; j < width; ++j)
        {
            double pressure = pressureMatrix[i][j];
            weightedSumX += j * pressure;  // j is x
            weightedSumY += i * pressure;  // i is y
            totalPressure += pressure;
        }
    }
    if (totalPressure == 0.0)
    {
        qDebug() << ("Total pressure is zero. Cannot compute center.");
    }

    return QPointF((weightedSumY / totalPressure) / maptoplotX, (weightedSumX / totalPressure) / maptoplotY);
}
double vHeatMap::areaToShoeSize(double area)
{
    // 反推原函数的计算过程
    double denom = (WIDTH_ELECTORDE + GAP_ELECTORDES);

    // 从面积反推 area_mm2
    double area_mm2 = area * (denom * denom) / (maptoplotX * maptoplotY);

    // 从 area_mm2 反推 footLength
    // 原公式: area_mm2 = pow(footLength, 2) * 0.273
    // 反推: footLength = sqrt(area_mm2 / 0.273)
    double footLength = sqrt(area_mm2 / 0.273);

    // 从 footLength 反推 size
    // 原公式: footLength = size * 5
    // 反推: size = footLength / 5
    int size = (int)round(footLength / 5.0);

    return size;
}
double vHeatMap::shoeSizeToArea(int size)
{
    // 根据中国/欧码鞋码估算脚底面积（单位：cm²）
    double footLength = size * 5;  // mm//脚长
    double area_mm2 = pow(footLength, 2) * 0.273;
    double denom = (WIDTH_ELECTORDE + GAP_ELECTORDES);
    double area = area_mm2 / (denom * denom) * maptoplotX * maptoplotY;
    return area;
}
int vHeatMap::plotLengthToShoeSize(double footLength_plot)
{
    double footLength_mm = footLength_plot * maptoplotX;
    double rawSize = (footLength_mm - 50) / 5.0;
    int shoeSize = static_cast<int>(std::floor(rawSize + 0.5));
    if (shoeSize < 0)
    {
        shoeSize = 0;
    }
    return shoeSize;
}

/**
 * @brief 根据画布上的脚宽（单位与 maptoplotX 一致）估算中国/欧码鞋码
 *
 * @param footWidth_plot 脚宽在画布单位下的值（例如像素）
 * @return int           推算得到的鞋码（四舍五入）
 */
int vHeatMap::plotWidthToShoeSize(double footWidth_plot) { return plotLengthToShoeSize(footWidth_plot * 2.5); }

/**
 * @brief 提取波动最小的长度为 k 的子段
 * @param data 输入数据
 * @param k    窗口长度 (k <= data.size())
 * @return     一个长度为 k 的子向量，其波动（方差）最小
 */
std::vector<double> extract_least_fluctuation(const std::vector<double> &data, int k)
{
    int n = data.size();
    if (k <= 0 || k > n) return {};

    // 1. 构造前缀和 S，前缀平方和 Q
    std::vector<double> S(n + 1, 0.0), Q(n + 1, 0.0);
    for (int i = 0; i < n; ++i)
    {
        S[i + 1] = S[i] + data[i];
        Q[i + 1] = Q[i] + data[i] * data[i];
    }

    // 2. 滑动计算每个窗口的方差，记录最小值
    double bestVar = std::numeric_limits<double>::infinity();
    int bestIdx = 0;

    for (int i = 0; i + k <= n; ++i)
    {
        double sum = S[i + k] - S[i];
        double sqsum = Q[i + k] - Q[i];
        double mean = sum / k;
        // 方差公式：E[x^2] - (E[x])^2
        double var = sqsum / k - mean * mean;
        if (var < bestVar)
        {
            bestVar = var;
            bestIdx = i;
        }
    }

    // 3. 提取并返回最好窗口
    return std::vector<double>(data.begin() + bestIdx, data.begin() + bestIdx + k);
}
/// @brief 更新热图数据
void vHeatMap::updateHeatMapData(DisplayHeatMapData *data, QCPColorMap *m_colorMaptar)
{
    static QPointF previousPressureCenter;  // 上一帧压力中心轨迹
    static bool hasPreviousCenter = false;  // 标记是否有之前的压力中心

    // 压力矩阵
    if (data->pressureMatrix == nullptr || data->pressureMatrix->empty() || (*data->pressureMatrix)[0].empty())
    {
        // qDebug() << "Pressure matrix is empty, cannot update heat map.";
        return;
    }
    for (size_t i = 0; i < data->pressureMatrix->size(); ++i)
    {
        for (size_t j = 0; j < (*data->pressureMatrix)[i].size(); ++j)
        {
            m_colorMaptar->data()->setCell(i, j, (*data->pressureMatrix)[i][j]);
        }
    }
    data->pressureMatrix = nullptr;  // 清除指针，避免重复标注

    // 标注脚掌
    if (data->annotatedFoot != nullptr)
    {
        // 标注脚掌
        annotateFoot(*data->annotatedFoot);
        data->annotatedFoot = nullptr;  // 清除指针，避免重复标注
    }

    // 计算压力中心轨迹
    if (data->currentPressureCenter == nullptr)
    {
        // qDebug() << "Pressure center is not set, cannot draw line.";
        hasPreviousCenter = false;  // 重置标记
    }
    else
    {
        if (hasPreviousCenter)
        {
            drawLine(previousPressureCenter.x(), previousPressureCenter.y(), data->currentPressureCenter->x(),
                     data->currentPressureCenter->y(), Qt::red);
        }

        // 更新前一帧的压力中心
        previousPressureCenter = *data->currentPressureCenter;
        hasPreviousCenter = true;
        data->currentPressureCenter = nullptr;  // 清除指针，保持一致性
    }
}
QTime staticTimer;

// void vHeatMap::colstatic(void)
// {
//     static startime = QTime::currentTime();
//     QTimer *statictimer = nullptr;  // 用于时间回放

//     if (!statictimer)
//     {
//         statictimer = new QTimer(this);
//         connect(statictimer, &QTimer::timeout, this, &vHeatMap::colstatic);
//     }
//     playbackTimer->start(10000);  // 启动下一帧的定时器
// }
// void vHeatMap::coldynamic(void) {}
