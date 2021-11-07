/*
 Copyright © 2021  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.
    
    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

//#pragma once
#include <QProcess>
#include <QDebug>
#include "mainwindow.h"

#define bufferSize 2048

const ushort MainWindow::BLCreative[64]={0,0,1,1,0,0,0,0,3,0,4,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const ushort MainWindow::BLCheaper[64]={0,0,0,0,1,0,5,2,3,0,4,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const ushort MainWindow::BLBetter[64]={0,1,1,0,0,1,0,2,0,0,3,2,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0};
const ushort MainWindow::BLGlowing[64]={0,1,2,0,0,2,4,2,0,0,3,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,1,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0};

const QString MainWindow::selfVersion="v3.6.0";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug("成功setupUi");
    Collected=false;

    Kernel=new TokiSlopeCraft(this);
    qDebug("成功创建内核");
    connect(Kernel,&TokiSlopeCraft::keepAwake,this,
            &MainWindow::keepAwake);
    connect(Kernel,&TokiSlopeCraft::progressRangeSet,
            this,&MainWindow::progressRangeSet);
    connect(Kernel,&TokiSlopeCraft::progressAdd,
            this,&MainWindow::progressAdd);
    connect(Kernel,&TokiSlopeCraft::algoProgressRangeSet,
            this,&MainWindow::algoProgressRangeSet);
    connect(Kernel,&TokiSlopeCraft::algoProgressAdd,
            this,&MainWindow::algoProgressAdd);
    connect(Kernel,&TokiSlopeCraft::reportError,
            this,&MainWindow::showError);
    connect(Kernel,&TokiSlopeCraft::reportWorkingStatue,
            this,&MainWindow::showWorkingStatue);
    proTracker=nullptr;

    ProductDir="";

    Manager=new BlockListManager(
                (QHBoxLayout*)ui->scrollAreaWidgetContents->layout());

    qDebug("成功创建方块列表管理者");
    connect(Manager,&BlockListManager::switchToCustom,
            this,&MainWindow::ChangeToCustom);
    //connect(Kernel,SIGNAL(convertProgressSetRange(int,int,int)));

    ui->maxHeight->setValue(255);

    transSubWind=nullptr;

        connect(ui->progressStart,&QRadioButton::clicked,this,&MainWindow::turnToPage0);
        connect(ui->progressImPic,&QRadioButton::clicked,this,&MainWindow::turnToPage1);
        connect(ui->progressType,&QRadioButton::clicked,this,&MainWindow::turnToPage2);
        connect(ui->progressBL,&QRadioButton::clicked,this,&MainWindow::turnToPage3);
        connect(ui->progressAdjPic,&QRadioButton::clicked,this,&MainWindow::turnToPage4);
        connect(ui->progressExLite,&QRadioButton::clicked,this,&MainWindow::turnToPage5);
        connect(ui->progressExStructure,&QRadioButton::clicked,this,&MainWindow::turnToPage5);
        //connect(ui->menuExMcF,&QAction::trigger,this,&MainWindow::turnToPage6);
        connect(ui->progressExData,&QRadioButton::clicked,this,&MainWindow::turnToPage7);
        connect(ui->progressAbout,&QRadioButton::clicked,this,&MainWindow::turnToPage8);
        connect(ui->actionAboutSlopeCraft,&QAction::triggered,this,&MainWindow::turnToPage8);
        connect(ui->actionChinese,&QAction::triggered,this,&MainWindow::turnCh);
        connect(ui->actionEnglish,&QAction::triggered,this,&MainWindow::turnEn);
        connect(ui->progressChinese,&QRadioButton::clicked,this,&MainWindow::turnCh);
        connect(ui->progressEnglish,&QRadioButton::clicked,this,&MainWindow::turnEn);
        connect(ui->actionToki,&QAction::triggered,this,&MainWindow::contactG);
        connect(ui->actionDoki,&QAction::triggered,this,&MainWindow::contactB);
        connect(ui->progressG,&QRadioButton::clicked,this,&MainWindow::contactG);
        connect(ui->progressB,&QRadioButton::clicked,this,&MainWindow::contactB);

        connect(ui->contact,&QPushButton::clicked,this,&MainWindow::contactB);
        connect(ui->contact,&QPushButton::clicked,this,&MainWindow::contactG);
        connect(ui->actionReportBugs,&QAction::triggered,
                this,&MainWindow::on_reportBugs_clicked);
        connect(ui->actionCheckUpdates,&QAction::triggered,
                this,&MainWindow::checkVersion);

    qDebug("成功connect所有的菜单");


        connect(ui->NextPage,&QRadioButton::clicked,
                this,&MainWindow::turnToPage2);
        connect(ui->NextPage2,&QRadioButton::clicked,
                this,&MainWindow::turnToPage3);
        connect(ui->NextPage3,&QRadioButton::clicked,
                this,&MainWindow::turnToPage4);
        connect(ui->ExLite,&QRadioButton::clicked,
                this,&MainWindow::turnToPage5);
        //connect(ui->ExMcF,&QRadioButton::clicked,this,&MainWindow::turnToPage6);
        connect(ui->ExData,&QRadioButton::clicked,
                this,&MainWindow::turnToPage7);
        connect(ui->FinishExLite,&QRadioButton::clicked,
                this,&MainWindow::turnToPage8);
        connect(ui->FinshExData,&QRadioButton::clicked,
                this,&MainWindow::turnToPage8);
        connect(ui->Exit,&QRadioButton::clicked,
                this,&MainWindow::close);
    qDebug("成功connect所有的翻页按钮");

    connect(ui->isGame12,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame13,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame14,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame15,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame16,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame17,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);

    connect(ui->isMapCreative,&QRadioButton::clicked,
            this,&MainWindow::onMapTypeClicked);
    connect(ui->isMapSurvival,&QRadioButton::clicked,
            this,&MainWindow::onMapTypeClicked);
    connect(ui->isMapWall,&QRadioButton::clicked,
            this,&MainWindow::onMapTypeClicked);
    connect(ui->isMapFlat,&QRadioButton::clicked,
            this,&MainWindow::onMapTypeClicked);

    connect(ui->isBLCreative,&QRadioButton::clicked,
            this,&MainWindow::onPresetsClicked);
    connect(ui->isBLSurvivalCheaper,&QRadioButton::clicked,
            this,&MainWindow::onPresetsClicked);
    connect(ui->isBLSurvivalBetter,&QRadioButton::clicked,
            this,&MainWindow::onPresetsClicked);
    connect(ui->isBLGlowing,&QRadioButton::clicked,
            this,&MainWindow::onPresetsClicked);

    connect(ui->isColorSpaceRGBOld,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceRGB,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceHSV,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceLab94,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceLab00,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceXYZ,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->AllowDither,&QCheckBox::clicked,
            this,&MainWindow::onAlgoClicked);

    turnToPage(0);

}

MainWindow::~MainWindow()
{
    delete Kernel;
    delete Manager;
    delete ui;
}

void MainWindow::showPreview()
{
    if(Kernel->queryStep()<Kernel->builded)return;

    PreviewWind*preWind=new PreviewWind(this);
    preWind->Src.resize(62);
    preWind->BlockCount.resize(62);

    preWind->Src=Manager->getQRadioButtonList();

    Kernel->getBlockCounts(preWind->BlockCount);

    qDebug()<<"去重前有："<<preWind->Src.size()<<"个元素";
    auto iS=preWind->Src.begin();
    for(auto ib=preWind->BlockCount.begin();ib!=preWind->BlockCount.end();)
    {
        if(*ib>0)
        {
            ib++;
            iS++;
            continue;
        }
        ib=preWind->BlockCount.erase(ib);
        iS=preWind->Src.erase(iS);
    }
    Kernel->get3DSize(preWind->size[0],preWind->size[1],preWind->size[2]);

    qDebug()<<"去重后有："<<preWind->Src.size()<<"个元素";
    //preWind->Water=Blocks[12][0];
    preWind->Water=Manager->getQRadioButtonList()[12];
    //preWind->Src[1]=Blocks[1][0];preWind->BlockCount[1]=1919810;

    QImage temp=EImage2QImage(Kernel->getConovertedImage());
    preWind->ShowMaterialList();
    preWind->showConvertedImage(temp);
    preWind->show();
}

void MainWindow::keepAwake() {
    QCoreApplication::processEvents();
}

QByteArray MainWindow::parseColormap(QString FilePath,
                                     const QString & rawName,
                                     const char * pattern) {
    QByteArray dst;
    QString title,text;
        while(true) {
            qDebug("225");
            QFile temp(FilePath);
            if(temp.exists()) {
                temp.open(QFile::OpenModeFlag::ReadOnly);
                dst=temp.readAll();

                if(QCryptographicHash::hash(
                            dst,QCryptographicHash::Algorithm::Md5).toHex()
                    ==pattern) {
                    break;
                } else {
                    //如果文件存在但校验失败
                    title=tr("颜色表文件")+rawName+tr("被篡改");
                    text=tr("这是程序运行所必须的文件，且绝对不允许篡改，请重新下载最新版的SlopeCraft，或者重新寻找它。");
                }
            } else {
                //如果文件不存在
                title=tr("颜色表文件")+rawName+tr("不存在");
                text=tr("这是程序运行所必须的文件，请重新寻找");
            }

            int userChoice=QMessageBox::critical(this,title,text,QMessageBox::StandardButton::Retry,
                                  QMessageBox::StandardButton::No);
            if(userChoice==QMessageBox::StandardButton::Retry) {
                FilePath=QFileDialog::getOpenFileName(this,
                                            tr("颜色表文件")+rawName+tr("不存在或被篡改，请手动寻找")
                                            ,"./Colors",rawName);
                if(FilePath.isEmpty()) {
                    qDebug("252");
                    exit(0);
                    return QByteArray();
                } else {
                    qDebug("255");
                    continue;
                }
            } else {
                qDebug("259");
                exit(0);
                return QByteArray();
            }
        }
    return dst;
}

void MainWindow::loadColormap() {

    QByteArray R,H,L,X;
    R=parseColormap("./Colors/RGB.TokiColor","RGB.TokiColor",
                    "ba56d5af2ba89d9ba3362a72778e1624");
    /*
    while(true) {
        QFile temp(ColorFilePath);
        if(temp.exists()) {
            temp.open(QFile::OpenModeFlag::ReadOnly);
            R=temp.readAll();
            if(QCryptographicHash::hash(R,QCryptographicHash::Algorithm::Md5).toHex()
                    =="ba56d5af2ba89d9ba3362a72778e1624") {
                break;
            }
        }
        qDebug("未找到颜色文件RGB.TokiColor");



        ColorFilePath=QFileDialog::getOpenFileName(this,
                            tr("颜色表文件")+"RGB.TokiColor"+tr("不存在或被篡改，请手动寻找")
                            ,"./Colors","RGB.TokiColor");
        temp.close();
    }*/

    H=parseColormap("./Colors/HSV.TokiColor","HSV.TokiColor",
                    "db47a74d0b32fa682d1256cce60bf574");
    /*
    while(true) {
        QFile temp(ColorFilePath);
        if(temp.exists()) {
            temp.open(QFile::OpenModeFlag::ReadOnly);
            H=temp.readAll();
            if(QCryptographicHash::hash(H,QCryptographicHash::Algorithm::Md5).toHex()
                    =="db47a74d0b32fa682d1256cce60bf574") {
                break;
            }
        }
        qDebug("未找到颜色文件HSV.TokiColor");
        ColorFilePath=QFileDialog::getOpenFileName(this,
                            tr("颜色表文件")+"HSV.TokiColor"+tr("不存在或被篡改，请手动寻找")
                            ,"./Colors","HSV.TokiColor");
        temp.close();
    }*/

    L=parseColormap("./Colors/Lab.TokiColor","Lab.TokiColor",
                    "2aec9d79b920745472c0ccf56cbb7669");
    /*
    while(true) {
        QFile temp(ColorFilePath);
        if(temp.exists()) {
            temp.open(QFile::OpenModeFlag::ReadOnly);
            L=temp.readAll();
            if(QCryptographicHash::hash(L,QCryptographicHash::Algorithm::Md5).toHex()
                    =="2aec9d79b920745472c0ccf56cbb7669") {
                break;
            }
        }
        qDebug("未找到颜色文件Lab.TokiColor");
        ColorFilePath=QFileDialog::getOpenFileName(this,
                            tr("颜色表文件")+"Lab.TokiColor"+tr("不存在或被篡改，请手动寻找")
                            ,"./Colors","Lab.TokiColor");
        temp.close();
    }
    */

    X=parseColormap("./Colors/XYZ.TokiColor","XYZ.TokiColor",
                    "6551171faf62961e3ae6bc3c2ee8d051");
    /*
    ColorFilePath=;
    while(true) {
        QFile temp(ColorFilePath);
        if(temp.exists()) {
            temp.open(QFile::OpenModeFlag::ReadOnly);
            X=temp.readAll();
            if(QCryptographicHash::hash(X,QCryptographicHash::Algorithm::Md5).toHex()
                    ==) {
                break;
            }
        }
        qDebug("未找到颜色文件XYZ.TokiColor");
        ColorFilePath=QFileDialog::getOpenFileName(this,
                            tr("颜色表文件")++tr("不存在或被篡改，请手动寻找")
                            ,"./Colors","XYZ.TokiColor");
        temp.close();
    }*/

    if(Kernel->setColorSet(R.data(),H.data(),L.data(),X.data()))
        qDebug("成功载入颜色");
    else
        qDebug("载入颜色失败");
}


