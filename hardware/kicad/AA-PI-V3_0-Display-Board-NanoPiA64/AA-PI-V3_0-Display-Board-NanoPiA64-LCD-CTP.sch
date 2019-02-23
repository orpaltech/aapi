EESchema Schematic File Version 4
LIBS:AA-PI-V3_0-Display-Board-NanoPiA64-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	5420 1660 5080 1660
Wire Wire Line
	5420 1560 5080 1560
Wire Wire Line
	5420 1360 5080 1360
Wire Wire Line
	5420 1260 5080 1260
Wire Wire Line
	5420 1860 5080 1860
Wire Wire Line
	5420 1960 5080 1960
Text GLabel 5080 1960 0    50   Output ~ 0
DSI_D1_N
Text GLabel 5080 1860 0    50   Output ~ 0
DSI_D1_P
Text GLabel 5080 1360 0    50   Output ~ 0
DSI_CLK_N
Text GLabel 5080 1260 0    50   Output ~ 0
DSI_CLK_P
Text GLabel 5080 1660 0    50   BiDi ~ 0
DSI_D0_N
Text GLabel 5080 1560 0    50   BiDi ~ 0
DSI_D0_P
Wire Wire Line
	5420 3360 5160 3360
Text GLabel 5160 3360 0    50   Input ~ 0
CTP_INT
Wire Wire Line
	5420 3660 4900 3660
Text GLabel 4740 3660 0    50   Input ~ 0
I2C0_SDA
Wire Wire Line
	5420 3560 5060 3560
Text GLabel 4740 3560 0    50   Input ~ 0
I2C0_SCL
Wire Wire Line
	5320 3460 5420 3460
Wire Wire Line
	5420 3760 5320 3760
Connection ~ 5320 3760
Wire Wire Line
	5320 3760 5320 3460
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D2AF2D3
P 5320 4310
AR Path="/5D2AF2D3" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D2AF2D3" Ref="#PWR0141"  Part="1" 
F 0 "#PWR0141" H 5320 4060 50  0001 C CNN
F 1 "GND" H 5320 4137 50  0000 C CNN
F 2 "" H 5320 4310 50  0001 C CNN
F 3 "" H 5320 4310 50  0001 C CNN
	1    5320 4310
	1    0    0    -1  
$EndComp
Connection ~ 5320 3460
Wire Wire Line
	5320 1760 5420 1760
Wire Wire Line
	5320 1760 5320 1460
Wire Wire Line
	5320 1460 5420 1460
Connection ~ 5320 1760
Wire Wire Line
	5420 2960 5160 2960
Text GLabel 5160 2960 0    50   Output ~ 0
LCD_RESET
Wire Wire Line
	5420 1160 5320 1160
Wire Wire Line
	5320 1160 5320 1460
Connection ~ 5320 1460
Wire Wire Line
	5320 3760 5320 4260
Wire Wire Line
	5420 3260 5320 3260
Connection ~ 5320 3260
Wire Wire Line
	5320 3260 5320 3460
Wire Wire Line
	5420 3060 5320 3060
Wire Wire Line
	5320 1760 5320 2060
Connection ~ 5320 3060
Wire Wire Line
	5320 3060 5320 3260
Wire Wire Line
	5420 2760 5320 2760
Connection ~ 5320 2760
Wire Wire Line
	5320 2760 5320 3060
Wire Wire Line
	5420 2660 5320 2660
Connection ~ 5320 2660
Wire Wire Line
	5320 2660 5320 2760
Wire Wire Line
	5420 2360 5320 2360
Connection ~ 5320 2360
Wire Wire Line
	5320 2360 5320 2660
Wire Wire Line
	5320 2060 5420 2060
Connection ~ 5320 2060
Wire Wire Line
	5320 2060 5320 2360
$Comp
L AA-PI-Components:Conn_01x30_Shielded J?
U 1 1 5D2AF2F7
P 5620 2560
AR Path="/5D2AF2F7" Ref="J?"  Part="1" 
AR Path="/5D22BC09/5D2AF2F7" Ref="J9"  Part="1" 
F 0 "J9" H 5540 4080 50  0000 L CNN
F 1 "MCU Display Interface" H 4830 4270 50  0000 L CNN
F 2 "AA-PI-Footprints:FPC-0.5-1-30P_Clamshell_Bottom" H 5620 2560 50  0001 C CNN
F 3 "~" H 5620 2560 50  0001 C CNN
	1    5620 2560
	1    0    0    -1  
