EESchema Schematic File Version 4
LIBS:EV2400-EVM-Interface-Board-cache
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 3 3
Title "EV2400 EVM Interface Board by Texas Instruments"
Date "2019-09-26"
Rev "1.0"
Comp "ORPALTECH"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L EV2400-Components:TPS73601DBV U?
U 1 1 5ED62174
P 5250 2600
AR Path="/5ED62174" Ref="U?"  Part="1" 
AR Path="/5D8CE23E/5ED62174" Ref="U10"  Part="1" 
F 0 "U10" H 5120 2870 50  0000 C CNN
F 1 "TPS73601DBV" H 5250 2320 50  0000 C CNN
F 2 "EV2400-Footprints:SOT95P280X145-5N" H 4900 1800 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps736.pdf" H 4900 1800 50  0001 L CNN
F 4 "LDO Voltage Regulators Cap-Free NMOS 400mA" H 4900 1700 50  0001 L CNN "Description"
F 5 "1.45" H 4900 1900 50  0001 L CNN "Height"
F 6 "Texas Instruments" H 4900 1600 50  0001 L CNN "Manufacturer_Name"
F 7 "TPS73601DBV" H 4900 1500 50  0001 L CNN "Manufacturer_Part_Number"
	1    5250 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 3400 6700 3400
Wire Wire Line
	6700 3400 6700 2500
Wire Wire Line
	5600 2700 5700 2700
Wire Wire Line
	5700 2700 5700 2900
$Comp
L EV2400-Components:R_Small R29
U 1 1 5ED753EE
P 5700 3100
F 0 "R29" H 5759 3146 50  0000 L CNN
F 1 "10K" H 5759 3055 50  0000 L CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 5700 3100 50  0001 C CNN
F 3 "~" H 5700 3100 50  0001 C CNN
	1    5700 3100
	1    0    0    -1  
$EndComp
$Comp
L EV2400-Components:GND #PWR0151
U 1 1 5ED7727F
P 5700 3330
F 0 "#PWR0151" H 5700 3080 50  0001 C CNN
F 1 "GND" H 5700 3205 50  0001 C CNN
F 2 "" H 5700 3330 50  0001 C CNN
F 3 "" H 5700 3330 50  0001 C CNN
	1    5700 3330
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 3330 5700 3300
$Comp
L EV2400-Components:C_Small C19
U 1 1 5ED78FD1
P 6200 2720
F 0 "C19" H 5960 2780 50  0000 L CNN
F 1 "0.1u" H 5960 2670 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 6200 2720 50  0001 C CNN
F 3 "~" H 6200 2720 50  0001 C CNN
	1    6200 2720
	1    0    0    -1  
$EndComp
$Comp
L EV2400-Components:C_Small C20
U 1 1 5ED7A6DD
P 6400 2720
F 0 "C20" H 6492 2766 50  0000 L CNN
F 1 "0.1u" H 6492 2675 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 6400 2720 50  0001 C CNN
F 3 "~" H 6400 2720 50  0001 C CNN
	1    6400 2720
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 2620 6400 2500
Wire Wire Line
	5600 2500 6200 2500
Connection ~ 6400 2500
Wire Wire Line
	6400 2500 6700 2500
Wire Wire Line
	5700 3300 6400 3300
Wire Wire Line
	6400 3300 6400 2820
Connection ~ 5700 3300
Wire Wire Line
	5700 3300 5700 3200
Wire Wire Line
	6200 3000 6200 2900
Wire Wire Line
	6200 2620 6200 2500
Connection ~ 6200 2500
Wire Wire Line
	6200 2500 6400 2500
Wire Wire Line
	5700 2900 6200 2900
Connection ~ 5700 2900
Wire Wire Line
	5700 2900 5700 3000
Connection ~ 6200 2900
Wire Wire Line
	6200 2900 6200 2820
Wire Wire Line
	4900 2500 4500 2500
Text GLabel 4200 2500 0    35   UnSpc ~ 0
5V-MUX
Wire Wire Line
	4900 2600 4700 2600
Wire Wire Line
	4700 2600 4700 2900
Wire Wire Line
	4700 2900 4600 2900
$Comp
L EV2400-Components:C_Small C18
U 1 1 5ED97319
P 4500 2720
F 0 "C18" H 4260 2780 50  0000 L CNN
F 1 "0.1u" H 4260 2670 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 4500 2720 50  0001 C CNN
F 3 "~" H 4500 2720 50  0001 C CNN
	1    4500 2720
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 2900 4500 2820
Wire Wire Line
	4500 2620 4500 2500
Connection ~ 4500 2500
Wire Wire Line
	4500 2500 4200 2500
$Comp
L EV2400-Components:GND #PWR0152
U 1 1 5ED9A50A
P 4600 3130
F 0 "#PWR0152" H 4600 2880 50  0001 C CNN
F 1 "GND" H 4600 3005 50  0001 C CNN
F 2 "" H 4600 3130 50  0001 C CNN
F 3 "" H 4600 3130 50  0001 C CNN
	1    4600 3130
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 3130 4600 2900
Connection ~ 4600 2900
Wire Wire Line
	4600 2900 4500 2900
Wire Wire Line
	4900 2700 4800 2700
Wire Wire Line
	4800 2700 4800 3000