QJsonArray MainWindow::getFixedBlocksList(QString Path) {
    QJsonDocument jd;
    while(true) {
        QFile temp(Path);
        if(temp.exists()) {
            QJsonParseError error;
            temp.open(QIODevice::ReadOnly | QIODevice::Text);
            jd=QJsonDocument::fromJson(temp.readAll(),&error);

            if(error.error==QJsonParseError::NoError&&jd.object().value("FixedBlocks").isArray()) {
                return jd.object().value("FixedBlocks").toArray();
            }
            else {

                QString errorInfo=(error.error!=QJsonParseError::NoError)?
                            error.errorString():
                            "Json file doesn't contain array named \"FixedBlocks\"";
                int userChoice=QMessageBox::critical(this,tr("错误：默认方块列表的JSON格式有错"),
                                      tr("JSON错误原因：")+errorInfo+tr("\n默认方块列表记录了SlopeCraft最基础的常用方块，是程序运行必须的。\n请点击Yes手动寻找它，或者点击No退出程序，重新下载最新版SlopeCraft/修复错误后再启动程序。"),
                                      QMessageBox::StandardButton::Yes,
                                                     QMessageBox::StandardButton::No);
                if(userChoice==QMessageBox::StandardButton::Yes) {
                    Path=QFileDialog::getOpenFileName(this,
                                                      "重新寻找默认方块列表文件FixedBlocks.json",
                                                      "./",
                                                      "FixedBlocks.json");
                    if(Path.isEmpty()) {
                        exit(0);
                    }
                    else {
                        continue;
                    }
                }
                else {
                    exit(0);
                }
            }

        }
        else {
            int userChoice=QMessageBox::critical(this,tr("错误：找不到默认方块列表"),
                                  tr("默认方块列表记录了SlopeCraft最基础的常用方块，是程序运行必须的。\n请点击Yes手动寻找它，或者点击No退出程序，重新下载最新版SlopeCraft后再启动程序。"),
                                  QMessageBox::StandardButton::Yes,
                                                 QMessageBox::StandardButton::No);
            if(userChoice==QMessageBox::StandardButton::Yes) {
                Path=QFileDialog::getOpenFileName(this,
                                                  "重新寻找默认方块列表文件FixedBlocks.json",
                                                  "./",
                                                  "FixedBlocks.json");
                if(Path.isEmpty()) {
                    exit(0);
                }
                else {
                    continue;
                }
            }
            else {
                exit(0);
            }
        }
    }
return QJsonArray();
}

QString MainWindow::getFixedBlockListDir(QString Dir) {
    while(true) {
        if(QDir(Dir).exists()) {
            return Dir;
        }
        else {
            QMessageBox::StandardButton userChoice=
            QMessageBox::warning(this,
                                 tr("错误：存放默认方块图标的文件夹FixedBlocks不存在"),
                                 tr("FixedBlocks文件夹存储了默认方块列表所有方块对应的图片。\n这个错误不致命，可以忽略，但最好重新下载SlopeCraft。\n点击Retry重新寻找这个文件夹，点击Ignore忽略这个错误，点击Close退出程序"),
                                 {QMessageBox::StandardButton::Retry,
                                  QMessageBox::StandardButton::Ignore,
                                   QMessageBox::StandardButton::Close},

                                 QMessageBox::StandardButton::Ignore);
            switch (userChoice) {
            case QMessageBox::StandardButton::Retry:
                Dir=QFileDialog::getExistingDirectory(this,
                                                         tr("重新寻找默认方块图标文件夹FixedBlocks"),
                                                      "./",
                                                      QFileDialog::Option::ReadOnly);
                if(!Dir.isEmpty()) {
                    continue;
                }
                return "./";
            case QMessageBox::StandardButton::Ignore:
                return "./";
            default:
                exit(0);
            }
        }
    }
    return Dir;
}

