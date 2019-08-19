EESchema Schematic File Version 4
LIBS:AA-PI-V3_1-Display-Board-NanoPiA64-cache
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
	7980 5760 8130 5760
Wire Wire Line
	8130 5760 8130 5960
Wire Wire Line
	7980 5560 8380 5560
Wire Wire Line
	7980 5660 8380 5660
Wire Wire Line
	7980 5460 8380 5460
Text GLabel 8380 5360 2    50   Input ~ 0
CTP_RESET
Text GLabel 8380 5460 2    50   Output ~ 0
CTP_INT
Text GLabel 8380 5560 2    50   Input ~ 0
PANEL_I2C_SDA
Text GLabel 8380 5660 2    50   Input ~ 0
PANEL_I2C_SCL
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D3724B8
P 8130 6020
AR Path="/5D3724B8" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D3724B8" Ref="#PWR0156"  Part="1" 
AR Path="/5C724EEF/5D3724B8" Ref="#PWR0166"  Part="1" 
F 0 "#PWR0166" H 8130 5770 50  0001 C CNN
F 1 "GND" H 8130 5847 50  0000 C CNN
F 2 "" H 8130 6020 50  0001 C CNN
F 3 "" H 8130 6020 50  0001 C CNN
	1    8130 6020
	1    0    0    -1  
$EndComp
Wire Wire Line
	7780 5960 8130 5960
Connection ~ 8130 5960
Wire Wire Line
	8130 5960 8130 6020
$Comp
L AA-PI-Components:Conn_01x06_Shielded J?
U 1 1 5D3724C1
P 7780 5460
AR Path="/5D3724C1" Ref="J?"  Part="1" 
AR Path="/5D22BC09/5D3724C1" Ref="J6"  Part="1" 
AR Path="/5C724EEF/5D3724C1" Ref="J1"  Part="1" 
F 0 "J1" H 7780 5790 50  0000 C CNN
F 1 "CTP Interface (6pin)" H 7330 5990 50  0000 C CNN
F 2 "AA-PI-Footprints:FPC-0.5-1-6P_Clamshell_Bottom" H 7780 5460 50  0001 C CNN
F 3 "~" H 7780 5460 50  0001 C CNN
	1    7780 5460
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7980 5260 8380 5260
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D3724C9
P 8380 5260
AR Path="/5D3724C9" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D3724C9" Ref="#PWR0157"  Part="1" 
AR Path="/5C724EEF/5D3724C9" Ref="#PWR0167"  Part="1" 
F 0 "#PWR0167" H 8380 5110 50  0001 C CNN
F 1 "+V_LCD" V 8380 5388 50  0000 L CNN
F 2 "" H 8380 5260 50  0001 C CNN
F 3 "" H 8380 5260 50  0001 C CNN
	1    8380 5260
	0    1    1    0   
$EndComp
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D41FA66
P 7900 1910
AR Path="/5D41FA66" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D41FA66" Ref="#PWR0159"  Part="1" 
AR Path="/5C724EEF/5D41FA66" Ref="#PWR0169"  Part="1" 
F 0 "#PWR0169" H 7900 1660 50  0001 C CNN
F 1 "GND" H 7900 1737 50  0001 C CNN
F 2 "" H 7900 1910 50  0001 C CNN
F 3 "" H 7900 1910 50  0001 C CNN
	1    7900 1910
	-1   0    0    -1  
$EndComp
Text GLabel 7210 1290 0    50   Input ~ 0
PANEL_EN
$Comp
L AA-PI-Components:+5V #PWR?
U 1 1 5D41FA6E
P 8190 1140
AR Path="/5D41FA6E" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D41FA6E" Ref="#PWR0160"  Part="1" 
AR Path="/5C724EEF/5D41FA6E" Ref="#PWR0170"  Part="1" 
F 0 "#PWR0170" H 8190 990 50  0001 C CNN
F 1 "+5V" H 8020 1290 50  0000 L CNN
F 2 "" H 8190 1140 50  0001 C CNN
F 3 "" H 8190 1140 50  0001 C CNN
	1    8190 1140
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D41FA8E
P 10380 1490
AR Path="/5D41FA8E" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D41FA8E" Ref="#PWR0162"  Part="1" 
AR Path="/5C724EEF/5D41FA8E" Ref="#PWR0172"  Part="1" 
F 0 "#PWR0172" H 10380 1340 50  0001 C CNN
F 1 "+V_LCD" V 10380 1618 50  0000 L CNN
F 2 "" H 10380 1490 50  0001 C CNN
F 3 "" H 10380 1490 50  0001 C CNN
	1    10380 1490
	0    1    1    0   
$EndComp
Text Notes 9250 2360 0    59   ~ 12
LCD Step-Down Converter
Wire Wire Line
	1940 6340 1800 6340
Wire Wire Line
	1940 5840 1800 5840
Wire Wire Line
	1940 5540 1800 5540
Wire Wire Line
	1800 5540 1800 5840
Wire Wire Line
	1940 5240 1800 5240
Wire Wire Line
	1800 5240 1800 5540
Wire Wire Line
	1940 5040 1800 5040
Wire Wire Line
	1800 5040 1800 5240
Wire Wire Line
	1940 4740 1800 4740
Wire Wire Line
	1800 4740 1800 5040
Connection ~ 1800 5540
Connection ~ 1800 5240
Connection ~ 1800 5040
Wire Wire Line
	2640 5940 2700 5940
Wire Wire Line
	2700 5940 2700 6440
Wire Wire Line
	2640 5640 2700 5640
Wire Wire Line
	2700 5640 2700 5940
Wire Wire Line
	2640 5340 2700 5340
Wire Wire Line
	2700 5340 2700 5640
Wire Wire Line
	2640 5040 2700 5040
Wire Wire Line
	2700 5040 2700 5340
Connection ~ 2700 5940
Connection ~ 2700 5640
Connection ~ 2700 5340
Wire Wire Line
	2840 6240 2640 6240
Wire Wire Line
	1940 6240 1540 6240
Wire Wire Line
	2840 6140 2640 6140
Wire Wire Line
	1940 6140 1540 6140
Wire Wire Line
	1940 5340 1550 5340
Wire Wire Line
	1940 5440 1550 5440
Wire Wire Line
	2640 5440 2840 5440
Wire Wire Line
	1940 4940 1560 4940
Wire Wire Line
	1940 5940 1540 5940
Wire Wire Line
	1940 5640 1550 5640
Wire Wire Line
	2640 5540 2840 5540
Wire Wire Line
	2640 5840 2840 5840
Text GLabel 1560 4940 0    50   Output ~ 0
LCD_TE_OUT
Text GLabel 1550 5340 0    50   BiDi ~ 0
DSI_D0_P
Text GLabel 1550 5440 0    50   Input ~ 0
DSI_CLK_N
Text GLabel 1550 5640 0    50   Input ~ 0
DSI_D1_P
Text GLabel 1540 5940 0    50   Output ~ 0
PWM_CABC
Text GLabel 1540 6240 0    50   Input ~ 0
LED-A
Text GLabel 2840 5240 2    50   BiDi ~ 0
DSI_D0_N
Text GLabel 2840 5440 2    50   Input ~ 0
DSI_CLK_P
Text GLabel 2840 5540 2    50   Input ~ 0
DSI_D1_N
Text GLabel 2840 5840 2    50   Input ~ 0
LCD_RESET
Text GLabel 2840 6240 2    50   Input ~ 0
LED-A
Wire Wire Line
	2640 6040 2840 6040
