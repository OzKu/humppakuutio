EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:wt32
LIBS:arduino_shieldsNCL
LIBS:arduino_shields
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Humppakuutio"
Date ""
Rev "0.1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L WT32 U2
U 1 1 583FF751
P 7000 2450
F 0 "U2" H 7000 4300 60  0000 C CNN
F 1 "WT32" H 7000 4200 60  0000 C CNN
F 2 "footprints:WT32" H 7000 2950 60  0001 C CNN
F 3 "" H 7000 2950 60  0000 C CNN
	1    7000 2450
	1    0    0    -1  
$EndComp
$Comp
L TL074 U1
U 2 1 583FF90D
P 9300 1250
F 0 "U1" H 9300 1450 50  0000 L CNN
F 1 "TL074" H 9300 1050 50  0000 L CNN
F 2 "Housings_DIP:DIP-14_W7.62mm" H 9250 1350 50  0001 C CNN
F 3 "" H 9350 1450 50  0000 C CNN
	2    9300 1250
	1    0    0    -1  
$EndComp
$Comp
L TL074 U1
U 1 1 583FF9A8
P 9300 2300
F 0 "U1" H 9300 2500 50  0000 L CNN
F 1 "TL074" H 9300 2100 50  0000 L CNN
F 2 "Housings_DIP:DIP-14_W7.62mm" H 9250 2400 50  0001 C CNN
F 3 "" H 9350 2500 50  0000 C CNN
	1    9300 2300
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 583FFD24
P 8300 1150
F 0 "R1" V 8380 1150 50  0000 C CNN
F 1 "10k 0.1%" V 8200 1150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8230 1150 50  0001 C CNN
F 3 "" H 8300 1150 50  0000 C CNN
	1    8300 1150
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 583FFE6B
P 8300 1400
F 0 "R2" V 8380 1400 50  0000 C CNN
F 1 "10k 0.1%" V 8200 1400 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8230 1400 50  0001 C CNN
F 3 "" H 8300 1400 50  0000 C CNN
	1    8300 1400
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 583FFEB9
P 8550 2200
F 0 "R3" V 8630 2200 50  0000 C CNN
F 1 "10k 0.1%" V 8450 2200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8480 2200 50  0001 C CNN
F 3 "" H 8550 2200 50  0000 C CNN
	1    8550 2200
	0    1    1    0   
$EndComp
$Comp
L R R4
U 1 1 5840005B
P 8550 2500
F 0 "R4" V 8630 2500 50  0000 C CNN
F 1 "10k 0.1%" V 8450 2500 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8480 2500 50  0001 C CNN
F 3 "" H 8550 2500 50  0000 C CNN
	1    8550 2500
	0    1    1    0   
$EndComp
$Comp
L R R9
U 1 1 5840008D
P 9150 2900
F 0 "R9" V 9230 2900 50  0000 C CNN
F 1 "10k 0.1%" V 9050 2900 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 9080 2900 50  0001 C CNN
F 3 "" H 9150 2900 50  0000 C CNN
	1    9150 2900
	0    1    1    0   
$EndComp
$Comp
L R R10
U 1 1 584000CE
P 9200 1750
F 0 "R10" V 9280 1750 50  0000 C CNN
F 1 "10k 0.1%" V 9100 1750 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 9130 1750 50  0001 C CNN
F 3 "" H 9200 1750 50  0000 C CNN
	1    9200 1750
	0    1    1    0   
$EndComp
$Comp
L C C5
U 1 1 58400100
P 10050 2300
F 0 "C5" H 10075 2400 50  0000 L CNN
F 1 "470u" H 10075 2200 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D12.5_L25_P5" H 10088 2150 50  0001 C CNN
F 3 "" H 10050 2300 50  0000 C CNN
	1    10050 2300
	0    1    1    0   