QJsonArray MainWindow::getCustomBlockList(QString Path) {
QJsonDocument jd;
while(true) {
    QFile temp(Path);
    if(temp.exists()) {
        temp.open(QIODevice::ReadOnly | QIODevice::Text);
        QJsonParseError error;
        jd=QJsonDocument::fromJson(temp.readAll(),&error);
        if(error.error==QJsonParseError::NoError&&
                jd.object().value("CustomBlocks").isArray()) {
            return jd.object().value("CustomBlocks").toArray();
        }
        else {
            QString errorInfo=(error.error!=QJsonParseError::NoError)?
                        error.errorString():
                        "Json file doesn't contain array named \"CustomBlocks\"";
            QMessageBox::StandardButton userChoice=
                    QMessageBox::warning(this,tr("错误：自定义方块列表的JSON格式有错"),
                                 tr("JSON错误原因：")+errorInfo
                                         +tr("\n自定义方块列表记录了SlopeCraft额外添加的可选方块，不是程序运行必须的。\n请点击Yes手动寻找它，或点击Ignore忽略这个错误，或者点击Close退出程序，重新下载最新版SlopeCraft/修复错误后再启动程序。"),
                                                {
                                                    QMessageBox::StandardButton::Yes,
                                                    QMessageBox::StandardButton::Ignore,
                                                    QMessageBox::StandardButton::Close
                                                },
                                         QMessageBox::StandardButton::Ignore);
            switch (userChoice) {
            case QMessageBox::StandardButton::Yes:
                Path=QFileDialog::getOpenFileName(this,
                                                  "重新寻找自定义方块列表文件CustomBlocks.json",
                                                  "./",
                                                  "CustomBlocks.json");
                if(Path.isEmpty()) {
                    QJsonArray a;
                    while(!a.empty())
                        a.removeFirst();
                    return a;
                }
                else {
                    continue;
                }
                break;
            case QMessageBox::StandardButton::Close:
                exit(0);
                break;
            default:
                QJsonArray a;
                while(!a.empty())
                    a.removeFirst();
                return a;
            }
        }
    }
    else {
        QMessageBox::StandardButton userChoice=QMessageBox::warning(this,tr("错误：自定义方块列表文件CustomBlocks.json不存在"),
                             tr("自定义方块列表记录了SlopeCraft额外添加的可选方块，不是程序运行必须的。\n请点击Yes手动寻找它，或点击Ignore忽略这个错误，或者点击Close退出程序，重新下载最新版SlopeCraft/修复错误后再启动程序。"),
                                            {QMessageBox::StandardButton::Yes,
                                             QMessageBox::StandardButton::Ignore,
                                             QMessageBox::StandardButton::Close},
                                       QMessageBox::StandardButton::Ignore
                             );
        switch (userChoice) {
        case QMessageBox::StandardButton::Yes:
            Path=QFileDialog::getOpenFileName(this,
                                              "重新寻找自定义方块列表文件CustomBlocks.json",
                                              "./",
                                              "CustomBlocks.json");
            if(Path.isEmpty()) {
                QJsonArray a;
                while(!a.empty())
                    a.removeFirst();
                return a;
            }
            else {
                continue;
            }
            break;
        case QMessageBox::StandardButton::Close:
            exit(0);
            break;
        default:
            QJsonArray a;
            while(!a.empty())
                a.removeFirst();
            return a;
        }
    }
}
return QJsonArray();
}

QString MainWindow::getCustomBlockListDir(QString Dir) {
    while(true) {
        if(QDir(Dir).exists()) {
            return Dir;
        }
        else {
            QMessageBox::StandardButton userChoice=
            QMessageBox::warning(this,
                                 tr("错误：存放自定义方块图标的文件夹CustomBlocks不存在"),
                                 tr("CustomBlocks文件夹存储了用户自定义方块列表所有方块对应的图片。\n这个错误不致命，可以忽略，但最好重新下载SlopeCraft。\n点击Retry重新寻找这个文件夹，点击Ignore忽略这个错误，点击Close退出程序"),
                                 {QMessageBox::StandardButton::Retry,
                                 QMessageBox::StandardButton::Ignore,
                                 QMessageBox::StandardButton::Close},
                                 QMessageBox::StandardButton::Ignore);
            switch (userChoice) {
            case QMessageBox::StandardButton::Retry:
                Dir=QFileDialog::getExistingDirectory(this,
                                                         tr("重新寻找自定义方块图标文件夹CustomBlocks"),
                                                      "./",
                                                      QFileDialog::Option::ReadOnly);
                if(!Dir.isEmpty()) {
                    continue;
                }
                return "./";
            case QMessageBox::StandardButton::Ignore:
                return "./";
            default:
                exit(0);
            }
        }
    }
    return Dir;
}

void MainWindow::loadBlockList() {

    QJsonArray ja=getFixedBlocksList("./Blocks/FixedBlocks.json");

/*
    while(!QFile(Path).exists()) {
        qDebug()<<"错误！找不到固定的方块列表文件"<<Path;
        int userChoice=QMessageBox::critical(this,
                                             tr("找不到默认方块列表"),
                                             tr("默认方块列表记录了SlopeCraft最基础的常用方块，是程序运行必须的。\n请手动寻找它。"),
                                             QMessageBox::StandardButton::Ok,
                              QMessageBox::StandardButton::No);
        if(userChoice==QMessageBox::StandardButton::Ok) {
            Path=QFileDialog::getOpenFileName(this,
                                              "找不到固定方块列表文件，请手动寻找",
                                              "./",
                                              "FixedBlocks.json");
        } else {
            exit(0);
        }

    }
    */

    QString Dir=getFixedBlockListDir("./Blocks/FixedBlocks");

/*
    while(!QDir(Dir).exists()) {
        qDebug()<<"错误！固定方块列表的图标路径"<<Dir<<"无效";
        Dir=QFileDialog::getExistingDirectory(this,
                                                 "找不到存放固定方块列表图片的文件夹，请手动寻找",
                                              "./",
                                              QFileDialog::Option::ReadOnly);

    }
    QJsonDocument jd;
    QJsonParseError error;
    QFile temp(Path);
    temp.open(QIODevice::ReadOnly | QIODevice::Text);
    jd=QJsonDocument::fromJson(temp.readAll(),&error);
    if(error.error!=QJsonParseError::NoError) {
        qDebug()<<"解析固定方块列表时出错："<<error.errorString();
        return;
    }
*/

    qDebug()<<ja.size();

    Manager->addBlocks(ja,Dir);



//开始解析用户自定义的方块列表
    /*
    while(!QFile(Path).exists()) {
        int choice=QMessageBox::question(this,
                              "找不到自定义方块列表文件CustomBlocks.json",
                              "这会导致你自定义的方块无法被加载。\n你可以手动寻找它（点确认），也可以忽略这个错误（点取消）。",
                              QMessageBox::Ok|QMessageBox::Ignore);
        if(choice==QMessageBox::Ok) {
            Path=QFileDialog::getOpenFileName(this,
                                              "找不到自定义列表文件，请手动寻找",
                                              "./",
                                              "FixedBlocks.json");
        } else
            return;
    }

    while(!QDir(Dir).exists()) {
        int choice=QMessageBox::question(this,
                              "找不到存放自定义方块图片的文件夹CustomBlocks",
                              "这会导致你自定义的方块图标无法被加载。\n你可以手动寻找它（点确认），也可以忽略这个错误（点取消）。",
                              QMessageBox::Ok|QMessageBox::Ignore);
        if(choice==QMessageBox::Ok) {
            Dir=QFileDialog::getExistingDirectory(this,
                                                     "找不到存放固定方块列表图片的文件夹，请手动寻找",
                                                  "./",
                                                  QFileDialog::Option::ReadOnly);
        } else
            break;
    }
    QFile temp2(Path);
    temp2.open(QFile::OpenModeFlag::ReadOnly);
    jd=QJsonDocument::fromJson(temp2.readAll(),&error);

    while(error.error!=QJsonParseError::NoError) {
        qDebug()<<"自定义方块列表json格式错误："<<error.errorString();
        QMessageBox::warning(this,
                              "解析自定义方块列表json时出错",
                              error.errorString()+"\n这是因为json格式错误。\n你自定义的方块无法被加载。",
                              QMessageBox::Abort);
        return;
    }*/
    ja=getCustomBlockList("./Blocks/CustomBlocks.json");

    Dir=getCustomBlockListDir("./Blocks/CustomBlocks");

    Manager->addBlocks(ja,Dir);

    if(Kernel->queryStep()>=TokiSlopeCraft::colorSetReady) {
        QRgb colors[64];
        Kernel->getARGB32(colors);
        Manager->setLabelColors(colors);
    }
    //applyPre(BLBetter);
    Manager->applyPreset(BLBetter);
}

void MainWindow::InitializeAll()
{
    ui->LeftScroll->verticalScrollBar()->setStyleSheet("QScrollBar{width: 7px;margin: 0px 0 0px 0;background-color: rgba(255, 255, 255, 64);color: rgba(255, 255, 255, 128);}");
    static bool needInitialize=true;
    if(needInitialize)
    {
        needInitialize=false;
#ifdef dispDerivative
    //checkBlockIds();
    makeImage(1);
#endif
    }
    if(!Collected)
    {
        loadColormap();
        qDebug("颜色表加载完毕");
        loadBlockList();
        qDebug("方块列表加载完毕");
        Manager->setVersion(TokiSlopeCraft::MC17);
        onPresetsClicked();
        Collected=true;
    }

}

