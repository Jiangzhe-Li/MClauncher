#include "mclauncher.h"
#include "ui_mclauncher.h"

//��Ϸ��������ȫ�ֱ���
QString playerName;//�������
QString   maxMem;//����ڴ�
QString javaPath;  //javaw.exe����·��

MClauncher::MClauncher(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MClauncher)
{
    ui->setupUi(this);

    QObject::connect(ui->minimize,SIGNAL(clicked()),this,SLOT(showMinimized()));//��С����ť
    QObject::connect(ui->exit,SIGNAL(clicked()),this,SLOT(close()));//�˳���ť
    QObject::connect(ui->startGame,SIGNAL(clicked()),this,SLOT(startGameSlot()));//������Ϸ��ť
    QObject::connect(ui->setting,SIGNAL(clicked()),this,SLOT(settingDialogSlot()));//���ð�ť

    //����version�İ汾����
    QDir versionDir;
    versionDir.setPath(".minecraft\\versions");

    QStringList verDirStrList = versionDir.entryList(QDir::NoFilter,QDir::NoSort);

     //�ж��Ƿ�����Ϸ�ļ�
    if(!(verDirStrList.size() > 2)){

        QMessageBox::information(this,QString::fromLocal8Bit("������Ϣ"),QString::fromLocal8Bit("δ����Ϸ�ļ���"));

        ui->startGame->setEnabled(false);//��ֹ�����а�ť
        ui->setting->setEnabled(false);//��ֹ���ð�ť
        ui->verSecComboBox->setEnabled(false);//��ֹ�汾ѡ��
    }
    else{
        //ȡ��Ŀ¼�е� "." �� ".."Ŀ¼
        verDirStrList.removeFirst();//�Ƴ���.��Ŀ¼
        verDirStrList.removeFirst();//�Ƴ���..��Ŀ¼

        ui->verSecComboBox->addItems(verDirStrList);//��ʾ�汾��
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

//�����öԻ���
void MClauncher::settingDialogSlot()
{
    SettingDialog *settingDialog = new SettingDialog;

    settingDialog->setWindowTitle(QString::fromLocal8Bit("����"));
    settingDialog->setWindowFlags(Qt::FramelessWindowHint);
    settingDialog->setModal(true);
    settingDialog->show();
}

//������Ϸ��ť
void MClauncher::startGameSlot(){

    //����ʱ���������ļ�
    QFile loadFile(QStringLiteral("MClauncher.cfg"));

     if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug("Couldn't open save file.");
        QMessageBox::information(this,QString::fromLocal8Bit("������Ϣ"),QString::fromLocal8Bit("δ�ҵ������ļ����������ý�����Ϸ����"));
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

                //����ѡ��Ҫ�����İ汾
                QString mcVersion;//��Ϸ�汾
                mcVersion = ui->verSecComboBox->currentText();
                qDebug()<<mcVersion;

                //������Ϸ
               int nativeError = checkNativesDir();//���natives�ļ��Ƿ����

               if(nativeError == 3)  QMessageBox::information(this,QString::fromLocal8Bit("������Ϣ"),QString::fromLocal8Bit("�Ҳ���unzip32.exe��") + nativeError);

               if(!nativeError){
                   int errorCode = MClaunch(playerName, maxMem, javaPath,mcVersion);
                    if(errorCode == 0){
                        close();//�ر�������
                    }
                    else{
                        QMessageBox::information(this,QString::fromLocal8Bit("������Ϣ"),QString::fromLocal8Bit("������룺") + errorCode);
                    }
               }
     }
}