Wire Wire Line
	2640 5240 2840 5240
Wire Wire Line
	2590 6440 2700 6440
Wire Wire Line
	2590 4590 2700 4590
Wire Wire Line
	2700 4590 2700 5040
Connection ~ 2700 5040
$Comp
L AA-PI-Components:FH26-33S-0.3SHW J?
U 1 1 5D4E2271
P 2290 5540
AR Path="/5D4E2271" Ref="J?"  Part="1" 
AR Path="/5D22BC09/5D4E2271" Ref="J8"  Part="1" 
AR Path="/5C724EEF/5D4E2271" Ref="J9"  Part="1" 
F 0 "J9" H 2190 6660 50  0000 C CNN
F 1 "LCD Interface" H 2440 6840 50  0000 C CNN
F 2 "AA-PI-Footprints:FH26-33S-0.3SHW" H 1690 4190 50  0001 L CNN
F 3 "http://www.mouser.com/ds/2/185/FH26_catalog-939414.pdf" H 1690 4090 50  0001 L CNN
F 4 "0.3 mm Pitch, 1.0 mm Height FPC Connector" H 1690 3990 50  0001 L CNN "Description"
F 5 "Hirose" H 1690 3790 50  0001 L CNN "Manufacturer_Name"
F 6 "FH26-33S-0.3SHW" H 1690 3690 50  0001 L CNN "Manufacturer_Part_Number"
	1    2290 5540
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D4E2278
P 2700 6620
AR Path="/5D4E2278" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E2278" Ref="#PWR0163"  Part="1" 
AR Path="/5C724EEF/5D4E2278" Ref="#PWR0173"  Part="1" 
F 0 "#PWR0173" H 2700 6370 50  0001 C CNN
F 1 "GND" H 2700 6447 50  0000 C CNN
F 2 "" H 2700 6620 50  0001 C CNN
F 3 "" H 2700 6620 50  0001 C CNN
	1    2700 6620
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D4E227E
P 1800 6620
AR Path="/5D4E227E" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E227E" Ref="#PWR0164"  Part="1" 
AR Path="/5C724EEF/5D4E227E" Ref="#PWR0174"  Part="1" 
F 0 "#PWR0174" H 1800 6370 50  0001 C CNN
F 1 "GND" H 1800 6447 50  0000 C CNN
F 2 "" H 1800 6620 50  0001 C CNN
F 3 "" H 1800 6620 50  0001 C CNN
	1    1800 6620
	1    0    0    -1  
$EndComp
Wire Wire Line
	1940 6040 1540 6040
Wire Wire Line
	2700 6440 2700 6620
Wire Wire Line
	3580 6960 3720 6960
Text GLabel 1540 6140 0    50   Input ~ 0
LED-K
Text GLabel 2840 6140 2    50   Input ~ 0
LED-K
$Comp
L AA-PI-Components:R_Small R?
U 1 1 5D4E228C
P 3900 6800
AR Path="/5D4E228C" Ref="R?"  Part="1" 
AR Path="/5D22BC09/5D4E228C" Ref="R29"  Part="1" 
AR Path="/5C724EEF/5D4E228C" Ref="R43"  Part="1" 
F 0 "R43" V 3970 6740 50  0000 L CNN
F 1 "10K" V 3820 6730 50  0001 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 3900 6800 50  0001 C CNN
F 3 "~" H 3900 6800 50  0001 C CNN
	1    3900 6800
	0    1    -1   0   
$EndComp
$Comp
L AA-PI-Components:R_Small R?
U 1 1 5D4E2293
P 3900 6960
AR Path="/5D4E2293" Ref="R?"  Part="1" 
AR Path="/5D22BC09/5D4E2293" Ref="R30"  Part="1" 
AR Path="/5C724EEF/5D4E2293" Ref="R44"  Part="1" 
F 0 "R44" V 3970 6900 50  0000 L CNN
F 1 "10K" V 3820 6890 50  0001 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 3900 6960 50  0001 C CNN
F 3 "~" H 3900 6960 50  0001 C CNN
	1    3900 6960
	0    1    -1   0   
$EndComp
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D4E229A
P 4060 6800
AR Path="/5D4E229A" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E229A" Ref="#PWR0165"  Part="1" 
AR Path="/5C724EEF/5D4E229A" Ref="#PWR0175"  Part="1" 
F 0 "#PWR0175" H 4060 6550 50  0001 C CNN
F 1 "GND" H 4060 6627 50  0000 C CNN
F 2 "" H 4060 6800 50  0001 C CNN
F 3 "" H 4060 6800 50  0001 C CNN
	1    4060 6800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4000 6800 4060 6800
Wire Wire Line
	4060 6960 4000 6960
Wire Wire Line
	3800 6800 3720 6800
Wire Wire Line
	3720 6800 3720 6960
Connection ~ 3720 6960
Wire Wire Line
	3720 6960 3800 6960
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D4E22A7
P 2840 6040
AR Path="/5D4E22A7" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E22A7" Ref="#PWR0166"  Part="1" 
AR Path="/5C724EEF/5D4E22A7" Ref="#PWR0176"  Part="1" 
F 0 "#PWR0176" H 2840 5890 50  0001 C CNN
F 1 "+V_LCD" V 2840 6168 50  0000 L CNN
F 2 "" H 2840 6040 50  0001 C CNN
F 3 "" H 2840 6040 50  0001 C CNN
	1    2840 6040
	0    1    1    0   
$EndComp
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D4E22AD
P 1540 6040
AR Path="/5D4E22AD" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E22AD" Ref="#PWR0167"  Part="1" 
AR Path="/5C724EEF/5D4E22AD" Ref="#PWR0177"  Part="1" 
F 0 "#PWR0177" H 1540 5890 50  0001 C CNN
F 1 "+V_LCD" V 1540 6168 50  0000 L CNN
F 2 "" H 1540 6040 50  0001 C CNN
F 3 "" H 1540 6040 50  0001 C CNN
	1    1540 6040
	0    -1   -1   0   
$EndComp
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5D4E22B3
P 4060 6960
AR Path="/5D4E22B3" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D4E22B3" Ref="#PWR0168"  Part="1" 
AR Path="/5C724EEF/5D4E22B3" Ref="#PWR0178"  Part="1" 
F 0 "#PWR0178" H 4060 6810 50  0001 C CNN
F 1 "+V_LCD" V 4060 7088 50  0000 L CNN
F 2 "" H 4060 6960 50  0001 C CNN
F 3 "" H 4060 6960 50  0001 C CNN
	1    4060 6960
	0    1    1    0   
