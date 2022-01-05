import QtQuick 2.0
import QtCharts 2.3
import "../Widgets"
Item {
    id:plot_item

    Window{
        //
        // Window properties
        //
        id: window
        anchors.fill: plot_item

        gradient: true
        title: qsTr("Data")

        icon.source: "qrc:/assets/icons/dataset.svg"


        borderColor: Cpp_ThemeManager.widgetWindowBorder

        ValuesAxis{id:line_y }
        ValuesAxis{id:line1_x}

        ChartView{
            anchors {
                fill: parent
                topMargin: window.headerHeight
                leftMargin: window.borderWidth
                rightMargin: window.borderWidth
                bottomMargin: window.borderWidth
            }
            title: "RMSPLOT"
            antialiasing: true

            LineSeries{
               id:line1
               name: qsTr("拉曼强度曲线")
               useOpenGL: true
               width: 1
               axisY: line_y
               axisX:line1_x
                       }


        }

    }
    function addxy(x,y){

        line1.clear()
        line1.append(x,y)


    }



}
