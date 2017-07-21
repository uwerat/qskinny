import QtQuick 2.0
import Skinny 1.0 as Qsk

Qsk.PushButton 
{
    sizePolicy
    {
        horizontalPolicy: Qsk.SizePolicy.Ignored
        verticalPolicy: Qsk.SizePolicy.Ignored
    }

    corner
    {
        mode: Qt.RelativeSize
        radius: 10
    }

    onClicked: 
    {
        console.log( "Clicked" )
    }

    onPressed: 
    {
        console.log( "Pressed" )
    }

    onReleased: 
    {
        console.log( "Released" )
    }

    onCanceled: 
    {
        console.log( "Canceled" )
    }

    onToggled:
    {
        console.log( "Toggled" )
    }
}