Wire Wire Line
	4800 3000 4200 3000
Text GLabel 4200 3000 0    35   UnSpc ~ 0
VVOD.1EN
Text GLabel 6300 2300 0    35   UnSpc ~ 0
VVOD.1
Wire Wire Line
	9000 2700 8900 2700
Wire Wire Line
	8900 2700 8900 2900
$Comp
L EV2400-Components:R_Small R30
U 1 1 5EDAEB3D
P 8900 3100
F 0 "R30" H 9110 3150 50  0000 R CNN
F 1 "10K" H 9110 3050 50  0000 R CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 8900 3100 50  0001 C CNN
F 3 "~" H 8900 3100 50  0001 C CNN
	1    8900 3100
	-1   0    0    -1  
$EndComp
$Comp
L EV2400-Components:GND #PWR0153
U 1 1 5EDAEB47
P 8900 3330
F 0 "#PWR0153" H 8900 3080 50  0001 C CNN
F 1 "GND" H 8900 3205 50  0001 C CNN
F 2 "" H 8900 3330 50  0001 C CNN
F 3 "" H 8900 3330 50  0001 C CNN
	1    8900 3330
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8900 3330 8900 3300
$Comp
L EV2400-Components:C_Small C22
U 1 1 5EDAEB52
P 8400 2720
F 0 "C22" H 8160 2780 50  0000 L CNN
F 1 "0.1u" H 8160 2670 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 8400 2720 50  0001 C CNN
F 3 "~" H 8400 2720 50  0001 C CNN
	1    8400 2720
	-1   0    0    -1  
$EndComp
$Comp
L EV2400-Components:C_Small C23
U 1 1 5EDAEB5C
P 8200 2720
F 0 "C23" H 8420 2790 50  0000 R CNN
F 1 "0.1u" H 8440 2670 50  0000 R CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 8200 2720 50  0001 C CNN
F 3 "~" H 8200 2720 50  0001 C CNN
	1    8200 2720
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8200 2620 8200 2500
Wire Wire Line
	9000 2500 8400 2500
Connection ~ 8200 2500
Wire Wire Line
	8200 2500 7900 2500
Wire Wire Line
	8900 3300 8200 3300
Wire Wire Line
	8200 3300 8200 2820
Connection ~ 8900 3300
Wire Wire Line
	8900 3300 8900 3200
Wire Wire Line
	8000 3000 8400 3000
Wire Wire Line
	8400 3000 8400 2900
Wire Wire Line
	8400 2620 8400 2500
Connection ~ 8400 2500
Wire Wire Line
	8400 2500 8200 2500
Wire Wire Line
	8900 2900 8400 2900
Connection ~ 8900 2900
Wire Wire Line
	8900 2900 8900 3000
Connection ~ 8400 2900
Wire Wire Line
	8400 2900 8400 2820
Wire Wire Line
	9700 2500 10100 2500
Text GLabel 10400 2500 2    35   UnSpc ~ 0
5V-MUX
Wire Wire Line
	9700 2600 9900 2600
Wire Wire Line
	9900 2600 9900 2900
Wire Wire Line
	9900 2900 10000 2900
$Comp
L EV2400-Components:C_Small C21
U 1 1 5EDAEB7F
P 10100 2720
F 0 "C21" H 9860 2780 50  0000 L CNN
F 1 "0.1u" H 9840 2670 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 10100 2720 50  0001 C CNN
F 3 "~" H 10100 2720 50  0001 C CNN
	1    10100 2720
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10100 2900 10100 2820
Wire Wire Line
	10100 2620 10100 2500
Connection ~ 10100 2500
Wire Wire Line
	10100 2500 10400 2500
$Comp
L EV2400-Components:GND #PWR0154
U 1 1 5EDAEB8D
P 10000 3130
F 0 "#PWR0154" H 10000 2880 50  0001 C CNN
F 1 "GND" H 10000 3005 50  0001 C CNN
F 2 "" H 10000 3130 50  0001 C CNN
F 3 "" H 10000 3130 50  0001 C CNN
	1    10000 3130
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10000 3130 10000 2900
Connection ~ 10000 2900
Wire Wire Line
	10000 2900 10100 2900
Wire Wire Line
	9700 2700 9800 2700
Wire Wire Line
	9800 2700 9800 3000
Wire Wire Line
	9800 3000 10400 3000
Text GLabel 10400 3000 2    35   UnSpc ~ 0
VVOD.3EN
Text GLabel 8300 2300 2    35   UnSpc ~ 0
VVOD.3
$Comp
L EV2400-Components:TPS73601DBV U?
U 1 1 5EDAEB2F
P 9350 2600
AR Path="/5EDAEB2F" Ref="U?"  Part="1" 
AR Path="/5D8CE23E/5EDAEB2F" Ref="U11"  Part="1" 
F 0 "U11" H 9530 2880 50  0000 C CNN
F 1 "TPS73601DBV" H 9350 2320 50  0000 C CNN
F 2 "EV2400-Footprints:SOT95P280X145-5N" H 9000 1800 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps736.pdf" H 9000 1800 50  0001 L CNN
F 4 "LDO Voltage Regulators Cap-Free NMOS 400mA" H 9000 1700 50  0001 L CNN "Description"
F 5 "1.45" H 9000 1900 50  0001 L CNN "Height"
F 6 "Texas Instruments" H 9000 1600 50  0001 L CNN "Manufacturer_Name"
F 7 "TPS73601DBV" H 9000 1500 50  0001 L CNN "Manufacturer_Part_Number"
	1    9350 2600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6800 3500 4400 3500