void MainWindow::contactG()
{
    static std::string Toki=Kernel->getAuthorURL()[1];
    QDesktopServices::openUrl(QUrl(QString::fromStdString(Toki)));
}

void MainWindow::contactB()
{
    static std::string Toki=Kernel->getAuthorURL()[0];
    QDesktopServices::openUrl(QUrl(QString::fromStdString(Toki)));
}

#ifndef tpSDestroyer
#define tpSDestroyer
tpS::~tpS()
{

}
#endif

void MainWindow::turnToPage(int page)
{
    page%=9;
    QString newtitle="SlopeCraft v3.6 Copyright © 2021 TokiNoBug    ";
    switch (page)
    {
        case 0:
                newtitle+="Step 0 / 6";
                newtitle+="    ";
                newtitle+=tr("开始");
                ui->stackedWidget->setCurrentIndex(page);
            break;

    case 1:
            newtitle+="Step 1 / 6";
            newtitle+="    ";
            newtitle+=tr("导入图片");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 2:
            newtitle+="Step 2 / 6";
            newtitle+="    ";
            newtitle+=tr("设置地图画类型");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 3:
            newtitle+="Step 3 / 6";
            newtitle+="    ";
            newtitle+=tr("设置方块列表");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 4:
            newtitle+="Step 4 / 6";
            newtitle+="    ";
            newtitle+=tr("调整颜色");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 5:
            newtitle+="Step 5 / 6";
            newtitle+="    ";
            newtitle+=tr("导出为投影文件");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 6:
            newtitle+="Step 5 / 6";
            newtitle+="    ";
            newtitle+=tr("导出为mcfunction");
            ui->stackedWidget->setCurrentIndex(page);
       break;
    case 7:
            newtitle+="Step 5 / 6";
            newtitle+="    ";
            newtitle+=tr("导出为地图文件");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 8:
            newtitle+="Step 6 / 6";
            newtitle+="    ";
            newtitle+=tr("结束");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    default:
        qDebug("尝试翻页错误");
        break;
    }
    this->setWindowTitle(newtitle);
    updateEnables();
    return;
}

void MainWindow::turnToPage0() {
    turnToPage(0);
    return;
}

void MainWindow::turnToPage1() {
    turnToPage(1);
    return;
}

void MainWindow::turnToPage2() {
    turnToPage(2);
    return;
}

void MainWindow::turnToPage3() {
    turnToPage(3);
    return;
}

void MainWindow::turnToPage4() {
    turnToPage(4);
    return;
}

void MainWindow::turnToPage5() {
    turnToPage(5);
    return;
}

void MainWindow::turnToPage6() {
    turnToPage(6);
    return;
}

void MainWindow::turnToPage7() {
    turnToPage(7);
    return;
}

void MainWindow::turnToPage8() {
    turnToPage(8);
    return;
}

void MainWindow::updateEnables() {
    bool temp=Kernel->queryStep()>=TokiSlopeCraft::colorSetReady;
    ui->StartWithFlat->setEnabled(temp);
    ui->StartWithNotVanilla->setEnabled(temp);
    ui->StartWithNotVanilla->setEnabled(temp);


    //temp=Kernel->queryStep()>=TokiSlopeCraft::convertionReady;
    ui->Convert->setEnabled(temp);

    temp=Kernel->queryStep()>=TokiSlopeCraft::convertionReady;
    ui->ShowRaw->setEnabled(temp);

    temp=Kernel->queryStep()>=TokiSlopeCraft::converted;
    ui->ShowAdjed->setEnabled(temp);
    ui->ExportData->setEnabled(temp);
    ui->progressEx->setEnabled(temp);
    ui->ExData->setEnabled(temp);
    ui->progressExData->setEnabled(temp);

    temp=(!ui->isMapCreative->isChecked())&&
            Kernel->queryStep()>=TokiSlopeCraft::converted;
    ui->ExLite->setEnabled(temp);
    ui->progressExLite->setEnabled(temp);
    ui->ExStructure->setEnabled(temp);
    ui->progressExStructure->setEnabled(temp);
    ui->Build4Lite->setEnabled(temp);

    temp=Kernel->queryStep()>=TokiSlopeCraft::builded;
    ui->ExportLite->setEnabled(temp);
    ui->ManualPreview->setEnabled(temp);


}

/*
#ifdef dispDerivative
void MainWindow::checkBlockIds()
{
    Collect();
    qDebug("开始检查方块列表");
    fstream out("D:/check.mcfunction",ios::out);
    int offset[3]={-80,1,-80};
    //QString command="";
    if(out.is_open())qDebug("文件正常打开");else return;
    for(short r=0;r<64;r++)//x
        for(short c=0;c<9;c++)//z
        {
            if(Blocks[r][c]==NULL)continue;
            out<<"setblock ";
            out<<c+offset[2]<<' '<<offset[1]<<' '<<r+offset[0];
            out<<' '<<BlockId[r][c].toLocal8Bit().data()<<'\n';
            //out<<command;
        }
    out.close();
}
void MainWindow::makeImage(int unitL)
{
    qDebug("开始makeImage");
    if(unitL<=0)unitL=16;
    MatrixXi PMat;
    PMat.setZero(4*unitL,60*unitL);
    int mapColor=0,index=0;
    QRgb CurrentColor;
    int cIndex;
    for(int Base=1;Base<61;Base++)
    {
        for(int depth=0;depth<4;depth++)
        {
            mapColor=4*Base+depth;
            index=mcMap::mapColor2Index(mapColor);
            CurrentColor=qRgb(255*Basic._RGB(index,0),255*Basic._RGB(index,1),255*Basic._RGB(index,2));
            switch (depth)
            {
            case 0:
                cIndex=2;break;
            case 1:
                cIndex=1;break;
            case 2:
                cIndex=0;break;
            case 3:
                cIndex=3;break;
            }

            PMat.block(unitL*cIndex,unitL*(Base-1),unitL,unitL).array()=CurrentColor;

            //qDebug("rua!");
        }
    }
    qDebug("图像mat构建完毕");
    QRgb*CL=nullptr;

    //QSize size(PMat.rows(),PMat.cols());
    QSize size(PMat.cols(),PMat.rows());
    QImage Pic(size,QImage::Format_ARGB32);
    for(int r=0;r<PMat.rows();r++)
    {
        CL=(QRgb*)Pic.scanLine(r);
        for(int c=0;c<PMat.cols();c++)
        {
            CL[c]=PMat(r,c);
        }
    }
    Pic.save("D:\\240Colors.png");
}
#endif

#ifdef putBlockList
void MainWindow::putBlockListInfo() {
    if(!Collected)return;
    QString jsonDest=QFileDialog::getSaveFileName(this,
                                                  "将方块列表保存为json","","*.json");
    if(jsonDest.isEmpty())return;

    QString imgDest=QFileDialog::getExistingDirectory(this,
                                                      "选择输出图片的文件夹","");
    if(imgDest.isEmpty())return;
    imgDest.replace("\\","/");


    switchLan(false);

    queue<TokiBlock> blockQueue;
    TokiBlock temp;

    for(int r=0;r<64;r++)
        for(int c=0;c<12;c++) {
            if(Blocks[r][c]==NULL)
                continue;
            temp.setBaseColor(r);
            temp.setId(BlockId[r][c].toStdString());
            temp.setVersion(BlockVersion[r][c]);
            temp.setIdOld(BlockIdfor12[r][c].toStdString());
            temp.setNeedGlass(NeedGlass[r][c]);
            temp.setIsGlowing(doGlow[r][c]);

            temp.btn=Blocks[r][c];
            temp.nameZH=temp.btn->text();

            blockQueue.push(temp);
        }

    switchLan(true);

    fstream jsonFile;
    jsonFile.open(jsonDest.toLocal8Bit().data(),ios::out);
    if(!jsonFile) {
        qDebug("错误！MainWindow::putBlockListInfo中文件流jsonFile打开失败");
        return;
    }
    jsonFile<<"[\n";

    while(!blockQueue.empty()) {
        blockQueue.front().nameEN=blockQueue.front().btn->text();
        QString imgPath=imgDest+"/"+QString::fromStdString(blockQueue.front().toPureBlockId())+".png";
        blockQueue.front().iconPath=blockQueue.front().toPureBlockId();
        jsonFile<<blockQueue.front().toJSON().toUtf8().data();
        jsonFile<<",\n";
        blockQueue.front().btn->icon().pixmap(QSize(16,16)).save(imgPath);
        blockQueue.pop();
    }

    jsonFile.close();
    switchLan(false);

}
#endif
*/
void MainWindow::on_StartWithSlope_clicked() {
    ui->isMapSurvival->setChecked(true);
    turnToPage(1);
}

void MainWindow::on_StartWithFlat_clicked() {
    ui->isMapFlat->setChecked(true);
    turnToPage(1);
}

