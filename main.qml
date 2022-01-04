import QtQuick
import "./assets/qml/Windows" as Windows
import QtQuick.Controls 2.5

Item{
    id: app

    //
    // Global propeties
    //
    readonly property int spacing: 8
    readonly property string monoFont: "Roboto Mono"
     property Windows.MainWindow mainWindow: null
//
// MainWindow
//
Loader {
    asynchronous: true
    sourceComponent: Windows.MainWindow {
        Component.onCompleted: {

            app.forceActiveFocus()
            app.mainWindow = this
        }
    }
}
}
