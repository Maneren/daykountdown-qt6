/*
* SPDX-FileCopyrightText: (C) 2021 Carl Schwan <carl@carlschwan.eu>
* SPDX-FileCopyrightText: (C) 2021 Claudio Cambra <claudio.cambra@gmail.com>
* 
* SPDX-LicenseRef: GPL-3.0-or-later
*/

#include <QApplication>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QtQml>

#include <QUrl>
#include <QIcon>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <KLocalizedContext>
#include <KAboutData>
#include <KLocalizedString>

#include "kountdownmodel.h"
#include "importexport.h"
#include "aboutdatapasser.h"
#include "daykountdownconfig.h"
#include "constants.h"

using namespace DayKountdown;

/* #ifdefs are ifs that affect the preprocessor.
 * We can use this to compile specific chunks of code depending on the platform!
 */
#ifdef Q_OS_ANDROID
Q_DECL_EXPORT 
#endif
int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Material"));
#else
	// QApplication handles initialisation and includes extensive functionality
	QApplication app(argc, argv);
	if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif

	KLocalizedString::setApplicationDomain("daykountdown");

	const KAboutData about(
		QStringLiteral("daykountdown"), 
		i18nc("@title", "DayKountdown"), 
		QStringLiteral("0.1"),
		i18nc("@title", "A day countdown application"),
		KAboutLicense::GPL_V3,
		{},
		{},
		{},
		QStringLiteral("claudio.cambra@gmail.com")
	);
	
	KAboutData aboutData = about;
	aboutData.addAuthor(
		i18nc("@info:credit", "Claudio Cambra"), 
		i18nc("@info:credit", "Creator"), 
		QStringLiteral("claudio.cambra@gmail.com")
	);
	aboutData.addAuthor(
		i18nc("@info:credit", "Carl Schwan"), 
		i18nc("@info:credit", "SQLite pro and code review")
	);

	KAboutData::setApplicationData(aboutData);
	QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.daykountdown")));
	
	auto* config = DayKountdownConfig::self();

	Q_ASSERT(QSqlDatabase::isDriverAvailable(Database::DRIVER));
	Q_ASSERT(QDir().mkpath(QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))));
	
	QSqlDatabase db = QSqlDatabase::addDatabase(Database::DRIVER);
	const auto path = QDir::cleanPath(
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + 
		QStringLiteral("/") + qApp->applicationName()
	);
	db.setDatabaseName(path);
	if (!db.open()) {
		qCritical() << db.lastError() << "while opening database at" << path;
	}

	QCommandLineParser parser;
	aboutData.setupCommandLine(&parser);
	parser.process(app);
	aboutData.processCommandLine(&parser);

	QQmlApplicationEngine engine;
	
	AboutDataPasser aboutDataPasser;
	aboutDataPasser.setAboutData(aboutData);
	
	qmlRegisterSingletonInstance("org.kde.daykountdown.private", 1, 0, "KountdownModel", new KountdownModel(qApp));
	qmlRegisterSingletonInstance("org.kde.daykountdown.private", 1, 0, "ImportExport", new ImportExport());
	qmlRegisterSingletonInstance("org.kde.daykountdown.private", 1, 0, "AboutData", &aboutDataPasser);
	qmlRegisterSingletonInstance("org.kde.daykountdown.private", 1, 0, "Config", config);

	engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	if (engine.rootObjects().isEmpty()) {
		return -1;
	}
	
	return app.exec();
}