void MainWindow::on_StartWithNotVanilla_clicked() {
    ui->isMapCreative->setChecked(true);
    turnToPage(1);
}

void MainWindow::on_StartWithWall_clicked() {
    ui->isMapWall->setChecked(true);
    turnToPage(1);
}

void MainWindow::on_ImportPic_clicked() {
    QString Path =QFileDialog::getOpenFileName(this,
                                               tr("选择图片"),
                                               "./",
                                               tr("图片(*.png *.bmp *.jpg *.tif *.GIF )"));
        if(Path.isEmpty())return;

        if(!rawPic.load(Path))
        {
            QMessageBox::information(this,tr("打开图片失败"),tr("要不试试换一张图片吧！"));
                        return;
        }
        bool needSearch=rawPic.hasAlphaChannel();
        rawPic=rawPic.convertToFormat(QImage::Format_ARGB32);
        bool OriginHasTp=false;
        if(needSearch)
        {
            QRgb*CL=nullptr;
            for(short r=0;r<rawPic.height();r++)
            {
                CL=(QRgb*)rawPic.scanLine(r);
                for(short c=0;c<rawPic.width();c++)
                {
                    if(qAlpha(CL[c])<255)
                    {
                        r=rawPic.height()+1;
                        OriginHasTp=true;
                        break;
                    }
                }
            }
        }

        //ui->ShowRawPic->setPixmap(QPixmap::fromImage(rawPic));

        ui->IntroPicInfo->setText(tr("图片尺寸：")+QString::number(rawPic.height())+"×"+QString::number(rawPic.width())+tr("像素"));
        if(OriginHasTp)
        {
            preProcess(Strategy.pTpS,Strategy.hTpS,Strategy.BGC);
            ui->IntroPicInfo->setText(ui->IntroPicInfo->text()+"\n"+tr("图片中存在透明/半透明像素，已处理，您可以点击“设置”重新选择处理透明/半透明像素的方式。\n重新设置处理方式后，需要重新导入一次。"));
        }
        else
        {
            rawPic=rawPic.copy();
        }
        ui->ShowRawPic->setPixmap(QPixmap::fromImage(rawPic));
        ui->ShowPic->setPixmap(QPixmap::fromImage(rawPic));

        Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
        updateEnables();

        return;
}

void MainWindow::on_ImportSettings_clicked() {
    if(transSubWind!=nullptr) {
            qDebug("子窗口已经打开，不能重复打开！");
            return;
        }
        transSubWind=new tpStrategyWind(this);
        transSubWind->show();
        connect(transSubWind,&tpStrategyWind::destroyed,
                this,&MainWindow::destroySubWindTrans);
        connect(transSubWind,&tpStrategyWind::Confirm,
                this,&MainWindow::ReceiveTPS);
        transSubWind->setVal(Strategy);
}

void MainWindow::destroySubWindTrans() {
    disconnect(transSubWind,&tpStrategyWind::Confirm,
               this,&MainWindow::ReceiveTPS);
    transSubWind=nullptr;
}

void MainWindow::ReceiveTPS(tpS t) {
    this->Strategy=t;
    qDebug("接收成功");
    qDebug()<<"pTpS="<<t.pTpS<<"；hTpS="<<t.hTpS;
}

void MainWindow::preProcess(char pureTpStrategy,
                char halfTpStrategy,
                QRgb BGC) {
    qDebug("调用了preProcess");
    //透明像素处理策略：B->替换为背景色；A->空气；W->暂缓，等待处理
    //半透明像素处理策略：B->替换为背景色；C->与背景色叠加；R->保留颜色；W->暂缓，等待处理
    qDebug("Cpoied");
    bool hasTotalTrans=false;
    if(pureTpStrategy!='W'&&halfTpStrategy!='W')
    {
        QRgb*CL=nullptr;
        for(int r=0;r<rawPic.height();r++)
        {
            CL=(QRgb*)rawPic.scanLine(r);
            for(int c=0;c<rawPic.width();c++)
            {
                if(qAlpha(CL[c])>=255)continue;
                if(qAlpha(CL[c])==0)
                switch (pureTpStrategy) {
                case 'B':
                    CL[c]=BGC;
                    continue;
                case 'A':
                    if(!hasTotalTrans)
                    {qDebug()<<"发现纯透明像素";
                    hasTotalTrans=true;}
                    CL[c]=qRgba(0,0,0,0);
                    continue;
                }

                //qDebug("neeeee");
                switch (halfTpStrategy) {
                case 'B':
                    CL[c]=BGC;
                    break;
                case 'C':
                    CL[c]=ComposeColor(CL[c],BGC);
                    break;
                case 'R':
                    CL[c]=qRgb(qRed(CL[c]),qGreen(CL[c]),qBlue(CL[c]));
                }
            }
        }
    }

}

void MainWindow::onGameVerClicked() {
    if(ui->isGame12->isChecked()) {
        Manager->setVersion(12);
    }
    if(ui->isGame13->isChecked()) {
        Manager->setVersion(13);
    }
    if(ui->isGame14->isChecked()) {
        Manager->setVersion(14);
    }
    if(ui->isGame15->isChecked()) {
        Manager->setVersion(15);
    }
    if(ui->isGame16->isChecked()) {
        Manager->setVersion(16);
    }
    if(ui->isGame17->isChecked()) {
        Manager->setVersion(17);
    }
    Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
    updateEnables();
}

void MainWindow::onMapTypeClicked() {
    if(ui->isMapCreative->isChecked()) {
        Manager->setEnabled(12,true);
    }
    if(ui->isMapFlat->isChecked()) {
        Manager->setEnabled(12,true);
    }
    if(ui->isMapSurvival->isChecked()) {
        Manager->setEnabled(12,false);
    }
    if(ui->isMapWall->isChecked()) {
        Manager->setEnabled(12,false);
    }
    Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
    updateEnables();
}

void MainWindow::ChangeToCustom() {
    ui->isBLCustom->setChecked(true);
    Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
    updateEnables();
}

void MainWindow::onPresetsClicked() {
    if(ui->isBLCreative->isChecked()) {
        Manager->applyPreset(BLCreative);
    }
    if(ui->isBLSurvivalCheaper->isChecked()) {
        Manager->applyPreset(BLCheaper);
    }
    if(ui->isBLSurvivalBetter->isChecked()) {
        Manager->applyPreset(BLBetter);
    }
    if(ui->isBLGlowing->isChecked()) {
        Manager->applyPreset(BLGlowing);
    }

    if(ui->isMapSurvival->isChecked())
        Manager->setEnabled(12,false);

    Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
    updateEnables();
}

void MainWindow::onAlgoClicked() {
    static TokiSlopeCraft::convertAlgo lastChoice=TokiSlopeCraft::convertAlgo::RGB_Better;
    static bool lastDither=false;

    TokiSlopeCraft::convertAlgo now;
    bool nowDither=ui->AllowDither->isChecked();
    if(ui->isColorSpaceRGBOld->isChecked())
        now=TokiSlopeCraft::convertAlgo::RGB;
    if(ui->isColorSpaceRGB->isChecked())
        now=TokiSlopeCraft::convertAlgo::RGB_Better;
    if(ui->isColorSpaceHSV->isChecked())
        now=TokiSlopeCraft::convertAlgo::HSV;
    if(ui->isColorSpaceLab94->isChecked())
        now=TokiSlopeCraft::convertAlgo::Lab94;
    if(ui->isColorSpaceLab00->isChecked())
        now=TokiSlopeCraft::convertAlgo::Lab00;
    if(ui->isColorSpaceXYZ->isChecked())
        now=TokiSlopeCraft::convertAlgo::XYZ;
    if(lastChoice!=now||lastDither!=nowDither)
        Kernel->decreaseStep(TokiSlopeCraft::step::convertionReady);

    updateEnables();
    lastChoice=now;
    lastDither=nowDither;
}

