#include <QApplication>
#include "mclauncher.h"
#include <QTextCodec>  //����֧��

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Qt������֧��
    QTextCodec *codec = QTextCodec::codecForName("GB2312");
    QTextCodec::setCodecForLocale(codec);
   // QTextCodec::setCodecForCStrings(codec);
    //QTextCodec::setCodecForTr(codec);

    MClauncher w;
    w.setWindowTitle(QString::fromLocal8Bit("Minecraft������"));
    w.setWindowOpacity(1);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setAttribute(Qt::WA_TranslucentBackground);
    w.show();
    w.move(200,100);

    return a.exec();
}
