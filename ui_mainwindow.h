/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_6;
    QTabWidget *tabWidget;
    QWidget *tab_4;
    QGridLayout *gridLayout_10;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_9;
    QSpacerItem *horizontalSpacer_4;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_14;
    QLineEdit *lineEditMaxPressPoint_3;
    QLineEdit *lineEditMaxPressValue_3;
    QLineEdit *lineEditAveragePress_3;
    QLineEdit *lineEditArea_3;
    QLineEdit *lineEditPressureCenterTrajectory_3;
    QLineEdit *lineEditTotalgePress_3;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QLabel *label_21;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *verticalSpacer_4;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox_6;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_heatmap;
    QWidget *tab_2;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox_7;
    QGroupBox *groupBox_9;
    QGroupBox *groupBox_10;
    QGroupBox *groupBox_8;
    QWidget *tab_3;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_8;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEditMaxPressPoint;
    QLineEdit *lineEditMaxPressValue;
    QLineEdit *lineEditAveragePress;
    QLineEdit *lineEditArea;
    QLineEdit *lineEditPressureCenterTrajectory;
    QLineEdit *lineEditTotalgePress;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_5;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_4;
    QLineEdit *lineEditCalibration1;
    QPushButton *pushButtonCalibration1;
    QLabel *label_6;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_5;
    QLineEdit *lineEditCalibration2;
    QPushButton *pushButtonCalibration2;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QWidget *tab_5;
    QFormLayout *formLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *labelCom7;
    QLabel *labelCom1;
    QLabel *label_stepFrequency;
    QSpinBox *spinBox;
    QLabel *labelCom5;
    QLabel *label_7;
    QPushButton *btnNextFrame;
    QPushButton *pushButtonOpen;
    QLabel *labelCom2;
    QLabel *label_8;
    QLabel *label_stancetimeL;
    QLabel *label_stancetimeR;
    QLabel *label_walkspeed;
    QLabel *labelCom3;
    QComboBox *comboBoxCom7;
    QPushButton *pushButtonExportData;
    QLabel *label_FPS;
    QCheckBox *checkBox_showChannel;
    QComboBox *comboBoxCom2;
    QLabel *label_3;
    QLabel *label_15;
    QLabel *label_10;
    QComboBox *comboBoxCom6;
    QLabel *label_stepwidth;
    QComboBox *comboBoxCom3;
    QComboBox *comboBoxCom5;
    QLabel *labelCom6;
    QLabel *labelCom4;
    QComboBox *comboBoxCom4;
    QPushButton *btnStartPlay;
    QComboBox *comboBoxCom1;
    QLabel *label_2;
    QLabel *label_9;
    QLabel *label_16;
    QLabel *label_steplenth;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QWidget *tab_6;
    QTableWidget *tableWidgetF;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1000, 1000);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_6 = new QGridLayout(centralwidget);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setEnabled(true);
        tabWidget->setTabBarAutoHide(false);
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        gridLayout_10 = new QGridLayout(tab_4);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        groupBox_4 = new QGroupBox(tab_4);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        sizePolicy.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy);
        gridLayout_9 = new QGridLayout(groupBox_4);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_9->addItem(horizontalSpacer_4, 1, 0, 1, 1);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        lineEditMaxPressPoint_3 = new QLineEdit(groupBox_4);
        lineEditMaxPressPoint_3->setObjectName(QString::fromUtf8("lineEditMaxPressPoint_3"));
        lineEditMaxPressPoint_3->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_14->addWidget(lineEditMaxPressPoint_3);

        lineEditMaxPressValue_3 = new QLineEdit(groupBox_4);
        lineEditMaxPressValue_3->setObjectName(QString::fromUtf8("lineEditMaxPressValue_3"));
        lineEditMaxPressValue_3->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_14->addWidget(lineEditMaxPressValue_3);


        verticalLayout_6->addLayout(horizontalLayout_14);

        lineEditAveragePress_3 = new QLineEdit(groupBox_4);
        lineEditAveragePress_3->setObjectName(QString::fromUtf8("lineEditAveragePress_3"));
        lineEditAveragePress_3->setMaximumSize(QSize(150, 16777215));

        verticalLayout_6->addWidget(lineEditAveragePress_3);

        lineEditArea_3 = new QLineEdit(groupBox_4);
        lineEditArea_3->setObjectName(QString::fromUtf8("lineEditArea_3"));
        lineEditArea_3->setMaximumSize(QSize(150, 16777215));

        verticalLayout_6->addWidget(lineEditArea_3);

        lineEditPressureCenterTrajectory_3 = new QLineEdit(groupBox_4);
        lineEditPressureCenterTrajectory_3->setObjectName(QString::fromUtf8("lineEditPressureCenterTrajectory_3"));
        lineEditPressureCenterTrajectory_3->setMaximumSize(QSize(150, 16777215));

        verticalLayout_6->addWidget(lineEditPressureCenterTrajectory_3);

        lineEditTotalgePress_3 = new QLineEdit(groupBox_4);
        lineEditTotalgePress_3->setObjectName(QString::fromUtf8("lineEditTotalgePress_3"));
        lineEditTotalgePress_3->setMaximumSize(QSize(150, 16777215));

        verticalLayout_6->addWidget(lineEditTotalgePress_3);


        gridLayout_9->addLayout(verticalLayout_6, 1, 2, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_9->addItem(horizontalSpacer_3, 1, 3, 1, 1);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        label_17 = new QLabel(groupBox_4);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        verticalLayout_7->addWidget(label_17);

        label_18 = new QLabel(groupBox_4);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        verticalLayout_7->addWidget(label_18);

        label_19 = new QLabel(groupBox_4);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        verticalLayout_7->addWidget(label_19);

        label_20 = new QLabel(groupBox_4);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        verticalLayout_7->addWidget(label_20);

        label_21 = new QLabel(groupBox_4);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        verticalLayout_7->addWidget(label_21);


        gridLayout_9->addLayout(verticalLayout_7, 1, 1, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_9->addItem(verticalSpacer_3, 2, 2, 1, 1);

        verticalSpacer_4 = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_9->addItem(verticalSpacer_4, 0, 2, 1, 1);


        gridLayout_10->addWidget(groupBox_4, 0, 0, 1, 1);

        tabWidget->addTab(tab_4, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        groupBox_6 = new QGroupBox(tab);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));

        gridLayout_2->addWidget(groupBox_6, 0, 3, 1, 1);

        groupBox_5 = new QGroupBox(tab);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setMaximumSize(QSize(222, 9999));

        gridLayout_2->addWidget(groupBox_5, 0, 0, 1, 1);

        verticalLayout_heatmap = new QVBoxLayout();
        verticalLayout_heatmap->setObjectName(QString::fromUtf8("verticalLayout_heatmap"));

        gridLayout_2->addLayout(verticalLayout_heatmap, 0, 1, 1, 2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_3 = new QGridLayout(tab_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        groupBox_7 = new QGroupBox(tab_2);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));

        gridLayout_3->addWidget(groupBox_7, 0, 2, 1, 1);

        groupBox_9 = new QGroupBox(tab_2);
        groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
        groupBox_9->setMaximumSize(QSize(11111, 200));

        gridLayout_3->addWidget(groupBox_9, 1, 0, 1, 3);

        groupBox_10 = new QGroupBox(tab_2);
        groupBox_10->setObjectName(QString::fromUtf8("groupBox_10"));
        groupBox_10->setMinimumSize(QSize(400, 0));

        gridLayout_3->addWidget(groupBox_10, 0, 1, 1, 1);

        groupBox_8 = new QGroupBox(tab_2);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));

        gridLayout_3->addWidget(groupBox_8, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        gridLayout_4 = new QGridLayout(tab_3);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        groupBox_2 = new QGroupBox(tab_3);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_8 = new QGridLayout(groupBox_2);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lineEditMaxPressPoint = new QLineEdit(groupBox_2);
        lineEditMaxPressPoint->setObjectName(QString::fromUtf8("lineEditMaxPressPoint"));
        lineEditMaxPressPoint->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_2->addWidget(lineEditMaxPressPoint);

        lineEditMaxPressValue = new QLineEdit(groupBox_2);
        lineEditMaxPressValue->setObjectName(QString::fromUtf8("lineEditMaxPressValue"));
        lineEditMaxPressValue->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_2->addWidget(lineEditMaxPressValue);


        verticalLayout_4->addLayout(horizontalLayout_2);

        lineEditAveragePress = new QLineEdit(groupBox_2);
        lineEditAveragePress->setObjectName(QString::fromUtf8("lineEditAveragePress"));
        lineEditAveragePress->setMaximumSize(QSize(150, 16777215));

        verticalLayout_4->addWidget(lineEditAveragePress);

        lineEditArea = new QLineEdit(groupBox_2);
        lineEditArea->setObjectName(QString::fromUtf8("lineEditArea"));
        lineEditArea->setMaximumSize(QSize(150, 16777215));

        verticalLayout_4->addWidget(lineEditArea);

        lineEditPressureCenterTrajectory = new QLineEdit(groupBox_2);
        lineEditPressureCenterTrajectory->setObjectName(QString::fromUtf8("lineEditPressureCenterTrajectory"));
        lineEditPressureCenterTrajectory->setMaximumSize(QSize(150, 16777215));

        verticalLayout_4->addWidget(lineEditPressureCenterTrajectory);

        lineEditTotalgePress = new QLineEdit(groupBox_2);
        lineEditTotalgePress->setObjectName(QString::fromUtf8("lineEditTotalgePress"));
        lineEditTotalgePress->setMaximumSize(QSize(150, 16777215));

        verticalLayout_4->addWidget(lineEditTotalgePress);


        gridLayout_8->addLayout(verticalLayout_4, 0, 1, 1, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        verticalLayout_3->addWidget(label_11);

        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        verticalLayout_3->addWidget(label_12);

        label_13 = new QLabel(groupBox_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        verticalLayout_3->addWidget(label_13);

        label_14 = new QLabel(groupBox_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        verticalLayout_3->addWidget(label_14);

        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_3->addWidget(label);


        gridLayout_8->addLayout(verticalLayout_3, 0, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_2, 0, 0, 1, 1);

        groupBox_3 = new QGroupBox(tab_3);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_5 = new QGridLayout(groupBox_3);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        label_4 = new QLabel(groupBox_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_9->addWidget(label_4);

        lineEditCalibration1 = new QLineEdit(groupBox_3);
        lineEditCalibration1->setObjectName(QString::fromUtf8("lineEditCalibration1"));
        lineEditCalibration1->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_9->addWidget(lineEditCalibration1);

        pushButtonCalibration1 = new QPushButton(groupBox_3);
        pushButtonCalibration1->setObjectName(QString::fromUtf8("pushButtonCalibration1"));

        horizontalLayout_9->addWidget(pushButtonCalibration1);


        gridLayout_5->addLayout(horizontalLayout_9, 0, 0, 1, 1);

        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        label_6->setFont(font);
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout_5->addWidget(label_6, 3, 0, 1, 1);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_10->addWidget(label_5);

        lineEditCalibration2 = new QLineEdit(groupBox_3);
        lineEditCalibration2->setObjectName(QString::fromUtf8("lineEditCalibration2"));
        lineEditCalibration2->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_10->addWidget(lineEditCalibration2);

        pushButtonCalibration2 = new QPushButton(groupBox_3);
        pushButtonCalibration2->setObjectName(QString::fromUtf8("pushButtonCalibration2"));

        horizontalLayout_10->addWidget(pushButtonCalibration2);


        gridLayout_5->addLayout(horizontalLayout_10, 2, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_2, 4, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_3, 1, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer, 0, 1, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        formLayout = new QFormLayout(tab_5);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        groupBox = new QGroupBox(tab_5);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        labelCom7 = new QLabel(groupBox);
        labelCom7->setObjectName(QString::fromUtf8("labelCom7"));
        QFont font1;
        font1.setBold(false);
        font1.setWeight(50);
        labelCom7->setFont(font1);
        labelCom7->setLayoutDirection(Qt::LeftToRight);
        labelCom7->setWordWrap(false);

        gridLayout->addWidget(labelCom7, 7, 0, 1, 1);

        labelCom1 = new QLabel(groupBox);
        labelCom1->setObjectName(QString::fromUtf8("labelCom1"));

        gridLayout->addWidget(labelCom1, 0, 0, 1, 1);

        label_stepFrequency = new QLabel(groupBox);
        label_stepFrequency->setObjectName(QString::fromUtf8("label_stepFrequency"));

        gridLayout->addWidget(label_stepFrequency, 10, 3, 1, 1);

        spinBox = new QSpinBox(groupBox);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));

        gridLayout->addWidget(spinBox, 8, 3, 1, 1);

        labelCom5 = new QLabel(groupBox);
        labelCom5->setObjectName(QString::fromUtf8("labelCom5"));

        gridLayout->addWidget(labelCom5, 5, 0, 1, 1);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 10, 0, 1, 1);

        btnNextFrame = new QPushButton(groupBox);
        btnNextFrame->setObjectName(QString::fromUtf8("btnNextFrame"));
        btnNextFrame->setEnabled(true);
        btnNextFrame->setMinimumSize(QSize(0, 70));
        btnNextFrame->setCheckable(true);

        gridLayout->addWidget(btnNextFrame, 19, 2, 1, 2);

        pushButtonOpen = new QPushButton(groupBox);
        pushButtonOpen->setObjectName(QString::fromUtf8("pushButtonOpen"));
        pushButtonOpen->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButtonOpen->sizePolicy().hasHeightForWidth());
        pushButtonOpen->setSizePolicy(sizePolicy2);
        pushButtonOpen->setMinimumSize(QSize(0, 70));
        pushButtonOpen->setCheckable(true);

        gridLayout->addWidget(pushButtonOpen, 17, 0, 1, 4);

        labelCom2 = new QLabel(groupBox);
        labelCom2->setObjectName(QString::fromUtf8("labelCom2"));

        gridLayout->addWidget(labelCom2, 2, 0, 1, 1);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 11, 0, 1, 1);

        label_stancetimeL = new QLabel(groupBox);
        label_stancetimeL->setObjectName(QString::fromUtf8("label_stancetimeL"));

        gridLayout->addWidget(label_stancetimeL, 11, 3, 1, 1);

        label_stancetimeR = new QLabel(groupBox);
        label_stancetimeR->setObjectName(QString::fromUtf8("label_stancetimeR"));

        gridLayout->addWidget(label_stancetimeR, 12, 3, 1, 1);

        label_walkspeed = new QLabel(groupBox);
        label_walkspeed->setObjectName(QString::fromUtf8("label_walkspeed"));

        gridLayout->addWidget(label_walkspeed, 14, 3, 1, 1);

        labelCom3 = new QLabel(groupBox);
        labelCom3->setObjectName(QString::fromUtf8("labelCom3"));

        gridLayout->addWidget(labelCom3, 3, 0, 1, 1);

        comboBoxCom7 = new QComboBox(groupBox);
        comboBoxCom7->setObjectName(QString::fromUtf8("comboBoxCom7"));
        comboBoxCom7->setMinimumSize(QSize(0, 25));

        gridLayout->addWidget(comboBoxCom7, 7, 3, 1, 1);

        pushButtonExportData = new QPushButton(groupBox);
        pushButtonExportData->setObjectName(QString::fromUtf8("pushButtonExportData"));
        pushButtonExportData->setEnabled(true);
        sizePolicy2.setHeightForWidth(pushButtonExportData->sizePolicy().hasHeightForWidth());
        pushButtonExportData->setSizePolicy(sizePolicy2);
        pushButtonExportData->setMinimumSize(QSize(0, 70));
        pushButtonExportData->setCheckable(true);

        gridLayout->addWidget(pushButtonExportData, 18, 0, 1, 4);

        label_FPS = new QLabel(groupBox);
        label_FPS->setObjectName(QString::fromUtf8("label_FPS"));

        gridLayout->addWidget(label_FPS, 9, 3, 1, 1);

        checkBox_showChannel = new QCheckBox(groupBox);
        checkBox_showChannel->setObjectName(QString::fromUtf8("checkBox_showChannel"));

        gridLayout->addWidget(checkBox_showChannel, 16, 0, 1, 2);

        comboBoxCom2 = new QComboBox(groupBox);
        comboBoxCom2->setObjectName(QString::fromUtf8("comboBoxCom2"));
        comboBoxCom2->setMinimumSize(QSize(0, 25));

        gridLayout->addWidget(comboBoxCom2, 2, 3, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 8, 0, 1, 2);

        label_15 = new QLabel(groupBox);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout->addWidget(label_15, 15, 0, 1, 1);

        label_10 = new QLabel(groupBox);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 13, 0, 1, 1);

        comboBoxCom6 = new QComboBox(groupBox);
        comboBoxCom6->setObjectName(QString::fromUtf8("comboBoxCom6"));
        comboBoxCom6->setMinimumSize(QSize(0, 25));

        gridLayout->addWidget(comboBoxCom6, 6, 3, 1, 1);

        label_stepwidth = new QLabel(groupBox);
        label_stepwidth->setObjectName(QString::fromUtf8("label_stepwidth"));

        gridLayout->addWidget(label_stepwidth, 15, 3, 1, 1);

        comboBoxCom3 = new QComboBox(groupBox);
        comboBoxCom3->setObjectName(QString::fromUtf8("comboBoxCom3"));
        comboBoxCom3->setMinimumSize(QSize(0, 25));

        gridLayout->addWidget(comboBoxCom3, 3, 3, 1, 1);

        comboBoxCom5 = new QComboBox(groupBox);
        comboBoxCom5->setObjectName(QString::fromUtf8("comboBoxCom5"));
        comboBoxCom5->setMinimumSize(QSize(0, 25));

        gridLayout->addWidget(comboBoxCom5, 5, 3, 1, 1);

        labelCom6 = new QLabel(groupBox);
        labelCom6->setObjectName(QString::fromUtf8("labelCom6"));

        gridLayout->addWidget(labelCom6, 6, 0, 1, 1);

        labelCom4 = new QLabel(groupBox);
        labelCom4->setObjectName(QString::fromUtf8("labelCom4"));

        gridLayout->addWidget(labelCom4, 4, 0, 1, 1);

        comboBoxCom4 = new QComboBox(groupBox);
        comboBoxCom4->setObjectName(QString::fromUtf8("comboBoxCom4"));
        comboBoxCom4->setMinimumSize(QSize(0, 25));

        gridLayout->addWidget(comboBoxCom4, 4, 3, 1, 1);

        btnStartPlay = new QPushButton(groupBox);
        btnStartPlay->setObjectName(QString::fromUtf8("btnStartPlay"));
        btnStartPlay->setMinimumSize(QSize(0, 70));

        gridLayout->addWidget(btnStartPlay, 19, 0, 1, 2);

        comboBoxCom1 = new QComboBox(groupBox);
        comboBoxCom1->setObjectName(QString::fromUtf8("comboBoxCom1"));
        comboBoxCom1->setMinimumSize(QSize(0, 25));

        gridLayout->addWidget(comboBoxCom1, 1, 0, 1, 4);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 9, 0, 1, 1);

        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 12, 0, 1, 1);

        label_16 = new QLabel(groupBox);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout->addWidget(label_16, 14, 0, 1, 1);

        label_steplenth = new QLabel(groupBox);
        label_steplenth->setObjectName(QString::fromUtf8("label_steplenth"));

        gridLayout->addWidget(label_steplenth, 13, 3, 1, 1);


        formLayout->setWidget(0, QFormLayout::LabelRole, groupBox);

        horizontalSpacer_2 = new QSpacerItem(595, 674, QSizePolicy::Preferred, QSizePolicy::Minimum);

        formLayout->setItem(0, QFormLayout::FieldRole, horizontalSpacer_2);

        verticalSpacer = new QSpacerItem(17, 594, QSizePolicy::Minimum, QSizePolicy::Preferred);

        formLayout->setItem(1, QFormLayout::LabelRole, verticalSpacer);

        tabWidget->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        tableWidgetF = new QTableWidget(tab_6);
        tableWidgetF->setObjectName(QString::fromUtf8("tableWidgetF"));
        tableWidgetF->setEnabled(true);
        tableWidgetF->setGeometry(QRect(9, 9, 598, 764));
        sizePolicy.setHeightForWidth(tableWidgetF->sizePolicy().hasHeightForWidth());
        tableWidgetF->setSizePolicy(sizePolicy);
        tableWidgetF->setLayoutDirection(Qt::LeftToRight);
        tabWidget->addTab(tab_6, QString());

        gridLayout_6->addWidget(tabWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1000, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "\347\233\270\345\205\263\346\225\260\346\215\256", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "\350\272\253\351\253\230", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "\344\275\223\351\207\215", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "\350\204\232\347\240\201", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "\345\216\213\345\212\233\344\270\255\345\277\203\350\275\250\350\277\271\357\274\232", nullptr));
        label_21->setText(QCoreApplication::translate("MainWindow", "\346\200\273\345\216\213\345\212\233", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225\350\200\205\346\225\260\346\215\2561", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "right", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "left", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "Map", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        groupBox_9->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        groupBox_10->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        groupBox_8->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "\346\212\245\345\221\212", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\347\233\270\345\205\263\346\225\260\346\215\256", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\346\234\200\345\244\247\345\216\213\345\212\233\347\202\271\357\274\232", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "\345\271\263\345\235\207\345\216\213\345\212\233\357\274\232", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "\346\216\245\350\247\246\351\235\242\347\247\257\357\274\232", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "\345\216\213\345\212\233\344\270\255\345\277\203\350\275\250\350\277\271\357\274\232", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\346\200\273\345\216\213\345\212\233", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "\346\240\241\345\207\206", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Value1:", nullptr));
        pushButtonCalibration1->setText(QCoreApplication::translate("MainWindow", "Calibration", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", " \345\244\207\346\263\250\357\274\232\344\275\277\347\224\250\347\254\2546\350\241\214, \347\254\2546\345\210\227\347\232\204\347\202\271\350\277\233\350\241\214\346\240\241\345\207\206\343\200\202\345\241\253\345\206\231\347\232\204\346\240\241\345\207\206\345\200\274\345\215\225\344\275\215\347\224\250 N", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Value2:", nullptr));
        pushButtonCalibration2->setText(QCoreApplication::translate("MainWindow", "Calibration", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MainWindow", "Index", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "ComConfig", nullptr));
        labelCom7->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\346\265\201\346\216\247", nullptr));
        labelCom1->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\351\200\211\346\213\251", nullptr));
        label_stepFrequency->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        labelCom5->setText(QCoreApplication::translate("MainWindow", "\345\245\207\345\201\266\346\240\241\351\252\214", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\346\255\245\351\242\221", nullptr));
        btnNextFrame->setText(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\200\345\270\247", nullptr));
        pushButtonOpen->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        labelCom2->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\345\267\246\350\204\232\346\224\257\346\222\221\346\227\266\351\227\264", nullptr));
        label_stancetimeL->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_stancetimeR->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        label_walkspeed->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        labelCom3->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\344\275\215", nullptr));
        pushButtonExportData->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272\346\225\260\346\215\256", nullptr));
        label_FPS->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        checkBox_showChannel->setText(QCoreApplication::translate("MainWindow", "\351\200\232\351\201\223\346\230\276\347\244\272", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\346\234\200\345\244\247\350\211\262\346\235\241\345\200\274:", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "\346\255\245\345\256\275", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\346\255\245\351\225\277", nullptr));
        label_stepwidth->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        labelCom6->setText(QCoreApplication::translate("MainWindow", "\347\274\226\347\240\201\346\240\274\345\274\217", nullptr));
        labelCom4->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\344\275\215", nullptr));
        btnStartPlay->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\346\222\255\346\224\276", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\345\270\247\347\216\207", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\345\217\263\350\204\232\346\224\257\346\222\221\346\227\266\351\227\264", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "\346\255\245\350\241\214\351\200\237\345\272\246", nullptr));
        label_steplenth->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QCoreApplication::translate("MainWindow", "Config", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QCoreApplication::translate("MainWindow", "\350\241\250\346\240\274", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
