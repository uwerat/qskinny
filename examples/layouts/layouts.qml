import QtQuick 2.0
import Skinny 1.0
import Test 1.0

GridBox
{
    //margins: 10 // only possible with Qt <= 6.1 || Qt >= 6.5
    margins { left: 10; top: 10; right: 10; bottom: 10 }

    background:
    ({
        linear: { x1: 0, y1: 0, x2: 1, y2: 1 }, // diagonal

        stops: [
            { position: 0.0, color: "Red" },
            { position: 1.0, color: "Yellow" }
        ]
    })

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

