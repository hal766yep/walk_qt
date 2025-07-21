#include "vserialcom.h"

vSerialCom::vSerialCom(QObject *parent) : QObject(parent)
{
    /*配置共享类的地址*/
    this->vQObjectRxCtr.vSerialAddrSet(&this->vSerial);
    this->vQObjectTxCtr.vSerialAddrSet(&this->vSerial);
    this->vMapCtr.vSerialAddrSet(&this->vSerial);
    /***********************基础串口类***********************/
    this->vSerial.setObjectName("vSerial");
//    this->vComThread.moveToThread(&this->vComThread);
    this->vSerial.moveToThread(&this->vComThread);
//    this->vSerial.qSerial->moveToThread(&this->vComThread);
    /***********************发送相关***********************/
    this->vQObjectTxCtr.setObjectName("vSerialTx");
//    this->vTxThread.moveToThread(&this->vTxThread);
    this->vQObjectTxCtr.moveToThread(&this->vTxThread);
    /***********************接收相关***********************/
    this->vQObjectRxCtr.setObjectName("vSerialRx");
//    this->vRxThread.moveToThread(&this->vRxThread);
    this->vQObjectRxCtr.moveToThread(&this->vRxThread);
    /***********************热力图相关***********************/
    this->vMapCtr.setObjectName("vHeatMap");
//    this->vRxThread.moveToThread(&this->vRxThread);
    this->vMapCtr.moveToThread(&this->vMapThread);

    //开启线程
    this->vComThread.start();
    this->vTxThread.start();
    this->vRxThread.start();
    this->vMapThread.start();

}
vSerialCom::~vSerialCom(void)
{
    this->vTxThread.exit();
    this->vTxThread.wait();
    this->vRxThread.exit();
    this->vRxThread.wait();
    this->vComThread.exit();
    this->vComThread.wait();
    this->vMapThread.exit();
    this->vMapThread.wait();

}
