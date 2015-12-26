#include "settingdialog.h"
#include "ui_settingdialog.h"

//��Ϸ��������ȫ�ֱ���
extern QString playerName;//�������
extern QString   maxMem;//����ڴ�
//java·����ʽ��C:\Program Files\Java\jre7\bin
extern QString javaPath;//javaw.exe����·��

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->exitPushButton,SIGNAL(clicked()),this,SLOT(close()));//�˳���ť
    QObject::connect(ui->autoRearchJavaPathPushButton,SIGNAL(clicked()),this,SLOT(autoRearchJavaPathSlot()));//�Զ�����
    QObject::connect(ui->BrowsePushButton,SIGNAL(clicked()),this,SLOT(browseSlot()));//���
    QObject::connect(ui->saveAndExitPushButton,SIGNAL(clicked()),this,SLOT(saveAndExitSlot()));//�����˳�
    QObject::connect(ui->aboutSoftLabel,SIGNAL(linkActivated(QString)),this,SLOT(aboutSoftSlot()));//�������

    //����ʱ���������ļ�
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

//�Զ�����Java·���ۺ���
void SettingDialog::autoRearchJavaPathSlot(){

    //��ȡϵͳ������������ɸѡ��PATH�е�·��
        QProcessEnvironment processEnv = QProcessEnvironment::systemEnvironment();
        QStringList environment = processEnv.toStringList();

       //ϵͳ��������
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
            //PATH·���ַ�����";"���������ҵ�����java����jre��·��
            QStringList strlist=str.split(";");//���ַ�����";"����

            bool javaPathOk = false;

            foreach(str,strlist)
            {
                if(str.contains("java", Qt::CaseInsensitive) || str.contains("jre", Qt::CaseInsensitive))
                {
                    qDebug()<<str;

                    //�ж�javaw.exe�ļ��Ƿ����

                    //java·����ʽת��
                    QString javaPathTst = str;
                    javaPathTst.replace("\\", "\\\\", Qt::CaseSensitive);

                    javaPathTst = javaPathTst + "\\\\javaw.exe";

                    //�жϸ�Ŀ¼���Ƿ���javaw.exe�Ĵ���

                    QFile javawFile(javaPathTst);
                    if(javawFile.exists()){
                        javaPath = str;//����java·��Ϊ��Ϸ������׼��
                        ui->javaPathLineEdit->setText(str);//��·���Զ�����Java·����
                        javaPathOk = true;
                        break;
                    }
                }
            }

            if(!javaPathOk) QMessageBox::information(this,QString::fromLocal8Bit("������Ϣ"),QString::fromLocal8Bit("δ�ҵ������ֶ����á�"));
        }
        else QMessageBox::information(this,QString::fromLocal8Bit("������Ϣ"),QString::fromLocal8Bit("δ�ҵ������ֶ����á�"));
}

//����ļ��ۺ���
void SettingDialog::browseSlot(){
    QString fileName=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("ѡ��javaw.exe"),QDir::currentPath());
    qDebug()<<fileName;

    QStringList fileNameStrList = fileName.split("/");
    QString fileNameLast = fileNameStrList.at(fileNameStrList.size() - 1);
    qDebug()<<fileNameLast;

    fileName.remove("/" + fileNameLast);

    ui->javaPathLineEdit->setText(fileName);//��·���Զ�����Java·����
    fileName = fileName.replace("/","\\",Qt::CaseSensitive);
    qDebug()<<fileName;
    javaPath = fileName;//����java·��Ϊ��Ϸ������׼��
}

//�����˳��ۺ���
void SettingDialog::saveAndExitSlot(){
    //��ȡ�������
    QString name = ui->playerNameLineEdit->text();
    if(name.isEmpty()){
        QMessageBox::information(this,QString::fromLocal8Bit("������Ϣ"),QString::fromLocal8Bit("������Ʋ���Ϊ��"));
        return;
    }
    playerName = name;//�洢�������

    if(ui->ManualSetRadioButton->isChecked()){
        QString mem = ui->maxMemLineEdit->text();
        if(mem.isEmpty()){
            QMessageBox::information(this,QString::fromLocal8Bit("������Ϣ"),QString::fromLocal8Bit("�ֶ��ڴ��С����Ϊ��"));
            return;
        }
        maxMem = mem;
    }
    else maxMem = "1024";//�Զ���������ڴ�Ϊ1024

    QString path = ui->javaPathLineEdit->text();
    if(path.isEmpty())  autoRearchJavaPathSlot();  //���û������java·�����Զ�����


    //��������Ϣ�����������ļ��У�ʹ��JSON
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

     close();//�ر����ô���
}

//�������
void SettingDialog::aboutSoftSlot(){

    InfoDialog *aboutSoftDia = new InfoDialog();

    aboutSoftDia->setInfo(QString::fromLocal8Bit("���ߣ�BlueCat"),QString::fromLocal8Bit("MC�汾��1.7.10"),QString::fromLocal8Bit("����汾��1.3.1.20151223_Alpha"));
    //aboutSoftDia->setWindowFlags(Qt::WindowStaysOnTopHint);
    //aboutSoftDia->show();
    aboutSoftDia->exec();
}
