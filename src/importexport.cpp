/*
* SPDX-FileCopyrightText: (C) 2021 Claudio Cambra <claudio.cambra@gmail.com>
* 
* SPDX-LicenseRef: GPL-3.0-or-later
*/

#include "importexport.h"
#include "constants.h"
#include <QErrorMessage>
#include <qsqlquery.h>

using namespace DayKountdown;

// Constructor function
ImportExport::ImportExport(QObject *parent) : QObject(parent)
{

}

// Function extracts kountdowns from SQLite table and converts to JSON array
QJsonDocument ImportExport::createJson() const {
	QJsonArray kountdownsJsonArr;
	
	QSqlQuery query(QStringLiteral("SELECT * FROM %1").arg(Database::TABLE_NAME));
	while(query.next()) {
		const QJsonObject kountdownToAdd {
			{Database::Columns::NAME_NAME, query.value(Database::Columns::NAME).toString()},
			{Database::Columns::DESCRIPTION_NAME, query.value(Database::Columns::DESCRIPTION).toString()},
			{Database::Columns::DATE_NAME, query.value(Database::Columns::DATE).toString()},
			{Database::Columns::COLOUR_NAME, query.value(Database::Columns::COLOUR).toString()}
		};
		kountdownsJsonArr.append(kountdownToAdd);
	}
	
	const QJsonObject mainObj {{Files::JSON_KEY_KOUNTDOWNS, kountdownsJsonArr}};
	return QJsonDocument(mainObj);
}

void ImportExport::exportFile() {
	const QString fileName = QFileDialog::getSaveFileName(nullptr, i18n("Save File As"), 
	                                                       Files::DEFAULT_EXPORT_NAME, 
	                                                       Files::JSON_FILTER);
	const QJsonDocument jsonDoc = createJson();
	QSaveFile file(fileName);
	if(file.open(QIODevice::WriteOnly)) {
		file.write(jsonDoc.toJson());
		file.commit();
	}
}

void ImportExport::fetchKountdowns() {
	m_kountdownArray.clear();
	
	const QUrl filePath = QFileDialog::getOpenFileUrl(nullptr, i18n("Import file"));
	QFile inFile(filePath.toLocalFile());
	
	if(!inFile.exists()) {
		return;
	}
	
	if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	
	const QByteArray data = inFile.readAll();
	inFile.close();
	
	QJsonParseError errorPtr;
	const QJsonDocument kountdownsDoc = QJsonDocument::fromJson(data, &errorPtr);
	if(kountdownsDoc.isNull()) {
		qCritical() << "Failed to parse JSON:" << errorPtr.errorString();
		return;
	}
	
	const QJsonObject rootObj = kountdownsDoc.object();
	const QJsonArray kountdownsJsonArray = rootObj.value(Files::JSON_KEY_KOUNTDOWNS).toArray();
	
	int i = 0;
	for (const QJsonValue& kountdownJson : kountdownsJsonArray) {
		const QJsonObject obj = kountdownJson.toObject();
		Kountdown currKountdown{
			.index = i,
			.name = obj.value(Database::Columns::NAME_NAME).toString(),
			.description = obj.value(Database::Columns::DESCRIPTION_NAME).toString(),
			.date = obj.value(Database::Columns::DATE_NAME).toString(),
			.colour = obj.contains(Database::Columns::COLOUR_NAME) 
			          ? obj.value(Database::Columns::COLOUR_NAME).toString() 
			          : UI::Colors::DEFAULT
		};
		m_kountdownArray.append(currKountdown);
		++i;
	}
}

QVariantList ImportExport::kountdownPopulator () {
	QVariantList kountdownsList;
	kountdownsList.reserve(m_kountdownArray.size());
	
	for(const Kountdown& k : m_kountdownArray) {
		kountdownsList << QVariant::fromValue(k);
	}
	
	return kountdownsList;
}