$EndComp
Wire Wire Line
	5620 4260 5320 4260
Connection ~ 5320 4260
Wire Wire Line
	5320 4260 5320 4310
Wire Wire Line
	5420 3160 5160 3160
Text GLabel 5160 3160 0    50   Input ~ 0
LCD_TE_OUT
$Comp
L AA-PI-Components:R_Small R?
U 1 1 5D2AF303
P 4900 3940
AR Path="/5D2AF303" Ref="R?"  Part="1" 
AR Path="/5D22BC09/5D2AF303" Ref="R31"  Part="1" 
F 0 "R31" H 4700 3980 50  0000 L CNN
F 1 "10K" H 4700 3890 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 4900 3940 50  0001 C CNN
F 3 "~" H 4900 3940 50  0001 C CNN
	1    4900 3940
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:R_Small R?
U 1 1 5D2AF30A
P 5060 3940
AR Path="/5D2AF30A" Ref="R?"  Part="1" 
AR Path="/5D22BC09/5D2AF30A" Ref="R32"  Part="1" 
F 0 "R32" H 5110 3980 50  0000 L CNN
F 1 "10K" H 5110 3890 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5060 3940 50  0001 C CNN
F 3 "~" H 5060 3940 50  0001 C CNN
	1    5060 3940
	1    0    0    -1  
$EndComp
Wire Wire Line
	5060 3840 5060 3560
$Comp
L AA-PI-Components:+3V3 #PWR?
U 1 1 5D2AF312
P 4770 4200
AR Path="/5D2AF312" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D2AF312" Ref="#PWR0155"  Part="1" 
F 0 "#PWR0155" H 4770 4050 50  0001 C CNN
F 1 "+3V3" V 4790 4320 50  0000 L CNN
F 2 "" H 4770 4200 50  0001 C CNN
F 3 "" H 4770 4200 50  0001 C CNN
	1    4770 4200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4770 4200 4900 4200
Wire Wire Line
	5060 4200 5060 4040
Wire Wire Line
	4900 4200 4900 4040
Connection ~ 4900 4200
Wire Wire Line
	4900 4200 5060 4200
Wire Wire Line
	4900 3840 4900 3660
Connection ~ 5060 3560
Wire Wire Line
	5060 3560 4740 3560
Connection ~ 4900 3660
Wire Wire Line
	4900 3660 4740 3660
Wire Wire Line
	1660 1670 1810 1670
Wire Wire Line
	1810 1670 1810 1870
Wire Wire Line
	1660 1470 2060 1470
Wire Wire Line
	1660 1570 2060 1570
Wire Wire Line
	1660 1270 2060 1270
Wire Wire Line
	1660 1370 2060 1370
Text GLabel 2060 1270 2    50   Input ~ 0
LCD_RESET
Text GLabel 2060 1370 2    50   Output ~ 0
CTP_INT
Text GLabel 2060 1470 2    50   Input ~ 0
CTP_I2C_SDA
Text GLabel 2060 1570 2    50   Input ~ 0
CTP_I2C_SCL
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D3724B8
P 1810 1930
AR Path="/5D3724B8" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D3724B8" Ref="#PWR0156"  Part="1" 
F 0 "#PWR0156" H 1810 1680 50  0001 C CNN
F 1 "GND" H 1810 1757 50  0000 C CNN
F 2 "" H 1810 1930 50  0001 C CNN
F 3 "" H 1810 1930 50  0001 C CNN
	1    1810 1930
	1    0    0    -1  
$EndComp
Wire Wire Line
	1460 1870 1810 1870
Connection ~ 1810 1870
Wire Wire Line
	1810 1870 1810 1930
$Comp
L AA-PI-Components:Conn_01x06_Shielded J?
U 1 1 5D3724C1
P 1460 1370
AR Path="/5D3724C1" Ref="J?"  Part="1" 
AR Path="/5D22BC09/5D3724C1" Ref="J6"  Part="1" 
F 0 "J6" H 1460 1700 50  0000 C CNN
F 1 "CTP Interface" H 1010 1900 50  0000 C CNN
F 2 "AA-PI-Footprints:FPC-0.5-1-6P_Clamshell_Bottom" H 1460 1370 50  0001 C CNN
F 3 "~" H 1460 1370 50  0001 C CNN
	1    1460 1370
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1660 1170 2060 1170
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D3724C9
P 2060 1170
AR Path="/5D3724C9" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D3724C9" Ref="#PWR0157"  Part="1" 
F 0 "#PWR0157" H 2060 1020 50  0001 C CNN
F 1 "+V_LCD" V 2060 1298 50  0000 L CNN
F 2 "" H 2060 1170 50  0001 C CNN
F 3 "" H 2060 1170 50  0001 C CNN
	1    2060 1170
	0    1    1    0   
