#include "QskHctColor.h"

#include <qmath.h>
#include <cmath>

/*
    Code has been copied, rearranged and translated to C++ without trying
    to understand the implemented algorithms. All credits go to the authors of
    https://github.com/material-foundation/material-color-utilities.
 */

/*
    Copyright 2021 Google LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

namespace
{
    class XYZ
    {
      public:
        constexpr XYZ() noexcept = default;

        constexpr XYZ( double x, double y, double z ) noexcept
            : x( x )
            , y( y )
            , z( z )
        {
        }

        double value( int axis ) const
        {
            switch( axis )
            {
                case 0:
                    return x;
                case 1:
                    return y;
                case 2:
                    return z;
            }

            return 0.0;
        }

        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
    };

    class ViewingConditions
    {
      public:

        const double backgroundYTowhitePointY = 0.18418651851244416;

        const double aw = 29.980997194447337;
        const double nbb = 1.0169191804458755;
        const double z = 1.909169568483652;
        const double fl = 0.38848145378003529;

        const XYZ rgbD = { 1.02117770275752, 0.98630772942801237, 0.93396050828022992 };
    };
}

static const XYZ Y_FROM_LINRGB = { 0.2126, 0.7152, 0.0722 };

static double planes[] =
{
    0.015176349177441876,
    0.045529047532325624,
    0.07588174588720938,
    0.10623444424209313,
    0.13658714259697685,
    0.16693984095186062,
    0.19729253930674434,
    0.2276452376616281,
    0.2579979360165119,
    0.28835063437139563,
    0.3188300904430532,
    0.350925934958123,
    0.3848314933096426,
    0.42057480301049466,
    0.458183274052838,
    0.4976837250274023,
    0.5391024159806381,
    0.5824650784040898,
    0.6277969426914107,
    0.6751227633498623,
    0.7244668422128921,
    0.775853049866786,
    0.829304845476233,
    0.8848452951698498,
    0.942497089126609,
    1.0022825574869039,
    1.0642236851973577,
    1.1283421258858297,
    1.1946592148522128,
    1.2631959812511864,
    1.3339731595349034,
    1.407011200216447,
    1.4823302800086415,
    1.5599503113873272,
    1.6398909516233677,
    1.7221716113234105,
    1.8068114625156377,
    1.8938294463134073,
    1.9832442801866852,
    2.075074464868551,
    2.1693382909216234,
    2.2660538449872063,
    2.36523901573795,
    2.4669114995532007,
    2.5710888059345764,
    2.6777882626779785,
    2.7870270208169257,
    2.898822059350997,
    3.0131901897720907,
    3.1301480604002863,
    3.2497121605402226,
    3.3718988244681087,
    3.4967242352587946,
    3.624204428461639,
    3.754355295633311,
    3.887192587735158,
    4.022731918402185,
    4.160988767090289,
    4.301978482107941,
    4.445716283538092,
    4.592217266055746,
    4.741496401646282,
    4.893568542229298,
    5.048448422192488,
    5.20615066083972,
    5.3666897647573375,
    5.5300801301023865,
    5.696336044816294,
    5.865471690767354,
    6.037501145825082,
    6.212438385869475,
    6.390297286737924,
    6.571091626112461,
    6.7548350853498045,
    6.941541251256611,
    7.131223617812143,
    7.323895587840543,
    7.5195704746346665,
    7.7182615035334345,
    7.919981813454504,
    8.124744458384042,
    8.332562408825165,
    8.543448553206703,
    8.757415699253682,
    8.974476575321063,
    9.194643831691977,
    9.417930041841839,
    9.644347703669503,
    9.873909240696694,
    10.106627003236781,
    10.342513269534024,
    10.58158024687427,
    10.8238400726681,
    11.069304815507364,
    11.317986476196008,
    11.569896988756009,
    11.825048221409341,
    12.083451977536606,
    12.345119996613247,
    12.610063955123938,
    12.878295467455942,
    13.149826086772048,
    13.42466730586372,
    13.702830557985108,
    13.984327217668513,
    14.269168601521828,
    14.55736596900856,
    14.848930523210871,
    15.143873411576273,
    15.44220572664832,
    15.743938506781891,
    16.04908273684337,
    16.35764934889634,
    16.66964922287304,
    16.985093187232053,
    17.30399201960269,
    17.62635644741625,
    17.95219714852476,
    18.281524751807332,
    18.614349837764564,
    18.95068293910138,
    19.290534541298456,
    19.633915083172692,
    19.98083495742689,
    20.331304511189067,
    20.685334046541502,
    21.042933821039977,
    21.404114048223256,
    21.76888489811322,
    22.137256497705877,
    22.50923893145328,
    22.884842241736916,
    23.264076429332462,
    23.6469514538663,
    24.033477234264016,
    24.42366364919083,
    24.817520537484558,
    25.21505769858089,
    25.61628489293138,
    26.021211842414342,
    26.429848230738664,
    26.842203703840827,
    27.258287870275353,
    27.678110301598522,
    28.10168053274597,
    28.529008062403893,
    28.96010235337422,
    29.39497283293396,
    29.83362889318845,
    30.276079891419332,
    30.722335150426627,
    31.172403958865512,
    31.62629557157785,
    32.08401920991837,
    32.54558406207592,
    33.010999283389665,
    33.4802739966603,
    33.953417292456834,
    34.430438229418264,
    34.911345834551085,
    35.39614910352207,
    35.88485700094671,
    36.37747846067349,
    36.87402238606382,
    37.37449765026789,
    37.87891309649659,
    38.38727753828926,
    38.89959975977785,
    39.41588851594697,
    39.93615253289054,
    40.460400508064545,
    40.98864111053629,
    41.520882981230194,
    42.05713473317016,
    42.597404951718396,
    43.141702194811224,
    43.6900349931913,
    44.24241185063697,
    44.798841244188324,
    45.35933162437017,
    45.92389141541209,
    46.49252901546552,
    47.065252796817916,
    47.64207110610409,
    48.22299226451468,
    48.808024568002054,
    49.3971762874833,
    49.9904556690408,
    50.587870934119984,
    51.189430279724725,
    51.79514187861014,
    52.40501387947288,
    53.0190544071392,
    53.637271562750364,
    54.259673423945976,
    54.88626804504493,
    55.517063457223934,
    56.15206766869424,
    56.79128866487574,
    57.43473440856916,
    58.08241284012621,
    58.734331877617365,
    59.39049941699807,
    60.05092333227251,
    60.715611475655585,
    61.38457167773311,
    62.057811747619894,
    62.7353394731159,
    63.417162620860914,
    64.10328893648692,
    64.79372614476921,
    65.48848194977529,
    66.18756403501224,
    66.89098006357258,
    67.59873767827808,
    68.31084450182222,
    69.02730813691093,
    69.74813616640164,
    70.47333615344107,
    71.20291564160104,
    71.93688215501312,
    72.67524319850172,
    73.41800625771542,
    74.16517879925733,
    74.9167682708136,
    75.67278210128072,
    76.43322770089146,
    77.1981124613393,
    77.96744375590167,
    78.74122893956174,
    79.51947534912904,
    80.30219030335869,
    81.08938110306934,
    81.88105503125999,
    82.67721935322541,
    83.4778813166706,
    84.28304815182372,
    85.09272707154808,
    85.90692527145302,
    86.72564993000343,
    87.54890820862819,
    88.3767072518277,
    89.2090541872801,
    90.04595612594655,
    90.88742016217518,
    91.73345337380438,
    92.58406282226491,
    93.43925555268066,
    94.29903859396902,
    95.16341895893969,
    96.03240364439274,
    96.9059996312159,
    97.78421388448044,
    98.6670533535366,
    99.55452497210776,
};

static inline int delinearized( double rgbComponent )
{
    const double normalized = rgbComponent / 100.0;

    double v = 0.0;

    if ( normalized <= 0.0031308 )
        v = normalized * 12.92;
    else
        v = 1.055 * pow( normalized, 1.0 / 2.4 ) - 0.055;

    return qBound( 0, qRound( v * 255 ), 255 );
}

static inline double labF( double t )
{
    constexpr double e = 216.0 / 24389.0;

    if ( t > e )
    {
        return pow( t, 1.0 / 3.0 );
    }
    else
    {
        constexpr double kappa = 24389.0 / 27.0;
        return ( kappa * t + 16 ) / 116;
    }
}

static inline double labInvf( double ft )
{
    const double e = 216.0 / 24389.0;
    const double kappa = 24389.0 / 27.0;
    const double ft3 = ft * ft * ft;

    if ( ft3 > e )
        return ft3;

    return ( 116 * ft - 16 ) / kappa;
}

static inline double sanitizeDegreesDouble( double degrees )
{
    degrees = fmod( degrees, 360.0 );

    if (degrees < 0)
        degrees = degrees + 360.0;

    return degrees;
}

static inline double yFromLstar( double lstar )
{
    return 100.0 * labInvf( ( lstar + 16.0 ) / 116.0 );
}

static inline QRgb argbFromLstar( double lstar )
{
    const double y = yFromLstar(lstar);
    const int component = delinearized(y);

    return qRgb( component, component, component );
}

static inline QRgb argbFromLinrgb( const XYZ& linrgb )
{
    const int r = delinearized( linrgb.x );
    const int g = delinearized( linrgb.y );
    const int b = delinearized( linrgb.z );

    return qRgb( r, g, b );
}

static inline double sanitizeRadians( double angle )
{
    return fmod( angle + M_PI * 8, M_PI * 2 );
}

static inline double trueDelinearized( double rgbComponent )
{
    const double normalized = rgbComponent / 100.0;
    double v = 0.0;

    if ( normalized <= 0.0031308 )
        v = normalized * 12.92;
    else
        v = 1.055 * pow( normalized, 1.0 / 2.4 ) - 0.055;

    return v * 255.0;
}

static inline int signum( double num )
{
    if (num == 0)
        return 0;

    return ( num < 0 ) ? -1 : 1;
}

static inline XYZ matrixMultiply( const XYZ& row, const XYZ matrix[3] )
{
    XYZ r;

    r.x = row.x * matrix[0].x + row.y * matrix[0].y + row.z * matrix[0].z;
    r.y = row.x * matrix[1].x + row.y * matrix[1].y + row.z * matrix[1].z;
    r.z = row.x * matrix[2].x + row.y * matrix[2].y + row.z * matrix[2].z;

    return r;
}

static inline double chromaticAdaptation( double component )
{
    const double af = pow( abs( component ), 0.42);
    return signum(component) * 400.0 * af / ( af + 27.13 );
}

static inline double hueOf( const XYZ& linrgb )
{
    constexpr XYZ matrix[3] =
    {
        { 0.001200833568784504, 0.002389694492170889, 0.0002795742885861124 },
        { 0.0005891086651375999, 0.0029785502573438758, 0.0003270666104008398 },
        { 0.00010146692491640572, 0.0005364214359186694, 0.0032979401770712076 }
    };

    const XYZ scaledDiscount = matrixMultiply( linrgb, matrix );

    const double rA = chromaticAdaptation( scaledDiscount.x );
    const double gA = chromaticAdaptation( scaledDiscount.y );
    const double bA = chromaticAdaptation( scaledDiscount.z );

    const double a = ( 11.0 * rA + -12.0 * gA + bA ) / 11.0;
    const double b = ( rA + gA - 2.0 * bA ) / 9.0;

    return atan2( b, a );
}

static bool areInCyclicOrder( double a, double b, double c )
{
    const double deltaAB = sanitizeRadians( b - a );
    const double deltaAC = sanitizeRadians( c - a );

    return deltaAB < deltaAC;
}

static inline double intercept( double source, double mid, double target )
{
    return ( mid - source ) / ( target - source );
}

static inline XYZ setCoordinate( const XYZ& source,
    double coordinate, const XYZ& target, int axis )
{
    const double t = intercept( source.value(axis), coordinate, target.value(axis) );

    XYZ r;

    r.x = source.x + ( target.x - source.x ) * t;
    r.y = source.y + ( target.y - source.y ) * t;
    r.z = source.z + ( target.z - source.z ) * t;

    return r;
}

static bool isBounded( double x )
{
    return 0.0 <= x && x <= 100.0;
}

static XYZ nthVertex( double y, int n )
{
    const double kR = Y_FROM_LINRGB.x;
    const double kG = Y_FROM_LINRGB.y;
    const double kB = Y_FROM_LINRGB.z;

    const double coordA = ( n % 4 <= 1 ) ? 0.0 : 100.0;
    const double coordB = ( n % 2 == 0 ) ? 0.0 : 100.0;

    if ( n < 4 )
    {
        const double g = coordA;
        const double b = coordB;
        const double r = ( y - g * kG - b * kB ) / kR;

        if ( isBounded(r) )
            return XYZ( r, g, b );
    }
    else if ( n < 8 )
    {
        const double b = coordA;
        const double r = coordB;
        const double g = ( y - r * kR - b * kB ) / kG;

        if ( isBounded(g) )
            return XYZ( r, g, b );
    }
    else
    {
        const double r = coordA;
        const double g = coordB;
        const double b = ( y - r * kR - g * kG ) / kB;

        if ( isBounded(b) )
            return XYZ( r, g, b );
    }

    return { -1.0, -1.0, -1.0 };
}

void bisectToSegment( double y, double targetHue, XYZ& left, XYZ& right )
{
    left = { -1.0, -1.0, -1.0 };
    right = left;

    double leftHue = 0.0;
    double rightHue = 0.0;

    bool initialized = false;
    bool uncut = true;

    for ( int n = 0; n < 12; n++ )
    {
        XYZ mid = nthVertex(y, n);
        if ( mid.x < 0 )
            continue;

        const double midHue = hueOf( mid );
        if ( !initialized )
        {
            left = mid;
            right = mid;
            leftHue = midHue;
            rightHue = midHue;
            initialized = true;

            continue;
        }

        if ( uncut || areInCyclicOrder( leftHue, midHue, rightHue ) )
        {
            uncut = false;

            if ( areInCyclicOrder( leftHue, targetHue, midHue ) )
            {
                right = mid;
                rightHue = midHue;
            }
            else
            {
                left = mid;
                leftHue = midHue;
            }
        }
    }
}

static XYZ midpoint( const XYZ& a, const XYZ& b )
{
    XYZ r;
    r.x = ( a.x + b.x ) / 2;
    r.y = ( a.y + b.y ) / 2;
    r.z = ( a.z + b.z ) / 2;

    return r;
}

static int planeBelow( double x )
{
    return qFloor( x - 0.5 );
}

static int planeAbove( double x )
{
    return qCeil( x - 0.5 );
}

static XYZ bisectToLimit( double y, double targetHue )
{
    XYZ left, right;
    bisectToSegment( y, targetHue, left, right );

    double leftHue = hueOf(left);

    for ( int axis = 0; axis < 3; axis++ )
    {
        const double l = left.value(axis);
        const double r = right.value(axis);

        if ( l != r )
        {
            int lPlane = -1;
            int rPlane = 255;

            if ( l < r )
            {
                lPlane = planeBelow( trueDelinearized( l ) );
                rPlane = planeAbove( trueDelinearized( r ) );
            }
            else
            {
                lPlane = planeAbove( trueDelinearized( l ) );
                rPlane = planeBelow( trueDelinearized( r ) );
            }

            for ( int i = 0; i < 8; i++ )
            {
                if ( abs( rPlane - lPlane ) <= 1 )
                    break;

                const int mPlane = qFloor( ( lPlane + rPlane ) / 2.0 );
                const double midPlaneCoordinate = planes[mPlane];

                const XYZ mid = setCoordinate(left, midPlaneCoordinate, right, axis);
                const double midHue = hueOf( mid );

                if ( areInCyclicOrder( leftHue, targetHue, midHue ) )
                {
                    right = mid;
                    rPlane = mPlane;
                }
                else
                {
                    left = mid;
                    leftHue = midHue;
                    lPlane = mPlane;
                }
            }
        }
    }

    return midpoint( left, right );
}

static double inverseChromaticAdaptation( double adapted )
{
    const double adaptedAbs = abs( adapted );

    double base = 27.13 * adaptedAbs / ( 400.0 - adaptedAbs );
    if ( base < 0.0 )
        base = 0.0;

    return signum(adapted) * pow( base, 1.0 / 0.42 );
}

static QRgb findResultByJ( double hueRadians, double chroma, double y )
{
    double j = sqrt(y) * 11.0;

    constexpr ViewingConditions vc;

    const double tInnerCoeff = 1.0 / pow( 1.64 - pow( 0.29, vc.backgroundYTowhitePointY ), 0.73 );
    const double eHue = 0.25 * ( cos( hueRadians + 2.0 ) + 3.8 );
    const double p1 = eHue * ( 50000.0 / 13.0 ) * vc.nbb;
    const double hSin = sin(hueRadians);
    const double hCos = cos(hueRadians);

    for ( int i = 0; i < 5; i++ )
    {
        const double jNormalized = j / 100.0;
        const double alpha = ( chroma == 0.0 || j == 0.0 ) ? 0.0 : chroma / sqrt(jNormalized);
        const double t = pow( alpha * tInnerCoeff, 1.0 / 0.9 );
        const double ac = vc.aw * pow( jNormalized, 1.0 / 0.69 / vc.z );
        const double p2 = ac / vc.nbb;

        const double gamma = 23.0 * ( p2 + 0.305 ) * t /
            ( 23.0 * p1 + 11 * t * hCos + 108.0 * t * hSin );
        const double a = gamma * hCos;
        const double b = gamma * hSin;

        const double rA = ( 460.0 * p2 + 451.0 * a + 288.0 * b ) / 1403.0;
        const double gA = ( 460.0 * p2 - 891.0 * a - 261.0 * b ) / 1403.0;
        const double bA = ( 460.0 * p2 - 220.0 * a - 6300.0 * b ) / 1403.0;

        XYZ rgbScaled;
        rgbScaled.x = inverseChromaticAdaptation( rA );
        rgbScaled.y = inverseChromaticAdaptation( gA );
        rgbScaled.z = inverseChromaticAdaptation( bA );

        constexpr XYZ matrix[3] =
        {
            { 1373.2198709594231, -1100.4251190754821, -7.278681089101213, },
            { -271.815969077903, 559.6580465940733, -32.46047482791194 },
            { 1.9622899599665666, -57.173814538844006, 308.7233197812385 }
        };

        const XYZ linrgb = matrixMultiply( rgbScaled, matrix );

        if ( linrgb.x < 0 || linrgb.y < 0 || linrgb.z < 0 )
            return 0;

        const double kR = Y_FROM_LINRGB.x;
        const double kG = Y_FROM_LINRGB.y;
        const double kB = Y_FROM_LINRGB.z;

        const double fnj = kR * linrgb.x + kG * linrgb.y + kB * linrgb.z;
        if ( fnj <= 0 )
            return 0;

        if ( i == 4 || abs(fnj - y) < 0.002 )
        {
            if ( linrgb.x > 100.01 || linrgb.y > 100.01 || linrgb.z > 100.01 )
                return 0;

            return argbFromLinrgb( linrgb );
        }

        j = j - ( fnj - y ) * j / ( 2 * fnj );
    }

    return 0;
}

static inline QRgb getRgb( double hue, double chroma, double tone )
{
    if ( chroma < 0.0001 || tone < 0.0001 || tone > 99.9999 )
        return argbFromLstar( tone );

    hue = sanitizeDegreesDouble( hue );

    const double hueRadians = hue / 180.0 * M_PI;
    const double y = yFromLstar( tone );

    const QRgb rgb = findResultByJ( hueRadians, chroma, y );
    if ( rgb != 0 )
        return rgb;

    const XYZ linrgb = bisectToLimit( y, hueRadians );
    return argbFromLinrgb( linrgb );
}

static const XYZ SRGB_TO_XYZ[3] =
{
    { 0.41233895, 0.35762064, 0.18051042 },
    { 0.2126, 0.7152, 0.0722 },
    { 0.01932141, 0.11916382, 0.95034478 }
};

static double linearized( int rgbComponent )
{
    const double normalized = rgbComponent / 255.0;

    if ( normalized <= 0.040449936 )
        return normalized / 12.92 * 100.0;
    else
        return pow( ( normalized + 0.055 ) / 1.055, 2.4) * 100.0;
}

static XYZ xyzFromArgb( QRgb rgb)
{
    XYZ xyz;

    xyz.x = linearized( qRed(rgb) );
    xyz.y = linearized( qGreen(rgb) );
    xyz.z = linearized( qBlue(rgb) );

    return matrixMultiply( xyz, SRGB_TO_XYZ );
}

static void getHTC( QRgb rgb, double& hue, double& chroma, double& tone )
{
    ViewingConditions vc;

    const XYZ xyz = xyzFromArgb( rgb );

    const double x = xyz.x;
    const double y = xyz.y;
    const double z = xyz.z;

    const double rC = 0.401288 * x + 0.650173 * y - 0.051461 * z;
    const double gC = -0.250268 * x + 1.204414 * y + 0.045854 * z;
    const double bC = -0.002079 * x + 0.048952 * y + 0.953127 * z;

    const double rD = vc.rgbD.x * rC;
    const double gD = vc.rgbD.y * gC;
    const double bD = vc.rgbD.z * bC;

    const double rAF = pow( vc.fl * fabs( rD ) / 100.0, 0.42 );
    const double gAF = pow( vc.fl * fabs( gD ) / 100.0, 0.42 );

    const double bAF = pow( vc.fl * fabs( bD ) / 100.0, 0.42);
    const double rA = signum(rD) * 400.0 * rAF / ( rAF + 27.13 );
    const double gA = signum(gD) * 400.0 * gAF / ( gAF + 27.13 );
    const double bA = signum(bD) * 400.0 * bAF / ( bAF + 27.13 );

    const double a = ( 11.0 * rA + -12.0 * gA + bA ) / 11.0;
    const double b = ( rA + gA - 2.0 * bA ) / 9.0;
    const double u = ( 20.0 * rA + 20.0 * gA + 21.0 * bA ) / 20.0;

    const double p2 = ( 40.0 * rA + 20.0 * gA + bA ) / 20.0;

    // hue
    const double atanDegrees = atan2(b, a) * 180.0 / M_PI;

    // fmod ???
    hue = ( atanDegrees < 0 ) ? atanDegrees + 360.0 : atanDegrees >=
        360 ? atanDegrees - 360 : atanDegrees;

    {
        const double ac = p2 * vc.nbb;

        const double J = 100.0 * pow( ac / vc.aw, 0.69 * vc.z );

        const double huePrime = ( hue < 20.14 ) ? hue + 360 : hue;
        const double eHue = ( 1.0 / 4.0 ) * ( cos( huePrime * M_PI / 180.0 + 2.0 ) + 3.8 );
        const double p1 = 50000.0 / 13.0 * eHue * vc.nbb;
        const double t = p1 * sqrt(a * a + b * b) / ( u + 0.305 );

        const double alpha =
            pow(t, 0.9) * pow( 1.64 - pow(0.29, vc.backgroundYTowhitePointY), 0.73);

        chroma = alpha * sqrt(J / 100.0);
    }

    {
        tone = 116.0 * labF( y / 100.0 ) - 16.0;
    }
}

QskHctColor::QskHctColor( QRgb rgb )
{
    getHTC( rgb, m_hue, m_chroma, m_tone );
}

void QskHctColor::setHue( qreal hue )
{
    m_hue = fmod( hue, 360.0 );
    if ( m_hue < 0.0 )
        m_hue += 360.0;
}

void QskHctColor::setChroma( qreal chroma ) noexcept
{
    m_chroma = ( chroma < 0.0 ) ? 0.0 : chroma;
}

void QskHctColor::setTone( qreal tone ) noexcept
{
    m_tone = qBound( 0.0, tone, 100.0 );
}

void QskHctColor::setRgb( QRgb rgb )
{
    getHTC( rgb, m_hue, m_chroma, m_tone );
}

QRgb QskHctColor::rgb() const
{
    return getRgb( m_hue, m_chroma, m_tone );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskHctColor& color )
{
    debug.nospace() << "HTC("
        << color.hue() << "," << color.chroma() << "," << color.tone() << ")";

    return debug.space();
}

#endif
