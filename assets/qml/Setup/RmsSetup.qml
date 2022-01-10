import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import "../Widgets" as Widgets
Control {

    //
    // Custom properties
    //
    property int setupMargin: 0
    property int displayedWidth: 380 + app.spacing * 1.5
    readonly property int maxItemWidth: column.width - 2 * spacing


    //
    // Window
    //
    Widgets.Window {
        gradient: true
        title: qsTr("设置")
        anchors.fill: parent
        anchors.leftMargin: 0
        headerDoubleClickEnabled: false
        icon.source: "qrc:/icons/settings.svg"
        anchors.margins: (app.spacing * 1.5) - 5
        backgroundColor: Cpp_ThemeManager.paneWindowBackground

        //
        // Control arrangement
        //
        ColumnLayout {
            id: column
            anchors.fill: parent
            spacing: app.spacing / 2
            anchors.margins: app.spacing * 1.5

            //
            // Comm mode selector
            //
            Label {
                font.bold: true
                text: qsTr("操作方式") + ":"
            }


            //
            // Spacer
            //
            Item {
                height: app.spacing / 2
            }


            //
            // Spacer
            //
            Item {
                height: app.spacing / 2
            }

            //
            // Tab bar
            //
            TabBar {
                height: 24
                id: tab
                Layout.fillWidth: true
                Layout.maximumWidth: root.maxItemWidth
                onCurrentIndexChanged: {
                    if (currentIndex < 2 && currentIndex !== Cpp_IO_Manager.dataSource)
                        Cpp_IO_Manager.dataSource = currentIndex
                }

                TabButton {
                    text: qsTr("串口")
                    height: tab.height + 3
                    width: implicitWidth + 2 * app.spacing
                }

                TabButton {
                    text: qsTr("网络")
                    height: tab.height + 3
                    width: implicitWidth + 2 * app.spacing
                }


            }

            //
            // Tab bar contents
            //
            StackLayout {
                id: stack
                clip: true
                Layout.fillWidth: true
                Layout.fillHeight: false
                currentIndex: tab.currentIndex
                Layout.topMargin: -parent.spacing - 1
                Layout.minimumHeight: implicitHeight
                Layout.maximumHeight: implicitHeight
                Layout.preferredHeight: implicitHeight
                onCurrentIndexChanged: updateHeight()
                Component.onCompleted: updateHeight()

                function updateHeight() {
                    stack.implicitHeight = 0

                    switch (currentIndex) {
                    case 0:
                        stack.implicitHeight = stackLayout_1.implicitHeight
                        break
                    case 1:
                        stack.implicitHeight = stackLayout_1.implicitHeight
                        break
                    case 2:
                        stack.implicitHeight = stackLayout_1.implicitHeight
                        break
                    case 3:
                        stack.implicitHeight = stackLayout_1.implicitHeight
                        break
                    default:
                        stack.implicitHeight = 0
                        break
                    }
                }

                 Control{
                     id:stackLayout_1
                     implicitHeight: layout.implicitHeight+app.spacing*2
                 ColumnLayout{
                     id: layout
                     anchors.fill: parent
                     anchors.margins: app.spacing
                 GridLayout{
                     columns: 2
                     Layout.fillWidth: true
                     rowSpacing: app.spacing
                     columnSpacing: app.spacing

                     Label{text:"feedPlot"}
                     Button{text: "FEED"
                     onClicked: {
                     for(var i=0;i<30000;i++){feedxy(i,i)}
                     }
                     }

                     Label{text:"serialtest"}
                     Button{text: "open";onClicked:cpp_SerialPort.serial_open()}
                     //
                     // Spacer
                     //
                     Item {
                         Layout.fillHeight: true
                         Layout.minimumHeight: app.spacing
                     }

                     //
                     // Spacer
                     //
                     Item {
                         Layout.fillHeight: true
                     }

                 }

                 }
                 }


            }

            //
            // Vertical spacer
            //
            Item {
                Layout.fillHeight: true
            }
        }
    }


}