$EndComp
$Comp
L AA-PI-Components:TLV75728PDBVR U?
U 1 1 5D41FA4C
P 9000 1530
AR Path="/5D41FA4C" Ref="U?"  Part="1" 
AR Path="/5D22BC09/5D41FA4C" Ref="U7"  Part="1" 
F 0 "U7" H 8800 1830 50  0000 C CNN
F 1 "TLV75728PDBVR" H 9050 1230 50  0000 C CNN
F 2 "AA-PI-Footprints:SOT95P280X145-5N" H 8550 880 50  0001 L CNN
F 3 "http://www.ti.com/lit/gpn/tlv757p" H 8550 780 50  0001 L CNN
F 4 "1A Low-Quiescent-Current Low-Dropout (LDO) Regulator" H 8550 680 50  0001 L CNN "Description"
F 5 "Texas Instruments" H 8550 480 50  0001 L CNN "Manufacturer_Name"
F 6 "TLV757P" H 8550 380 50  0001 L CNN "Manufacturer_Part_Number"
	1    9000 1530
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 1430 9760 1430
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D41FA54
P 9560 1670
AR Path="/5D41FA54" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D41FA54" Ref="#PWR0158"  Part="1" 
F 0 "#PWR0158" H 9560 1420 50  0001 C CNN
F 1 "GND" H 9560 1497 50  0000 C CNN
F 2 "" H 9560 1670 50  0001 C CNN
F 3 "" H 9560 1670 50  0001 C CNN
	1    9560 1670
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9450 1630 9560 1630
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D41FA66
P 8440 2050
AR Path="/5D41FA66" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D41FA66" Ref="#PWR0159"  Part="1" 
F 0 "#PWR0159" H 8440 1800 50  0001 C CNN
F 1 "GND" H 8440 1877 50  0000 C CNN
F 2 "" H 8440 2050 50  0001 C CNN
F 3 "" H 8440 2050 50  0001 C CNN
	1    8440 2050
	-1   0    0    -1  
$EndComp
Text GLabel 8080 1630 0    50   Input ~ 0
LCD_EN
$Comp
L AA-PI-Components:+5V #PWR?
U 1 1 5D41FA6E
P 8060 1430
AR Path="/5D41FA6E" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D41FA6E" Ref="#PWR0160"  Part="1" 
F 0 "#PWR0160" H 8060 1280 50  0001 C CNN
F 1 "+5V" V 8060 1558 50  0000 L CNN
F 2 "" H 8060 1430 50  0001 C CNN
F 3 "" H 8060 1430 50  0001 C CNN
	1    8060 1430
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9760 1590 9760 1430
Connection ~ 9760 1430
Wire Wire Line
	9760 1430 9980 1430
Wire Wire Line
	9760 1790 9760 1970
Wire Wire Line
	9560 1630 9560 1670
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D41FA79
P 9760 2020
AR Path="/5D41FA79" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D41FA79" Ref="#PWR0161"  Part="1" 
F 0 "#PWR0161" H 9760 1770 50  0001 C CNN
F 1 "GND" H 9760 1847 50  0000 C CNN
F 2 "" H 9760 2020 50  0001 C CNN
F 3 "" H 9760 2020 50  0001 C CNN
	1    9760 2020
	-1   0    0    -1  
$EndComp
$Comp
L AA-PI-Components:C_Small C?
U 1 1 5D41FA7F
P 8440 1820
AR Path="/5D41FA7F" Ref="C?"  Part="1" 
AR Path="/5D22BC09/5D41FA7F" Ref="C39"  Part="1" 
F 0 "C39" H 8270 1740 50  0000 L CNN
F 1 "1u" H 8290 1900 50  0000 L CNN
F 2 "AA-PI-Footprints:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8440 1820 50  0001 C CNN
F 3 "~" H 8440 1820 50  0001 C CNN
	1    8440 1820
	1    0    0    1   
$EndComp
Wire Wire Line
	8440 1720 8440 1430
Connection ~ 8440 1430
Wire Wire Line
	8440 1430 8060 1430
