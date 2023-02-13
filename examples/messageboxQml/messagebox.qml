import QtQuick 2.0 
import Skinny 1.0 as Qsk
import "qrc:/qml"

Qsk.Window 
{
    id: window

    visible: true
    width: 600
    height: 400
    // color: "Beige"

    Qsk.LinearBox
    {
        orientation: Qt.Horizontal
        dimension: 2

        margins { left: 10; top: 10; right: 10; bottom: 10 }
        //margins: 10

        Qsk.PushButton
        {
            id: messageButton
            text: "Message"

            focus: true

            sizePolicy.vertical: Qsk.SizePolicy.MinimumExpanding

            onClicked:
            {
                var answer = Qsk.Dialog.message(
                    "Message", "Request vector, over.",
                    Qsk.StandardSymbol.NoSymbol, Qsk.Dialog.Close
                );
            }
        }

        Qsk.PushButton
        {
            id: informationButton
            text: "Information"

            sizePolicy.vertical: Qsk.SizePolicy.MinimumExpanding

            onClicked:
            {
                var answer = Qsk.Dialog.information(
                    "Information", "We have clearance, Clarence."
                );
            }
        }

        Qsk.PushButton
        {
            id: questionButton
            text: "Question"

            sizePolicy.vertical: Qsk.SizePolicy.MinimumExpanding

            onClicked:
            {
                var answer = Qsk.Dialog.question(
                    "Question", "Roger, Roger. Do we have a vector, Victor ?", 
                    Qsk.Dialog.Yes | Qsk.Dialog.No
                )

                if ( answer == Qsk.Dialog.Yes )
                    console.log( "Yes" )
                else
                    console.log( "No" )
            }
        }

        Qsk.PushButton
        {
            id: warningButton
            text: "Warning"

            sizePolicy.vertical: Qsk.SizePolicy.MinimumExpanding

            onClicked:
            {
                var answer = Qsk.Dialog.warning(
                    "Warning", "We have clearance, Clarence."
                )
            } 
        }

        Qsk.PushButton
        {
            id: criticalButton
            text: "Critical"

            sizePolicy.vertical: Qsk.SizePolicy.MinimumExpanding

            onClicked:
            {
                var answer = Qsk.Dialog.critical(
                    "Critical", "That's Clarence Oveur. Over."
                );
            }
        }

        Qsk.PushButton
        {   
            id: selectionButton
            text: "Selection"
            
            sizePolicy.vertical: Qsk.SizePolicy.MinimumExpanding
            
            onClicked:
            {   
                var hotTracks = [
                    "Give Me More",
                    "Gimme Gimme Your Love",
                    "1-2-3-4 Red Light",
                    "New York",
                    "If You Walk Away",
                    "Eloise",
                    "On The Radio",
                    "We Are The Teens",
                    "Never Gonna Tell No Lie To You",
                    "Walking On Sunshine ",
                    "Get Out Of My Mind",
                    "Cover Girl ",
                    "Here I Stand",
                    "Gypsy Caravan",
                    "It´s Good To Have A Friend",
                    "We´ll Have A Party Tonite ´nite",
                    "Automatic World", 
                    "Gimme Gimme Gimme Gimme Gimme Your Love"
                ]

                var hottestTrackEver = Qsk.Dialog.select(
                    "Here we go ...", "Hot Hot Hot", hotTracks, 7 )

                console.log( hottestTrackEver )
            }
        }
    }

    Qsk.FocusIndicator
    {
    }
}
