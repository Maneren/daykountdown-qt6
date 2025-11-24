/*
 *   SPDX-FileCopyrightText: 2019 David Edmundson <davidedmundson@kde.org>
 *   From kirigami-addons: https://invent.kde.org/libraries/kirigami-addons/-/blob/master/src/dateandtime/DatePicker.qml
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

//This class serves as an encapsulation of the QQC1 calendar so that it can be replaced at any time
import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

/**
 * A large date picker
 *
 * Use case is for picking a date and visualising that in
 * context of a calendar view
 */
// FocusScope {
//     property alias selectedDate: calendar.selectedDate
//
//     implicitWidth: calendar.implicitWidth
//     implicitHeight: calendar.implicitHeight
//     Layout.fillWidth: true
//
//     Calendar {
//         id: calendar
//         frameVisible: false
//         weekNumbersVisible: false
//         selectedDate: new Date()
//
//         focus: true
//         //style stuff here?
//         width: parent.width
//     }
// }

ColumnLayout {
    id: datePicker

    property date selectedDate: new Date()
    property alias month: grid.month
    property alias year: grid.year
    property alias locale: grid.locale

    Component.onCompleted: {
        grid.month = selectedDate.getMonth()
        grid.year = selectedDate.getFullYear()
    }

    RowLayout {
        Layout.fillWidth: true
        
        Button {
            text: "<"
            onClicked: {
                if (grid.month === 0) {
                    grid.month = 11
                    grid.year--
                } else {
                    grid.month--
                }
            }
        }
        
        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: Qt.locale().monthName(grid.month) + " " + grid.year
        }
        
        Button {
            text: ">"
            onClicked: {
                if (grid.month === 11) {
                    grid.month = 0
                    grid.year++
                } else {
                    grid.month++
                }
            }
        }
    }

    DayOfWeekRow {
        locale: grid.locale
        Layout.fillWidth: true
        
        delegate: Text {
            text: model.shortName
            font: grid.font
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Kirigami.Theme.textColor
        }
    }

    MonthGrid {
        id: grid
        month: new Date().getMonth()
        year: new Date().getFullYear()
        locale: Qt.locale()
        Layout.fillWidth: true

        delegate: AbstractButton {
            required property var model
            property MonthGrid control: grid
            property bool isCurrentItem: model.date.getTime() === selectedDate.getTime()
            
            width: grid.width / 7
            height: width
            
            opacity: model.month === control.month ? 1 : 0.5
            
            contentItem: Text {
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: model.day
                font: control.font
                color: isCurrentItem ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            }
            
            background: Rectangle {
                anchors.fill: parent
                anchors.margins: 2
                radius: 4
                visible: isCurrentItem
                color: Kirigami.Theme.highlightColor
            }
            
            onClicked: {
                selectedDate = model.date
            }
        }
    }
}