$EndComp
Wire Wire Line
	9200 3340 9200 3220
Wire Wire Line
	9200 3220 9450 3220
Wire Wire Line
	7790 3750 7740 3750
Wire Wire Line
	7740 3750 7740 3940
Wire Wire Line
	7560 3750 7740 3750
Connection ~ 7740 3750
Wire Wire Line
	7160 3650 6960 3650
Wire Wire Line
	7160 3850 6960 3850
Wire Wire Line
	7580 3320 7580 3220
Wire Wire Line
	7580 3220 7240 3220
Wire Wire Line
	8140 3220 7740 3220
Wire Wire Line
	7740 3220 7580 3220
Wire Wire Line
	7790 3550 7740 3550
Wire Wire Line
	7740 3550 7740 3220
Connection ~ 7580 3220
Connection ~ 7740 3220
Text GLabel 6960 3650 0    50   Input ~ 0
PWM_CABC
Text GLabel 6960 3850 0    50   Input ~ 0
PWM_MCU
Text GLabel 9670 3220 2    50   Output ~ 0
LED-A
Text GLabel 9670 3900 2    50   Output ~ 0
LED-K
$Comp
L AA-PI-Components:SW_SPDT SW2
U 1 1 5D625ACC
P 7360 3750
F 0 "SW2" H 7290 3620 50  0000 C CNN
F 1 "SW_SPDT" H 7360 3944 50  0001 C CNN
F 2 "AA-PI-Footprints:SW_SPDT_PCM12" H 7360 3750 50  0001 C CNN
F 3 "" H 7360 3750 50  0001 C CNN
	1    7360 3750
	-1   0    0    -1  
$EndComp
$Comp
L AA-PI-Components:C_Small C18
U 1 1 5D625AD3
P 7580 3420
F 0 "C18" H 7370 3480 50  0000 L CNN
F 1 "4.7u" H 7370 3360 50  0000 L CNN
F 2 "AA-PI-Footprints:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 7580 3420 50  0001 C CNN
F 3 "~" H 7580 3420 50  0001 C CNN
	1    7580 3420
	1    0    0    -1  
$EndComp
Wire Wire Line
	7580 3560 7580 3520
$Comp
L AA-PI-Components:GND #PWR0182
U 1 1 5D625ADB
P 7580 3560
F 0 "#PWR0182" H 7580 3310 50  0001 C CNN
F 1 "GND" H 7620 3420 50  0001 C CNN
F 2 "" H 7580 3560 50  0001 C CNN
F 3 "" H 7580 3560 50  0001 C CNN
	1    7580 3560
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:R_Small R22
U 1 1 5D625AE1
P 7740 4040
F 0 "R22" H 7799 4086 50  0000 L CNN
F 1 "100K" H 7799 3995 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 7740 4040 50  0001 C CNN
F 3 "~" H 7740 4040 50  0001 C CNN
	1    7740 4040
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR0183
U 1 1 5D625AE8
P 7740 4180
F 0 "#PWR0183" H 7740 3930 50  0001 C CNN
F 1 "GND" H 7740 4007 50  0000 C CNN
F 2 "" H 7740 4180 50  0001 C CNN
F 3 "" H 7740 4180 50  0001 C CNN
	1    7740 4180
	1    0    0    -1  
$EndComp
Wire Wire Line
	7740 4140 7740 4180
$Comp
L AA-PI-Components:R_Small R30
U 1 1 5D625AEF
P 8940 4070
F 0 "R30" H 8999 4116 50  0000 L CNN
F 1 "15" H 8999 4025 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0805_2012Metric" H 8940 4070 50  0001 C CNN
F 3 "~" H 8940 4070 50  0001 C CNN
	1    8940 4070
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:C_Small C25
U 1 1 5D625AF6
P 9200 3440
F 0 "C25" H 8960 3440 50  0000 L CNN
F 1 "1u" H 9020 3360 50  0000 L CNN
F 2 "AA-PI-Footprints:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 9200 3440 50  0001 C CNN
F 3 "~" H 9200 3440 50  0001 C CNN
	1    9200 3440
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR0184
U 1 1 5D625AFD
P 9200 3700
F 0 "#PWR0184" H 9200 3450 50  0001 C CNN
F 1 "GND" H 9330 3690 50  0001 C CNN
F 2 "" H 9200 3700 50  0001 C CNN
F 3 "" H 9200 3700 50  0001 C CNN
	1    9200 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9090 3220 9200 3220
Connection ~ 9200 3220
$Comp
L AA-PI-Components:L_Core_Ferrite L4
U 1 1 5D625B0C
P 8290 3220
F 0 "L4" V 8410 3220 50  0000 C CNN
F 1 "15uH" V 8220 3220 50  0000 C CNN
F 2 "AA-PI-Footprints:COILCRAFT_XEL5050_HandSolder" H 8290 3220 50  0001 C CNN
F 3 "~" H 8290 3220 50  0001 C CNN
	1    8290 3220
	0    -1   -1   0   
$EndComp
$Comp
L AA-PI-Components:FAN5333BSX U6
U 1 1 5D625B16
P 8290 3650
F 0 "U6" H 8490 3920 50  0000 C CNN
F 1 "FAN5333" H 8290 3370 50  0000 C CNN
F 2 "AA-PI-Footprints:SOT95P270X145-5N" H 7540 3100 50  0001 L CNN
F 3 "http://www.onsemi.com/pub/Collateral/FAN5333B-D.pdf" H 7540 3000 50  0001 L CNN
F 4 "LED Driver, 30V Integrated Switch SOT23" H 7540 2900 50  0001 L CNN "Description"
F 5 "Fairchild Semiconductor" H 7540 2700 50  0001 L CNN "Manufacturer_Name"
F 6 "FAN5333BSX" H 7540 2600 50  0001 L CNN "Manufacturer_Part_Number"
	1    8290 3650
	-1   0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR0187
U 1 1 5D625B1D
P 8940 4190
F 0 "#PWR0187" H 8940 3940 50  0001 C CNN
F 1 "GND" H 8940 4017 50  0000 C CNN
F 2 "" H 8940 4190 50  0001 C CNN
F 3 "" H 8940 4190 50  0001 C CNN
	1    8940 4190
	1    0    0    -1  
$EndComp
Wire Wire Line
	8940 3900 9670 3900
Wire Wire Line
	8940 4170 8940 4190
Wire Wire Line
	9200 3540 9200 3650
Connection ~ 9200 3650
Wire Wire Line
	9200 3650 9200 3700
Wire Wire Line
	8790 3650 9200 3650
Wire Wire Line
	8440 3220 8800 3220
Wire Wire Line
	8790 3550 8800 3550
Wire Wire Line
	8800 3550 8800 3220
Connection ~ 8800 3220
Wire Wire Line
	8800 3220 8890 3220
Wire Wire Line
	8790 3750 8940 3750
Wire Wire Line
	8940 3750 8940 3900
Connection ~ 8940 3900
Wire Wire Line
	8940 3900 8940 3970