$EndComp
$Comp
L C C4
U 1 1 5840019C
P 10000 1250
F 0 "C4" H 10025 1350 50  0000 L CNN
F 1 "470u" H 10025 1150 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D12.5_L25_P5" H 10038 1100 50  0001 C CNN
F 3 "" H 10000 1250 50  0000 C CNN
	1    10000 1250
	0    1    1    0   
$EndComp
$Comp
L R R7
U 1 1 584001FA
P 8750 900
F 0 "R7" V 8830 900 50  0000 C CNN
F 1 "10k 0.1%" V 8650 900 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8680 900 50  0001 C CNN
F 3 "" H 8750 900 50  0000 C CNN
	1    8750 900 
	1    0    0    1   
$EndComp
$Comp
L R R8
U 1 1 5840028D
P 8900 2100
F 0 "R8" V 8980 2100 50  0000 C CNN
F 1 "10k 0.1%" V 8800 2100 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8830 2100 50  0001 C CNN
F 3 "" H 8900 2100 50  0000 C CNN
	1    8900 2100
	0    -1   1    0   
$EndComp
$Comp
L VCOM #PWR01
U 1 1 58400913
P 9050 2050
F 0 "#PWR01" H 9050 1900 50  0001 C CNN
F 1 "VCOM" H 9050 2200 50  0000 C CNN
F 2 "" H 9050 2050 50  0000 C CNN
F 3 "" H 9050 2050 50  0000 C CNN
	1    9050 2050
	1    0    0    -1  
$EndComp
$Comp
L VCOM #PWR02
U 1 1 58400A37
P 8750 700
F 0 "#PWR02" H 8750 550 50  0001 C CNN
F 1 "VCOM" H 8750 850 50  0000 C CNN
F 2 "" H 8750 700 50  0000 C CNN
F 3 "" H 8750 700 50  0000 C CNN
	1    8750 700 
	1    0    0    -1  
$EndComp
$Comp
L GNDD #PWR03
U 1 1 5840321B
P 6800 4500
F 0 "#PWR03" H 6800 4250 50  0001 C CNN
F 1 "GNDD" H 6800 4350 50  0000 C CNN
F 2 "" H 6800 4500 50  0000 C CNN
F 3 "" H 6800 4500 50  0000 C CNN
	1    6800 4500
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR04
U 1 1 5840339B
P 7300 4500
F 0 "#PWR04" H 7300 4250 50  0001 C CNN
F 1 "GNDA" H 7300 4350 50  0000 C CNN
F 2 "" H 7300 4500 50  0000 C CNN
F 3 "" H 7300 4500 50  0000 C CNN
	1    7300 4500
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR05
U 1 1 5840342D
P 10050 1650
F 0 "#PWR05" H 10050 1400 50  0001 C CNN
F 1 "GNDA" H 10050 1500 50  0000 C CNN
F 2 "" H 10050 1650 50  0000 C CNN
F 3 "" H 10050 1650 50  0000 C CNN
	1    10050 1650
	0    1    1    0   
$EndComp
$Comp
L C C1
U 1 1 58403647
P 8000 2500
F 0 "C1" H 8025 2600 50  0000 L CNN
F 1 "470u" H 8025 2400 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D12.5_L25_P5" H 8038 2350 50  0001 C CNN
F 3 "" H 8000 2500 50  0000 C CNN
	1    8000 2500
	1    0    0    -1  
$EndComp
$Comp
L GNDD #PWR06
U 1 1 58403690
P 8000 2750
F 0 "#PWR06" H 8000 2500 50  0001 C CNN
F 1 "GNDD" H 8000 2600 50  0000 C CNN
F 2 "" H 8000 2750 50  0000 C CNN
F 3 "" H 8000 2750 50  0000 C CNN
	1    8000 2750
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR07
U 1 1 58403C98
P 5950 850
F 0 "#PWR07" H 5950 700 50  0001 C CNN
F 1 "VCC" H 5950 1000 50  0000 C CNN
F 2 "" H 5950 850 50  0000 C CNN
F 3 "" H 5950 850 50  0000 C CNN
	1    5950 850 
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR08
U 1 1 58403F82
P 5950 1150
F 0 "#PWR08" H 5950 1000 50  0001 C CNN
F 1 "VCC" H 5950 1300 50  0000 C CNN
F 2 "" H 5950 1150 50  0000 C CNN
F 3 "" H 5950 1150 50  0000 C CNN
	1    5950 1150
	1    0    0    1   
