import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: qsTr("Search Engine")

    // Set the Material Design theme based on the current theme property
    Material.theme: theme === "dark" ? Material.Dark : Material.Light

    property string theme: "light" // Theme switching between "light" and "dark"

    Component.onCompleted: {
        applyTheme()
    }

    // Function to apply the selected theme to the application window
    function applyTheme() {
        appWindow.color = theme === "dark" ? "#121212" : "#ffffff"
    }

    Rectangle {
        id: appWindow
        anchors.fill: parent
        color: theme === "dark" ? "#121212" : "#ffffff"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 15

            // Application header
            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: "Panda Search Engine"
                    font.pixelSize: 36
                    font.bold: true
                    color: theme === "dark" ? "#ffffff" : "#000000"
                    Layout.alignment: Qt.AlignLeft
                }

                // Theme switcher button
                Button {
                    text: theme === "dark" ? "Light Mode" : "Dark Mode"
                    Layout.alignment: Qt.AlignRight
                    width: 120
                    height: 40
                    onClicked: {
                        theme = theme === "dark" ? "light" : "dark"
                        applyTheme()
                    }
                }
            }

            // Search bar
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    id: searchBar
                    Layout.fillWidth: true
                    height: 50
                    radius: 25
                    border.color: "#E0E0E0"
                    border.width: 1
                    color: theme === "dark" ? "#333333" : "#ffffff"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10

                        // Icon
                        Image {
                            source: "qrc:/icons/panda.png"
                            width: 30
                            height: 30
                            fillMode: Image.PreserveAspectFit
                        }

                        // Input field for search queries
                        TextField {
                            id: searchField
                            placeholderText: qsTr("Enter your search query")
                            font.pixelSize: 16
                            Layout.fillWidth: true
                            background: null
                            color: theme === "dark" ? "#ffffff" : "#000000"
                            Keys.onReturnPressed: {
                                cppInterface.performSearch(searchField.text)
                                historyModel.append({query: searchField.text})
                            }
                        }
                    }
                }
            }

            // Search history section
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 10

                // History header
                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "Search History"
                        font.pixelSize: 18
                        font.bold: true
                        color: theme === "dark" ? "#ffffff" : "#000000"
                        Layout.alignment: Qt.AlignLeft
                    }

                    // Button to clear search history
                    Button {
                        text: "Clear All"
                        Layout.alignment: Qt.AlignRight
                        width: 120
                        height: 40
                        onClicked: {
                            if (historyModel.count > 0) {
                                historyModel.clear()
                            }
                        }
                    }
                }

                // List view for displaying search history
                ListView {
                    id: historyView
                    Layout.fillWidth: true
                    height: 150
                    spacing: 5 // Spacing between items
                    model: ListModel {
                        id: historyModel
                    }
                    delegate: Rectangle {
                        width: parent.width
                        height: 40
                        color: theme === "dark" ? "#333333" : "#f0f0f0"
                        radius: 8 // Rounded corners
                        border.color: "#888888"

                        // Display the query text
                        Text {
                            id: queryText
                            text: query
                            anchors.fill: parent
                            anchors.margins: 10 // Margins on all sides
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            color: theme === "dark" ? "#ffffff" : "#000000"
                            elide: Text.ElideRight
                            clip: true // Clip text at the boundaries
                        }

                        // Clickable area for each history item
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (query) {
                                    searchField.text = query
                                    searchField.focus = true
                                }
                            }
                        }
                    }
                }
            }

            // Search results section
            ListView {
                id: resultsView
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: resultsModel
                delegate: Rectangle {
                    width: parent.width
                    height: 50
                    color: theme === "dark" ? "#333333" : "#f9f9f9"
                    Text {
                        text: "Document ID: " + docid + ", Rank: " + rank.toFixed(2)
                        anchors.centerIn: parent
                        color: theme === "dark" ? "#ffffff" : "#000000"
                    }
                }
            }
        }
    }
}