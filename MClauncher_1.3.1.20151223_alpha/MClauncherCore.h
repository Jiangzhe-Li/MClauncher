#ifndef MCLAUNCHERCORE_H
#define MCLAUNCHERCORE_H

#include <QString>

int MClaunch(QString playerName, QString maxMem, QString javaPath, QString versionDirStr);
int checkNativesDir();

#endif // MCLAUNCHERCORE_H
