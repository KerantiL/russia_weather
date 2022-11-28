import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Shapes 1.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import com.myinc.Region 1.0

Window {
    id: root
    width: 1200
    height: 600
    color: "#ACCDEF"
    visible: true
    title: qsTr("Hello World")

    RowLayout {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 16

        TextField {
            id: textField
        }

        Button {
            id: searchButton

            property double latitude
            property double longitude

            text: "Search"

            onClicked: {
                let res = textField.text.split(",");
                latitude = res[0]
                longitude = res[1]
                core.search(res[0], res[1]);
            }
        }

        z: 1

        Connections {
            target: core

            function onSearched(temp) {
                searchPopup.latitude =  searchButton.latitude
                searchPopup.longitude = searchButton.longitude
                searchPopup.temp = temp
                searchPopup.open()
            }
        }
    }

    RowLayout {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 16
        z: 1

        Button {
            text: "Update temp"

            onClicked: core.updateTemp()
        }

        Button {
            text: "Show temp map"

            onClicked: country.setVisibleTempMap(true)
        }
    }

    Country {
        id: country
        width: parent.width - 64
        height: parent.height - 64
        anchors.centerIn: parent

        Component.onCompleted: load("/home/danil/Загрузки/Regions.json")

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            onMouseXChanged:  {
                country.getRegion(Qt.point(mouse.x, mouse.y))
            }

            onMouseYChanged:  {
                country.getRegion(Qt.point(mouse.x, mouse.y))
            }

            onClicked: {
                let info = country.getCurrentRegionInfo()
                popup.name = info["name"]
                popup.open()
            }
        }
    }

    Popup {
        id: popup
        property string name: ""

        y: 100

        contentItem: Text {
            text: popup.name
        }
    }

    Popup {
        id: searchPopup

        property string temp
        property double latitude
        property double longitude

        anchors.centerIn: parent

        contentItem: Text {
            text: "Ширина: %1\nДолгота: %2\nТемпература: %3".arg(searchPopup.latitude).arg(searchPopup.longitude).arg(searchPopup.temp)
        }
    }
}