Wire Wire Line
	8440 1430 8550 1430
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D41FA8E
P 10040 1430
AR Path="/5D41FA8E" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D41FA8E" Ref="#PWR0162"  Part="1" 
F 0 "#PWR0162" H 10040 1280 50  0001 C CNN
F 1 "+V_LCD" V 10040 1558 50  0000 L CNN
F 2 "" H 10040 1430 50  0001 C CNN
F 3 "" H 10040 1430 50  0001 C CNN
	1    10040 1430
	0    1    1    0   
$EndComp
$Comp
L AA-PI-Components:CP_Small C?
U 1 1 5D41FA94
P 9760 1690
AR Path="/5D41FA94" Ref="C?"  Part="1" 
AR Path="/5D22BC09/5D41FA94" Ref="C41"  Part="1" 
F 0 "C41" H 9780 1770 50  0000 L CNN
F 1 "47u" H 9780 1600 50  0000 L CNN
F 2 "AA-PI-Footprints:CP_EIA-3528-21_Kemet-B" H 9760 1690 50  0001 C CNN
F 3 "~" H 9760 1690 50  0001 C CNN
	1    9760 1690
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:C_Small C?
U 1 1 5D41FA9B
P 9980 1690
AR Path="/5D41FA9B" Ref="C?"  Part="1" 
AR Path="/5D22BC09/5D41FA9B" Ref="C42"  Part="1" 
F 0 "C42" H 10100 1770 50  0000 C CNN
F 1 "100n" H 10120 1620 50  0000 C CNN
F 2 "AA-PI-Footprints:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9980 1690 50  0001 C CNN
F 3 "~" H 9980 1690 50  0001 C CNN
	1    9980 1690
	1    0    0    -1  
$EndComp
Wire Wire Line
	9980 1590 9980 1430
Connection ~ 9980 1430
Wire Wire Line
	9980 1430 10040 1430
Wire Wire Line
	9760 1970 9980 1970
Wire Wire Line
	9980 1970 9980 1790
Connection ~ 9760 1970
Wire Wire Line
	9760 1970 9760 2020
Text Notes 8610 2400 0    59   ~ 12
LCD Power Switch
Wire Wire Line
	1830 4520 1690 4520
Wire Wire Line
	1830 4020 1690 4020
Wire Wire Line
	1690 4020 1690 4520
Wire Wire Line
	1830 3720 1690 3720
Wire Wire Line
	1690 3720 1690 4020
Wire Wire Line
	1830 3420 1690 3420
Wire Wire Line
	1690 3420 1690 3720
Wire Wire Line
	1830 3220 1690 3220
Wire Wire Line
	1690 3220 1690 3420
Wire Wire Line
	1830 2920 1690 2920
Wire Wire Line
	1690 2920 1690 3220
Connection ~ 1690 4020
Connection ~ 1690 3720
Connection ~ 1690 3420
Connection ~ 1690 3220
Wire Wire Line
	2530 4120 2590 4120
Wire Wire Line
	2590 4120 2590 4620
Wire Wire Line
	2530 3820 2590 3820
Wire Wire Line
	2590 3820 2590 4120
Wire Wire Line
	2530 3520 2590 3520
Wire Wire Line
	2590 3520 2590 3820
Wire Wire Line
	2530 3220 2590 3220
Wire Wire Line
	2590 3220 2590 3520
Connection ~ 2590 4120
Connection ~ 2590 3820
Connection ~ 2590 3520
Wire Wire Line
	2850 4420 2530 4420
Wire Wire Line
	1830 4420 1430 4420
Wire Wire Line
	2850 4320 2530 4320
Wire Wire Line
	1830 4320 1430 4320
Wire Wire Line
	1830 3520 1440 3520
Wire Wire Line
	1830 3620 1440 3620
Wire Wire Line
	2530 3620 2850 3620
Wire Wire Line
	1830 3120 1450 3120
Wire Wire Line
	1830 4120 1430 4120
Wire Wire Line
	1830 3820 1440 3820
Wire Wire Line
	2530 3720 2850 3720
Wire Wire Line
	2530 4020 2850 4020
