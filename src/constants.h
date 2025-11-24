// SPDX-FileCopyrightText: 2024
// SPDX-LicenseRef: GPL-3.0-or-later

#pragma once

#include <QString>

namespace DayKountdown {

namespace Database {
	inline const QString DRIVER = QStringLiteral("QSQLITE");
	inline const QString TABLE_NAME = QStringLiteral("KountdownModel");
	
	namespace Columns {
		inline constexpr int ID = 0;
		inline constexpr int NAME = 1;
		inline constexpr int DESCRIPTION = 2;
		inline constexpr int DATE = 3;
		inline constexpr int DATE_IN_MS = 4;
		inline constexpr int COLOUR = 5;
		
		inline const QString ID_NAME = QStringLiteral("id");
		inline const QString NAME_NAME = QStringLiteral("name");
		inline const QString DESCRIPTION_NAME = QStringLiteral("description");
		inline const QString DATE_NAME = QStringLiteral("date");
		inline const QString DATE_IN_MS_NAME = QStringLiteral("dateInMs");
		inline const QString COLOUR_NAME = QStringLiteral("colour");
	}
}

namespace UI {
	namespace Colors {
		inline const QString CRIMSON = QStringLiteral("crimson");
		inline const QString CORAL = QStringLiteral("coral");
		inline const QString GOLDENROD = QStringLiteral("goldenrod");
		inline const QString LIGHTSEAGREEN = QStringLiteral("lightseagreen");
		inline const QString DEEPSKYBLUE = QStringLiteral("deepskyblue");
		inline const QString HOTPINK = QStringLiteral("hotpink");
		inline const QString DEFAULT = QStringLiteral("palette.text");
	}
}

namespace Files {
	inline const QString DEFAULT_EXPORT_NAME = QStringLiteral("exported_kountdowns.json");
	inline const QString JSON_FILTER = QStringLiteral("JSON (*.json)");
	inline const QString JSON_KEY_KOUNTDOWNS = QStringLiteral("kountdowns");
}

} // namespace DayKountdown