$Comp
L AA-PI-Components:+5V #PWR0189
U 1 1 5D625B32
P 7240 3220
F 0 "#PWR0189" H 7240 3070 50  0001 C CNN
F 1 "+5V" V 7240 3348 50  0000 L CNN
F 2 "" H 7240 3220 50  0001 C CNN
F 3 "" H 7240 3220 50  0001 C CNN
	1    7240 3220
	0    -1   -1   0   
$EndComp
Text Notes 8040 4380 0    59   ~ 12
LCD Backlight
$Comp
L AA-PI-Components:TCA9802 U10
U 1 1 5E34D438
P 6730 2510
F 0 "U10" H 6510 2840 50  0000 C CNN
F 1 "TCA9802" H 6730 2180 50  0000 C CNN
F 2 "AA-PI-Footprints:SOP65P490X110-8N" H 6630 1210 50  0001 L CNN
F 3 "http://www.ti.com/lit/gpn/tca9802" H 6630 1110 50  0001 L CNN
F 4 "Level-Translating I2C Bus Buffer/Repeater" H 6630 1010 50  0001 L CNN "Description"
F 5 "Texas Instruments" H 6630 810 50  0001 L CNN "Manufacturer_Name"
F 6 "TCA9802DGKR" H 6630 710 50  0001 L CNN "Manufacturer_Part_Number"
	1    6730 2510
	1    0    0    -1  
$EndComp
Wire Wire Line
	7210 1290 7340 1290
Wire Wire Line
	7900 1910 7900 1800
Wire Wire Line
	7500 2660 7500 1290
Wire Wire Line
	7280 2660 7500 2660
Text GLabel 7720 2560 2    50   Input ~ 0
PANEL_I2C_SDA
Text GLabel 7720 2460 2    50   Input ~ 0
PANEL_I2C_SCL
$Comp
L AA-PI-Components:+3V3 #PWR0196
U 1 1 5E3E729F
P 6120 2200
F 0 "#PWR0196" H 6120 2050 50  0001 C CNN
F 1 "+3V3" H 6120 2374 50  0000 C CNN
F 2 "" H 6120 2200 50  0001 C CNN
F 3 "" H 6120 2200 50  0001 C CNN
	1    6120 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7280 2360 7340 2360
Wire Wire Line
	7340 2360 7340 2200
Wire Wire Line
	6180 2360 6120 2360
Wire Wire Line
	6120 2360 6120 2200
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5E3F4A74
P 6120 2760
AR Path="/5E3F4A74" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5E3F4A74" Ref="#PWR0178"  Part="1" 
AR Path="/5C724EEF/5E3F4A74" Ref="#PWR0185"  Part="1" 
F 0 "#PWR0185" H 6120 2510 50  0001 C CNN
F 1 "GND" H 6120 2587 50  0000 C CNN
F 2 "" H 6120 2760 50  0001 C CNN
F 3 "" H 6120 2760 50  0001 C CNN
	1    6120 2760
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6180 2660 6120 2660
Wire Wire Line
	6120 2660 6120 2760
Text GLabel 5540 2560 0    50   Input ~ 0
I2C0_SDA
Text GLabel 5540 2460 0    50   Input ~ 0
I2C0_SCL
Wire Wire Line
	6180 2460 5800 2460
Wire Wire Line
	6180 2560 5680 2560
Text Notes 6390 1960 0    59   ~ 12
I2C Bus Repeater
Wire Wire Line
	7280 2560 7720 2560
Wire Wire Line
	7280 2460 7720 2460
$Comp
L AA-PI-Components:D_Schottky_Small D8
U 1 1 5C64A7F3
P 8990 3220
F 0 "D8" H 9020 3320 50  0000 C CNN
F 1 "1N5819" H 8970 3130 50  0000 C CNN
F 2 "AA-PI-Footprints:D_SOD-123" V 8990 3220 50  0001 C CNN
F 3 "~" V 8990 3220 50  0001 C CNN
	1    8990 3220
	-1   0    0    1   
$EndComp
$Comp
L AA-PI-Components:TPS62046DGQ U12
U 1 1 5C717C6F
P 8970 1590
F 0 "U12" H 8570 2090 50  0000 C CNN
F 1 "TPS62046DGQ" H 9200 2090 50  0000 C CNN
F 2 "AA-PI-Footprints:SOP50P490X110-11N" H 8420 490 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps62046.pdf" H 8420 390 50  0001 L CNN
F 4 "TPS62046 3.3V dc-dc SD converter, MSOP10" H 8420 290 50  0001 L CNN "Description"
F 5 "Texas Instruments" H 8420 90  50  0001 L CNN "Manufacturer_Name"
F 6 "TPS62046DGQ" H 8420 -10 50  0001 L CNN "Manufacturer_Part_Number"
	1    8970 1590
	1    0    0    -1  
$EndComp
Wire Wire Line
	8270 1390 8190 1390
Wire Wire Line
	8270 1490 8190 1490
Wire Wire Line
	8190 1490 8190 1390
Connection ~ 8190 1390
Wire Wire Line
	8190 1390 7900 1390
Wire Wire Line
	8270 1290 7500 1290
Connection ~ 7500 1290
Wire Wire Line
	7900 1600 7900 1390
Wire Wire Line
	8190 1390 8190 1140
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5C7732E7
P 8050 1910
AR Path="/5C7732E7" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C7732E7" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C7732E7" Ref="#PWR0103"  Part="1" 
F 0 "#PWR0103" H 8050 1660 50  0001 C CNN
F 1 "GND" H 8050 1737 50  0001 C CNN
F 2 "" H 8050 1910 50  0001 C CNN
F 3 "" H 8050 1910 50  0001 C CNN
	1    8050 1910
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8270 1590 8050 1590
Wire Wire Line
	8050 1590 8050 1910
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5C787691
P 9790 1910
AR Path="/5C787691" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C787691" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C787691" Ref="#PWR0104"  Part="1" 
F 0 "#PWR0104" H 9790 1660 50  0001 C CNN
F 1 "GND" H 9790 1737 50  0001 C CNN
F 2 "" H 9790 1910 50  0001 C CNN
F 3 "" H 9790 1910 50  0001 C CNN
	1    9790 1910
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9670 1690 9790 1690
Wire Wire Line
	9790 1690 9790 1910
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5C78DF05
P 8970 2250
AR Path="/5C78DF05" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C78DF05" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C78DF05" Ref="#PWR0168"  Part="1" 
F 0 "#PWR0168" H 8970 2000 50  0001 C CNN
F 1 "GND" H 8970 2077 50  0001 C CNN
F 2 "" H 8970 2250 50  0001 C CNN
F 3 "" H 8970 2250 50  0001 C CNN
	1    8970 2250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9670 1590 9790 1590
Wire Wire Line
	9790 1590 9790 1490
Wire Wire Line
	9790 1490 9670 1490
Wire Wire Line
	9670 1290 9790 1290
Wire Wire Line
	9790 1290 9790 1390
