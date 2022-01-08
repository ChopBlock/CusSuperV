/*
 * Copyright (c) 2020-2021 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import QtQuick 2.3
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

import Qt.labs.settings 1.0


import "../Windows"
import "../Widgets"

import "../FramelessWindow" as FramelessWindow
import "../PlatformDependent" as PlatformDependent
import "../DashBoard"
import "../Setup"
FramelessWindow.CustomWindow {
    id: root

    //
    // Quit application when this window is closed
    //
    onClosed: Qt.quit()

    //
    // Customize window border
    //
    showIcon: true
    borderWidth: 1
    borderColor: Qt.darker(Cpp_ThemeManager.toolbarGradient2, 1.5)






    //
    // Window geometry
    //
    visible: false
    title: Cpp_AppName
    width: minimumWidth
    height: minimumHeight
    minimumWidth: 1120 + 2 * root.shadowMargin
    backgroundColor: Cpp_ThemeManager.windowBackground
    minimumHeight: 650 + 2 * root.shadowMargin + root.titlebar.height

    //
    // Startup code
    //
    Component.onCompleted: {


        // Show app window
        if (root.isFullscreen)
            root.showFullScreen()
        else if (root.isMaximized)
            root.showMaximized()
        else {
            // Fix maximize not working on first try on macOS & Windows
            root.opacity = 0
            var x = root.x
            var y = root.y
            var w = root.width
            var h = root.height
            root.showMaximized()
            root.showNormal()
            root.setGeometry(x, y, w,h)
            root.opacity = 1
        }

        // Force active focus
        root.requestActivate()
        root.requestUpdate()

    }




    //
    // Save window size & position
    //
    Settings {
        property alias wx: root.x
        property alias wy: root.y
        property alias ww: root.width
        property alias wh: root.height
        property alias wm: root.isMaximized
        property alias wf: root.isFullscreen


    }





    //
    // Main layout
    //
    Page {
        clip: true
        anchors.fill: parent
        anchors.margins: root.shadowMargin
        palette.text: Cpp_ThemeManager.text
        palette.buttonText: Cpp_ThemeManager.text
        palette.windowText: Cpp_ThemeManager.text
        //anchors.topMargin: menubarLayout.height + root.shadowMargin

        background: Rectangle {
            radius: root.radius
            color: Cpp_ThemeManager.windowBackground
        }

        ColumnLayout {
            spacing: 0
            anchors.fill: parent


            //
            // Application toolbar
            //
            Item {

                id: toolbar

                z: titlebar.z
                Layout.fillWidth: true
                Layout.minimumHeight: 48
                Layout.maximumHeight: 48

                Rectangle {  anchors.fill: parent
                                radius: 3
                                border.width: 1
                                color: "transparent"
                                border.color: "#040600"
                                opacity: parent.checked ? 0.2 : 0.0

                                Rectangle {

                                    border.width: 1
                                    color: "#626262"
                                    anchors.fill: parent
                                    border.color: "#c2c2c2"
                                    radius: parent.radius - 1
                                    anchors.margins: parent.border.width
                                }
                            }
            }

            //
            //
            //
            RowLayout {
                spacing:0
                Layout.fillWidth: true
                Layout.fillHeight: true

                StackView {
                    id: stack
                    clip: true
                    initialItem: rmsplot
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    hoverEnabled: true
                    data: [

                        RmsPlot{
                            id:rmsplot
                            width: parent.width
                            height: parent.height
                        }



                    ]
                }

                RmsSetup{
                    Layout.fillHeight: true
                    Layout.rightMargin: setupMargin
                    Layout.minimumWidth: displayedWidth
                    Layout.maximumWidth: displayedWidth
                }

            }


        }
    }

function feedxy(x,y){
rmsplot.addxy(x,y)
}

    //
    // Resize handler
    //
    FramelessWindow.ResizeHandles {
        window: root
        anchors.fill: parent
        handleSize: root.handleSize
    }
}
