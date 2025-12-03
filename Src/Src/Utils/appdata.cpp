#include "appdata.h"

//将视频面板窗体指针放全局(需要的地方直接用就好)
frmVideoPanel *AppData::videoPanel = 0;

QString AppData::Version = "V20250102";
int AppData::MapWidth = 800;
int AppData::MapHeight = 600;
bool AppData::IsMove = false;
bool AppData::IsReboot = false;
QDateTime AppData::LastLiveTime = QDateTime::currentDateTime();
QString AppData::CurrentUrl = "";
QString AppData::CurrentImage = "bg_alarm.jpg";

int AppData::RowHeight = 25;
int AppData::LeftWidth = 160;
int AppData::RightWidth = 200;
int AppData::TopHeight = 80;
int AppData::BottomHeight = 0;
int AppData::ToolHeight = 40;
int AppData::DeviceWidth = 75;
int AppData::DeviceHeight = 35;

int AppData::BtnMinWidth = 70;
int AppData::BtnMinHeight = 55;
int AppData::BtnIconSize = 18;
int AppData::BorderWidth = 4;
int AppData::SwitchBtnWidth = 63;
int AppData::SwitchBtnHeight = 25;

QString AppData::MarkerImage = "../mapimage/ipc_red.png";
int AppData::MarkerWidth = 25;
int AppData::MarkerHeight = 30;

QStringList AppData::GpsDeviceNames = QStringList();
QStringList AppData::GpsDevicePoints = QStringList();
QStringList AppData::GpsDeviceColors = QStringList();

QStringList AppData::BtnMainTexts = QStringList();
QStringList AppData::BtnConfigTexts = QStringList();

QString AppData::MapPath = QString();
QString AppData::SoundPath = QString();
QList<QString> AppData::MapNames = QList<QString>();
QList<QString> AppData::SoundNames = QList<QString>();
QList<QString> AppData::PortNames = QList<QString>();
