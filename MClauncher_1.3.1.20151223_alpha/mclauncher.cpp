#include "mclauncher.h"
#include "ui_mclauncher.h"

//游戏启动参数全局变量
QString playerName;//玩家名字
QString   maxMem;//最大内存
QString javaPath;  //javaw.exe所在路径

MClauncher::MClauncher(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MClauncher)
{
    ui->setupUi(this);

    QObject::connect(ui->minimize,SIGNAL(clicked()),this,SLOT(showMinimized()));//最小化按钮
    QObject::connect(ui->exit,SIGNAL(clicked()),this,SLOT(close()));//退出按钮
    QObject::connect(ui->startGame,SIGNAL(clicked()),this,SLOT(startGameSlot()));//启动游戏按钮
    QObject::connect(ui->setting,SIGNAL(clicked()),this,SLOT(settingDialogSlot()));//设置按钮

    //查找version的版本个数
    QDir versionDir;
    versionDir.setPath(".minecraft\\versions");

    QStringList verDirStrList = versionDir.entryList(QDir::NoFilter,QDir::NoSort);

     //判断是否有游戏文件
    if(!(verDirStrList.size() > 2)){

        QMessageBox::information(this,QString::fromLocal8Bit("错误信息"),QString::fromLocal8Bit("未找游戏文件！"));

        ui->startGame->setEnabled(false);//禁止启动有按钮
        ui->setting->setEnabled(false);//禁止设置按钮
        ui->verSecComboBox->setEnabled(false);//禁止版本选择
    }
    else{
        //取消目录中的 "." 和 ".."目录
        verDirStrList.removeFirst();//移除“.”目录
        verDirStrList.removeFirst();//移除“..”目录

        ui->verSecComboBox->addItems(verDirStrList);//显示版本数
    }
}

MClauncher::~MClauncher()
{
    delete ui;
}


void MClauncher::mousePressEvent(QMouseEvent *event){
    this->windowPos = this->pos();
    this->mousePos = event->globalPos();
    this->dPos = mousePos - windowPos;
}

void MClauncher::mouseMoveEvent(QMouseEvent *event){
    this->move(event->globalPos() - this->dPos);
}

//打开设置对话框
void MClauncher::settingDialogSlot()
{
    SettingDialog *settingDialog = new SettingDialog;

    settingDialog->setWindowTitle(QString::fromLocal8Bit("设置"));
    settingDialog->setWindowFlags(Qt::FramelessWindowHint);
    settingDialog->setModal(true);
    settingDialog->show();
}

//启动游戏按钮
void MClauncher::startGameSlot(){

    //启动时加载配置文件
    QFile loadFile(QStringLiteral("MClauncher.cfg"));

     if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug("Couldn't open save file.");
        QMessageBox::information(this,QString::fromLocal8Bit("错误信息"),QString::fromLocal8Bit("未找到配置文件，请点击设置进行游戏配置"));
     }
     else{
         QByteArray saveData = loadFile.readAll();
          loadFile.close();

         QJsonParseError json_error;
         QJsonDocument loadDoc(QJsonDocument::fromJson(saveData, &json_error));

          if(json_error.error == QJsonParseError::NoError)
          {
                if(loadDoc.isObject())
                {
                    QJsonObject obj = loadDoc.object();

                    QJsonValue nameJsonVal = obj.take("playerName");
                    playerName = nameJsonVal.toString();

                    QJsonValue maxMemJsonVal = obj.take("maxMem");
                    maxMem = maxMemJsonVal.toString();

                    QJsonValue javaPathJsonVal = obj.take("javaPath");
                    javaPath = javaPathJsonVal.toString();
                    }
                }

                //查找选择要启动的版本
                QString mcVersion;//游戏版本
                mcVersion = ui->verSecComboBox->currentText();
                qDebug()<<mcVersion;

                //启动游戏
               int nativeError = checkNativesDir();//检查natives文件是否存在

               if(nativeError == 3)  QMessageBox::information(this,QString::fromLocal8Bit("错误信息"),QString::fromLocal8Bit("找不到unzip32.exe：") + nativeError);

               if(!nativeError){
                   int errorCode = MClaunch(playerName, maxMem, javaPath,mcVersion);
                    if(errorCode == 0){
                        close();//关闭启动器
                    }
                    else{
                        QMessageBox::information(this,QString::fromLocal8Bit("错误信息"),QString::fromLocal8Bit("错误代码：") + errorCode);
                    }
               }
     }
}