$EndComp
$Comp
L TEST_1P W1
U 1 1 584041B2
P 10350 1500
F 0 "W1" H 10350 1770 50  0001 C CNN
F 1 "LEFT" V 10350 1700 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_1-2mmDrill" H 10550 1500 50  0001 C CNN
F 3 "" H 10550 1500 50  0000 C CNN
	1    10350 1500
	0    1    1    0   
$EndComp
$Comp
L TEST_1P W2
U 1 1 58404255
P 10350 1650
F 0 "W2" H 10350 1920 50  0001 C CNN
F 1 "GNDA" V 10350 1850 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_1-2mmDrill" H 10550 1650 50  0001 C CNN
F 3 "" H 10550 1650 50  0000 C CNN
	1    10350 1650
	0    1    1    0   
$EndComp
$Comp
L TEST_1P W3
U 1 1 584042C0
P 10350 1800
F 0 "W3" H 10350 2070 50  0001 C CNN
F 1 "RIGHT" V 10350 2000 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_1-2mmDrill" H 10550 1800 50  0001 C CNN
F 3 "" H 10550 1800 50  0000 C CNN
	1    10350 1800
	0    1    1    0   
$EndComp
$Comp
L TL074 U1
U 3 1 584047FF
P 9200 3700
F 0 "U1" H 9200 3900 50  0000 L CNN
F 1 "TL074" H 9200 3500 50  0000 L CNN
F 2 "Housings_DIP:DIP-14_W7.62mm" H 9150 3800 50  0001 C CNN
F 3 "" H 9250 3900 50  0000 C CNN
	3    9200 3700
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 584049FF
P 8350 3350
F 0 "R5" V 8430 3350 50  0000 C CNN
F 1 "10k 0.1%" V 8350 3350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8280 3350 50  0001 C CNN
F 3 "" H 8350 3350 50  0000 C CNN
	1    8350 3350
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 58404A7F
P 8350 3850
F 0 "R6" V 8430 3850 50  0000 C CNN
F 1 "10k 0.1%" V 8350 3850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8280 3850 50  0001 C CNN
F 3 "" H 8350 3850 50  0000 C CNN
	1    8350 3850
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 58404AC9
P 8600 3850
F 0 "C2" H 8625 3950 50  0000 L CNN
F 1 "2u2" H 8625 3750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8638 3700 50  0001 C CNN
F 3 "" H 8600 3850 50  0000 C CNN
	1    8600 3850
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 58404B57
P 9500 3300
F 0 "C3" H 9525 3400 50  0000 L CNN
F 1 "2u2" H 9525 3200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9538 3150 50  0001 C CNN
F 3 "" H 9500 3300 50  0000 C CNN
	1    9500 3300
	0    1    1    0   
$EndComp
$Comp
L GNDA #PWR09
U 1 1 58404BE5
P 9800 3300
F 0 "#PWR09" H 9800 3050 50  0001 C CNN
F 1 "GNDA" H 9800 3150 50  0000 C CNN
F 2 "" H 9800 3300 50  0000 C CNN
F 3 "" H 9800 3300 50  0000 C CNN
	1    9800 3300
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR010
U 1 1 58404C33
P 8600 4100
F 0 "#PWR010" H 8600 3850 50  0001 C CNN
F 1 "GNDA" H 8600 3950 50  0000 C CNN
F 2 "" H 8600 4100 50  0000 C CNN
F 3 "" H 8600 4100 50  0000 C CNN
	1    8600 4100
	1    0    0    -1  