Wire Wire Line
	9790 1390 9670 1390
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5C7CD048
P 9790 1140
AR Path="/5C7CD048" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C7CD048" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C7CD048" Ref="#PWR0171"  Part="1" 
F 0 "#PWR0171" H 9790 890 50  0001 C CNN
F 1 "GND" H 9790 967 50  0001 C CNN
F 2 "" H 9790 1140 50  0001 C CNN
F 3 "" H 9790 1140 50  0001 C CNN
	1    9790 1140
	1    0    0    1   
$EndComp
Wire Wire Line
	9790 1290 9790 1140
$Comp
L AA-PI-Components:L_Core_Ferrite L5
U 1 1 5C803CD4
P 10040 1490
F 0 "L5" V 10150 1500 50  0000 C CNN
F 1 "6.2uH" V 9960 1500 50  0000 C CNN
F 2 "AA-PI-Footprints:COILCRAFT_XEL5050_HandSolder" H 10040 1490 50  0001 C CNN
F 3 "~" H 10040 1490 50  0001 C CNN
	1    10040 1490
	0    -1   -1   0   
$EndComp
Connection ~ 9790 1290
Wire Wire Line
	8270 1690 8190 1690
Wire Wire Line
	8190 1690 8190 1980
Wire Wire Line
	9790 1490 9890 1490
Connection ~ 9790 1490
Wire Wire Line
	10290 1620 10290 1490
Connection ~ 10290 1490
Wire Wire Line
	10290 1490 10380 1490
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5C8ADAC7
P 10290 1910
AR Path="/5C8ADAC7" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C8ADAC7" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C8ADAC7" Ref="#PWR0192"  Part="1" 
F 0 "#PWR0192" H 10290 1660 50  0001 C CNN
F 1 "GND" H 10290 1737 50  0001 C CNN
F 2 "" H 10290 1910 50  0001 C CNN
F 3 "" H 10290 1910 50  0001 C CNN
	1    10290 1910
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10290 1820 10290 1910
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5C6968D0
P 7340 2200
AR Path="/5C6968D0" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C6968D0" Ref="#PWR0179"  Part="1" 
AR Path="/5C724EEF/5C6968D0" Ref="#PWR0186"  Part="1" 
F 0 "#PWR0186" H 7340 2050 50  0001 C CNN
F 1 "+V_LCD" H 7150 2370 50  0000 L CNN
F 2 "" H 7340 2200 50  0001 C CNN
F 3 "" H 7340 2200 50  0001 C CNN
	1    7340 2200
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:CP_Small C43
U 1 1 5C8D7229
P 10290 1720
F 0 "C43" H 10390 1780 50  0000 L CNN
F 1 "47u" H 10378 1675 50  0000 L CNN
F 2 "AA-PI-Footprints:CP_EIA-3528-21_Kemet-B" H 10290 1720 50  0001 C CNN
F 3 "~" H 10290 1720 50  0001 C CNN
	1    10290 1720
	1    0    0    -1  
$EndComp
Wire Wire Line
	8970 2250 8970 2190
Wire Wire Line
	10190 1490 10290 1490
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5C93CF15
P 8190 1980
AR Path="/5C93CF15" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C93CF15" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C93CF15" Ref="#PWR0199"  Part="1" 
F 0 "#PWR0199" H 8190 1830 50  0001 C CNN
F 1 "+V_LCD" H 7970 2140 50  0000 L CNN
F 2 "" H 8190 1980 50  0001 C CNN
F 3 "" H 8190 1980 50  0001 C CNN
	1    8190 1980
	-1   0    0    1   
$EndComp
$Comp
L AA-PI-Components:C_Small C41
U 1 1 5C71B4BE
P 7900 1700
F 0 "C41" H 7690 1760 50  0000 L CNN
F 1 "22u" H 7690 1640 50  0000 L CNN
F 2 "AA-PI-Footprints:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 7900 1700 50  0001 C CNN
F 3 "~" H 7900 1700 50  0001 C CNN
	1    7900 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4470 1630 4130 1630
Wire Wire Line
	4470 1530 4130 1530
Wire Wire Line
	4470 1330 4130 1330
Wire Wire Line
	4470 1230 4130 1230
Wire Wire Line
	4470 1830 4130 1830
Wire Wire Line
	4470 1930 4130 1930
Text GLabel 4130 1930 0    50   Output ~ 0
DSI_D1_N
Text GLabel 4130 1830 0    50   Output ~ 0
DSI_D1_P
Text GLabel 4130 1330 0    50   Output ~ 0
DSI_CLK_N
Text GLabel 4130 1230 0    50   Output ~ 0
DSI_CLK_P
Text GLabel 4130 1630 0    50   BiDi ~ 0
DSI_D0_N
Text GLabel 4130 1530 0    50   BiDi ~ 0
DSI_D0_P
Wire Wire Line
	4470 3330 4020 3330
Text GLabel 4020 3330 0    50   Input ~ 0
CTP_INT
Text GLabel 4020 3630 0    50   Input ~ 0
I2C0_SDA
Text GLabel 4020 3530 0    50   Input ~ 0
I2C0_SCL
Wire Wire Line
	4370 3430 4470 3430
Wire Wire Line
	4470 3730 4370 3730
Connection ~ 4370 3730
Wire Wire Line
	4370 3730 4370 3430
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5C837D50
P 4370 4280
AR Path="/5C837D50" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C837D50" Ref="#PWR?"  Part="1" 
AR Path="/5C7E466D/5C837D50" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C837D50" Ref="#PWR0200"  Part="1" 
F 0 "#PWR0200" H 4370 4030 50  0001 C CNN
F 1 "GND" H 4370 4107 50  0000 C CNN
F 2 "" H 4370 4280 50  0001 C CNN
F 3 "" H 4370 4280 50  0001 C CNN
	1    4370 4280
	1    0    0    -1  
$EndComp
Connection ~ 4370 3430
Wire Wire Line
	4370 1730 4470 1730
Wire Wire Line
	4370 1730 4370 1430
Wire Wire Line
	4370 1430 4470 1430
Connection ~ 4370 1730
Text GLabel 4000 2930 0    50   Output ~ 0
LCD_RESET
Wire Wire Line
	4470 1130 4370 1130
Wire Wire Line
	4370 1130 4370 1430
Connection ~ 4370 1430
Wire Wire Line
	4370 3730 4370 4230
Wire Wire Line
	4470 3230 4370 3230
Connection ~ 4370 3230
Wire Wire Line
	4370 3230 4370 3430
Wire Wire Line
	4470 3030 4370 3030
Wire Wire Line
	4370 1730 4370 2030
Connection ~ 4370 3030
Wire Wire Line
	4370 3030 4370 3230
Wire Wire Line
	4470 2730 4370 2730
Connection ~ 4370 2730
Wire Wire Line
	4370 2730 4370 3030
Wire Wire Line
	4470 2630 4370 2630
Connection ~ 4370 2630
Wire Wire Line
	4370 2630 4370 2730
Wire Wire Line
	4470 2330 4370 2330
Connection ~ 4370 2330
Wire Wire Line
	4370 2330 4370 2630