Wire Wire Line
	6800 3600 4700 3600
Text GLabel 4200 3500 0    35   UnSpc ~ 0
SCLBRD
Text GLabel 4200 3600 0    35   UnSpc ~ 0
SDABRD
$Comp
L EV2400-Components:R_Small R31
U 1 1 5EDCCAF5
P 4400 3870
F 0 "R31" H 4450 3910 50  0000 L CNN
F 1 "10K" H 4460 3820 50  0000 L CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 4400 3870 50  0001 C CNN
F 3 "~" H 4400 3870 50  0001 C CNN
	1    4400 3870
	1    0    0    -1  
$EndComp
$Comp
L EV2400-Components:R_Small R32
U 1 1 5EDCD9FE
P 4700 3870
F 0 "R32" H 4759 3916 50  0000 L CNN
F 1 "10K" H 4759 3825 50  0000 L CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 4700 3870 50  0001 C CNN
F 3 "~" H 4700 3870 50  0001 C CNN
	1    4700 3870
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 3770 4700 3600
Connection ~ 4700 3600
Wire Wire Line
	4700 3600 4200 3600
Wire Wire Line
	4400 3770 4400 3500
Connection ~ 4400 3500
Wire Wire Line
	4400 3500 4200 3500
Wire Wire Line
	4700 3970 4700 4100
Wire Wire Line
	4700 4100 4400 4100
Wire Wire Line
	4400 3970 4400 4100
Connection ~ 4400 4100
Wire Wire Line
	4400 4100 4200 4100
$Comp
L EV2400-Components:+3V3 #PWR0155
U 1 1 5EDD9DBE
P 4200 4100
F 0 "#PWR0155" H 4200 3950 50  0001 C CNN
F 1 "+3V3" V 4200 4228 50  0000 L CNN
F 2 "" H 4200 4100 50  0001 C CNN
F 3 "" H 4200 4100 50  0001 C CNN
	1    4200 4100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6800 3700 6400 3700
Wire Wire Line
	6400 3700 6400 3730
$Comp
L EV2400-Components:GND #PWR0156
U 1 1 5EDEC854
P 6400 3730
F 0 "#PWR0156" H 6400 3480 50  0001 C CNN
F 1 "GND" H 6400 3605 50  0001 C CNN
F 2 "" H 6400 3730 50  0001 C CNN
F 3 "" H 6400 3730 50  0001 C CNN
	1    6400 3730
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 4900 5700 4900
Wire Wire Line
	5700 4900 5700 5100
$Comp
L EV2400-Components:R_Small R33
U 1 1 5EDF4E38
P 5700 5300
F 0 "R33" H 5759 5346 50  0000 L CNN
F 1 "10K" H 5759 5255 50  0000 L CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 5700 5300 50  0001 C CNN
F 3 "~" H 5700 5300 50  0001 C CNN
	1    5700 5300
	1    0    0    -1  
$EndComp
$Comp
L EV2400-Components:GND #PWR0157
U 1 1 5EDF4E42
P 5700 5530
F 0 "#PWR0157" H 5700 5280 50  0001 C CNN
F 1 "GND" H 5700 5405 50  0001 C CNN
F 2 "" H 5700 5530 50  0001 C CNN
F 3 "" H 5700 5530 50  0001 C CNN
	1    5700 5530
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 5530 5700 5500
$Comp
L EV2400-Components:C_Small C25
U 1 1 5EDF4E4D
P 6200 4920
F 0 "C25" H 5960 4980 50  0000 L CNN
F 1 "0.1u" H 5960 4870 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 6200 4920 50  0001 C CNN
F 3 "~" H 6200 4920 50  0001 C CNN
	1    6200 4920
	1    0    0    -1  
$EndComp
$Comp
L EV2400-Components:C_Small C26
U 1 1 5EDF4E57
P 6400 4920
F 0 "C26" H 6492 4966 50  0000 L CNN
F 1 "0.1u" H 6492 4875 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 6400 4920 50  0001 C CNN
F 3 "~" H 6400 4920 50  0001 C CNN
	1    6400 4920
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 4820 6400 4700
Wire Wire Line
	5600 4700 6200 4700
Wire Wire Line
	5700 5500 6400 5500
Wire Wire Line
	6400 5500 6400 5020
Connection ~ 5700 5500
Wire Wire Line
	5700 5500 5700 5400
Wire Wire Line
	6200 4820 6200 4700
Connection ~ 6200 4700
Wire Wire Line
	6200 4700 6400 4700
Wire Wire Line
	5700 5100 6200 5100
Connection ~ 5700 5100
Wire Wire Line
	5700 5100 5700 5200
Connection ~ 6200 5100
Wire Wire Line
	6200 5100 6200 5020
Wire Wire Line
	4900 4700 4500 4700
Text GLabel 4200 4700 0    35   UnSpc ~ 0
5V-MUX
Wire Wire Line
	4900 4800 4700 4800
Wire Wire Line
	4700 4800 4700 5100