$EndComp
$Comp
L GNDA #PWR011
U 1 1 58404C7A
P 8350 4100
F 0 "#PWR011" H 8350 3850 50  0001 C CNN
F 1 "GNDA" H 8350 3950 50  0000 C CNN
F 2 "" H 8350 4100 50  0000 C CNN
F 3 "" H 8350 4100 50  0000 C CNN
	1    8350 4100
	1    0    0    -1  
$EndComp
$Comp
L GNDD #PWR012
U 1 1 58404CAF
P 9100 4100
F 0 "#PWR012" H 9100 3850 50  0001 C CNN
F 1 "GNDD" H 9100 3950 50  0000 C CNN
F 2 "" H 9100 4100 50  0000 C CNN
F 3 "" H 9100 4100 50  0000 C CNN
	1    9100 4100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR013
U 1 1 58404CEB
P 9100 3200
F 0 "#PWR013" H 9100 3050 50  0001 C CNN
F 1 "VCC" H 9100 3350 50  0000 C CNN
F 2 "" H 9100 3200 50  0000 C CNN
F 3 "" H 9100 3200 50  0000 C CNN
	1    9100 3200
	1    0    0    -1  
$EndComp
$Comp
L VCOM #PWR014
U 1 1 5840515C
P 9900 3700
F 0 "#PWR014" H 9900 3550 50  0001 C CNN
F 1 "VCOM" H 9900 3850 50  0000 C CNN
F 2 "" H 9900 3700 50  0000 C CNN
F 3 "" H 9900 3700 50  0000 C CNN
	1    9900 3700
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR015
U 1 1 58405549
P 8350 3050
F 0 "#PWR015" H 8350 2900 50  0001 C CNN
F 1 "VCC" H 8350 3200 50  0000 C CNN
F 2 "" H 8350 3050 50  0000 C CNN
F 3 "" H 8350 3050 50  0000 C CNN
	1    8350 3050
	1    0    0    -1  
$EndComp
NoConn ~ 7800 1450
NoConn ~ 7800 1550
NoConn ~ 7800 1750
NoConn ~ 7800 1850
NoConn ~ 7800 1950
NoConn ~ 7800 2150
NoConn ~ 7800 2450
NoConn ~ 7800 2650
NoConn ~ 7800 2750
NoConn ~ 7800 2850
NoConn ~ 7800 2950
NoConn ~ 7800 3150
NoConn ~ 7800 3250
NoConn ~ 7800 3350
NoConn ~ 7800 3450
NoConn ~ 6200 3150
NoConn ~ 6200 3050
NoConn ~ 6200 2650
NoConn ~ 6200 2550
NoConn ~ 6200 2450
NoConn ~ 6200 2350
NoConn ~ 6200 2250
NoConn ~ 6200 2150
NoConn ~ 6200 2050
NoConn ~ 6200 1950
NoConn ~ 6200 1850
NoConn ~ 6200 1750
NoConn ~ 6200 1650
NoConn ~ 6200 1450
NoConn ~ 6200 1350
NoConn ~ 6200 3550
NoConn ~ 6200 3650
$Comp
L VCC #PWR016
U 1 1 58401816
P 8150 2250
F 0 "#PWR016" H 8150 2100 50  0001 C CNN
F 1 "VCC" H 8150 2400 50  0000 C CNN
F 2 "" H 8150 2250 50  0000 C CNN
F 3 "" H 8150 2250 50  0000 C CNN
	1    8150 2250
	1    0    0    -1  
$EndComp
$Comp
L ARDUINO_MEGA_SHIELD SHIELD1
U 1 1 58502A5D
P 3600 3550
F 0 "SHIELD1" H 3200 6050 60  0000 C CNN
F 1 "ARDUINO_MEGA_SHIELD" H 3500 850 60  0000 C CNN
F 2 "" H 3600 3550 60  0001 C CNN
F 3 "" H 3600 3550 60  0001 C CNN
	1    3600 3550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR017
