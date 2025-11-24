// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
//
// SPDX-LicenseRef: GPL-3.0-or-later

#include "kountdownmodel.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QSqlRecord>
#include <QSqlError>

KountdownModel::KountdownModel(QObject *parent)
	: QSqlTableModel(parent)
{
	// If database does not contain KountdownModel table, then create it
	if (!QSqlDatabase::database().tables().contains(QStringLiteral("KountdownModel"))) {
		const auto statement = QStringLiteral(R"RAWSTRING(
			CREATE TABLE IF NOT EXISTS KountdownModel (
				id INTEGER PRIMARY KEY AUTOINCREMENT,
				name TEXT NOT NULL,
				description TEXT NOT NULL,
				date TEXT NOT NULL,
				dateInMs INTEGER NOT NULL,
				colour TEXT NOT NULL
			)
		)RAWSTRING");
		auto query = QSqlQuery(statement);
		// QSqlQuery returns false if query was unsuccessful
		if (!query.exec()) {
			qCritical() << query.lastError() << "while creating table";
		}
	}
	
	//QSqlQuery("DROP TABLE KountdownModel");
	
	// Sets data table on which the model is going to operate
	setTable(QStringLiteral("KountdownModel"));
	// All changed will be cached in the model until submitAll() ot revertAll() is called
	setEditStrategy(QSqlTableModel::OnManualSubmit);
	// Populates the model with data from the table set above
	select();
}

// Returns value for the specified item and role (important when accessed by QML)
QVariant KountdownModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::EditRole) {
		return QSqlTableModel::data(index, Qt::EditRole);
	}
	
	// Map role to column index
	int parentColumn = 0;
	switch (role) {
		case Qt::UserRole + 1: // ID
			parentColumn = 0;
			break;
		case Qt::UserRole + 2: // Name
			parentColumn = 1;
			break;
		case Qt::UserRole + 3: // Description
			parentColumn = 2;
			break;
		case Qt::UserRole + 4: // Date
			parentColumn = 3;
			{
				const QModelIndex parentIndex = createIndex(index.row(), parentColumn);
				return QDateTime::fromString(QSqlTableModel::data(parentIndex, Qt::DisplayRole).toString(), Qt::ISODate);
			}
		case Qt::UserRole + 5: // DateInMs
			parentColumn = 4;
			break;
		case Qt::UserRole + 6: // Colour
			parentColumn = 5;
			break;
		default:
			return QSqlTableModel::data(index, role);
	}
	
	const QModelIndex parentIndex = createIndex(index.row(), parentColumn);
	return QSqlTableModel::data(parentIndex, Qt::DisplayRole);
}

QHash<int, QByteArray> KountdownModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	for (int i = 0; i < this->record().count(); i ++) {
		roles.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
	}
	return roles;
}

bool KountdownModel::addKountdown(const QString& name, const QString& description, const QDateTime& date, const QString& colour)
{
	QSqlRecord newRecord = this->record();
	newRecord.setValue(QStringLiteral("Name"), name);
	newRecord.setValue(QStringLiteral("Description"), description);
	newRecord.setValue(QStringLiteral("Date"), date.toString(Qt::ISODate));
	newRecord.setValue(QStringLiteral("DateInMs"), date.toMSecsSinceEpoch());
	newRecord.setValue(QStringLiteral("Colour"), colour);
	
	return insertRecord(rowCount(), newRecord) && submitAll();
}

bool KountdownModel::editKountdown(int index, const QString& name, const QString& description, const QDateTime& date, const QString& colour)
{
	QSqlRecord record = this->record();
	record.setValue(QStringLiteral("ID"), index);
	record.setValue(QStringLiteral("Name"), name);
	record.setValue(QStringLiteral("Description"), description);
	record.setValue(QStringLiteral("Date"), date.toString(Qt::ISODate));
	record.setValue(QStringLiteral("DateInMs"), date.toMSecsSinceEpoch());
	record.setValue(QStringLiteral("Colour"), colour);
	return setRecord(index, record) && submitAll();
}

bool KountdownModel::removeKountdown(int index)
{
	return removeRow(index) && submitAll();
}

bool KountdownModel::removeAllKountdowns()
{
	QSqlQuery query;
	return query.exec(QStringLiteral("DELETE FROM KountdownModel")) && submitAll();
}

void KountdownModel::sortModel(int sort_by) {
	// Switch based on enum defined in kountdownmodel.h
	switch(static_cast<SortTypes>(sort_by)) {
		case SortTypes::AlphabeticalAsc:
			this->setSort(1, Qt::AscendingOrder);
			break;
		case SortTypes::AlphabeticalDesc:
			this->setSort(1, Qt::DescendingOrder);
			break;
		case SortTypes::DateAsc:
			this->setSort(3, Qt::AscendingOrder);
			break;
		case SortTypes::DateDesc:
			this->setSort(3, Qt::DescendingOrder);
			break;
		case SortTypes::CreationDesc:
			this->setSort(0, Qt::DescendingOrder);
			break;
		case SortTypes::CreationAsc:
		default:
			this->setSort(0, Qt::AscendingOrder);
			break;
	}
	select();
}
