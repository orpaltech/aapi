/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2025 ORPAL Technology, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


import QtQuick
import QtQml
import aapi


Canvas {
    readonly property color tickColor: AapiTheme.style.smith.tickColor
    readonly property string tickFont: AapiTheme.style.smith.tickFont
    readonly property color fillColor:  AapiTheme.style.smith.fillColor
    readonly property color circleColor: AapiTheme.style.smith.circleColor
    readonly property color axisColor: AapiTheme.style.smith.axisColor
    readonly property color gammaColor: AapiTheme.style.smith.gammaColor

    readonly property point center: Qt.point(canvas.width / 2, canvas.height / 2)
    readonly property int minSize: Math.min(canvas.width, canvas.height)
    readonly property int margin: minSize / 31
    readonly property int radius: minSize / 2 - margin

    property rect plotArea: calcPlotArea()
    property var gammaVals: []
    property PanoramicScanViewBackend backend: aapi.view_panoramic_scan

    width: 1140
    height: 620

    id: canvas
    antialiasing: true

    onPaint: {
        var ctx = canvas.getContext("2d");
        ctx.reset();

        drawImpedanceCircles(ctx);

        drawHorizontalAxis(ctx);

        //drawGammaCurve(ctx);
    }

    function calcPlotArea() {
        console.log("calc plot area: center=(" + center.x + ", " +  center.y + "), minSize=" + minSize + ", margin=" + margin + ", radius=" + radius);
        return Qt.rect(center.x - radius, center.y - radius, radius*2, radius*2)
    }

    function append(gamma) {
        canvas.gammaVals.push(gamma);
        refresh();
    }

    function refresh() {
        canvas.requestPaint();
    }

    function clear() {
        canvas.gammaVals = [];
        canvas.requestPaint();
    }

    function setReCircleStyle(ctx, isOuter) {
        ctx.strokeStyle = circleColor;
        ctx.fillStyle = fillColor;
        ctx.lineWidth = isOuter ? 2 : 1;
    }

    function setImCircleStyle(ctx) {
        ctx.strokeStyle = circleColor;
        ctx.lineWidth = 1;
    }

    function setAxisStyle(ctx) {
        ctx.strokeStyle = axisColor;
        ctx.lineWidth = 1;
    }

    function setReTickStyle(ctx) {
        ctx.strokeStyle = tickColor;
        ctx.font = tickFont;
        ctx.textAlign = "right";
        ctx.textBaseline = "top";
    }

    function setImTickStyle(ctx, pt) {
        ctx.strokeStyle = tickColor;
        ctx.font = tickFont;
        ctx.textAlign = pt.x < center.x ? "right" : (pt.x > center.x ? "left" : "center");
        ctx.textBaseline = pt.y < center.y ? "bottom" : "top";
        return Qt.point(pt.x, pt.y);
    }

    function setGammaStyle(ctx) {
        ctx.strokeStyle = gammaColor
        ctx.lineWidth = 1
    }

    function getReRadius(re) {
        return (1.0 / (1.0 + re)) * radius;
    }

    function getImRadius(im) {
        return (1.0 / im) * radius;
    }

    function getImCenterY(im) {
        return (1.0 / im) * radius;
    }

    function getImTickPoint(x1, y1, r1) {
        var x0 = center.x;
        var y0 = center.y;
        var r0 = radius;
        /* Find crossing point */
        var d2 = Math.pow(x1-x0,2) + Math.pow(y1-y0,2);
        var d = Math.sqrt(d2);
        var a = (Math.pow(r0,2) - Math.pow(r1,2) + d2)/(d*2);
        var h = Math.sqrt(Math.pow(r0,2) - Math.pow(a,2));
        var x2 = x0 + a*(x1 - x0)/d;
        var y2 = y0 + a*(y1 - y0)/d;
        var sign = y1 < y0 ? 1 : -1;
        var x3 = x2 + sign*h*(y1 - y0)/d;
        var y3 = y2 - sign*h*(x1 - x0)/d;
        return Qt.point(x3,y3);
    }

    function getTickText(val) {
        var r0 = backend ? backend.base_r0 : 50;
        var res = val * r0;
        return res.toFixed((res % 1)!=0 ?1:0);
    }

    /* Method draws horizontal axis */
    function drawHorizontalAxis(ctx) {
        setAxisStyle(ctx);
        ctx.beginPath();
        ctx.moveTo(center.x - radius, center.y);
        ctx.lineTo(center.x + radius, center.y);
        ctx.lineWidth = 0.8;
        ctx.stroke();
    }

    function drawImpedanceCircles(ctx) {
        var normImp = [ 0.0, 0.2, 0.5, 1.0, 2.0, 5.0 ];
        /* Draw real impedance circles */
        for (var i = 0; i < normImp.length; i++) {
            var reRadius = getReRadius(normImp[i]);
            var x = (center.x + radius) - reRadius;
            var y = center.y;
            ctx.beginPath();
            ctx.arc(x, y, reRadius, 0, Math.PI * 2);
            setReCircleStyle(ctx, i==0);
            if (i==0)
                ctx.fill();
            ctx.stroke();
            /* Draw real impedance ticks */
            setReTickStyle(ctx);
            ctx.strokeText(getTickText(normImp[i]), x - reRadius - 3, y + 2);
        }

        ctx.save();

        /* Set clipping region */
        ctx.beginPath();
        ctx.arc(center.x, center.y, radius+1, 0, Math.PI * 2);
        ctx.clip();

        /* Draw imaginary impedance arcs */
        var imTickPoints = [];
        var imTickVals = [];
        setImCircleStyle(ctx);
        x = center.x + radius; // constant x-position
        for (i = 1; i < normImp.length; i++) {
            var imRadius = getImRadius(normImp[i]);
            for (var j=-1; j<=1; j+=2) {
                var imImp = normImp[i]*j;
                y = center.y + getImCenterY(imImp);
                ctx.beginPath();
                ctx.arc(x,y, imRadius, 0, Math.PI * 2, true);
                ctx.stroke();
                /* Prepare tick points & labels */
                imTickPoints.push(getImTickPoint(x,y,imRadius));
                imTickVals.push(-1.0*imImp);
            }
        }

        ctx.restore();

        /* Draw imaginary ticks */
        for (i = 0; i < imTickPoints.length; i++) {
            var pt = setImTickStyle(ctx,imTickPoints[i]);
            var sign = pt.y < center.y ?-1:1;
            pt.y = pt.y + sign*3;
            ctx.strokeText(getTickText(imTickVals[i]), pt.x, pt.y);
        }
    }

    function gamma2Point(gamma) {
        return Qt.point(
            Math.round(center.x + gamma.x * radius),
            Math.round(center.y - gamma.y * radius)
        );
    }

    function drawGammaCurve(ctx) {
        if (gammaVals.length < 2)
            return;

        ctx.beginPath();
        var g0 = gammaVals[0];
        var pt0 = gamma2Point(g0);
        ctx.moveTo(pt0.x, pt0.y);

        for (var i = 1; i < gammaVals.length; i++) {
            var g = gammaVals[i];
            var pt = gamma2Point(g);
            ctx.lineTo(pt.x, pt.y);
        }
        setGammaStyle(ctx)
        ctx.stroke();
    }
}