U 1 1 585033DA
P 2500 1950
F 0 "#PWR017" H 2500 1700 50  0001 C CNN
F 1 "GND" H 2500 1800 50  0000 C CNN
F 2 "" H 2500 1950 50  0000 C CNN
F 3 "" H 2500 1950 50  0000 C CNN
	1    2500 1950
	0    1    1    0   
$EndComp
$Comp
L CONN_02X06 P1
U 1 1 58503DE1
P 1350 2700
F 0 "P1" H 1350 3050 50  0000 C CNN
F 1 "CONN_02X06" H 1350 2350 50  0000 C CNN
F 2 "" H 1350 1500 50  0000 C CNN
F 3 "" H 1350 1500 50  0000 C CNN
	1    1350 2700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 58503F83
P 1350 2250
F 0 "#PWR018" H 1350 2000 50  0001 C CNN
F 1 "GND" H 1350 2100 50  0000 C CNN
F 2 "" H 1350 2250 50  0000 C CNN
F 3 "" H 1350 2250 50  0000 C CNN
	1    1350 2250
	-1   0    0    1   
$EndComp
Wire Wire Line
	8450 1150 9000 1150
Connection ~ 8750 1150
Wire Wire Line
	9600 1250 9850 1250
Wire Wire Line
	8750 1750 9050 1750
Wire Wire Line
	9350 1750 9700 1750
Wire Wire Line
	9700 1750 9700 1250
Connection ~ 9700 1250
Wire Wire Line
	8750 2100 8750 2200
Connection ~ 8750 2200
Wire Wire Line
	9000 2400 8750 2400
Wire Wire Line
	8750 2400 8750 2900
Wire Wire Line
	8650 1350 9000 1350
Wire Wire Line
	8650 1350 8650 1400
Wire Wire Line
	8650 1400 8450 1400
Wire Wire Line
	8750 1750 8750 1350
Connection ~ 8750 1350
Wire Wire Line
	8750 2900 9000 2900
Connection ~ 8750 2500
Wire Wire Line
	9600 2300 9900 2300
Wire Wire Line
	9300 2900 9700 2900
Wire Wire Line
	9700 2900 9700 2300
Connection ~ 9700 2300
Wire Wire Line
	8750 1050 8750 1150
Wire Wire Line
	8750 700  8750 750 
Wire Wire Line
	9050 2100 9050 2050
Wire Wire Line
	8000 2350 8000 2250
Connection ~ 8000 2250
Wire Wire Line
	8000 2650 8000 2750
Wire Wire Line
	7200 4400 7400 4400
Connection ~ 7300 4400
Wire Wire Line
	7200 4400 7200 4350
Wire Wire Line
	6800 4350 6800 4500
Wire Wire Line
	6600 4400 7000 4400
Wire Wire Line
	7000 4400 7000 4350
Wire Wire Line
	6600 4350 6600 4400
Connection ~ 6800 4400
Wire Wire Line
	6700 4350 6700 4400
Connection ~ 6700 4400
Wire Wire Line
	6900 4350 6900 4400
Connection ~ 6900 4400
Wire Wire Line
	6200 1150 5950 1150
Wire Wire Line
	10350 1650 10050 1650
Wire Wire Line
	10350 1250 10350 1500
Wire Wire Line
	10200 2300 10350 2300
Wire Wire Line
	8350 3600 8900 3600
Wire Wire Line
	8350 3500 8350 3700
Connection ~ 8350 3600
Wire Wire Line
	8350 4000 8350 4100
Wire Wire Line
	8600 3700 8600 3600
Connection ~ 8600 3600
Wire Wire Line
	9100 4000 9100 4100
Wire Wire Line
	9500 3700 9900 3700
Wire Wire Line
	9750 3700 9750 4350
