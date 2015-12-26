#include "MClauncherCore.h"
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QObject>
#include <QProcess>
#include <QJsonParseError>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "infodialog.h"


//����ȫ�ֱ���
QString currentDir;
QStringList classPathList;
QString mainClassStr;//����
QString launchArgStr;//���������ַ���
QString versionID;
QString assets;

/*
 �������˵����
QString playerName;//�������
QString   maxMem;//����ڴ�
QString javaPath;  //javaw.exe����·��
QString versionDirStr;//Ҫ��������Ϸ�汾
*/

int MClaunch(QString playerName, QString maxMem, QString javaPath, QString versionDirStr){

        //��ȡ��ǰ����·��
        currentDir = QDir::currentPath();
        currentDir.replace("/", "\\", Qt::CaseSensitive);//��ʽ��·��
        //qDebug()<<currentDir;

        //java·����ʽת��
        javaPath.replace("\\", "\\\\", Qt::CaseSensitive);
        QString program = javaPath+"\\\\javaw.exe";
        //qDebug()<<program;        

        //����MC��JSON�ļ�
        //����JSONʵ��
        QFile loadFile(".minecraft\\versions\\"+versionDirStr+"\\\\"+versionDirStr+".json");

        if (!loadFile.open(QIODevice::ReadOnly)) {
            qDebug("Couldn't open save file.");
            return 1;
        }

        QByteArray saveData = loadFile.readAll();
        loadFile.close();

        QJsonParseError json_error;
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData, &json_error));

        if(json_error.error == QJsonParseError::NoError)
        {
                if(loadDoc.isObject())
                {
                    QJsonObject obj = loadDoc.object();
                    //��ȡ��������
                    QJsonValue launchArgJsonValue = obj.take("minecraftArguments");
                    launchArgStr = launchArgJsonValue.toString();

                    //��ȡ��������jar�ļ�
                    QJsonValue mainClassJsonValue = obj.take("mainClass");
                    mainClassStr = mainClassJsonValue.toString();

                    //��ȡ�汾ID
                    QJsonValue versionIDJsonValue = obj.take("id");
                    versionID = versionIDJsonValue.toString();

                    //��ȡassets
                    QJsonValue assetsJsonValue = obj.take("assets");
                    assets = assetsJsonValue.toString();

                    //��ȡ�������library�ļ�
                    QJsonValue libJsonValue = obj.take("libraries");
                    QJsonArray libJsonArray = libJsonValue.toArray();

                    for(int i=0;i<libJsonArray.size();i++){
                        QJsonValue subLibJsonValue = libJsonArray.at(i);
                        QJsonObject subLibJsonObj = subLibJsonValue.toObject();

                        if(subLibJsonObj.contains("natives")){
                            QJsonValue libNatives = subLibJsonObj.take("natives");
                            QJsonObject nativeJsonObj = libNatives.toObject();
                            QJsonValue nativeJsonValue = nativeJsonObj.take("windows");

                            //������nativeFileName = natives-windows
                           QString nativeFileName = nativeJsonValue.toString();

                           //qDebug()<<nativeFileName;

                            QJsonValue nativeName = subLibJsonObj.take("name");
                            //������org.lwjgl.lwjgl:lwjgl-platform:2.9.1
                            QString nativeNameStr = nativeName.toString();
                            //Ŀ¼Ҫ��
                            //\\.minecraft\\libraries\\org\\lwjgl\\lwjgl\\lwjgl-platform\\2.9.1\\lwjgl-platform-2.9.1-natives-windows.jar
                            QString nativeDirName = nativeNameStr.section(":",1,1)+"-"+nativeNameStr.section(":",2,2)+"-"+nativeFileName+".jar";

                            QStringList nativeDirList= nativeNameStr.split(":");
                            QString nativeDir = nativeDirList.at(0);
                            nativeDir = nativeDir.replace(".","\\\\",Qt::CaseSensitive);
                            nativeDir = "\\\\.minecraft\\\\libraries\\\\" + nativeDir+"\\\\"+nativeDirName;

                            classPathList<<nativeDir;
                        }
                        else{

                            QJsonValue libNameJsonValue = subLibJsonObj.take("name");
                            QString nameStr = libNameJsonValue.toString();

                            // libFileName = realms\\1.2.4\\realms-1.2.4.jar
                            QString libFileName = nameStr.section(":",1,1)+"\\\\"+nameStr.section(":",2,2)+"\\\\"+nameStr.section(":",1,1)+"-"+nameStr.section(":",2,2)+".jar";

                            QStringList libFileDirList= nameStr.split(":");
                            QString libFileDir = libFileDirList.at(0);
                            libFileDir = libFileDir.replace(".","\\\\",Qt::CaseSensitive);
                            libFileDir = libFileDir+"\\\\"+libFileName;

                            libFileDir = "\\\\.minecraft\\\\libraries\\\\"+libFileDir;

                            classPathList<<libFileDir;
                        }
                    }
                }
        }
        else {
            qDebug("Failed to analyze JSON");
            return 2;
        }


        QStringList arguments;

        arguments << "-Xmx"+maxMem+"m";//��������ڴ�

        arguments<<"-Dfml.ignoreInvalidMinecraftCertificates=true";
        arguments<<"-Dfml.ignorePatchDiscrepancies=true";
        arguments<<"-Djava.library.path=.minecraft\\natives";

        //ָ���汾jar�����ļ�
        classPathList<<("\\.minecraft\\versions\\" + versionDirStr + "\\" + versionDirStr + ".jar");

        QString classpath;
        for(int i=0;i<classPathList.size();i++){
            classpath += (currentDir+classPathList.at(i));
            if(i < classPathList.size() - 1) classpath += ";";
        }

        arguments<<"-cp";
        arguments<<classpath;

        //��������
        arguments<<mainClassStr;

        //��������launchArgStr
        QStringList launchStrList = launchArgStr.split(" ");

        //���ҹؼ��������޸���ز���
        for(int i=0;i<launchStrList.size();){
            QString tmp = launchStrList.at(i);

            if(tmp.contains("--username",Qt::CaseSensitive)){
                launchStrList.replace(i+1, playerName);
            }
            else if(tmp.contains("--version",Qt::CaseSensitive)){
                 launchStrList.replace(i+1, versionID);
            }
            else if(tmp.contains("--assetIndex",Qt::CaseSensitive)){
                launchStrList.replace(i+1, assets);
            }
            else if(tmp.contains("--gameDir",Qt::CaseSensitive)){
                 launchStrList.replace(i+1, ".minecraft");
            }
            else if(tmp.contains("--assetsDir",Qt::CaseSensitive)){
                launchStrList.replace(i+1, ".minecraft\\assets");
            }
            else if(tmp.contains("--userProperties",Qt::CaseSensitive)){
                launchStrList.replace(i+1, "{}");
            }
            else if(tmp.contains("--userType",Qt::CaseSensitive)){
                launchStrList.replace(i+1, "legacy");
            }

            i = i+2;
        }

        arguments += launchStrList;

        qDebug()<<arguments;


        // MC����
        QObject *p=0;
        QProcess *myProcess = new QProcess(p);
        myProcess->start(program, arguments);

        return 0;
}


