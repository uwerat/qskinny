import QtQuick 2.0
import Skinny 1.0 as Qsk
import Shapes 1.0
import "qrc:/qml"

Qsk.Window
{
    visible: true
    width: 800
    height: 600

    color: "Gray"

    Qsk.LinearBox
    {
        id: pageLinear

        orientation: Qt.Horizontal
        dimension: 2

        Shape
        {
            figure: Shape.Hexagon
            border: "indigo"

            gradient:
            ({
                linear: { x1: 0, y1: 0, x2: 0.2, y2: 0.5 },
                spreadMode: Qsk.Gradient.ReflectSpread,

                // PhonixStart
                stops: [
                    { position: 0.0, color: "#f83600" },
                    { position: 1.0, color: "#f9d423" }
                ]
            })
        }

        Shape
        {
            figure: Shape.Star
            border: "black"

            gradient:
            ({
                linear: { x1: 0, y1: 0, x2: 0.05, y2: 0.1 },
                spreadMode: Qsk.Gradient.RepeatSpread,

                stops: [
                    { position: 0.5, color: "RoyalBlue" },
                    { position: 0.5, color: "LemonChiffon" }
                ]
            })  
        }

        Shape
        {
            figure: Shape.Rectangle
            border: "black"

            gradient:
            ({
                linear: { x1: 0.3, y1: 0.7, x2: 0.75, y2: 0.3 },
    
                stops: [
                    { position: 0.5, color: "MediumVioletRed" },
                    { position: 0.5, color: "Navy" }
                ]
            })
        }
    }

    Qsk.LinearBox
    {
        id: pageConic

        orientation: Qt.Horizontal
        dimension: 2

        Shape
        {
            figure: Shape.Ellipse
            border: "black"

            gradient:
            ({
                conic: { x: 0.5, y: 0.5, startAngle: 30, spanAngle: 60 },
                spreadMode: Qsk.Gradient.ReflectSpread,

                // JuicyPeach
                stops: [
                    { position: 0.0, color: "#ffecd2" },
                    { position: 1.0, color: "#fcb69f" }
                ]
            })
        }

       Shape
        {
            figure: Shape.TriangleUp
            border: "black"

            gradient:
            ({
                conic: { x: 0.5, y: 0.5, startAngle: 30, spanAngle: 60 },
                spreadMode: Qsk.Gradient.RepeatSpread,

                // WinterNeva
                stops: [
                    { position: 0.0, color: "#a1c4fd" },
                    { position: 1.0, color: "#c2e9fb" }
                ]
            })
        }

       Shape
        {
            figure: Shape.Arc
            border: "black"

            gradient:
            ({
                conic: { x: 0.5, y: 0.5, startAngle: 300, spanAngle: -240 },

                // SpikyNaga
                stops: [
                    { position: 0.00, color: "#505285" },
                    { position: 0.12, color: "#585e92" },
                    { position: 0.25, color: "#65689f" },
                    { position: 0.37, color: "#7474b0" },
                    { position: 0.50, color: "#7e7ebb" },
                    { position: 0.62, color: "#8389c7" },
                    { position: 0.75, color: "#9795d4" },
                    { position: 0.87, color: "#a2a1dc" },
                    { position: 1.00, color: "#b5aee4" }
                ]
            })
        }

        Shape
        {
            figure: Shape.Diamond

            gradient:
            ({
                conic: { x: 0.5, y: 0.5, startAngle: 45, spanAngle: 180 },
                spreadMode: Qsk.Gradient.ReflectSpread,

                // FabledSunset
                stops: [
                    { position: 0.00, color: "#231557" },
                    { position: 0.29, color: "#44107a" },
                    { position: 0.67, color: "#ff1361" },
                    { position: 1.00, color: "#fff800" }
                ]
            })
        }
    }

    Qsk.LinearBox
    {
        id: pageRadial

        orientation: Qt.Horizontal
        dimension: 2

        Shape
        {
            figure: Shape.Rectangle
            border: "indigo"

            gradient:
            ({
                radial: { x: 0.7, y: 0.3, radiusX: 0.25, radiusY: 0.0 },

                stops: [
                    { position: 0.0, color: "LightYellow" },
                    { position: 1.0, color: "MidnightBlue" }
                ]
            })
        }

        Shape
        {
            figure: Shape.Ellipse
            border: "black"

            gradient:
            ({
                radial: { x: 0.5, y: 0.5, radiusX: 0.5, radiusY: 0.5 },
                spreadMode: Qsk.Gradient.PadSpread,

                stops: [
                    { position: 0.0, color: "lime" },
                    { position: 0.2, color: "lime" },
                    { position: 0.2, color: "red" },
                    { position: 0.4, color: "red" },
                    { position: 0.4, color: "yellow" },
                    { position: 0.6, color: "yellow" },
                    { position: 0.6, color: "cyan" },
                    { position: 0.8, color: "cyan" },
                    { position: 0.8, color: "darkcyan" },
                    { position: 1.0, color: "darkcyan" }
                ]
            })
        }

        Shape
        {
            figure: Shape.Rectangle
            border: "indigo"

            gradient:
            ({
                radial: { x: 0.5, y: 0.7, radiusX: 0.25, radiusY: 0.25 },
                spreadMode: Qsk.Gradient.RepeatSpread,

                // LilyMeadow
                stops: [
					{ position: 0.00, color: "#65379b" },
					{ position: 0.53, color: "#886aea" },
					{ position: 1.00, color: "#6457C6" }
                ]
            })
        }

        Shape
        {
            figure: Shape.Rectangle
            border: "indigo"

            gradient:
            ({
                radial: { x: 0.6, y: 0.4, radiusX: 0.1, radiusY: 0.1 },
                spreadMode: Qsk.Gradient.ReflectSpread,

                stops: [
                    { position: 0.0, color: "red" },
                    { position: 1.0, color: "blue" }
                ]
            })
        }
    }

    Qsk.TabView
    {
        margins { left: 10; top: 10; right: 10; bottom: 10 }
        autoFitTabs: true
        tabBarEdge: Qt.TopEdge

        Component.onCompleted:
        {
            addTab( "Radial Gradients", pageRadial );
            addTab( "Conic Gradients", pageConic );
            addTab( "Linear Gradients", pageLinear );
        }
    }
}
