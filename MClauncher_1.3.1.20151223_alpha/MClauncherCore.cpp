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


//参数全局变量
QString currentDir;
QStringList classPathList;
QString mainClassStr;//主类
QString launchArgStr;//启动参数字符串
QString versionID;
QString assets;

/*
 传入参数说明：
QString playerName;//玩家名字
QString   maxMem;//最大内存
QString javaPath;  //javaw.exe所在路径
QString versionDirStr;//要启动的游戏版本
*/

int MClaunch(QString playerName, QString maxMem, QString javaPath, QString versionDirStr){

        //获取当前程序路径
        currentDir = QDir::currentPath();
        currentDir.replace("/", "\\", Qt::CaseSensitive);//格式化路径
        //qDebug()<<currentDir;

        //java路径格式转换
        javaPath.replace("\\", "\\\\", Qt::CaseSensitive);
        QString program = javaPath+"\\\\javaw.exe";
        //qDebug()<<program;        

        //解析MC的JSON文件
        //解析JSON实例
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
                    //获取启动参数
                    QJsonValue launchArgJsonValue = obj.take("minecraftArguments");
                    launchArgStr = launchArgJsonValue.toString();

                    //获取主类所在jar文件
                    QJsonValue mainClassJsonValue = obj.take("mainClass");
                    mainClassStr = mainClassJsonValue.toString();

                    //获取版本ID
                    QJsonValue versionIDJsonValue = obj.take("id");
                    versionID = versionIDJsonValue.toString();

                    //获取assets
                    QJsonValue assetsJsonValue = obj.take("assets");
                    assets = assetsJsonValue.toString();

                    //获取启动相关library文件
                    QJsonValue libJsonValue = obj.take("libraries");
                    QJsonArray libJsonArray = libJsonValue.toArray();

                    for(int i=0;i<libJsonArray.size();i++){
                        QJsonValue subLibJsonValue = libJsonArray.at(i);
                        QJsonObject subLibJsonObj = subLibJsonValue.toObject();

                        if(subLibJsonObj.contains("natives")){
                            QJsonValue libNatives = subLibJsonObj.take("natives");
                            QJsonObject nativeJsonObj = libNatives.toObject();
                            QJsonValue nativeJsonValue = nativeJsonObj.take("windows");

                            //举例：nativeFileName = natives-windows
                           QString nativeFileName = nativeJsonValue.toString();

                           //qDebug()<<nativeFileName;

                            QJsonValue nativeName = subLibJsonObj.take("name");
                            //举例：org.lwjgl.lwjgl:lwjgl-platform:2.9.1
                            QString nativeNameStr = nativeName.toString();
                            //目录要求：
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

        arguments << "-Xmx"+maxMem+"m";//设置最大内存

        arguments<<"-Dfml.ignoreInvalidMinecraftCertificates=true";
        arguments<<"-Dfml.ignorePatchDiscrepancies=true";
        arguments<<"-Djava.library.path=.minecraft\\natives";

        //指定版本jar所在文件
        classPathList<<("\\.minecraft\\versions\\" + versionDirStr + "\\" + versionDirStr + ".jar");

        QString classpath;
        for(int i=0;i<classPathList.size();i++){
            classpath += (currentDir+classPathList.at(i));
            if(i < classPathList.size() - 1) classpath += ";";
        }

        arguments<<"-cp";
        arguments<<classpath;

        //传递主类
        arguments<<mainClassStr;

        //启动参数launchArgStr
        QStringList launchStrList = launchArgStr.split(" ");

        //查找关键参数，修改相关参数
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


        // MC启动
        QObject *p=0;
        QProcess *myProcess = new QProcess(p);
        myProcess->start(program, arguments);

        return 0;
}


int checkNativesDir(){

    //启动启动器时判断是否存在natives文件夹
    QDir nativesDir;
    nativesDir.setPath(".minecraft");

    bool ok = nativesDir.exists("natives");

    qDebug()<<"have natives?:"<<ok;
    //不存在的话，创建natives文件夹，并解压文件到该目录
    if(!ok){

         //判断该目录下是否有unzip32.exe的存在
         QString unzipProcessStr = "platforms\\unzip32.exe";
         QFile unzipFile(unzipProcessStr);
         if(!unzipFile.exists()){
                 return 3;
          }

          InfoDialog *nativeDia = new InfoDialog();
          nativeDia->setInfo("",QString::fromLocal8Bit("未找到natives，正在解压文件..."),"");
          nativeDia->setWindowFlags(Qt::WindowStaysOnTopHint);
          nativeDia->show();

          nativesDir.mkdir("natives");

          QStringList nativesDirStrList;

                    //查找version的版本个数
                    QDir versionDir;
                    versionDir.setPath(".minecraft\\versions");
                    QStringList verDirStrList = versionDir.entryList(QDir::NoFilter,QDir::NoSort);

                    qDebug()<<verDirStrList;

                    //暂时取出最后一个版本的JSON文件路径
                    QString versionDirStr = verDirStrList.at(verDirStrList.size() -1);

                    qDebug()<<versionDirStr;

                    //解析MC的JSON文件
                    //解析JSON实例
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


                                //获取启动相关library文件
                                QJsonValue libJsonValue = obj.take("libraries");
                                QJsonArray libJsonArray = libJsonValue.toArray();

                                for(int i=0;i<libJsonArray.size();i++){
                                    QJsonValue subLibJsonValue = libJsonArray.at(i);
                                    QJsonObject subLibJsonObj = subLibJsonValue.toObject();

                                    if(subLibJsonObj.contains("natives")){
                                        QJsonValue libNatives = subLibJsonObj.take("natives");
                                        QJsonObject nativeJsonObj = libNatives.toObject();
                                        QJsonValue nativeJsonValue = nativeJsonObj.take("windows");

                                        //举例：nativeFileName = natives-windows
                                       QString nativeFileName = nativeJsonValue.toString();

                                       //qDebug()<<nativeFileName;

                                        QJsonValue nativeName = subLibJsonObj.take("name");
                                        //举例：org.lwjgl.lwjgl:lwjgl-platform:2.9.1
                                        QString nativeNameStr = nativeName.toString();
                                        //目录要求：
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

                    //将路径中含有平台相关的去掉
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

                    //解压相关文件到natives目录
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