Wire Wire Line
	4370 2030 4470 2030
Connection ~ 4370 2030
Wire Wire Line
	4370 2030 4370 2330
$Comp
L AA-PI-Components:Conn_01x30_Shielded J?
U 1 1 5C837D74
P 4670 2530
AR Path="/5C837D74" Ref="J?"  Part="1" 
AR Path="/5D22BC09/5C837D74" Ref="J?"  Part="1" 
AR Path="/5C7E466D/5C837D74" Ref="J?"  Part="1" 
AR Path="/5C724EEF/5C837D74" Ref="J8"  Part="1" 
F 0 "J8" H 4590 4050 50  0000 L CNN
F 1 "NanoPi A64 Display Interface" H 3950 4290 50  0000 L CNN
F 2 "AA-PI-Footprints:FPC-0.5-1-30P_Clamshell_Bottom" H 4670 2530 50  0001 C CNN
F 3 "~" H 4670 2530 50  0001 C CNN
	1    4670 2530
	1    0    0    -1  
$EndComp
Wire Wire Line
	4670 4230 4370 4230
Connection ~ 4370 4230
Wire Wire Line
	4370 4230 4370 4280
$Comp
L AA-PI-Components:R_Small R?
U 1 1 5C837D80
P 5680 2740
AR Path="/5C837D80" Ref="R?"  Part="1" 
AR Path="/5D22BC09/5C837D80" Ref="R?"  Part="1" 
AR Path="/5C7E466D/5C837D80" Ref="R?"  Part="1" 
AR Path="/5C724EEF/5C837D80" Ref="R40"  Part="1" 
F 0 "R40" H 5480 2780 50  0000 L CNN
F 1 "10K" H 5480 2690 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5680 2740 50  0001 C CNN
F 3 "~" H 5680 2740 50  0001 C CNN
	1    5680 2740
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:R_Small R?
U 1 1 5C837D87
P 5800 2740
AR Path="/5C837D87" Ref="R?"  Part="1" 
AR Path="/5D22BC09/5C837D87" Ref="R?"  Part="1" 
AR Path="/5C7E466D/5C837D87" Ref="R?"  Part="1" 
AR Path="/5C724EEF/5C837D87" Ref="R38"  Part="1" 
F 0 "R38" H 5850 2780 50  0000 L CNN
F 1 "10K" H 5850 2690 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5800 2740 50  0001 C CNN
F 3 "~" H 5800 2740 50  0001 C CNN
	1    5800 2740
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 2640 5800 2460
$Comp
L AA-PI-Components:+3V3 #PWR?
U 1 1 5C837D8F
P 5570 2960
AR Path="/5C837D8F" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C837D8F" Ref="#PWR?"  Part="1" 
AR Path="/5C7E466D/5C837D8F" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C837D8F" Ref="#PWR0201"  Part="1" 
F 0 "#PWR0201" H 5570 2810 50  0001 C CNN
F 1 "+3V3" V 5590 3080 50  0000 L CNN
F 2 "" H 5570 2960 50  0001 C CNN
F 3 "" H 5570 2960 50  0001 C CNN
	1    5570 2960
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5570 2960 5680 2960
Wire Wire Line
	5800 2960 5800 2840
Wire Wire Line
	5680 2960 5680 2840
Connection ~ 5680 2960
Wire Wire Line
	5680 2960 5800 2960
Wire Wire Line
	5680 2640 5680 2560
Wire Wire Line
	7980 5360 8380 5360
Text GLabel 4000 3130 0    50   Output ~ 0
CTP_RESET
Wire Wire Line
	4000 3130 4470 3130
$Comp
L AA-PI-Components:R_Small R?
U 1 1 5C931A42
P 3930 2740
AR Path="/5C931A42" Ref="R?"  Part="1" 
AR Path="/5D22BC09/5C931A42" Ref="R?"  Part="1" 
AR Path="/5C7E466D/5C931A42" Ref="R?"  Part="1" 
AR Path="/5C724EEF/5C931A42" Ref="R42"  Part="1" 
F 0 "R42" V 4000 2680 50  0000 L CNN
F 1 "10K" V 3850 2680 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 3930 2740 50  0001 C CNN
F 3 "~" H 3930 2740 50  0001 C CNN
	1    3930 2740
	0    -1   -1   0   
$EndComp
$Comp
L AA-PI-Components:+3V3 #PWR?
U 1 1 5C94089B
P 3780 2740
AR Path="/5C94089B" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C94089B" Ref="#PWR?"  Part="1" 
AR Path="/5C7E466D/5C94089B" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C94089B" Ref="#PWR0202"  Part="1" 
F 0 "#PWR0202" H 3780 2590 50  0001 C CNN
F 1 "+3V3" V 3820 2870 50  0000 L CNN
F 2 "" H 3780 2740 50  0001 C CNN
F 3 "" H 3780 2740 50  0001 C CNN
	1    3780 2740
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3830 2740 3780 2740
$Comp
L AA-PI-Components:FH26-25S-0.3SHW J13
U 1 1 5C9F8357
P 5550 5490
F 0 "J13" H 5460 6380 50  0000 C CNN
F 1 "LCD Interface" H 5580 4370 50  0000 C CNN
F 2 "AA-PI-Footprints:FH26-25S-0.3SHW" H 4900 4190 50  0001 L CNN
F 3 "http://www.mouser.com/ds/2/185/FH26_catalog-939414.pdf" H 4900 4090 50  0001 L CNN
F 4 "0.3 mm Pitch, 1.0 mm Height FPC Connector" H 4900 3990 50  0001 L CNN "Description"
F 5 "Hirose" H 4900 3790 50  0001 L CNN "Manufacturer_Name"
F 6 "FH26-25S-0.3SHW" H 4900 3690 50  0001 L CNN "Manufacturer_Part_Number"
	1    5550 5490
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:Conn_01x24_Shielded J11
U 1 1 5CA6BF90
P 2280 2210
F 0 "J11" H 2270 3460 50  0000 L CNN
F 1 "BananaPi M2magic Display Interface" H 1850 3660 50  0000 L CNN
F 2 "AA-PI-Footprints:FPC-0.5-1-24P_CLAMSH_BTM" H 2280 2210 50  0001 C CNN
F 3 "~" H 2280 2210 50  0001 C CNN
	1    2280 2210
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5C9CEBE6
P 6000 4670
AR Path="/5C9CEBE6" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5C9CEBE6" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5C9CEBE6" Ref="#PWR0203"  Part="1" 
F 0 "#PWR0203" H 6000 4420 50  0001 C CNN
F 1 "GND" H 6000 4497 50  0000 C CNN
F 2 "" H 6000 4670 50  0001 C CNN
F 3 "" H 6000 4670 50  0001 C CNN
	1    6000 4670
	-1   0    0    1   
$EndComp
Wire Wire Line
	5850 6290 6000 6290
Wire Wire Line
	5000 6140 5200 6140
Text GLabel 5000 6140 0    50   Input ~ 0
LED-A
Wire Wire Line
	6100 6040 5900 6040
