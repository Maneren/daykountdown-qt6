// SPDX-FileCopyrightText: 2024
// SPDX-LicenseRef: GPL-3.0-or-later

#include "kountdownservice.h"
#include "constants.h"

#include <QFileDialog>
#include <QSaveFile>
#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QSqlQuery>
#include <QVariant>
#include <KLocalizedString>

using namespace DayKountdown;

// ==================== KountdownExporter ====================

KountdownExporter::KountdownExporter(QObject *parent) 
	: QObject(parent)
{
}

QJsonDocument KountdownExporter::createJsonFromDatabase() const
{
	QJsonArray kountdownsArray;
	
	QSqlQuery query(QStringLiteral("SELECT * FROM %1").arg(Database::TABLE_NAME));
	while(query.next()) {
		const QJsonObject kountdown {
			{Database::Columns::NAME_NAME, query.value(Database::Columns::NAME).toString()},
			{Database::Columns::DESCRIPTION_NAME, query.value(Database::Columns::DESCRIPTION).toString()},
			{Database::Columns::DATE_NAME, query.value(Database::Columns::DATE).toString()},
			{Database::Columns::COLOUR_NAME, query.value(Database::Columns::COLOUR).toString()}
		};
		kountdownsArray.append(kountdown);
	}
	
	const QJsonObject root {{Files::JSON_KEY_KOUNTDOWNS, kountdownsArray}};
	return QJsonDocument(root);
}

void KountdownExporter::exportToFile()
{
	const QString fileName = QFileDialog::getSaveFileName(
		nullptr, 
		i18n("Save File As"), 
		Files::DEFAULT_EXPORT_NAME, 
		Files::JSON_FILTER
	);
	
	if (fileName.isEmpty()) {
		return; // User cancelled
	}
	
	const QJsonDocument jsonDoc = createJsonFromDatabase();
	QSaveFile file(fileName);
	
	if (file.open(QIODevice::WriteOnly)) {
		file.write(jsonDoc.toJson());
		file.commit();
	}
}

// ==================== KountdownImporter ====================

KountdownImporter::KountdownImporter(QObject *parent) 
	: QObject(parent)
{
}

QVariantList KountdownImporter::importedKountdowns() const
{
	QVariantList result;
	result.reserve(m_importedData.size());
	
	for (const KountdownData& data : m_importedData) {
		QVariantMap variant;
		variant[QStringLiteral("name")] = data.name;
		variant[QStringLiteral("description")] = data.description;
		variant[QStringLiteral("date")] = data.date;
		variant[QStringLiteral("colour")] = data.colour;
		result.append(variant);
	}
	
	return result;
}

void KountdownImporter::parseJsonDocument(const QJsonDocument& doc)
{
	m_importedData.clear();
	
	const QJsonObject rootObj = doc.object();
	const QJsonArray kountdownsArray = rootObj.value(Files::JSON_KEY_KOUNTDOWNS).toArray();
	
	for (const QJsonValue& kountdownValue : kountdownsArray) {
		const QJsonObject obj = kountdownValue.toObject();
		
		KountdownData data{
			.name = obj.value(Database::Columns::NAME_NAME).toString(),
			.description = obj.value(Database::Columns::DESCRIPTION_NAME).toString(),
			.date = obj.value(Database::Columns::DATE_NAME).toString(),
			.colour = obj.contains(Database::Columns::COLOUR_NAME) 
				? obj.value(Database::Columns::COLOUR_NAME).toString() 
				: UI::Colors::DEFAULT
		};
		
		m_importedData.append(data);
	}
}

void KountdownImporter::importFromFile()
{
	const QUrl filePath = QFileDialog::getOpenFileUrl(nullptr, i18n("Import file"));
	QFile inFile(filePath.toLocalFile());
	
	if (!inFile.exists()) {
		return;
	}
	
	if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	
	const QByteArray data = inFile.readAll();
	inFile.close();
	
	QJsonParseError errorPtr;
	const QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
	
	if (doc.isNull()) {
		qCritical() << "Failed to parse JSON:" << errorPtr.errorString();
		return;
	}
	
	parseJsonDocument(doc);
	Q_EMIT importCompleted();
}