Text GLabel 1450 3120 0    50   Output ~ 0
LCD_TE_OUT
Text GLabel 1440 3520 0    50   Input ~ 0
DSI_D0_P
Text GLabel 1440 3620 0    50   Input ~ 0
DSI_CLK_N
Text GLabel 1440 3820 0    50   Input ~ 0
DSI_D1_P
Text GLabel 1430 4120 0    50   Output ~ 0
PWM_CABC
Text GLabel 1430 4420 0    50   Input ~ 0
LED-A
Text GLabel 2850 3420 2    50   Input ~ 0
DSI_D0_N
Text GLabel 2850 3620 2    50   Input ~ 0
DSI_CLK_P
Text GLabel 2850 3720 2    50   Input ~ 0
DSI_D1_N
Text GLabel 2850 4020 2    50   Input ~ 0
LCD_RESET
Text GLabel 2850 4420 2    50   Input ~ 0
LED-A
Wire Wire Line
	2530 4220 2850 4220
Wire Wire Line
	2530 3420 2850 3420
Wire Wire Line
	2480 4620 2590 4620
Wire Wire Line
	2480 2770 2590 2770
Wire Wire Line
	2590 2770 2590 3220
Connection ~ 2590 3220
$Comp
L AA-PI-Components:FH26-33S-0.3SHW J?
U 1 1 5D4E2271
P 2180 3720
AR Path="/5D4E2271" Ref="J?"  Part="1" 
AR Path="/5D22BC09/5D4E2271" Ref="J8"  Part="1" 
F 0 "J8" H 2080 4840 50  0000 C CNN
F 1 "LCD Interface" H 2450 4970 50  0000 C CNN
F 2 "AA-PI-Footprints:FH26W-33S-0.3SHW" H 1580 2370 50  0001 L CNN
F 3 "http://www.mouser.com/ds/2/185/FH26_catalog-939414.pdf" H 1580 2270 50  0001 L CNN
F 4 "0.3 mm Pitch, 1.0 mm Height FPC Connector" H 1580 2170 50  0001 L CNN "Description"
F 5 "Hirose" H 1580 1970 50  0001 L CNN "Manufacturer_Name"
F 6 "FH26-33S-0.3SHW" H 1580 1870 50  0001 L CNN "Manufacturer_Part_Number"
	1    2180 3720
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D4E2278
P 2800 4620
AR Path="/5D4E2278" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E2278" Ref="#PWR0163"  Part="1" 
F 0 "#PWR0163" H 2800 4370 50  0001 C CNN
F 1 "GND" H 2800 4447 50  0000 C CNN
F 2 "" H 2800 4620 50  0001 C CNN
F 3 "" H 2800 4620 50  0001 C CNN
	1    2800 4620
	0    -1   -1   0   
$EndComp
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D4E227E
P 1500 2920
AR Path="/5D4E227E" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E227E" Ref="#PWR0164"  Part="1" 
F 0 "#PWR0164" H 1500 2670 50  0001 C CNN
F 1 "GND" H 1500 2747 50  0000 C CNN
F 2 "" H 1500 2920 50  0001 C CNN
F 3 "" H 1500 2920 50  0001 C CNN
	1    1500 2920
	0    1    1    0   
$EndComp
Wire Wire Line
	1830 4220 1430 4220
Wire Wire Line
	2590 4620 2800 4620
Connection ~ 2590 4620
Wire Wire Line
	1500 2920 1690 2920
Connection ~ 1690 2920
Wire Wire Line
	2530 3020 2830 3020
Text GLabel 1430 4320 0    50   Input ~ 0
LED-K
Text GLabel 2850 4320 2    50   Input ~ 0
LED-K
$Comp
L AA-PI-Components:R_Small R?
U 1 1 5D4E228C
P 3010 2860
AR Path="/5D4E228C" Ref="R?"  Part="1" 
AR Path="/5D22BC09/5D4E228C" Ref="R29"  Part="1" 
F 0 "R29" V 3080 2800 50  0000 L CNN
F 1 "10K" V 2930 2790 50  0001 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 3010 2860 50  0001 C CNN
F 3 "~" H 3010 2860 50  0001 C CNN
	1    3010 2860
	0    1    -1   0   
$EndComp
$Comp
L AA-PI-Components:R_Small R?
U 1 1 5D4E2293
P 3010 3020
AR Path="/5D4E2293" Ref="R?"  Part="1" 
AR Path="/5D22BC09/5D4E2293" Ref="R30"  Part="1" 
F 0 "R30" V 3080 2960 50  0000 L CNN
F 1 "10K" V 2930 2950 50  0001 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 3010 3020 50  0001 C CNN
F 3 "~" H 3010 3020 50  0001 C CNN
	1    3010 3020
	0    1    -1   0   