Wire Wire Line
	9750 4350 8900 4350
Wire Wire Line
	8900 4350 8900 3800
Connection ~ 9750 3700
Wire Wire Line
	9350 3300 9100 3300
Connection ~ 9100 3300
Wire Wire Line
	9650 3300 9800 3300
Wire Wire Line
	8350 3050 8350 3200
Wire Wire Line
	7800 2250 8150 2250
Wire Wire Line
	7400 4400 7400 4350
Wire Wire Line
	7300 4350 7300 4500
Wire Wire Line
	8600 4100 8600 4000
Wire Wire Line
	6200 850  5950 850 
Wire Wire Line
	9100 3200 9100 3400
Wire Wire Line
	10350 1250 10150 1250
Wire Wire Line
	8700 2200 9000 2200
Wire Wire Line
	8700 2500 8750 2500
Wire Wire Line
	10350 2300 10350 1800
Wire Wire Line
	7800 850  8150 850 
Wire Wire Line
	8150 850  8150 1150
Wire Wire Line
	7800 950  8100 950 
Wire Wire Line
	8100 950  8100 1400
Wire Wire Line
	8100 1400 8150 1400
Wire Wire Line
	7800 1150 8050 1150
Wire Wire Line
	8050 1150 8050 1850
Wire Wire Line
	8050 1850 8400 1850
Wire Wire Line
	8400 1850 8400 2200
Wire Wire Line
	7800 1250 8000 1250
Wire Wire Line
	8000 1250 8000 1900
Wire Wire Line
	8000 1900 8350 1900
Wire Wire Line
	8350 1900 8350 2500
Wire Wire Line
	8350 2500 8400 2500
Wire Wire Line
	6200 3350 4700 3350
Wire Wire Line
	4700 3350 4700 3400
Wire Wire Line
	4700 3400 4500 3400
Wire Wire Line
	4500 3500 4800 3500
Wire Wire Line
	4800 3500 4800 3450
Wire Wire Line
	4800 3450 6200 3450
Wire Wire Line
	2600 1900 2500 1900
Wire Wire Line
	2500 1900 2500 2000
Wire Wire Line
	2500 2000 2600 2000
Connection ~ 2500 1950
Wire Wire Line
	1100 2450 1100 2250
Wire Wire Line
	1100 2250 1600 2250
Wire Wire Line
	1600 2250 1600 2450
Wire Wire Line
	1100 2550 1950 2550
Wire Wire Line
	1950 2550 1950 1800
Wire Wire Line
	1950 1800 2600 1800
Connection ~ 1600 2550
Wire Wire Line
	1100 2650 1050 2650
Wire Wire Line
	1050 2650 1050 1050
Wire Wire Line
	1050 1050 4600 1050
Wire Wire Line
	4600 1050 4600 1800
Wire Wire Line
	4600 1800 4500 1800
Wire Wire Line
	4500 1900 4700 1900
Wire Wire Line
	4700 1900 4700 1000
Wire Wire Line
	4700 1000 1750 1000
Wire Wire Line
	1750 1000 1750 2650
Wire Wire Line
	1750 2650 1600 2650
Wire Wire Line
	1100 2750 1000 2750
Wire Wire Line
	1000 2750 1000 950 
Wire Wire Line
	1000 950  4750 950 
Wire Wire Line
	4750 950  4750 2100
Wire Wire Line
	4750 2100 4500 2100
Wire Wire Line
	1600 2750 2500 2750
Wire Wire Line
	2500 2750 2500 4600
Wire Wire Line
	2500 4600 2600 4600
Wire Wire Line
	1100 2850 1050 2850
Wire Wire Line
	1050 2850 1050 4700
Wire Wire Line
	1050 4700 2600 4700
Wire Wire Line
	1600 2850 2400 2850
Wire Wire Line
	2400 2850 2400 4500
Wire Wire Line
	2400 4500 2600 4500
