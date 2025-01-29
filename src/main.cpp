#include <QApplication>
#include <KAboutData>
#include <KLocalizedString>
#include "diarywindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("kdailynote");

    KAboutData aboutData(
        QStringLiteral("kdailynote"),
        i18n("KDailyNote"),
        QStringLiteral("0.1"),
        i18n("A daily note-taking application"),
        KAboutLicense::GPL_V3,
        i18n("(c) 2024")
    );

    KAboutData::setApplicationData(aboutData);

    DiaryWindow *window = new DiaryWindow();
    return app.exec();
}