Wire Wire Line
	4700 5100 4600 5100
$Comp
L EV2400-Components:C_Small C24
U 1 1 5EDF4E78
P 4500 4920
F 0 "C24" H 4260 4980 50  0000 L CNN
F 1 "0.1u" H 4260 4870 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 4500 4920 50  0001 C CNN
F 3 "~" H 4500 4920 50  0001 C CNN
	1    4500 4920
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 5100 4500 5020
Wire Wire Line
	4500 4820 4500 4700
Connection ~ 4500 4700
Wire Wire Line
	4500 4700 4200 4700
$Comp
L EV2400-Components:GND #PWR0158
U 1 1 5EDF4E86
P 4600 5330
F 0 "#PWR0158" H 4600 5080 50  0001 C CNN
F 1 "GND" H 4600 5205 50  0001 C CNN
F 2 "" H 4600 5330 50  0001 C CNN
F 3 "" H 4600 5330 50  0001 C CNN
	1    4600 5330
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 5330 4600 5100
Connection ~ 4600 5100
Wire Wire Line
	4600 5100 4500 5100
Wire Wire Line
	4900 4900 4800 4900
Wire Wire Line
	4800 4900 4800 5200
Wire Wire Line
	4800 5200 4200 5200
Text GLabel 4200 5200 0    35   UnSpc ~ 0
VVOD.2EN
Wire Wire Line
	6200 5100 6200 5200
Wire Wire Line
	6800 3800 6600 3800
Wire Wire Line
	6600 3800 6600 4700
Wire Wire Line
	6600 4700 6400 4700
Connection ~ 6400 4700
Wire Wire Line
	6600 3000 6200 3000
Wire Wire Line
	6600 3300 6600 3000
Wire Wire Line
	6800 3300 6600 3300
Wire Wire Line
	6700 3900 6800 3900
$Comp
L EV2400-Components:TPS73601DBV U?
U 1 1 5EDF4E2C
P 5250 4800
AR Path="/5EDF4E2C" Ref="U?"  Part="1" 
AR Path="/5D8CE23E/5EDF4E2C" Ref="U12"  Part="1" 
F 0 "U12" H 5120 5070 50  0000 C CNN
F 1 "TPS73601DBV" H 5250 4520 50  0000 C CNN
F 2 "EV2400-Footprints:SOT95P280X145-5N" H 4900 4000 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps736.pdf" H 4900 4000 50  0001 L CNN
F 4 "LDO Voltage Regulators Cap-Free NMOS 400mA" H 4900 3900 50  0001 L CNN "Description"
F 5 "1.45" H 4900 4100 50  0001 L CNN "Height"
F 6 "Texas Instruments" H 4900 3800 50  0001 L CNN "Manufacturer_Name"
F 7 "TPS73601DBV" H 4900 3700 50  0001 L CNN "Manufacturer_Part_Number"
	1    5250 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 4700 6400 4500
Wire Wire Line
	6400 4500 6300 4500
Text GLabel 6300 4500 0    35   UnSpc ~ 0
VVOD.2
Wire Wire Line
	6400 2500 6400 2300
Wire Wire Line
	6400 2300 6300 2300
Wire Wire Line
	8200 2500 8200 2300
Wire Wire Line
	8200 2300 8300 2300
Wire Wire Line
	6200 5200 6700 5200
Wire Wire Line
	6700 5200 6700 3900
Wire Wire Line
	9000 4900 8900 4900
Wire Wire Line
	8900 4900 8900 5100
$Comp
L EV2400-Components:R_Small R34
U 1 1 5EE838E2
P 8900 5300
F 0 "R34" H 9120 5350 50  0000 R CNN
F 1 "10K" H 9120 5250 50  0000 R CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 8900 5300 50  0001 C CNN
F 3 "~" H 8900 5300 50  0001 C CNN
	1    8900 5300
	-1   0    0    -1  
$EndComp
$Comp
L EV2400-Components:GND #PWR0159
U 1 1 5EE838EC
P 8900 5530
F 0 "#PWR0159" H 8900 5280 50  0001 C CNN
F 1 "GND" H 8900 5405 50  0001 C CNN
F 2 "" H 8900 5530 50  0001 C CNN
F 3 "" H 8900 5530 50  0001 C CNN
	1    8900 5530
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8900 5530 8900 5500
$Comp
L EV2400-Components:C_Small C28
U 1 1 5EE838F7
P 8400 4920
F 0 "C28" H 8160 4980 50  0000 L CNN
F 1 "0.1u" H 8150 4860 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 8400 4920 50  0001 C CNN
F 3 "~" H 8400 4920 50  0001 C CNN
	1    8400 4920
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8200 4820 8200 4700
Wire Wire Line
	9000 4700 8400 4700
Wire Wire Line
	8900 5500 8200 5500
Wire Wire Line
	8200 5500 8200 5020
Connection ~ 8900 5500
Wire Wire Line
	8900 5500 8900 5400
Wire Wire Line
	8400 4820 8400 4700
Connection ~ 8400 4700
Wire Wire Line
	8400 4700 8200 4700
Wire Wire Line
	8900 5100 8400 5100
Connection ~ 8900 5100
Wire Wire Line
	8900 5100 8900 5200
Connection ~ 8400 5100
Wire Wire Line
	8400 5100 8400 5020
