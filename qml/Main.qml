import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ModbusBMS

ApplicationWindow {
    id: window
    visible: true
    width: 1024
    height: 600
    title: qsTr("Modbus BMS")
    visibility: ApplicationWindow.FullScreen
    color: Theme.backgroundColor

    property var currentPage: dashboardPage

    Header {
        id: header
        width: parent.width
        height: 60
        background: Rectangle {
            color: Theme.headerColor
        }
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            
            Label {
                text: "🏠 Modbus BMS"
                font.pixelSize: 24
                font.bold: true
                color: Theme.textColor
            }
            
            Item { Layout.fillWidth: true }
            
            Label {
                text: Qt.formatTime(new Date(), "hh:mm")
                font.pixelSize: 18
                color: Theme.textColor
            }
        }
    }

    StackLayout {
        id: stackLayout
        anchors.top: header.bottom
        anchors.bottom: footer.top
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: drawer.currentIndex
        
        DashboardPage { id: dashboardPage }
        DevicesPage { id: devicesPage }
        DeviceDetailsPage { id: deviceDetailsPage }
        AddDevicePage { id: addDevicePage }
        SettingsPage { id: settingsPage }
        LogsPage { id: logsPage }
    }

    Footer {
        id: footer
        width: parent.width
        height: 70
        background: Rectangle {
            color: Theme.footerColor
        }
    }

    Drawer {
        id: drawer
        width: Math.min(window.width * 0.7, 300)
        height: window.height
        interactive: true
        background: Rectangle {
            color: Theme.drawerColor
        }
        
        Column {
            anchors.fill: parent
            anchors.margins: 20
            
            Repeater {
                model: [
                    { name: "Dashboard", page: 0, icon: "📊" },
                    { name: "Devices", page: 1, icon: "🔌" },
                    { name: "Add Device", page: 3, icon: "➕" },
                    { name: "Settings", page: 4, icon: "⚙️" },
                    { name: "Logs", page: 5, icon: "📋" }
                ]
                
                delegate: ItemDelegate {
                    width: drawer.width - 40
                    height: 50
                    contentItem: RowLayout {
                        Label { text: modelData.icon; font.pixelSize: 20 }
                        Label { 
                            text: modelData.name
                            font.pixelSize: 16
                            color: Theme.textColor
                            elide: Text.ElideRight
                        }
                    }
                    onClicked: {
                        stackLayout.currentIndex = modelData.page
                        drawer.close()
                    }
                }
            }
        }
    }
}
