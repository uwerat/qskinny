import QtQuick 2.0
import Skinny 1.0 as Qsk

Qsk.PushButton 
{
    sizePolicy
    {
        // avoid the effect of long texts
        horizontal: Qsk.SizePolicy.Ignored
        vertical: Qsk.SizePolicy.Ignored
    }

    minimumSize
    {
        width: 80
        height: 60
    }

    shape
    {
        sizeMode: Qt.RelativeSize
        aspectRatioMode: Qt.KeepAspectRatio
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