$EndComp
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D4E229A
P 3170 2860
AR Path="/5D4E229A" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E229A" Ref="#PWR0165"  Part="1" 
F 0 "#PWR0165" H 3170 2610 50  0001 C CNN
F 1 "GND" H 3170 2687 50  0000 C CNN
F 2 "" H 3170 2860 50  0001 C CNN
F 3 "" H 3170 2860 50  0001 C CNN
	1    3170 2860
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3110 2860 3170 2860
Wire Wire Line
	3170 3020 3110 3020
Wire Wire Line
	2910 2860 2830 2860
Wire Wire Line
	2830 2860 2830 3020
Connection ~ 2830 3020
Wire Wire Line
	2830 3020 2910 3020
Text Label 2630 3020 0    30   ~ 0
LCM_ID
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D4E22A7
P 2850 4220
AR Path="/5D4E22A7" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E22A7" Ref="#PWR0166"  Part="1" 
F 0 "#PWR0166" H 2850 4070 50  0001 C CNN
F 1 "+V_LCD" V 2850 4348 50  0000 L CNN
F 2 "" H 2850 4220 50  0001 C CNN
F 3 "" H 2850 4220 50  0001 C CNN
	1    2850 4220
	0    1    1    0   
$EndComp
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D4E22AD
P 1430 4220
AR Path="/5D4E22AD" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E22AD" Ref="#PWR0167"  Part="1" 
F 0 "#PWR0167" H 1430 4070 50  0001 C CNN
F 1 "+V_LCD" V 1430 4348 50  0000 L CNN
F 2 "" H 1430 4220 50  0001 C CNN
F 3 "" H 1430 4220 50  0001 C CNN
	1    1430 4220
	0    -1   -1   0   
$EndComp
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D4E22B3
P 3170 3020
AR Path="/5D4E22B3" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E22B3" Ref="#PWR0168"  Part="1" 
F 0 "#PWR0168" H 3170 2870 50  0001 C CNN
F 1 "+V_LCD" V 3170 3148 50  0000 L CNN
F 2 "" H 3170 3020 50  0001 C CNN
F 3 "" H 3170 3020 50  0001 C CNN
	1    3170 3020
	0    1    1    0   
$EndComp
Wire Wire Line
	5320 5900 5320 5780
Wire Wire Line
	5320 5780 5600 5780
Wire Wire Line
	3910 6310 3860 6310
Wire Wire Line
	3860 6310 3860 6460
Wire Wire Line
	3560 6310 3860 6310
Connection ~ 3860 6310
Wire Wire Line
	3160 6210 3080 6210
Wire Wire Line
	3160 6410 3080 6410
Wire Wire Line
	3580 5880 3580 5780
Wire Wire Line
	3580 5780 3360 5780
Wire Wire Line
	4330 5780 3860 5780
Wire Wire Line
	3860 5780 3580 5780
Wire Wire Line
	3910 6110 3860 6110
Wire Wire Line
	3860 6110 3860 5780
Connection ~ 3580 5780
Connection ~ 3860 5780
Text GLabel 3080 6210 0    50   Input ~ 0
PWM_CABC
Text GLabel 3080 6410 0    50   Input ~ 0
PWM_MCU
Text GLabel 5600 5780 2    50   Output ~ 0
LED-A
Text GLabel 5600 6410 2    50   Output ~ 0
LED-K
$Comp
L AA-PI-Components:SW_SPDT SW2
U 1 1 5D625ACC
P 3360 6310
F 0 "SW2" H 3290 6180 50  0000 C CNN
F 1 "SW_SPDT" H 3360 6504 50  0001 C CNN
F 2 "AA-PI-Footprints:SW_SPDT_PCM12" H 3360 6310 50  0001 C CNN
F 3 "" H 3360 6310 50  0001 C CNN
	1    3360 6310
	-1   0    0    -1  
$EndComp
$Comp
L AA-PI-Components:C_Small C40
U 1 1 5D625AD3
P 3580 5980
F 0 "C40" H 3630 6070 50  0000 L CNN
F 1 "4.7u" H 3620 5910 50  0000 L CNN
F 2 "AA-PI-Footprints:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3580 5980 50  0001 C CNN
F 3 "~" H 3580 5980 50  0001 C CNN
	1    3580 5980
	1    0    0    -1  
$EndComp
Wire Wire Line
	3580 6120 3580 6080
