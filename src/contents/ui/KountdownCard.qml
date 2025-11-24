/*
* SPDX-FileCopyrightText: (C) 2021 Claudio Cambra <claudio.cambra@gmail.com>
*
* SPDX-LicenseRef: GPL-3.0-or-later
*/

import QtQuick 2.6
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.13 as Kirigami

Kirigami.Card {
	id: kountdownDelegate

	showClickFeedback: true
	onReleased: openPopulateSheet("edit", index, name, description, date, colour)

	contentItem: RowLayout {
		// spacing: Kirigami.Units.largeSpacing
		Layout.alignment: Qt.AlignVCenter
		Layout.margins: Kirigami.Units.smallSpacing

		Rectangle {
			Layout.fillHeight: true
			Layout.preferredWidth: 12
			color: colour
		}

		Kirigami.Heading {
			Layout.alignment: Qt.AlignVCenter
			level: 1
			property var daysLeft: Math.ceil((date.getTime() - nowDate.getTime()) / 86400000)
			text: daysLeft < 0 ? i18np("%1 day ago", "%1 days ago", daysLeft*-1) : i18np("%1 day", "%1 days", daysLeft)
			color: colour
		}

		ColumnLayout {
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignVCenter
			spacing: Kirigami.Units.smallSpacing

			Kirigami.Heading {
				Layout.fillWidth: true
				wrapMode: Text.Wrap
				level: 2
				text: name
			}

			Kirigami.Separator {
				Layout.fillWidth: true
				visible: description.length > 0
			}

			Controls.Label {
				Layout.fillWidth: true
				wrapMode: Text.Wrap
				text: description
				visible: description.length > 0
			}
		}
	}
}
