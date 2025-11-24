// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
//
// SPDX-LicenseRef: GPL-3.0-or-later

#include "kountdownmodel.h"

#include "constants.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlRecord>
#include <QStandardPaths>

using namespace DayKountdown;

KountdownModel::KountdownModel(QObject *parent)
    : QSqlTableModel(parent)
{
    if (!QSqlDatabase::database().tables().contains(Database::TABLE_NAME)) {
        const auto statement = QStringLiteral(R"RAWSTRING(
			CREATE TABLE IF NOT EXISTS %1 (
				%2 INTEGER PRIMARY KEY AUTOINCREMENT,
				%3 TEXT NOT NULL,
				%4 TEXT NOT NULL,
				%5 TEXT NOT NULL,
				%7 TEXT NOT NULL
			)
		)RAWSTRING")
                                   .arg(Database::TABLE_NAME)
                                   .arg(Database::Columns::ID_NAME)
                                   .arg(Database::Columns::NAME_NAME)
                                   .arg(Database::Columns::DESCRIPTION_NAME)
                                   .arg(Database::Columns::DATE_NAME)
                                   .arg(Database::Columns::COLOUR_NAME);

        auto query = QSqlQuery(statement);
        if (!query.exec()) {
            qCritical() << query.lastError() << "while creating table";
        }
    }

    setTable(Database::TABLE_NAME);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
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
        parentColumn = Database::Columns::ID;
        break;
    case Qt::UserRole + 2: // Name
        parentColumn = Database::Columns::NAME;
        break;
    case Qt::UserRole + 3: // Description
        parentColumn = Database::Columns::DESCRIPTION;
        break;
    case Qt::UserRole + 4: // Date
        parentColumn = Database::Columns::DATE;
        {
            const QModelIndex parentIndex = createIndex(index.row(), parentColumn);
            return QDateTime::fromString(QSqlTableModel::data(parentIndex, Qt::DisplayRole).toString(), Qt::ISODate);
        }
    case Qt::UserRole + 5: // DateInMs
        parentColumn = Database::Columns::DATE_IN_MS;
        break;
    case Qt::UserRole + 6: // Colour
        parentColumn = Database::Columns::COLOUR;
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
    for (int i = 0; i < this->record().count(); i++) {
        roles.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
    }
    return roles;
}

bool KountdownModel::addKountdown(const QString &name, const QString &description, const QDateTime &date, const QString &colour)
{
    QSqlRecord newRecord = this->record();
    newRecord.setValue(Database::Columns::NAME_NAME, name);
    newRecord.setValue(Database::Columns::DESCRIPTION_NAME, description);
    newRecord.setValue(Database::Columns::DATE_NAME, date.toString(Qt::ISODate));
    newRecord.setValue(Database::Columns::COLOUR_NAME, colour);

    return insertRecord(rowCount(), newRecord) && submitAll();
}

bool KountdownModel::editKountdown(int index, const QString &name, const QString &description, const QDateTime &date, const QString &colour)
{
    QSqlRecord record = this->record();
    record.setValue(Database::Columns::ID_NAME, index);
    record.setValue(Database::Columns::NAME_NAME, name);
    record.setValue(Database::Columns::DESCRIPTION_NAME, description);
    record.setValue(Database::Columns::DATE_NAME, date.toString(Qt::ISODate));
    record.setValue(Database::Columns::COLOUR_NAME, colour);
    return setRecord(index, record) && submitAll();
}

bool KountdownModel::removeKountdown(int index)
{
    return removeRow(index) && submitAll();
}

bool KountdownModel::removeAllKountdowns()
{
    QSqlQuery query;
    return query.exec(QStringLiteral("DELETE FROM %1").arg(Database::TABLE_NAME)) && submitAll();
}

void KountdownModel::sortModel(int sort_by)
{
    // Switch based on enum defined in kountdownmodel.h
    switch (static_cast<SortTypes>(sort_by)) {
    case SortTypes::AlphabeticalAsc:
        this->setSort(Database::Columns::NAME, Qt::AscendingOrder);
        break;
    case SortTypes::AlphabeticalDesc:
        this->setSort(Database::Columns::NAME, Qt::DescendingOrder);
        break;
    case SortTypes::DateAsc:
        this->setSort(Database::Columns::DATE, Qt::AscendingOrder);
        break;
    case SortTypes::DateDesc:
        this->setSort(Database::Columns::DATE, Qt::DescendingOrder);
        break;
    case SortTypes::CreationDesc:
        this->setSort(Database::Columns::ID, Qt::DescendingOrder);
        break;
    case SortTypes::CreationAsc:
    default:
        this->setSort(Database::Columns::ID, Qt::AscendingOrder);
        break;
    }
    select();
}