$Comp
L AA-PI-Components:GND #PWR0169
U 1 1 5D625ADB
P 3580 6120
F 0 "#PWR0169" H 3580 5870 50  0001 C CNN
F 1 "GND" H 3620 5980 50  0001 C CNN
F 2 "" H 3580 6120 50  0001 C CNN
F 3 "" H 3580 6120 50  0001 C CNN
	1    3580 6120
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:R_Small R40
U 1 1 5D625AE1
P 3860 6560
F 0 "R40" H 3919 6606 50  0000 L CNN
F 1 "100K" H 3919 6515 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 3860 6560 50  0001 C CNN
F 3 "~" H 3860 6560 50  0001 C CNN
	1    3860 6560
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR0170
U 1 1 5D625AE8
P 3860 6700
F 0 "#PWR0170" H 3860 6450 50  0001 C CNN
F 1 "GND" H 3860 6527 50  0000 C CNN
F 2 "" H 3860 6700 50  0001 C CNN
F 3 "" H 3860 6700 50  0001 C CNN
	1    3860 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3860 6660 3860 6700
$Comp
L AA-PI-Components:R_Small R41
U 1 1 5D625AEF
P 5060 6580
F 0 "R41" H 5119 6626 50  0000 L CNN
F 1 "15" H 5119 6535 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5060 6580 50  0001 C CNN
F 3 "~" H 5060 6580 50  0001 C CNN
	1    5060 6580
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:C_Small C43
U 1 1 5D625AF6
P 5320 6000
F 0 "C43" H 5370 6060 50  0000 L CNN
F 1 "1u (>30V)" H 5380 5940 50  0000 L CNN
F 2 "AA-PI-Footprints:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 5320 6000 50  0001 C CNN
F 3 "~" H 5320 6000 50  0001 C CNN
	1    5320 6000
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR0171
U 1 1 5D625AFD
P 5320 6260
F 0 "#PWR0171" H 5320 6010 50  0001 C CNN
F 1 "GND" H 5450 6250 50  0000 C CNN
F 2 "" H 5320 6260 50  0001 C CNN
F 3 "" H 5320 6260 50  0001 C CNN
	1    5320 6260
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:D_Schottky D4
U 1 1 5D625B03
P 5120 5780
F 0 "D4" H 5120 5680 50  0000 C CNN
F 1 "SS14" H 5120 5890 50  0000 C CNN
F 2 "AA-PI-Footprints:D_SMA" H 5120 5780 50  0001 C CNN
F 3 "~" H 5120 5780 50  0001 C CNN
	1    5120 5780
	-1   0    0    1   
$EndComp
Wire Wire Line
	5270 5780 5320 5780
Connection ~ 5320 5780
$Comp
L AA-PI-Components:L_Core_Ferrite L4
U 1 1 5D625B0C
P 4480 5780
F 0 "L4" V 4600 5780 50  0000 C CNN
F 1 "22uH" V 4410 5780 50  0000 C CNN
F 2 "AA-PI-Footprints:CD54" H 4480 5780 50  0001 C CNN
F 3 "~" H 4480 5780 50  0001 C CNN
	1    4480 5780
	0    -1   -1   0   
$EndComp
$Comp
L AA-PI-Components:FAN5333BSX U9
U 1 1 5D625B16
P 4410 6210
F 0 "U9" H 4610 6480 50  0000 C CNN
F 1 "FAN5333B" H 4410 5920 50  0000 C CNN
F 2 "AA-PI-Footprints:SOT95P270X145-5N" H 3660 5660 50  0001 L CNN
F 3 "http://www.onsemi.com/pub/Collateral/FAN5333B-D.pdf" H 3660 5560 50  0001 L CNN
F 4 "LED Driver, 30V Integrated Switch SOT23" H 3660 5460 50  0001 L CNN "Description"
F 5 "Fairchild Semiconductor" H 3660 5260 50  0001 L CNN "Manufacturer_Name"
F 6 "FAN5333BSX" H 3660 5160 50  0001 L CNN "Manufacturer_Part_Number"
	1    4410 6210
	-1   0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR0172
U 1 1 5D625B1D
P 5060 6700
F 0 "#PWR0172" H 5060 6450 50  0001 C CNN
F 1 "GND" H 5060 6527 50  0000 C CNN
F 2 "" H 5060 6700 50  0001 C CNN
F 3 "" H 5060 6700 50  0001 C CNN
	1    5060 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5060 6410 5600 6410
Wire Wire Line
	5060 6680 5060 6700
Wire Wire Line
	5320 6100 5320 6210
Connection ~ 5320 6210
Wire Wire Line
	5320 6210 5320 6260
Wire Wire Line
	4910 6210 5320 6210