Wire Wire Line
	9700 4700 10100 4700
Text GLabel 10400 4700 2    35   UnSpc ~ 0
5V-MUX
Wire Wire Line
	9700 4800 9900 4800
Wire Wire Line
	9900 4800 9900 5100
Wire Wire Line
	9900 5100 10000 5100
$Comp
L EV2400-Components:C_Small C27
U 1 1 5EE8391E
P 10100 4920
F 0 "C27" H 9860 4980 50  0000 L CNN
F 1 "0.1u" H 9830 4870 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 10100 4920 50  0001 C CNN
F 3 "~" H 10100 4920 50  0001 C CNN
	1    10100 4920
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10100 5100 10100 5020
Wire Wire Line
	10100 4820 10100 4700
Connection ~ 10100 4700
Wire Wire Line
	10100 4700 10400 4700
$Comp
L EV2400-Components:GND #PWR0160
U 1 1 5EE8392C
P 10000 5330
F 0 "#PWR0160" H 10000 5080 50  0001 C CNN
F 1 "GND" H 10000 5205 50  0001 C CNN
F 2 "" H 10000 5330 50  0001 C CNN
F 3 "" H 10000 5330 50  0001 C CNN
	1    10000 5330
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10000 5330 10000 5100
Connection ~ 10000 5100
Wire Wire Line
	10000 5100 10100 5100
Wire Wire Line
	9700 4900 9800 4900
Wire Wire Line
	9800 4900 9800 5200
Wire Wire Line
	9800 5200 10400 5200
Text GLabel 10400 5200 2    35   UnSpc ~ 0
VVOD.4EN
Wire Wire Line
	8400 5100 8400 5200
Connection ~ 8200 4700
$Comp
L EV2400-Components:TPS73601DBV U?
U 1 1 5EE83944
P 9350 4800
AR Path="/5EE83944" Ref="U?"  Part="1" 
AR Path="/5D8CE23E/5EE83944" Ref="U13"  Part="1" 
F 0 "U13" H 9520 5080 50  0000 C CNN
F 1 "TPS73601DBV" H 9350 4520 50  0000 C CNN
F 2 "EV2400-Footprints:SOT95P280X145-5N" H 9000 4000 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps736.pdf" H 9000 4000 50  0001 L CNN
F 4 "LDO Voltage Regulators Cap-Free NMOS 400mA" H 9000 3900 50  0001 L CNN "Description"
F 5 "1.45" H 9000 4100 50  0001 L CNN "Height"
F 6 "Texas Instruments" H 9000 3800 50  0001 L CNN "Manufacturer_Name"
F 7 "TPS73601DBV" H 9000 3700 50  0001 L CNN "Manufacturer_Part_Number"
	1    9350 4800
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8200 4700 8200 4500
Wire Wire Line
	8200 4500 8300 4500
Text GLabel 8300 4500 2    35   UnSpc ~ 0
VVOD.4
Wire Wire Line
	7800 3400 8000 3400
Wire Wire Line
	8000 3000 8000 3400
Wire Wire Line
	7800 3300 7900 3300
Wire Wire Line
	7900 3300 7900 2500
$Comp
L EV2400-Components:C_Small C29
U 1 1 5EE83901
P 8200 4920
F 0 "C29" H 8420 4990 50  0000 R CNN
F 1 "0.1u" H 8430 4850 50  0000 R CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 8200 4920 50  0001 C CNN
F 3 "~" H 8200 4920 50  0001 C CNN
	1    8200 4920
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7800 3800 7900 3800
Wire Wire Line
	7900 3800 7900 5200
Wire Wire Line
	7900 5200 8400 5200
Wire Wire Line
	7800 3900 8000 3900
Wire Wire Line
	8000 3900 8000 4700
Wire Wire Line
	8000 4700 8200 4700
Wire Wire Line
	7800 3500 10400 3500
Text GLabel 10400 3500 2    35   UnSpc ~ 0
5V-MUX
Wire Wire Line
	7800 3600 8200 3600
Wire Wire Line
	8200 3600 8200 3700
$Comp
L EV2400-Components:GND #PWR0161
U 1 1 5EFA2C3B
P 8200 4030
F 0 "#PWR0161" H 8200 3780 50  0001 C CNN
F 1 "GND" H 8200 3905 50  0001 C CNN
F 2 "" H 8200 4030 50  0001 C CNN
F 3 "" H 8200 4030 50  0001 C CNN
	1    8200 4030
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 3700 8200 3700
Connection ~ 8200 3700
Wire Wire Line
	8200 3700 8200 4030
Wire Wire Line
	9000 6700 8900 6700
Wire Wire Line
	8900 6700 8900 6900
$Comp
L EV2400-Components:R_Small R37
U 1 1 5EFECC2D
P 8900 7100
F 0 "R37" H 9110 7150 50  0000 R CNN
F 1 "10K" H 9110 7050 50  0000 R CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 8900 7100 50  0001 C CNN
F 3 "~" H 8900 7100 50  0001 C CNN
	1    8900 7100
	-1   0    0    -1  