int checkNativesDir(){

    //����������ʱ�ж��Ƿ����natives�ļ���
    QDir nativesDir;
    nativesDir.setPath(".minecraft");

    bool ok = nativesDir.exists("natives");

    qDebug()<<"have natives?:"<<ok;
    //�����ڵĻ�������natives�ļ��У�����ѹ�ļ�����Ŀ¼
    if(!ok){

         //�жϸ�Ŀ¼���Ƿ���unzip32.exe�Ĵ���
         QString unzipProcessStr = "platforms\\unzip32.exe";
         QFile unzipFile(unzipProcessStr);
         if(!unzipFile.exists()){
                 return 3;
          }

          InfoDialog *nativeDia = new InfoDialog();
          nativeDia->setInfo("",QString::fromLocal8Bit("δ�ҵ�natives�����ڽ�ѹ�ļ�..."),"");
          nativeDia->setWindowFlags(Qt::WindowStaysOnTopHint);
          nativeDia->show();

          nativesDir.mkdir("natives");

          QStringList nativesDirStrList;

                    //����version�İ汾����
                    QDir versionDir;
                    versionDir.setPath(".minecraft\\versions");
                    QStringList verDirStrList = versionDir.entryList(QDir::NoFilter,QDir::NoSort);

                    qDebug()<<verDirStrList;

                    //��ʱȡ�����һ���汾��JSON�ļ�·��
                    QString versionDirStr = verDirStrList.at(verDirStrList.size() -1);

                    qDebug()<<versionDirStr;

                    //����MC��JSON�ļ�
                    //����JSONʵ��
                    QFile loadFile(".minecraft\\versions\\"+versionDirStr+"\\\\"+versionDirStr+".json");

                    if (!loadFile.open(QIODevice::ReadOnly)) {
                        qDebug("Couldn't open save file.");
                    }

                    QByteArray saveData = loadFile.readAll();
                    loadFile.close();

                    QJsonParseError json_error;
                    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData, &json_error));

                    if(json_error.error == QJsonParseError::NoError)
                    {
                            if(loadDoc.isObject())
                            {
                                QJsonObject obj = loadDoc.object();


                                //��ȡ�������library�ļ�
                                QJsonValue libJsonValue = obj.take("libraries");
                                QJsonArray libJsonArray = libJsonValue.toArray();

                                for(int i=0;i<libJsonArray.size();i++){
                                    QJsonValue subLibJsonValue = libJsonArray.at(i);
                                    QJsonObject subLibJsonObj = subLibJsonValue.toObject();

                                    if(subLibJsonObj.contains("natives")){
                                        QJsonValue libNatives = subLibJsonObj.take("natives");
                                        QJsonObject nativeJsonObj = libNatives.toObject();
                                        QJsonValue nativeJsonValue = nativeJsonObj.take("windows");

                                        //������nativeFileName = natives-windows
                                       QString nativeFileName = nativeJsonValue.toString();

                                       //qDebug()<<nativeFileName;

                                        QJsonValue nativeName = subLibJsonObj.take("name");
                                        //������org.lwjgl.lwjgl:lwjgl-platform:2.9.1
                                        QString nativeNameStr = nativeName.toString();
                                        //Ŀ¼Ҫ��
                                        //.minecraft\\libraries\\org\\lwjgl\\lwjgl\\lwjgl-platform\\2.9.1\\lwjgl-platform-2.9.1-natives-windows.jar
                                        QString nativeDirName = "\\" + nativeNameStr.section(":",1,1)+"\\"+nativeNameStr.section(":",2,2)+"\\" + nativeNameStr.section(":",1,1)+"-"+nativeNameStr.section(":",2,2)+"-"+nativeFileName+".jar";



                                        QStringList nativeDirList= nativeNameStr.split(":");
                                        QString nativeDir = nativeDirList.at(0);
                                        nativeDir = nativeDir.replace(".","\\",Qt::CaseSensitive);
                                        nativeDir = ".minecraft\\libraries\\" + nativeDir+nativeDirName;

                                        nativesDirStrList<<nativeDir;
                                    }
                                }
                            }
                    }
                    else  qDebug("Failed to analyze JSON");

                    //��·���к���ƽ̨��ص�ȥ��
                    QString nStr;
                    QStringList nTmpStrList;
                    for(int i=0;i<nativesDirStrList.size();i++){
                        nStr = nativesDirStrList.at(i);
                        if(!nStr.contains("${arch}", Qt::CaseInsensitive)){
                            nTmpStrList << nStr;
                        }
                    }

                    nativesDirStrList = nTmpStrList;

                    qDebug()<<nativesDirStrList;

                    //��ѹ����ļ���nativesĿ¼
                   QObject *p=0;
                    QProcess *myProcess = new QProcess(p);                  

                    QStringList unzipArgList;

                    for(int i=0;i<nativesDirStrList.size();i++){

                        unzipArgList.clear();
                        unzipArgList << nativesDirStrList.at(i);
                        unzipArgList << "-d";
                        unzipArgList << ".minecraft\\natives";

                        qDebug()<<unzipArgList;

                        myProcess->execute(unzipProcessStr, unzipArgList);

                    }

                   delete p;
                    delete myProcess;

                  nativeDia->close();
                    delete nativeDia;
                }

     return 0;
}
