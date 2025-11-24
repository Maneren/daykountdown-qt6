/*
* SPDX-FileCopyrightText: (C) 2021 Claudio Cambra <claudio.cambra@gmail.com>
* 
* SPDX-LicenseRef: GPL-3.0-or-later
*/

#include "importexport.h"
#include <QErrorMessage>
#include <qsqlquery.h>

// Constructor function
ImportExport::ImportExport(QObject *parent) : QObject(parent)
{

}

// Function extracts kountdowns from SQLite table and converts to JSON array
QJsonDocument ImportExport::createJson() const {
	QJsonArray kountdownsJsonArr;
	
	QSqlQuery query(QStringLiteral("SELECT * FROM KountdownModel"));
	while(query.next()) {
		const QJsonObject kountdownToAdd {
			{QStringLiteral("name"), query.value(1).toString()},
			{QStringLiteral("description"), query.value(2).toString()},
			{QStringLiteral("date"), query.value(3).toString()},
			{QStringLiteral("colour"), query.value(5).toString()}
		};
		kountdownsJsonArr.append(kountdownToAdd);
	}
	
	const QJsonObject mainObj {{QStringLiteral("kountdowns"), kountdownsJsonArr}};
	return QJsonDocument(mainObj);
}

void ImportExport::exportFile() {
	const QString fileName = QFileDialog::getSaveFileName(nullptr, i18n("Save File As"), 
	                                                       QStringLiteral("exported_kountdowns.json"), 
	                                                       QStringLiteral("JSON (*.json)"));
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
	const QJsonArray kountdownsJsonArray = rootObj.value(QStringLiteral("kountdowns")).toArray();
	
	int i = 0;
	for (const QJsonValue& kountdownJson : kountdownsJsonArray) {
		const QJsonObject obj = kountdownJson.toObject();
		Kountdown currKountdown{
			.index = i,
			.name = obj.value(QStringLiteral("name")).toString(),
			.description = obj.value(QStringLiteral("description")).toString(),
			.date = obj.value(QStringLiteral("date")).toString(),
			.colour = obj.contains(QStringLiteral("colour")) 
			          ? obj.value(QStringLiteral("colour")).toString() 
			          : QStringLiteral("palette.text")
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