Wire Wire Line
	4630 5780 4920 5780
Wire Wire Line
	4910 6110 4920 6110
Wire Wire Line
	4920 6110 4920 5780
Connection ~ 4920 5780
Wire Wire Line
	4920 5780 4970 5780
Wire Wire Line
	4910 6310 5060 6310
Wire Wire Line
	5060 6310 5060 6410
Connection ~ 5060 6410
Wire Wire Line
	5060 6410 5060 6480
$Comp
L AA-PI-Components:+5V #PWR0173
U 1 1 5D625B32
P 3360 5780
F 0 "#PWR0173" H 3360 5630 50  0001 C CNN
F 1 "+5V" V 3360 5908 50  0000 L CNN
F 2 "" H 3360 5780 50  0001 C CNN
F 3 "" H 3360 5780 50  0001 C CNN
	1    3360 5780
	0    -1   -1   0   
$EndComp
Text Notes 4150 7080 0    59   ~ 12
LCD Backlight
$Comp
L AA-PI-Components:TCA9802 U12
U 1 1 5E34D438
P 7440 2970
F 0 "U12" H 7220 3300 50  0000 C CNN
F 1 "TCA9802" H 7440 2640 50  0000 C CNN
F 2 "AA-PI-Footprints:SOP65P490X110-8N" H 7340 1670 50  0001 L CNN
F 3 "http://www.ti.com/lit/gpn/tca9802" H 7340 1570 50  0001 L CNN
F 4 "Level-Translating I2C Bus Buffer/Repeater" H 7340 1470 50  0001 L CNN "Description"
F 5 "Texas Instruments" H 7340 1270 50  0001 L CNN "Manufacturer_Name"
F 6 "TCA9802DGKR" H 7340 1170 50  0001 L CNN "Manufacturer_Part_Number"
	1    7440 2970
	1    0    0    -1  
$EndComp
Wire Wire Line
	8080 1630 8200 1630
Wire Wire Line
	8440 2050 8440 1920
Wire Wire Line
	8200 3120 8200 1630
Connection ~ 8200 1630
Wire Wire Line
	8200 1630 8550 1630
Wire Wire Line
	7990 3120 8200 3120
Wire Wire Line
	7990 3020 8390 3020
Wire Wire Line
	7990 2920 8390 2920
Text GLabel 8390 3020 2    50   Input ~ 0
CTP_I2C_SDA
Text GLabel 8390 2920 2    50   Input ~ 0
CTP_I2C_SCL
$Comp
L AA-PI-Components:+3V3 #PWR0177
U 1 1 5E3E729F
P 6800 2660
F 0 "#PWR0177" H 6800 2510 50  0001 C CNN
F 1 "+3V3" H 6800 2834 50  0000 C CNN
F 2 "" H 6800 2660 50  0001 C CNN
F 3 "" H 6800 2660 50  0001 C CNN
	1    6800 2660
	1    0    0    -1  
$EndComp
Wire Wire Line
	7990 2820 8060 2820
Wire Wire Line
	8060 2820 8060 2660
Wire Wire Line
	6890 2820 6800 2820
Wire Wire Line
	6800 2820 6800 2660
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5E3F4A74
P 6800 3220
AR Path="/5E3F4A74" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5E3F4A74" Ref="#PWR0178"  Part="1" 
F 0 "#PWR0178" H 6800 2970 50  0001 C CNN
F 1 "GND" H 6800 3047 50  0000 C CNN
F 2 "" H 6800 3220 50  0001 C CNN
F 3 "" H 6800 3220 50  0001 C CNN
	1    6800 3220
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6890 3120 6800 3120
Wire Wire Line
	6800 3120 6800 3220
Text GLabel 6570 3020 0    50   Input ~ 0
I2C0_SDA
Text GLabel 6570 2920 0    50   Input ~ 0
I2C0_SCL
Wire Wire Line
	6890 2920 6570 2920
Wire Wire Line
	6890 3020 6570 3020
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5C6968D0
P 8060 2660
AR Path="/5C6968D0" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C6968D0" Ref="#PWR0179"  Part="1" 
F 0 "#PWR0179" H 8060 2510 50  0001 C CNN
F 1 "+V_LCD" H 7870 2830 50  0000 L CNN
F 2 "" H 8060 2660 50  0001 C CNN
F 3 "" H 8060 2660 50  0001 C CNN
	1    8060 2660
	1    0    0    -1  
$EndComp
$EndSCHEMATC
