import QtQuick
import QtCharts 2.3
import "../Widgets"
import QtQuick.Controls
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
        anchors.margins: (app.spacing * 1.5) - 5

        borderColor: Cpp_ThemeManager.widgetWindowBorder



        ChartView{

            id:chart_view
            anchors {
                fill: parent
                topMargin: window.headerHeight
                leftMargin: window.borderWidth
                rightMargin: window.borderWidth
                bottomMargin: window.borderWidth
            }
            title: "RMSPLOT"
            antialiasing: true
            dropShadowEnabled: true
            animationOptions: ChartView.NoAnimation

            ValuesAxis{id:line_y ;min:0;max:5000}
            ValuesAxis{id:line1_x ;min:0;max:5000}

            LineSeries{
                id:line1
                name: qsTr("拉曼强度曲线")
                useOpenGL: true
                width: 2
                axisY: line_y
                axisX:line1_x
              //  onClicked:(ppoint)=>{ console.log("onClicked: " + ppoint.x + ", " + ppoint.y);}





            }




        }


    }
    function addxy(x,y){


        line1.append(x,y)
        line_y.max=5000
        line1_x.max=5000

      //  console.log(x,y)
    }



}
