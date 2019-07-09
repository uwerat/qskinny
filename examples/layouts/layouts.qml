import QtQuick 2.0
import Skinny 1.0
import Test 1.0

GridBox
{
    margins: 10
    autoFillBackground : true

    background
    {
        stops: [
            { position: 0.0, color: "White" },
            { position: 1.0, color: "White" },
        ]
    }

    TestRectangle
    {
        id: paleVioletRed
        color: "PaleVioletRed"
    }

    TestRectangle
    {
        id: darkSeaGreen
        color: "DarkSeaGreen"
    }

    TestRectangle
    {
        id: skyBlue
        color: "SkyBlue"
    }

    TestRectangle
    {
        id: navajoWhite
        color: "NavajoWhite"
    }

    Component.onCompleted:
    {
        addItem( paleVioletRed, 0, 0, 1, 2 );
        addItem( darkSeaGreen, 1, 0, 2, 1 );
        addItem( skyBlue, 2, 1, 1, 1 );
        addItem( navajoWhite, 0, 2, -1, 1 );

        setRowStretchFactor( 0, 1 );
        setRowStretchFactor( 1, 2 );
        setRowStretchFactor( 2, 1 );
    }
}