$EndComp
$Comp
L EV2400-Components:GND #PWR0162
U 1 1 5EFECC37
P 8900 7330
F 0 "#PWR0162" H 8900 7080 50  0001 C CNN
F 1 "GND" H 8900 7205 50  0001 C CNN
F 2 "" H 8900 7330 50  0001 C CNN
F 3 "" H 8900 7330 50  0001 C CNN
	1    8900 7330
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8900 7330 8900 7300
$Comp
L EV2400-Components:C_Small C34
U 1 1 5EFECC42
P 8400 6720
F 0 "C34" H 8160 6780 50  0000 L CNN
F 1 "0.1u" H 8160 6670 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 8400 6720 50  0001 C CNN
F 3 "~" H 8400 6720 50  0001 C CNN
	1    8400 6720
	-1   0    0    -1  
$EndComp
$Comp
L EV2400-Components:C_Small C33
U 1 1 5EFECC4C
P 8200 6720
F 0 "C33" H 8420 6790 50  0000 R CNN
F 1 "0.1u" H 8440 6670 50  0000 R CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 8200 6720 50  0001 C CNN
F 3 "~" H 8200 6720 50  0001 C CNN
	1    8200 6720
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8200 6620 8200 6500
Wire Wire Line
	9000 6500 8400 6500
Connection ~ 8200 6500
Wire Wire Line
	8200 6500 7900 6500
Wire Wire Line
	8900 7300 8200 7300
Wire Wire Line
	8200 7300 8200 6820
Connection ~ 8900 7300
Wire Wire Line
	8900 7300 8900 7200
Wire Wire Line
	8000 7000 8400 7000
Wire Wire Line
	8400 7000 8400 6900
Wire Wire Line
	8400 6620 8400 6500
Connection ~ 8400 6500
Wire Wire Line
	8400 6500 8200 6500
Wire Wire Line
	8900 6900 8400 6900
Connection ~ 8900 6900
Wire Wire Line
	8900 6900 8900 7000
Connection ~ 8400 6900
Wire Wire Line
	8400 6900 8400 6820
Wire Wire Line
	9700 6500 9800 6500
Text GLabel 10400 6500 2    35   UnSpc ~ 0
5V-MUX
Wire Wire Line
	9700 6600 9900 6600
Wire Wire Line
	9900 6600 9900 6900
Wire Wire Line
	9900 6900 10000 6900
$Comp
L EV2400-Components:C_Small C35
U 1 1 5EFECC6D
P 10100 6720
F 0 "C35" H 9860 6780 50  0000 L CNN
F 1 "0.1u" H 9840 6670 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 10100 6720 50  0001 C CNN
F 3 "~" H 10100 6720 50  0001 C CNN
	1    10100 6720
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10100 6900 10100 6820
Wire Wire Line
	10100 6620 10100 6500
Connection ~ 10100 6500
Wire Wire Line
	10100 6500 10400 6500
$Comp
L EV2400-Components:GND #PWR0163
U 1 1 5EFECC7B
P 10000 7130
F 0 "#PWR0163" H 10000 6880 50  0001 C CNN
F 1 "GND" H 10000 7005 50  0001 C CNN
F 2 "" H 10000 7130 50  0001 C CNN
F 3 "" H 10000 7130 50  0001 C CNN
	1    10000 7130
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10000 7130 10000 6900
Connection ~ 10000 6900
Wire Wire Line
	10000 6900 10100 6900
Wire Wire Line
	9700 6700 9800 6700
Text GLabel 8300 6300 2    35   UnSpc ~ 0
VPUV.1
$Comp
L EV2400-Components:TPS73601DBV U?
U 1 1 5EFECC91
P 9350 6600
AR Path="/5EFECC91" Ref="U?"  Part="1" 
AR Path="/5D8CE23E/5EFECC91" Ref="U15"  Part="1" 
F 0 "U15" H 9530 6880 50  0000 C CNN
F 1 "TPS73601DBV" H 9350 6320 50  0000 C CNN
F 2 "EV2400-Footprints:SOT95P280X145-5N" H 9000 5800 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps736.pdf" H 9000 5800 50  0001 L CNN
F 4 "LDO Voltage Regulators Cap-Free NMOS 400mA" H 9000 5700 50  0001 L CNN "Description"
F 5 "1.45" H 9000 5900 50  0001 L CNN "Height"
F 6 "Texas Instruments" H 9000 5600 50  0001 L CNN "Manufacturer_Name"
F 7 "TPS73601DBV" H 9000 5500 50  0001 L CNN "Manufacturer_Part_Number"
	1    9350 6600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8200 6500 8200 6300
Wire Wire Line
	8200 6300 8300 6300
Wire Wire Line
	7800 7300 7900 7300
Wire Wire Line
	7900 7300 7900 6500
Wire Wire Line
	7800 7400 8000 7400
Wire Wire Line
	8000 7400 8000 7000
Wire Wire Line
	9800 6700 9800 6500
Connection ~ 9800 6500
Wire Wire Line
	9800 6500 10100 6500
Wire Wire Line
	7800 7500 8200 7500
Text GLabel 10400 7500 2    35   UnSpc ~ 0
5V-MUX
Wire Wire Line
	7800 7600 8200 7600
Wire Wire Line
	8200 7500 8200 7600
Connection ~ 8200 7500
Wire Wire Line
	8200 7500 10400 7500
