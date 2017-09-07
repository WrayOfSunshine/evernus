/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick.Layouts 1.3
import QtQml.Models 2.2
import QtQuick 2.7
import QtQml 2.2

import "qrc:/qml/Industry/Manufacturing"

DelegateModel {
    property bool isOutput: false

    id: mainModel

    delegate: RowLayout {
        ColumnLayout {
            Repeater {
                id: materials

                onItemAdded: {
                    var connection = Qt.createQmlObject("
import com.evernus.qmlcomponents 2.6

BezierCurve {
    anchors.fill: parent
    antialiasing: true
    color: '#9a9a9a'
}", connections);

                    function setCurveSourcePoints() {
                        connection.p1 = Qt.point(0, (item.y + item.height / 2) / connections.height);
                        connection.p2 = Qt.point(0.5, (item.y + item.height / 2) / connections.height);
                    }

                    connection.p3 = Qt.point(0.5, 0.5);
                    connection.p4 = Qt.point(1, 0.5);

                    setCurveSourcePoints();
                    connections.heightChanged.connect(setCurveSourcePoints);
                }
            }
        }

        Item {
            id: connections
            width: 50
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }

        Type {
            id: type
            isOutput: mainModel.isOutput
        }

        Timer {
            id: childrenDelayTimer
            interval: 0

            onTriggered: materials.model = childrenLoader.item
        }

        Loader {
            id: childrenLoader
            asynchronous: true

            onLoaded: childrenDelayTimer.running = true
        }

        Component.onCompleted: {
            if (model && model.hasModelChildren) {
                childrenLoader.setSource("TypeDelegateModel.qml", {
                    "model": mainModel.model,
                    "rootIndex": mainModel.modelIndex(index)
                });
            }
        }
    }
}