Text GLabel 6100 6040 2    50   Input ~ 0
LED-K
Wire Wire Line
	5900 5940 6050 5940
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5CADB311
P 6050 5940
AR Path="/5CADB311" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5CADB311" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5CADB311" Ref="#PWR0204"  Part="1" 
F 0 "#PWR0204" H 6050 5790 50  0001 C CNN
F 1 "+V_LCD" V 6050 6068 50  0000 L CNN
F 2 "" H 6050 5940 50  0001 C CNN
F 3 "" H 6050 5940 50  0001 C CNN
	1    6050 5940
	0    1    1    0   
$EndComp
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5CB20BF9
P 5050 5940
AR Path="/5CB20BF9" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5CB20BF9" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5CB20BF9" Ref="#PWR0205"  Part="1" 
F 0 "#PWR0205" H 5050 5790 50  0001 C CNN
F 1 "+V_LCD" V 5050 6068 50  0000 L CNN
F 2 "" H 5050 5940 50  0001 C CNN
F 3 "" H 5050 5940 50  0001 C CNN
	1    5050 5940
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5200 5940 5050 5940
Wire Wire Line
	5200 5840 5000 5840
Text GLabel 5000 5840 0    50   Input ~ 0
LCD_RESET
Wire Wire Line
	5900 5740 6100 5740
Text GLabel 6100 5740 2    50   Output ~ 0
PWM_CABC
Wire Wire Line
	5850 4790 6000 4790
Connection ~ 2700 6440
Wire Wire Line
	5200 5640 5000 5640
Text GLabel 5000 5640 0    50   Input ~ 0
DSI_D2_P
Text GLabel 6100 5540 2    50   Input ~ 0
DSI_D2_N
Wire Wire Line
	5900 5540 6100 5540
Wire Wire Line
	5900 5440 6100 5440
Text GLabel 6100 5440 2    50   Input ~ 0
DSI_D1_P
Wire Wire Line
	5200 5440 5000 5440
Text GLabel 5000 5440 0    50   Input ~ 0
DSI_D1_N
Wire Wire Line
	5200 5540 5100 5540
Wire Wire Line
	5200 5340 5000 5340
Text GLabel 5000 5340 0    50   Input ~ 0
DSI_CLK_P
Wire Wire Line
	5900 5240 6100 5240
Text GLabel 6100 5240 2    50   Input ~ 0
DSI_CLK_N
Text GLabel 6100 4940 2    50   Input ~ 0
DSI_D3_N
Wire Wire Line
	5900 4940 6100 4940
Wire Wire Line
	5200 5040 5000 5040
Text GLabel 5000 5040 0    50   Input ~ 0
DSI_D3_P
Text GLabel 6100 5140 2    50   BiDi ~ 0
DSI_D0_P
Wire Wire Line
	5900 5140 6100 5140
Text GLabel 5000 5140 0    50   BiDi ~ 0
DSI_D0_N
Wire Wire Line
	5200 5140 5000 5140
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5CECFB52
P 1960 3710
AR Path="/5CECFB52" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5CECFB52" Ref="#PWR?"  Part="1" 
AR Path="/5C7E466D/5CECFB52" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5CECFB52" Ref="#PWR0207"  Part="1" 
F 0 "#PWR0207" H 1960 3460 50  0001 C CNN
F 1 "GND" H 1960 3537 50  0000 C CNN
F 2 "" H 1960 3710 50  0001 C CNN
F 3 "" H 1960 3710 50  0001 C CNN
	1    1960 3710
	1    0    0    -1  
$EndComp
Wire Wire Line
	2280 3610 2280 3660
Wire Wire Line
	2280 3660 1960 3660
Wire Wire Line
	1960 3660 1960 3210
Connection ~ 1960 3660
Wire Wire Line
	1960 3660 1960 3710
Text GLabel 1800 1110 0    50   BiDi ~ 0
DSI_D0_N
Wire Wire Line
	2080 1110 1800 1110
Text GLabel 1800 1210 0    50   BiDi ~ 0
DSI_D0_P
Wire Wire Line
	2080 1210 1800 1210
Wire Wire Line
	1960 3110 2080 3110
Wire Wire Line
	2080 3210 1960 3210
Connection ~ 1960 3210
Wire Wire Line
	1960 3210 1960 3110
Wire Wire Line
	2080 3010 1800 3010
Text GLabel 1800 3010 0    50   Input ~ 0
PWM_MCU
Text GLabel 1800 2810 0    50   Output ~ 0
LCD_RESET
Wire Wire Line
	1800 2810 2080 2810
Text GLabel 1800 2610 0    50   Output ~ 0
CTP_RESET
Wire Wire Line
	1800 2610 2080 2610
Wire Wire Line
	2080 2510 1800 2510
Text GLabel 1800 2510 0    50   Input ~ 0
CTP_INT
Text GLabel 1800 2310 0    50   Input ~ 0
I2C0_SDA
Text GLabel 1800 2410 0    50   Input ~ 0
I2C0_SCL
Wire Wire Line
	2080 2410 1800 2410
Wire Wire Line
	2080 2310 1800 2310
Wire Wire Line
	2080 2010 1800 2010
Text GLabel 1800 2010 0    50   Output ~ 0
DSI_D3_P
Text GLabel 1800 1910 0    50   Output ~ 0
DSI_D3_N
Wire Wire Line
	2080 1910 1800 1910
Wire Wire Line
	2080 1810 1800 1810
Text GLabel 1800 1810 0    50   Output ~ 0
DSI_D2_P
Text GLabel 1800 1710 0    50   Output ~ 0
DSI_D2_N
Wire Wire Line
	2080 1710 1800 1710
Wire Wire Line
	2080 1610 1800 1610
Text GLabel 1800 1610 0    50   Output ~ 0
DSI_CLK_P
Wire Wire Line
	2080 1510 1800 1510
Text GLabel 1800 1510 0    50   Output ~ 0
DSI_CLK_N
Wire Wire Line
	2080 1410 1800 1410
Text GLabel 1800 1410 0    50   Output ~ 0
DSI_D1_P
Wire Wire Line
	2080 1310 1800 1310
Text GLabel 1800 1310 0    50   Output ~ 0
DSI_D1_N
Connection ~ 5680 2560
Wire Wire Line
	5680 2560 5540 2560
Connection ~ 5800 2460
Wire Wire Line
	5800 2460 5540 2460
Wire Wire Line
	4020 3630 4470 3630
Wire Wire Line
	4020 3530 4470 3530
Connection ~ 1800 5840
Wire Wire Line
	1800 6620 1800 6340
Wire Wire Line
	1800 5840 1800 6340
Connection ~ 1800 6340
Text GLabel 2840 4840 2    50   Input ~ 0
LCM_ID
Wire Wire Line
	2640 4840 2840 4840
Text GLabel 3580 6960 0    50   Input ~ 0
LCM_ID
Wire Wire Line
	4470 2130 4140 2130