$Comp
L EV2400-Components:GND #PWR0164
U 1 1 5F0939A4
P 8200 8030
F 0 "#PWR0164" H 8200 7780 50  0001 C CNN
F 1 "GND" H 8200 7905 50  0001 C CNN
F 2 "" H 8200 8030 50  0001 C CNN
F 3 "" H 8200 8030 50  0001 C CNN
	1    8200 8030
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 7700 8200 7700
Wire Wire Line
	8200 7700 8200 8030
Wire Wire Line
	6800 7500 4400 7500
Wire Wire Line
	6800 7600 4700 7600
Text GLabel 4200 7500 0    35   UnSpc ~ 0
SCLBRD
Text GLabel 4200 7600 0    35   UnSpc ~ 0
SDABRD
$Comp
L EV2400-Components:R_Small R35
U 1 1 5F0CD277
P 4400 7870
F 0 "R35" H 4450 7910 50  0000 L CNN
F 1 "10K" H 4460 7820 50  0000 L CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 4400 7870 50  0001 C CNN
F 3 "~" H 4400 7870 50  0001 C CNN
	1    4400 7870
	1    0    0    -1  
$EndComp
$Comp
L EV2400-Components:R_Small R36
U 1 1 5F0CD281
P 4700 7870
F 0 "R36" H 4759 7916 50  0000 L CNN
F 1 "10K" H 4759 7825 50  0000 L CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 4700 7870 50  0001 C CNN
F 3 "~" H 4700 7870 50  0001 C CNN
	1    4700 7870
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 7770 4700 7600
Connection ~ 4700 7600
Wire Wire Line
	4700 7600 4200 7600
Wire Wire Line
	4400 7770 4400 7500
Connection ~ 4400 7500
Wire Wire Line
	4400 7500 4200 7500
Wire Wire Line
	4700 7970 4700 8100
Wire Wire Line
	4700 8100 4400 8100
Wire Wire Line
	4400 7970 4400 8100
Connection ~ 4400 8100
Wire Wire Line
	4400 8100 4200 8100
$Comp
L EV2400-Components:+3V3 #PWR0165
U 1 1 5F0CD296
P 4200 8100
F 0 "#PWR0165" H 4200 7950 50  0001 C CNN
F 1 "+3V3" V 4200 8228 50  0000 L CNN
F 2 "" H 4200 8100 50  0001 C CNN
F 3 "" H 4200 8100 50  0001 C CNN
	1    4200 8100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6800 7700 6400 7700
Wire Wire Line
	6400 7700 6400 7730
$Comp
L EV2400-Components:GND #PWR0166
U 1 1 5F0E87A5
P 6400 7730
F 0 "#PWR0166" H 6400 7480 50  0001 C CNN
F 1 "GND" H 6400 7605 50  0001 C CNN
F 2 "" H 6400 7730 50  0001 C CNN
F 3 "" H 6400 7730 50  0001 C CNN
	1    6400 7730
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 8900 5700 8900
Wire Wire Line
	5700 8900 5700 9100
$Comp
L EV2400-Components:R_Small R38
U 1 1 5F0FCBB9
P 5700 9300
F 0 "R38" H 5759 9346 50  0000 L CNN
F 1 "10K" H 5759 9255 50  0000 L CNN
F 2 "EV2400-Footprints:R_0603_1608Metric" H 5700 9300 50  0001 C CNN
F 3 "~" H 5700 9300 50  0001 C CNN
	1    5700 9300
	1    0    0    -1  
$EndComp
$Comp
L EV2400-Components:GND #PWR0167
U 1 1 5F0FCBC3
P 5700 9530
F 0 "#PWR0167" H 5700 9280 50  0001 C CNN
F 1 "GND" H 5700 9405 50  0001 C CNN
F 2 "" H 5700 9530 50  0001 C CNN
F 3 "" H 5700 9530 50  0001 C CNN
	1    5700 9530
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 9530 5700 9500
$Comp
L EV2400-Components:C_Small C31
U 1 1 5F0FCBCE
P 6200 8920
F 0 "C31" H 5960 8980 50  0000 L CNN
F 1 "0.1u" H 5960 8870 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 6200 8920 50  0001 C CNN
F 3 "~" H 6200 8920 50  0001 C CNN
	1    6200 8920
	1    0    0    -1  
$EndComp
$Comp
L EV2400-Components:C_Small C32
U 1 1 5F0FCBD8
P 6400 8920
F 0 "C32" H 6492 8966 50  0000 L CNN
F 1 "0.1u" H 6492 8875 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 6400 8920 50  0001 C CNN
F 3 "~" H 6400 8920 50  0001 C CNN
	1    6400 8920
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 8820 6400 8700
Wire Wire Line
	5600 8700 6200 8700
Wire Wire Line
	5700 9500 6400 9500
Wire Wire Line
	6400 9500 6400 9020
Connection ~ 5700 9500
Wire Wire Line
	5700 9500 5700 9400
Wire Wire Line
	6200 8820 6200 8700
Connection ~ 6200 8700
Wire Wire Line
	6200 8700 6400 8700
Wire Wire Line
	5700 9100 6200 9100
Connection ~ 5700 9100
Wire Wire Line
	5700 9100 5700 9200
Connection ~ 6200 9100
Wire Wire Line
	6200 9100 6200 9020
Wire Wire Line
	4900 8700 4800 8700
