// SPDX-FileCopyrightText: 2024
// SPDX-LicenseRef: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QJsonDocument>

// Simple data structure for import/export operations
struct KountdownData {
	QString name;
	QString description;
	QString date;
	QString colour;
};

// Service for exporting kountdowns to JSON
class KountdownExporter : public QObject
{
	Q_OBJECT
	
public:
	explicit KountdownExporter(QObject *parent = nullptr);
	virtual ~KountdownExporter() override = default;
	
	Q_INVOKABLE void exportToFile();
	
private:
	[[nodiscard]] QJsonDocument createJsonFromDatabase() const;
};

// Service for importing kountdowns from JSON
class KountdownImporter : public QObject
{
	Q_OBJECT
	
public:
	explicit KountdownImporter(QObject *parent = nullptr);
	virtual ~KountdownImporter() override = default;
	
	Q_INVOKABLE void importFromFile();
	Q_INVOKABLE QVariantList importedKountdowns() const;
	
Q_SIGNALS:
	void importCompleted();
	
private:
	QList<KountdownData> m_importedData;
	
	void parseJsonDocument(const QJsonDocument& doc);
};
