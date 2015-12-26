#include <QApplication>
#include "mclauncher.h"
#include <QTextCodec>  //中文支持

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Qt对中文支持
    QTextCodec *codec = QTextCodec::codecForName("GB2312");
    QTextCodec::setCodecForLocale(codec);
   // QTextCodec::setCodecForCStrings(codec);
    //QTextCodec::setCodecForTr(codec);

    MClauncher w;
    w.setWindowTitle(QString::fromLocal8Bit("Minecraft启动器"));
    w.setWindowOpacity(1);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setAttribute(Qt::WA_TranslucentBackground);
    w.show();
    w.move(200,100);

    return a.exec();
}