Wire Wire Line
	1600 2950 2050 2950
Wire Wire Line
	2050 2950 2050 900 
Wire Wire Line
	2050 900  4800 900 
Wire Wire Line
	4800 900  4800 2200
Wire Wire Line
	4800 2200 4500 2200
NoConn ~ 1100 2950
$Comp
L CONN_01X02 P2
U 1 1 58505396
P 4900 2350
F 0 "P2" H 4900 2500 50  0000 C CNN
F 1 "CONN_01X02" V 5000 2350 50  0000 C CNN
F 2 "" H 4900 2350 50  0000 C CNN
F 3 "" H 4900 2350 50  0000 C CNN
	1    4900 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 2300 4600 2300
Wire Wire Line
	4600 2300 4600 2400
Wire Wire Line
	4600 2400 4700 2400
Wire Wire Line
	4500 1300 4900 1300
Wire Wire Line
	4900 1300 4900 2150
Wire Wire Line
	4900 2150 4700 2150
Wire Wire Line
	4700 2150 4700 2300
$Comp
L GND #PWR019
U 1 1 585059BF
P 2300 4250
F 0 "#PWR019" H 2300 4000 50  0001 C CNN
F 1 "GND" H 2300 4100 50  0000 C CNN
F 2 "" H 2300 4250 50  0000 C CNN
F 3 "" H 2300 4250 50  0000 C CNN
	1    2300 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 4150 2300 4150
Wire Wire Line
	2300 4150 2300 4250
Wire Wire Line
	2300 4250 2600 4250
NoConn ~ 4500 1200
NoConn ~ 4500 1400
NoConn ~ 4500 1500
NoConn ~ 4500 1600
NoConn ~ 4500 1700
NoConn ~ 4500 2400
NoConn ~ 4500 2500
NoConn ~ 4500 2600
NoConn ~ 4500 2700
NoConn ~ 4500 2800
NoConn ~ 4500 3000
NoConn ~ 4500 3100
NoConn ~ 4500 3200
NoConn ~ 4500 3300
NoConn ~ 4500 3600
NoConn ~ 4500 3700
NoConn ~ 4500 3950
NoConn ~ 4500 4050
NoConn ~ 4500 4200
NoConn ~ 4500 4300
NoConn ~ 4500 4400
NoConn ~ 4500 4500
NoConn ~ 4500 4600
NoConn ~ 4500 4700
NoConn ~ 4500 4800
NoConn ~ 4500 4900
NoConn ~ 4500 5050
NoConn ~ 4500 5150
NoConn ~ 4500 5250
NoConn ~ 4500 5350
NoConn ~ 4500 5450
NoConn ~ 4500 5550
NoConn ~ 4500 5650
NoConn ~ 4500 5750
NoConn ~ 2600 6000
NoConn ~ 2600 5900
NoConn ~ 2600 5800
NoConn ~ 2600 5700
NoConn ~ 2600 5550
NoConn ~ 2600 5450
NoConn ~ 2600 5350
NoConn ~ 2600 5250
NoConn ~ 2600 5150
NoConn ~ 2600 5050
NoConn ~ 2600 4950
NoConn ~ 2600 4850
NoConn ~ 2600 4400
NoConn ~ 2600 3900
NoConn ~ 2600 3800
NoConn ~ 2600 3700
NoConn ~ 2600 3600
NoConn ~ 2600 3500
NoConn ~ 2600 3400
NoConn ~ 2600 3300
NoConn ~ 2600 3200
NoConn ~ 2600 3000
NoConn ~ 2600 2900
NoConn ~ 2600 2800
NoConn ~ 2600 2700
NoConn ~ 2600 2600
NoConn ~ 2600 2500
NoConn ~ 2600 2400
NoConn ~ 2600 2300
NoConn ~ 2600 1600
NoConn ~ 2600 1700
NoConn ~ 6200 3850
$EndSCHEMATC