Text GLabel 4140 2130 0    50   Output ~ 0
DSI_D2_P
Text GLabel 4140 2230 0    50   Output ~ 0
DSI_D2_N
Wire Wire Line
	4470 2230 4140 2230
Wire Wire Line
	4000 2930 4260 2930
Wire Wire Line
	4470 2530 4140 2530
Text GLabel 4140 2530 0    50   Output ~ 0
DSI_D3_P
Text GLabel 4140 2430 0    50   Output ~ 0
DSI_D3_N
Wire Wire Line
	4470 2430 4140 2430
Wire Wire Line
	4260 2930 4260 2740
Wire Wire Line
	4030 2740 4260 2740
Connection ~ 4260 2930
Wire Wire Line
	4260 2930 4470 2930
$Comp
L AA-PI-Components:D_Zener D9
U 1 1 5CAE8CA6
P 9450 3460
F 0 "D9" V 9404 3539 50  0000 L CNN
F 1 "BZT52" V 9495 3539 50  0000 L CNN
F 2 "AA-PI-Footprints:D_SOD-123" H 9450 3460 50  0001 C CNN
F 3 "~" H 9450 3460 50  0001 C CNN
	1    9450 3460
	0    1    1    0   
$EndComp
Wire Wire Line
	9450 3310 9450 3220
Connection ~ 9450 3220
Wire Wire Line
	9450 3220 9670 3220
$Comp
L AA-PI-Components:GND #PWR0208
U 1 1 5CB0A929
P 9450 3700
F 0 "#PWR0208" H 9450 3450 50  0001 C CNN
F 1 "GND" H 9580 3690 50  0001 C CNN
F 2 "" H 9450 3700 50  0001 C CNN
F 3 "" H 9450 3700 50  0001 C CNN
	1    9450 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 3700 9450 3610
Text Notes 9240 4210 0    50   ~ 0
C25 max voltage > 30V\nD9 zener diode 32V (22V for FAN5331)
$Comp
L AA-PI-Components:Conn_01x10_Shielded J14
U 1 1 5CACD2E9
P 9570 5460
F 0 "J14" H 9580 5980 50  0000 C CNN
F 1 "CTP Interface (10pin)" H 8940 6090 50  0000 C CNN
F 2 "AA-PI-Footprints:FPC-0.5-1-10P_Clamshell_Bottom" H 9570 5460 50  0001 C CNN
F 3 "~" H 9570 5460 50  0001 C CNN
	1    9570 5460
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9770 5660 10170 5660
Wire Wire Line
	9770 5760 10170 5760
Text GLabel 10170 5960 2    50   Input ~ 0
CTP_RESET
Text GLabel 10170 5760 2    50   Output ~ 0
CTP_INT
Text GLabel 10170 5660 2    50   Input ~ 0
PANEL_I2C_SDA
Text GLabel 10170 5560 2    50   Input ~ 0
PANEL_I2C_SCL
Wire Wire Line
	9770 5860 10170 5860
$Comp
L AA-PI-Components:+V_LCD #PWR?
U 1 1 5CADEE2F
P 10170 5860
AR Path="/5CADEE2F" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5CADEE2F" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5CADEE2F" Ref="#PWR0209"  Part="1" 
F 0 "#PWR0209" H 10170 5710 50  0001 C CNN
F 1 "+V_LCD" V 10170 5988 50  0000 L CNN
F 2 "" H 10170 5860 50  0001 C CNN
F 3 "" H 10170 5860 50  0001 C CNN
	1    10170 5860
	0    1    1    0   
$EndComp
Wire Wire Line
	9770 5960 10170 5960
Wire Wire Line
	9920 5460 9920 6160
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5CB035B4
P 9920 6220
AR Path="/5CB035B4" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5CB035B4" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5CB035B4" Ref="#PWR0210"  Part="1" 
F 0 "#PWR0210" H 9920 5970 50  0001 C CNN
F 1 "GND" H 9920 6047 50  0000 C CNN
F 2 "" H 9920 6220 50  0001 C CNN
F 3 "" H 9920 6220 50  0001 C CNN
	1    9920 6220
	1    0    0    -1  
$EndComp
Wire Wire Line
	9570 6160 9920 6160
Connection ~ 9920 6160
Wire Wire Line
	9920 6160 9920 6220
Wire Wire Line
	9770 5560 10170 5560
Wire Wire Line
	9770 5460 9920 5460
Wire Wire Line
	7340 1290 7340 1430
$Comp
L AA-PI-Components:R_Small R46
U 1 1 5CD8DA00
P 7340 1530
F 0 "R46" H 7080 1580 50  0000 L CNN
F 1 "100K" H 7070 1490 50  0000 L CNN
F 2 "AA-PI-Footprints:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 7340 1530 50  0001 C CNN
F 3 "~" H 7340 1530 50  0001 C CNN
	1    7340 1530
	1    0    0    -1  
$EndComp
$Comp
L AA-PI-Components:GND #PWR0212
U 1 1 5CD8DA06
P 7340 1670
F 0 "#PWR0212" H 7340 1420 50  0001 C CNN
F 1 "GND" H 7340 1497 50  0001 C CNN
F 2 "" H 7340 1670 50  0001 C CNN
F 3 "" H 7340 1670 50  0001 C CNN
	1    7340 1670
	1    0    0    -1  
$EndComp
Wire Wire Line
	7340 1630 7340 1670
Connection ~ 7340 1290
Wire Wire Line
	7340 1290 7500 1290
$Comp
L AA-PI-Components:GND #PWR?
U 1 1 5D2FF4CB
P 5100 4670
AR Path="/5D2FF4CB" Ref="#PWR?"  Part="1" 
AR Path="/5D22BC09/5D2FF4CB" Ref="#PWR?"  Part="1" 
AR Path="/5C724EEF/5D2FF4CB" Ref="#PWR0217"  Part="1" 
F 0 "#PWR0217" H 5100 4420 50  0001 C CNN
F 1 "GND" H 5100 4497 50  0000 C CNN
F 2 "" H 5100 4670 50  0001 C CNN
F 3 "" H 5100 4670 50  0001 C CNN
	1    5100 4670
	-1   0    0    1   
$EndComp
Wire Wire Line
	5200 4940 5100 4940
Wire Wire Line
	5100 4940 5100 4670
Wire Wire Line
	5200 5240 5100 5240
Wire Wire Line
	5100 5240 5100 4940
Connection ~ 5100 4940
Wire Wire Line
	5100 5240 5100 5540
Connection ~ 5100 5240
Wire Wire Line
	6000 4790 6000 4670
Connection ~ 6000 4790
Wire Wire Line
	6000 4790 6000 5040
Wire Wire Line
	5900 5640 6000 5640
Connection ~ 6000 5640
Wire Wire Line
	6000 5640 6000 6290
Wire Wire Line
	5900 5340 6000 5340
Connection ~ 6000 5340
Wire Wire Line
	6000 5340 6000 5640
Wire Wire Line
	5900 5040 6000 5040
Connection ~ 6000 5040
Wire Wire Line
	6000 5040 6000 5340
$EndSCHEMATC