void MainWindow::kernelSetType() {
    /*
bool TokiSlopeCraft::setType(mapTypes type,
                             gameVersion ver,
                             const bool * allowedBaseColor,
                             simpleBlock * palettes,
                             const ArrayXXi & _rawimg)
*/
    TokiSlopeCraft::mapTypes type=TokiSlopeCraft::mapTypes::Slope;
    {
    if(ui->isMapCreative->isChecked())
        type=TokiSlopeCraft::mapTypes::FileOnly;
    if(ui->isMapFlat->isChecked())
        type=TokiSlopeCraft::mapTypes::Flat;
    if(ui->isMapSurvival->isChecked())
        type=TokiSlopeCraft::mapTypes::Slope;
    if(ui->isMapWall->isChecked())
        type=TokiSlopeCraft::mapTypes::Wall;
    }


    TokiSlopeCraft::gameVersion ver=TokiSlopeCraft::gameVersion::MC17;
    {
    if(ui->isGame12->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC12;
    if(ui->isGame13->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC13;
    if(ui->isGame14->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC14;
    if(ui->isGame15->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC15;
    if(ui->isGame16->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC16;
    if(ui->isGame17->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC17;
    }

    bool allowedBaseColor[64];
    Manager->getEnableList(allowedBaseColor);

    simpleBlock palette[64];
    Manager->getSimpleBlockList(palette);


    /*for(uchar i=0;i<temp.size();i++){qDebug()<<i;
        palette[i].copyFrom(temp[i]);}*/


    Kernel->setType(type,ver,allowedBaseColor,palette);

    updateEnables();
}

void MainWindow::kernelSetImg() {
    EImage rawImg=QImage2EImage(rawPic);
    Kernel->setRawImage(rawImg);
}

EImage QImage2EImage(const QImage & qi) {
    EImage ei;
    if(qi.isNull()||qi.height()<=0||qi.width()<=0) {
        ei.setZero(0,0);
        return ei;
    }
    ei.setZero(qi.height(),qi.width());
    const QRgb * CL=nullptr;
    for(int r=0;r<ei.rows();r++) {
        CL=(const QRgb *)qi.scanLine(r);
        for(int c=0;c<ei.cols();c++)
            ei(r,c)=CL[c];
    }
    return ei;
}

QImage EImage2QImage(const EImage & ei,ushort scale) {
    QImage qi(ei.cols()*scale,ei.rows()*scale,QImage::Format::Format_ARGB32);
    QRgb * CL=nullptr;
    for(int r=0;r<qi.height();r++) {
        CL=(QRgb *)qi.scanLine(r);
        for(int c=0;c<qi.width();c++)
            CL[c]=ei(r/scale,c/scale);
    }
    return qi;
}

void MainWindow::progressRangeSet(int min,int max,int val) {
    //设置进度条的取值范围和值
    if(proTracker==nullptr) {
        qDebug("错误！proTracker==nullptr");
        return;
    }
    proTracker->setRange(min,max);
    proTracker->setValue(val);
}

void MainWindow::progressAdd(int deltaVal) {
    if(proTracker==nullptr) {
        qDebug("错误！proTracker==nullptr");
        return;
    }
    proTracker->setValue(deltaVal+
                                    proTracker->value());
}

void MainWindow::algoProgressRangeSet(int min,int max,int val) {
    ui->algoBar->setRange(min,max);
    ui->algoBar->setValue(val);
}

void MainWindow::algoProgressAdd(int deltaVal) {
    ui->algoBar->setValue(ui->algoBar->value()+deltaVal);
}

void MainWindow::on_Convert_clicked() {
if(Kernel->queryStep()<TokiSlopeCraft::wait4Image) {
    qDebug("重新setType");
    kernelSetType();
if(Kernel->queryStep()<TokiSlopeCraft::wait4Image)
    return;
}

if(Kernel->queryStep()<TokiSlopeCraft::convertionReady) {
    qDebug("重新setImage");
    kernelSetImg();
if(Kernel->queryStep()<TokiSlopeCraft::convertionReady)
    return;
}

TokiSlopeCraft::convertAlgo now;
bool nowDither=ui->AllowDither->isChecked();
{
if(ui->isColorSpaceRGBOld->isChecked())
    now=TokiSlopeCraft::convertAlgo::RGB;
if(ui->isColorSpaceRGB->isChecked())
    now=TokiSlopeCraft::convertAlgo::RGB_Better;
if(ui->isColorSpaceHSV->isChecked())
    now=TokiSlopeCraft::convertAlgo::HSV;
if(ui->isColorSpaceLab94->isChecked())
    now=TokiSlopeCraft::convertAlgo::Lab94;
if(ui->isColorSpaceLab00->isChecked())
    now=TokiSlopeCraft::convertAlgo::Lab00;
if(ui->isColorSpaceXYZ->isChecked())
    now=TokiSlopeCraft::convertAlgo::XYZ;
}

proTracker=ui->ShowProgressABbar;

Kernel->decreaseStep(TokiSlopeCraft::step::convertionReady);
updateEnables();

bool temp=false;
ui->Convert->setEnabled(temp);//防止用户在繁忙时重复操作
ui->isColorSpaceHSV->setEnabled(temp);
ui->isColorSpaceRGB->setEnabled(temp);
ui->isColorSpaceLab94->setEnabled(temp);
ui->isColorSpaceLab00->setEnabled(temp);
ui->isColorSpaceXYZ->setEnabled(temp);
ui->isColorSpaceRGBOld->setEnabled(temp);
ui->AllowDither->setEnabled(temp);

std::clock_t startTime=std::clock();
qDebug("开始convert");
Kernel->convert(now,nowDither);

qDebug()<<"convert用时"
       <<double(std::clock()-startTime)*1000.0/CLOCKS_PER_SEC
      <<"毫秒";
proTracker=nullptr;

temp=true;
ui->Convert->setEnabled(temp);//恢复锁定
ui->isColorSpaceHSV->setEnabled(temp);
ui->isColorSpaceRGB->setEnabled(temp);
ui->isColorSpaceLab94->setEnabled(temp);
ui->isColorSpaceLab00->setEnabled(temp);
ui->isColorSpaceXYZ->setEnabled(temp);
ui->isColorSpaceRGBOld->setEnabled(temp);
ui->AllowDither->setEnabled(temp);
on_ShowAdjed_clicked();
updateEnables();
}

void MainWindow::on_ShowRaw_clicked() {
    ui->ShowPic->setPixmap(QPixmap::fromImage(rawPic));
}

void MainWindow::on_ShowAdjed_clicked() {
    ui->ShowPic->setPixmap(QPixmap::fromImage(
                               EImage2QImage(Kernel->getConovertedImage())));
}

void MainWindow::on_ExData_clicked() {
    int mapRows=ceil(Kernel->getImageRows()/128.0);
    int mapCols=ceil(Kernel->getImageCols()/128.0);
    int mapCounts=mapRows*mapCols;
    ui->ShowDataRows->setText(QString::number(mapRows));
    ui->ShowDataCols->setText(QString::number(mapCols));
    ui->ShowDataCounts->setText(QString::number(mapCounts));
    ui->InputDataIndex->setText("0");
}

//Page5

void MainWindow::on_Build4Lite_clicked() {

    bool naturalCompress=ui->AllowNaturalOpti->isChecked();
    bool forcedCompress=ui->AllowForcedOpti->isChecked();
    TokiSlopeCraft::compressSettings cS;
    if(naturalCompress) {
        if(forcedCompress)
            cS=TokiSlopeCraft::compressSettings::Both;
        else
            cS=TokiSlopeCraft::compressSettings::NaturalOnly;
    } else {
        if(forcedCompress)
            cS=TokiSlopeCraft::compressSettings::ForcedOnly;
        else cS=TokiSlopeCraft::compressSettings::noCompress;
    }

    bool allowBridge=ui->allowGlassBridge->isChecked();
    TokiSlopeCraft::glassBridgeSettings gBS=
            allowBridge?TokiSlopeCraft::withBridge:TokiSlopeCraft::noBridge;

    Kernel->decreaseStep(TokiSlopeCraft::step::converted);
    ui->ExportLite->setEnabled(false);
    ui->FinishExLite->setEnabled(false);
    ui->ManualPreview->setEnabled(false);

    proTracker=ui->ShowProgressExLite;
    qDebug()<<"ui->maxHeight->value()="<<ui->maxHeight->value();
    Kernel->build(cS,ui->maxHeight->value(),
                  gBS,ui->glassBridgeInterval->value(),
                  ui->allowAntiFire->isChecked(),ui->allowAntiEnderman->isChecked());

    int size3D[3],total;

    Kernel->get3DSize(size3D[0],size3D[1],size3D[2]);
    total=Kernel->getBlockCounts();
    ui->ShowLiteBlocks->setText(QString::number(total));
    ui->ShowLiteXYZ->setText(QString::fromStdString(
                                     "X:"+std::to_string(size3D[0])+
                                 "  × Y:"+std::to_string(size3D[1])+
                "  × Z:"+std::to_string(size3D[2])));
    proTracker=nullptr;
    updateEnables();
    showPreview();
}

void MainWindow::on_ManualPreview_clicked() {
    showPreview();
}

void MainWindow::on_ExportLite_clicked() {
        std::string FileName=
                QFileDialog::getSaveFileName(this,
                                             tr("导出为投影/结构方块文件"),
                                             "",
                                             tr("投影文件(*.litematic) ;; 结构方块文件(*.nbt)")
                                             ).toLocal8Bit().data();
        std::string unCompressed;
        if(FileName.empty())return;
        bool putLitematic=(FileName.substr(FileName.length()-strlen(".litematic"))==".litematic");
        bool putStructure=(FileName.substr(FileName.length()-strlen(".nbt"))==".nbt");

        if(!putLitematic&&!putStructure)
        {
            qDebug("得到的文件路径有错！");
            return;
        }
        qDebug("开始导出投影");
        std::cout<<FileName<<std::endl;


        ui->FinishExLite->setEnabled(false);
        ui->seeExported->setEnabled(false);
        ui->Build4Lite->setEnabled(false);

        this->proTracker=ui->ShowProgressExLite;

        if(putStructure)
            unCompressed=Kernel->exportAsStructure(FileName);
        else
            unCompressed=Kernel->exportAsLitematic(FileName,
                                                   ui->InputLiteName->toPlainText().toUtf8().data(),
                                                   ui->InputAuthor->toPlainText().toUtf8().data(),
                                                   (ui->InputRegionName->toPlainText()+tr("(xz坐标=-65±128×整数)")).toUtf8().data());

        if(compressFile(unCompressed.data(),FileName.data()))
        {
            qDebug("压缩成功");
            QFile umComFile(QString::fromLocal8Bit(unCompressed.data()));
            if(umComFile.remove())
                qDebug("删除成功");
            else
                qDebug("删除失败");
            qDebug()<<QString::fromLocal8Bit(unCompressed.data());
            ProductDir=QString::fromLocal8Bit(FileName.data());
            ProductDir=ProductDir.replace('\\','/');
            ProductDir=ProductDir.left(ProductDir.lastIndexOf('/'));

            qDebug()<<"ProductDir="<<ProductDir;

        }
        else
        {
            qDebug("压缩失败");
            QMessageBox::warning(this,tr("投影文件导出失败"),
                                 tr("这可能是汉字编码错误造成的。请检查路径中是否有汉字"));
            return;
        };

        ui->FinishExLite->setEnabled(true);
        ui->seeExported->setEnabled(true);
        ui->Build4Lite->setEnabled(true);

        updateEnables();
        this->proTracker=nullptr;
        qDebug("导出为投影成功");
        return;
}


void MainWindow::on_InputDataIndex_textChanged() {
    bool isIndexValid=false;
        const int indexStart=ui->InputDataIndex->toPlainText().toInt(&isIndexValid);
        isIndexValid=isIndexValid&&(indexStart>=0);
        if(isIndexValid)
        {
            if(ceil(Kernel->getImageRows()/128.0f)==1&&
                    ceil(Kernel->getImageCols()/128.0f)==1)
            ui->ShowDataFileName->setText("map_"+QString::number(indexStart)+".dat");
            else
                ui->ShowDataFileName->setText(
                            "map_"+QString::number(indexStart)+".dat"+
                            "~"+"map_"+
                            QString::number(indexStart+ceil(Kernel->getImageRows()/128.0f)*ceil(Kernel->getImageRows()/128.0f)-1)+".dat");
            ui->ExportData->setEnabled(true);
            return;
        }

        ui->ShowDataFileName->setText(tr("你输入的起始序号不可用，请输入大于等于0的整数！"));
        ui->ExportData->setEnabled(false);
        return;
}


void MainWindow::on_ExportData_clicked() {
    bool isIndexValid=false;
        const int indexStart=ui->InputDataIndex->toPlainText().toInt(&isIndexValid);
        isIndexValid=isIndexValid&&(indexStart>=0);
        if(!isIndexValid)
        {
            QMessageBox::information(this,tr("你输入的起始序号不可用"),tr("请输入大于等于0的整数！"));
                        return;
        }
        QString FolderPath=(QFileDialog::getExistingDirectory(this,tr("请选择导出的文件夹")));
        if(FolderPath.isEmpty())
        {
            QMessageBox::information(this,tr("你选择的文件夹不存在！"),tr("你可以选择存档中的data文件夹"));
            return;
        }


        ui->InputDataIndex->setEnabled(false);
        ui->ExportData->setEnabled(false);
        ui->FinshExData->setEnabled(false);
        ui->ExportData->setText(tr("请稍等"));

        proTracker=ui->ShowProgressExData;

        FolderPath=FolderPath.replace('\\','/');
        ProductDir=FolderPath;

        auto unCompressedList=Kernel->exportAsData(FolderPath.toLocal8Bit().data(),
                                                   indexStart);
        qDebug("导出地图文件成功");
        //QString::fromLocal8Bit(unCompressed.data())

        for(auto it=unCompressedList.begin();it!=unCompressedList.end();it++) {
            QString unName=QString::fromLocal8Bit(it->data());
            QString dstName=unName.left(unName.lastIndexOf(".TokiNoBug"));
            qDebug()<<"unName="<<unName;
            qDebug()<<"dstName="<<dstName;

            if(compressFile(unName.toLocal8Bit(),dstName.toLocal8Bit())) {
                    QFile unFile(unName);
                    if(unFile.remove()) {
                        qDebug()<<"删除"<<unName<<"成功";
                    }
                    else {
                        qDebug()<<"删除"<<unName<<"失败";
                    }
            }

        }

        ui->InputDataIndex->setEnabled(true);
        ui->ExportData->setEnabled(true);
        ui->FinshExData->setEnabled(true);
        ui->ExportData->setText(tr("导出"));
        proTracker=nullptr;
        updateEnables();
}

void MainWindow::turnCh() {
    switchLan(ZH);
}

void MainWindow::turnEn() {
    switchLan(EN);
}

void MainWindow::switchLan(Language lang) {
    qDebug("开始调整语言");
    emit Manager->translate(lang);
        if(lang==EN)
        {
            if(!trans.load(":/i18n/SlopeCraft_en_US.qm"))
            {
                qDebug("载入\":/i18n/SlopeCraft_en_US.qm\"失败");
                return;
            }
            qApp->installTranslator(&trans);
            ui->retranslateUi(this);
            qDebug("成功调整为英语界面");
        }
        else
        {
            if(!trans.load(":/i18n/SlopeCraft_zh_CN.qm"))
            {
                qDebug("载入\":/i18n/SlopeCraft_zh_CN.qm\"失败");
                return;
            }
            qApp->installTranslator(&trans);
            ui->retranslateUi(this);
            qDebug("成功调整为简体中文界面");
        }
        return;
}

bool compressFile(const char*sourcePath,const char*destPath) {
    char buf[bufferSize]={0};
    FILE*in=NULL;
    gzFile out=NULL;
    int len=0;
    fopen_s(&in,sourcePath,"rb");
    out=gzopen(destPath,"wb");
    if(in==NULL||out==NULL)
        return false;
    while(true)
    {
        len=(int)fread(buf,1,sizeof(buf),in);
        if(ferror(in))return false;
        if(len==0)break;
        if(len!=gzwrite(out,buf,(unsigned)len))
            return false;
        memset(buf,0,sizeof(buf));
    }
    fclose(in);
    gzclose(out);
    qDebug("succeed");
    return true;
}

void MainWindow::on_allowGlassBridge_stateChanged(int arg1) {
    ui->glassBridgeInterval->setEnabled(arg1);
}

void MainWindow::showError(TokiSlopeCraft::errorFlag error) {
    QString title,text;
    bool isFatal=false;
    switch (error) {
    case TokiSlopeCraft::errorFlag::NO_ERROR_OCCUR:
        return;
    case TokiSlopeCraft::errorFlag::EMPTY_RAW_IMAGE:
        title=tr("转化原图为地图画时出错");
        text=tr("原图为空！你可能没有导入原图！");
        break;
    case TokiSlopeCraft::errorFlag::DEPTH_3_IN_VANILLA_MAP:
        title=tr("构建高度矩阵时出现错误");
        text=tr("原版地图画不允许出现第三个阴影（不存在的几何关系不可能生存实装！）\n" \
        "请检查你的地图画类型，纯文件地图画不可以导出为投影！");
        break;
    case TokiSlopeCraft::errorFlag::HASTY_MANIPULATION:
        title=tr("跳步操作");
        text=tr("SlopeCraft不允许你跳步操作，请按照左侧竖边栏的顺序操作！");
        break;
    case TokiSlopeCraft::errorFlag::LOSSYCOMPRESS_FAILED:
        title=tr("有损压缩失败");
        text=tr("在构建高度矩阵时，有损压缩失败，没能将地图画压缩到目标高度。 \
        这可能是因为地图画行数过大。 \
        尝试启用无损压缩，或者提高最大允许高度——不要给软件地图画太大的压力！");
        break;
    case TokiSlopeCraft::errorFlag::MAX_ALLOWED_HEIGHT_LESS_THAN_14:
        title=tr("最大允许高度太小了");
        text=tr("有损压缩的最大允许不要低于14，否则很容易压缩失败");
        break;
    case TokiSlopeCraft::errorFlag::PARSING_COLORMAP_HSV_FAILED:
        isFatal=true;
        title=tr("严重错误：颜色表文件HSV.TokiColor损坏");
        text=tr("SlopeCraft不能正常解析颜色表文件，它是不可以被修改的！");
        break;
    case TokiSlopeCraft::errorFlag::PARSING_COLORMAP_Lab_FAILED:
        isFatal=true;
        title=tr("严重错误：颜色表文件Lab.TokiColor损坏");
        text=tr("SlopeCraft不能正常解析颜色表文件，它是不可以被修改的！");
        break;
    case TokiSlopeCraft::errorFlag::PARSING_COLORMAP_XYZ_FAILED:
        isFatal=true;
        title=tr("严重错误：颜色表文件XYZ.TokiColor损坏");
        text=tr("SlopeCraft不能正常解析颜色表文件，它是不可以被修改的！");
        break;
    case TokiSlopeCraft::errorFlag::PARSING_COLORMAP_RGB_FAILED:
        isFatal=true;
        title=tr("严重错误：颜色表文件RGB.TokiColor损坏");
        text=tr("SlopeCraft不能正常解析颜色表文件，它是不可以被修改的！");
        break;
    case TokiSlopeCraft::errorFlag::USEABLE_COLOR_TO_LITTLE:
        title=tr("允许使用的颜色过少");
        text=tr("你应该勾选启用尽可能多的基色，颜色太少是不行的！");
        break;
    }
    if(isFatal)
        QMessageBox::warning(this,title,text,
                         QMessageBox::StandardButton::Ok,
                         QMessageBox::StandardButton::NoButton);
    else {
        QMessageBox::critical(this,title,text,QMessageBox::StandardButton::Close);
        emit ui->Exit->clicked();
    }
    updateEnables();
    return;
}

void MainWindow::showWorkingStatue(TokiSlopeCraft::workStatues statue) {
QString title=this->windowTitle();
const char spacer[]="   |   ";
if(title.contains(spacer)) {
    title=title.left(title.lastIndexOf(spacer));
}

if(statue!=TokiSlopeCraft::workStatues::none)
    title+= spacer;

switch (statue) {
case TokiSlopeCraft::workStatues::none:
    break;
case TokiSlopeCraft::workStatues::buidingHeighMap:
    title+=tr("正在构建高度矩阵");
    break;
case TokiSlopeCraft::workStatues::building3D:
    title+=tr("正在构建三维结构");
    break;
case TokiSlopeCraft::workStatues::collectingColors:
    title+=tr("正在收集整张图片的颜色");
    break;
case TokiSlopeCraft::workStatues::compressing:
    title+=tr("正在压缩立体地图画");
    break;
case TokiSlopeCraft::workStatues::constructingBridges:
    title+=tr("正在为立体地图画搭桥");
    break;
case TokiSlopeCraft::workStatues::converting:
    title+=tr("正在匹配颜色");
    break;
case TokiSlopeCraft::workStatues::dithering:
    title+=tr("正在使用抖动仿色");
    break;
case TokiSlopeCraft::workStatues::flippingToWall:
    title+=tr("正在将平板地图画变为墙面地图画");
    break;
case TokiSlopeCraft::workStatues::writing3D:
    title+=tr("正在写入三维结构");
    break;
case TokiSlopeCraft::workStatues::writingBlockPalette:
    title+=tr("正在写入方块列表");
    break;
case TokiSlopeCraft::workStatues::writingMapDataFiles:
    title+=tr("正在写入地图数据文件");
    break;
case TokiSlopeCraft::workStatues::writingMetaInfo:
    title+=tr("正在写入基础信息");
    break;
}

setWindowTitle(title);
return;
}



void MainWindow::on_seeExported_clicked() {
if(ProductDir.isEmpty()) {
    return;
}
qDebug()<<"ProductDir="<<ProductDir;
QDesktopServices::openUrl(QUrl::fromLocalFile(ProductDir));
}


void MainWindow::on_AllowForcedOpti_stateChanged(int arg1) {
    ui->maxHeight->setEnabled(arg1);
}


void MainWindow::on_reportBugs_clicked() {
    QUrl url(
                tr("https://github.com/ToKiNoBug/SlopeCraft/issues/new?assignees=&labels=&template=ReportBugs_ZH.md"));
    QDesktopServices::openUrl(url);
}

void MainWindow::checkVersion() {

    //QtConcurrent::run(grabVersion,this);
    grabVersion(this);
    return;
}

void grabVersion(MainWindow * parent) {
    static bool isRunning=false;
    if(isRunning) {
        return;
    }
    isRunning=true;

    static const QString url="https://api.github.com/repos/TokiNoBug/SlopeCraft/releases/latest";

    QEventLoop tempLoop;
    QNetworkAccessManager * manager=new QNetworkAccessManager;
    QNetworkRequest request;
    request.setUrl(
                QUrl(url));
    QNetworkReply * reply=manager->get(request);
    QObject::connect(reply,&QNetworkReply::finished,
                     &tempLoop,&QEventLoop::quit);
    qDebug()<<"waitting for reply";
    tempLoop.exec();
    qDebug()<<reply->isFinished();
    QByteArray result=reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument jd=QJsonDocument::fromJson(result,&error);
    if(error.error!=error.NoError) {
        int userReply=
                QMessageBox::information(parent,
                                         QObject::tr("检查更新时遇到Json解析错误"),
                                         QObject::tr("网址  ")+url
                                         +QObject::tr("  回复的信息无法通过json解析。\n点击No以忽略这个错误；点击NoToAll则不会再自动检查更新。\n具体的错误为：\n")
                                         +error.errorString()+
                                         QObject::tr("\n具体回复的信息为：\n")+result,
                                         QMessageBox::StandardButton::No,
                                         QMessageBox::StandardButton::NoToAll);
        if(userReply==QMessageBox::StandardButton::NoToAll) {
            parent->setAutoCheckUpdate(false);
        }
        isRunning=false;
        return;
    }


    QJsonObject jo=jd.object();

    bool hasKey=jo.contains("tag_name");
    if(!hasKey) {
        int userReply=
                QMessageBox::information(parent,
                                         QObject::tr("检查更新时返回信息错误"),
                                         QObject::tr("网址  ")+url
                                         +QObject::tr("  回复的信息中不包含版本号（\"tag_name\"）。\n点击No以忽略这个错误；点击NoToAll则不会再自动检查更新。\n")
                                         +
                                         QObject::tr("\n具体回复的信息为：\n")+result,
                                         QMessageBox::StandardButton::No,
                                         QMessageBox::StandardButton::NoToAll);
        if(userReply==QMessageBox::StandardButton::NoToAll) {
            parent->setAutoCheckUpdate(false);
        }
        isRunning=false;
        return;
    }

    bool isKeyString=jo.value("tag_name").isString();
    if(!isKeyString) {
        int userReply=
                QMessageBox::information(parent,
                                         QObject::tr("检查更新时返回信息错误"),
                                         QObject::tr("网址  ")+url
                                         +QObject::tr("  回复的信息中，版本号（\"tag_name\"）不是字符串。\n点击No以忽略这个错误；点击NoToAll则不会再自动检查更新。\n")
                                         +QObject::tr("\n具体回复的信息为：\n")+result
                                         ,
                                         QMessageBox::StandardButton::No,
                                         QMessageBox::StandardButton::NoToAll);
        if(userReply==QMessageBox::StandardButton::NoToAll) {
            parent->setAutoCheckUpdate(false);
        }
        isRunning=false;
        return;
    }

    QString updateInfo=jo["body"].toString();

    QString latestVersion=jo["tag_name"].toString();
    if(latestVersion==parent->selfVersion) {
        isRunning=false;
        return;
    } else {
        QMessageBox::StandardButton userReply =
                QMessageBox::information(parent,
                                         QObject::tr("SlopeCraft已更新"),
                                         QObject::tr("好消息！好消息！SlopeCraft更新了！\n")+
                                         QObject::tr("当前版本为")+parent->selfVersion+
                                         QObject::tr("，检查到最新版本为")+latestVersion
                                         +QObject::tr("\n点击Ok前往下载；点击No关闭提示；点击NoToAll不再自动检查更新。")
                                         //+QObject::tr("\n更新内容：\n")+updateInfo
                                         ,

                                             {QMessageBox::StandardButton::Ok,
                                              QMessageBox::StandardButton::No,
                                               QMessageBox::StandardButton::NoToAll},

                                         QMessageBox::StandardButton::Ok);

        if(userReply==QMessageBox::StandardButton::Ok) {
            QDesktopServices::openUrl(
                        QUrl("https://github.com/ToKiNoBug/SlopeCraft/releases/latest"));
        }
        if(userReply==QMessageBox::StandardButton::NoToAll) {
            parent->setAutoCheckUpdate(false);
        }

        isRunning=false;
        return;
    }
}

/*
void MainWindow::receiveFinish(bool ok, QByteArray data) {

}
*/

void MainWindow::putSettings(const QJsonObject & jo) {
    QFile ini("./settings.json");
    ini.open(QFile::OpenModeFlag::ReadWrite|QFile::OpenModeFlag::Text);

    ini.write(QJsonDocument(jo).toJson());
    ini.close();
}

void MainWindow::setAutoCheckUpdate(bool autoCheckUpdate) {
    QJsonObject jo=loadIni();
    jo["autoCheckUpdates"]=autoCheckUpdate;
    putSettings(jo);
}