Text GLabel 4200 8700 0    35   UnSpc ~ 0
5V-MUX
Wire Wire Line
	4900 8800 4700 8800
Wire Wire Line
	4700 8800 4700 9100
Wire Wire Line
	4700 9100 4600 9100
$Comp
L EV2400-Components:C_Small C30
U 1 1 5F0FCBF5
P 4500 8920
F 0 "C30" H 4260 8980 50  0000 L CNN
F 1 "0.1u" H 4260 8870 50  0000 L CNN
F 2 "EV2400-Footprints:C_0603_1608Metric" H 4500 8920 50  0001 C CNN
F 3 "~" H 4500 8920 50  0001 C CNN
	1    4500 8920
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 9100 4500 9020
Wire Wire Line
	4500 8820 4500 8700
Connection ~ 4500 8700
Wire Wire Line
	4500 8700 4200 8700
$Comp
L EV2400-Components:GND #PWR0168
U 1 1 5F0FCC03
P 4600 9330
F 0 "#PWR0168" H 4600 9080 50  0001 C CNN
F 1 "GND" H 4600 9205 50  0001 C CNN
F 2 "" H 4600 9330 50  0001 C CNN
F 3 "" H 4600 9330 50  0001 C CNN
	1    4600 9330
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 9330 4600 9100
Connection ~ 4600 9100
Wire Wire Line
	4600 9100 4500 9100
Wire Wire Line
	4900 8900 4800 8900
Wire Wire Line
	6200 9100 6200 9200
Wire Wire Line
	6600 7800 6600 8700
Wire Wire Line
	6600 8700 6400 8700
Connection ~ 6400 8700
$Comp
L EV2400-Components:TPS73601DBV U?
U 1 1 5F0FCC1C
P 5250 8800
AR Path="/5F0FCC1C" Ref="U?"  Part="1" 
AR Path="/5D8CE23E/5F0FCC1C" Ref="U18"  Part="1" 
F 0 "U18" H 5120 9070 50  0000 C CNN
F 1 "TPS73601DBV" H 5250 8520 50  0000 C CNN
F 2 "EV2400-Footprints:SOT95P280X145-5N" H 4900 8000 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps736.pdf" H 4900 8000 50  0001 L CNN
F 4 "LDO Voltage Regulators Cap-Free NMOS 400mA" H 4900 7900 50  0001 L CNN "Description"
F 5 "1.45" H 4900 8100 50  0001 L CNN "Height"
F 6 "Texas Instruments" H 4900 7800 50  0001 L CNN "Manufacturer_Name"
F 7 "TPS73601DBV" H 4900 7700 50  0001 L CNN "Manufacturer_Part_Number"
	1    5250 8800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 8700 6400 8500
Wire Wire Line
	6400 8500 6300 8500
Text GLabel 6300 8500 0    35   UnSpc ~ 0
VPUV.2
Wire Wire Line
	6200 9200 6700 9200
Wire Wire Line
	6700 9200 6700 7900
Wire Wire Line
	6600 7800 6800 7800
Wire Wire Line
	6700 7900 6800 7900
Wire Wire Line
	4800 8900 4800 8700
Connection ~ 4800 8700
Wire Wire Line
	4800 8700 4500 8700
$Comp
L EV2400-Components:ISL90842 U14
U 1 1 5F26BF49
P 7300 7600
F 0 "U14" H 7110 8090 50  0000 C CNN
F 1 "ISL90842UIV1427Z" H 7290 7110 50  0000 C CNN
F 2 "EV2400-Footprints:SOP65P640X105-14N" H 6900 6900 50  0001 L CNN
F 3 "http://www.mouser.com/datasheet/2/698/isl90842-1529542.pdf" H 7650 7900 50  0001 L CNN
F 4 "Digital Potentiometer ICs ISL90842UIV1427Z LW NOISE LW PWR I2C BUS" H 6900 6800 50  0001 L CNN "Description"
F 5 "1.05" H 6900 6600 50  0001 L CNN "Height"
F 6 "Renesas / Intersil" H 6900 6700 50  0001 L CNN "Manufacturer_Name"
F 7 "ISL90842UIV1427Z" H 6900 6500 50  0001 L CNN "Manufacturer_Part_Number"
	1    7300 7600
	1    0    0    -1  
$EndComp
$Comp
L EV2400-Components:ISL90842 U9
U 1 1 5F26E3FF
P 7300 3600
F 0 "U9" H 7120 4090 50  0000 C CNN
F 1 "ISL90842UIV1427Z" H 7280 3100 50  0000 C CNN
F 2 "EV2400-Footprints:SOP65P640X105-14N" H 6900 2900 50  0001 L CNN
F 3 "http://www.mouser.com/datasheet/2/698/isl90842-1529542.pdf" H 7650 3900 50  0001 L CNN
F 4 "Digital Potentiometer ICs ISL90842UIV1427Z LW NOISE LW PWR I2C BUS" H 6900 2800 50  0001 L CNN "Description"
F 5 "1.05" H 6900 2600 50  0001 L CNN "Height"
F 6 "Renesas / Intersil" H 6900 2700 50  0001 L CNN "Manufacturer_Name"
F 7 "ISL90842UIV1427Z" H 6900 2500 50  0001 L CNN "Manufacturer_Part_Number"
	1    7300 3600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
