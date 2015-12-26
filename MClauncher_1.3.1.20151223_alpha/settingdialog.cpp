#include "settingdialog.h"
#include "ui_settingdialog.h"

//游戏启动参数全局变量
extern QString playerName;//玩家名字
extern QString   maxMem;//最大内存
//java路径格式：C:\Program Files\Java\jre7\bin
extern QString javaPath;//javaw.exe所在路径

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->exitPushButton,SIGNAL(clicked()),this,SLOT(close()));//退出按钮
    QObject::connect(ui->autoRearchJavaPathPushButton,SIGNAL(clicked()),this,SLOT(autoRearchJavaPathSlot()));//自动搜索
    QObject::connect(ui->BrowsePushButton,SIGNAL(clicked()),this,SLOT(browseSlot()));//浏览
    QObject::connect(ui->saveAndExitPushButton,SIGNAL(clicked()),this,SLOT(saveAndExitSlot()));//保存退出
    QObject::connect(ui->aboutSoftLabel,SIGNAL(linkActivated(QString)),this,SLOT(aboutSoftSlot()));//关于软件

    //启动时加载配置文件
    QFile loadFile(QStringLiteral("MClauncher.cfg"));

     if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug("Couldn't open save file.");
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
                    ui->playerNameLineEdit->setText(playerName);

                    QJsonValue maxMemJsonVal = obj.take("maxMem");
                    maxMem = maxMemJsonVal.toString();
                    ui->ManualSetRadioButton->setChecked(true);
                    ui->maxMemLineEdit->setText(maxMem);

                    QJsonValue javaPathJsonVal = obj.take("javaPath");
                    javaPath = javaPathJsonVal.toString();
                    ui->javaPathLineEdit->setText(javaPath);
                    }
                }
     }
}

SettingDialog::~SettingDialog()
{
    delete ui;
}


void SettingDialog::mousePressEvent(QMouseEvent *event){
    this->windowPos = this->pos();
    this->mousePos = event->globalPos();
    this->dPos = mousePos - windowPos;
}

void SettingDialog::mouseMoveEvent(QMouseEvent *event){
    this->move(event->globalPos() - this->dPos);
}

//自动搜索Java路径槽函数
void SettingDialog::autoRearchJavaPathSlot(){

    //读取系统环境变量，并筛选出PATH中的路径
        QProcessEnvironment processEnv = QProcessEnvironment::systemEnvironment();
        QStringList environment = processEnv.toStringList();

       //系统环境处理
        bool getPathOk = false;
        QString str;
        foreach(str,environment)
        {
            if (str.startsWith("PATH=",Qt::CaseInsensitive))
            {
                getPathOk =  true;
                break;
            }
        }

        if(getPathOk){
            //PATH路径字符串按";"隔开，并找到含有java或者jre的路径
            QStringList strlist=str.split(";");//将字符串按";"隔开

            bool javaPathOk = false;

            foreach(str,strlist)
            {
                if(str.contains("java", Qt::CaseInsensitive) || str.contains("jre", Qt::CaseInsensitive))
                {
                    qDebug()<<str;

                    //判断javaw.exe文件是否存在

                    //java路径格式转换
                    QString javaPathTst = str;
                    javaPathTst.replace("\\", "\\\\", Qt::CaseSensitive);

                    javaPathTst = javaPathTst + "\\\\javaw.exe";

                    //判断该目录下是否有javaw.exe的存在

                    QFile javawFile(javaPathTst);
                    if(javawFile.exists()){
                        javaPath = str;//保存java路径为游戏启动做准备
                        ui->javaPathLineEdit->setText(str);//将路径自动填入Java路径框
                        javaPathOk = true;
                        break;
                    }
                }
            }

            if(!javaPathOk) QMessageBox::information(this,QString::fromLocal8Bit("错误信息"),QString::fromLocal8Bit("未找到！请手动设置。"));
        }
        else QMessageBox::information(this,QString::fromLocal8Bit("错误信息"),QString::fromLocal8Bit("未找到！请手动设置。"));
}

//浏览文件槽函数
void SettingDialog::browseSlot(){
    QString fileName=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("选择javaw.exe"),QDir::currentPath());
    qDebug()<<fileName;

    QStringList fileNameStrList = fileName.split("/");
    QString fileNameLast = fileNameStrList.at(fileNameStrList.size() - 1);
    qDebug()<<fileNameLast;

    fileName.remove("/" + fileNameLast);

    ui->javaPathLineEdit->setText(fileName);//将路径自动填入Java路径框
    fileName = fileName.replace("/","\\",Qt::CaseSensitive);
    qDebug()<<fileName;
    javaPath = fileName;//保存java路径为游戏启动做准备
}

//保存退出槽函数
void SettingDialog::saveAndExitSlot(){
    //获取玩家名字
    QString name = ui->playerNameLineEdit->text();
    if(name.isEmpty()){
        QMessageBox::information(this,QString::fromLocal8Bit("错误信息"),QString::fromLocal8Bit("玩家名称不能为空"));
        return;
    }
    playerName = name;//存储玩家姓名

    if(ui->ManualSetRadioButton->isChecked()){
        QString mem = ui->maxMemLineEdit->text();
        if(mem.isEmpty()){
            QMessageBox::information(this,QString::fromLocal8Bit("错误信息"),QString::fromLocal8Bit("手动内存大小不能为空"));
            return;
        }
        maxMem = mem;
    }
    else maxMem = "1024";//自动设置最大内存为1024

    QString path = ui->javaPathLineEdit->text();
    if(path.isEmpty())  autoRearchJavaPathSlot();  //如果没有设置java路径，自动设置


    //将配置信息保存在配置文件中，使用JSON
    QFile saveFile(QStringLiteral("MClauncher.cfg"));

     if (!saveFile.open(QIODevice::WriteOnly)){
            qDebug("Couldn't open save file.");
      }
      else{
         QJsonObject json;
         json.insert("playerName", playerName);
         json.insert("maxMem", maxMem);
         json.insert("javaPath", javaPath);

        QJsonDocument saveDoc(json);

        saveFile.write(saveDoc.toJson());
        saveFile.close();

        qDebug("Save successed");
     }

     close();//关闭设置窗口
}

//关于软件
void SettingDialog::aboutSoftSlot(){

    InfoDialog *aboutSoftDia = new InfoDialog();

    aboutSoftDia->setInfo(QString::fromLocal8Bit("作者：BlueCat"),QString::fromLocal8Bit("MC版本：1.7.10"),QString::fromLocal8Bit("软件版本：1.3.1.20151223_Alpha"));
    //aboutSoftDia->setWindowFlags(Qt::WindowStaysOnTopHint);
    //aboutSoftDia->show();
    aboutSoftDia->exec();
}
