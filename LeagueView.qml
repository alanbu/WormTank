import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    id: leagueWindow
    visible: true
    width: 600
    height: 400

    ColumnLayout
    {
        anchors.fill: parent

        TableView
        {
            Layout.preferredWidth: 600
            Layout.preferredHeight: 300

            TableViewColumn {
                    role: "teamName"
                    title: "Team"
                    width: 180
                }

            TableViewColumn {
                    role: "played"
                    title: "Played"
                    width: 50
                    horizontalAlignment: Text.AlignHCenter
            }

            TableViewColumn {
                    role: "won"
                    title: "Won"
                    width: 50
                    horizontalAlignment: Text.AlignHCenter
            }

            TableViewColumn {
                role: "drew"
                title: "Drew"
                width: 50
                horizontalAlignment: Text.AlignHCenter
            }

            TableViewColumn {
                role: "lost"
                title: "Lost"
                width: 50
                horizontalAlignment: Text.AlignHCenter
            }

            TableViewColumn {
                role: "scoreFor"
                title: "For"
                width: 50
                horizontalAlignment: Text.AlignHCenter
            }

            TableViewColumn {
                role: "scoreAgainst"
                title: "Agnst"
                width: 50
                horizontalAlignment: Text.AlignHCenter
            }

            TableViewColumn {
                role: "scoreDifference"
                title: "Diff"
                width: 50
                horizontalAlignment: Text.AlignHCenter
            }

            TableViewColumn {
                role: "points"
                title: "Points"
                width: 50
                horizontalAlignment: Text.AlignHCenter
            }

            model: wormTank.league.table
        }

        GridLayout
        {
            columns: 5
            Text
            {
                text:"Current Match"
                font.bold: true
            }
            Text
            {
                Layout.row: 1
                text: wormTank.league.currentTeam1
            }
            Text
            {
                Layout.row: 1
                Layout.column: 2
                text: "v"
            }
            Text
            {
                Layout.row: 1
                Layout.column: 4
                text: wormTank.league.currentTeam2
            }
            Text
            {
                Layout.row: 2
                text:"Previous Match"
                font.bold: true
            }
            Text
            {
                Layout.row: 3
                text: wormTank.league.previousTeam1
            }
            Text
            {
                text: wormTank.league.previousTeam1Score
            }

            Text
            {
                text: "v"
            }
            Text
            {
                text: wormTank.league.previousTeam2Score
            }
            Text
            {
                text: wormTank.league.previousTeam2
            }
        }
    }
}

