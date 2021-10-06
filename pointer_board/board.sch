EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "Pointer board 2021"
Date "2021-06-12"
Rev ""
Comp "www.drekkar.com"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L board-rescue:C-Device-transistor_board-rescue C11
U 1 1 5F3038BA
P 13500 2950
F 0 "C11" H 13615 2996 50  0000 L CNN
F 1 "1uF" H 13615 2905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 13538 2800 50  0001 C CNN
F 3 "~" H 13500 2950 50  0001 C CNN
	1    13500 2950
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR010
U 1 1 5F3049EF
P 13500 3200
F 0 "#PWR010" H 13500 2950 50  0001 C CNN
F 1 "GND" H 13505 3027 50  0000 C CNN
F 2 "" H 13500 3200 50  0001 C CNN
F 3 "" H 13500 3200 50  0001 C CNN
	1    13500 3200
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:LED-Device D12
U 1 1 5F9420FF
P 14750 2950
F 0 "D12" V 14789 2833 50  0000 R CNN
F 1 "red" V 14698 2833 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 14750 2950 50  0001 C CNN
F 3 "~" H 14750 2950 50  0001 C CNN
	1    14750 2950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	14750 2800 14750 2750
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR05
U 1 1 5F9463A2
P 14750 3500
F 0 "#PWR05" H 14750 3250 50  0001 C CNN
F 1 "GND" H 14755 3327 50  0000 C CNN
F 2 "" H 14750 3500 50  0001 C CNN
F 3 "" H 14750 3500 50  0001 C CNN
	1    14750 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	14750 3150 14750 3100
Wire Wire Line
	6950 3700 6950 3800
$Comp
L board-rescue:R-Device-transistor_board-rescue R18
U 1 1 5F620284
P 6950 3550
F 0 "R18" V 6850 3500 50  0000 L CNN
F 1 "47K" V 7050 3500 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 6880 3550 50  0001 C CNN
F 3 "~" H 6950 3550 50  0001 C CNN
	1    6950 3550
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:R-Device-transistor_board-rescue R19
U 1 1 5F61F0F4
P 6950 4050
F 0 "R19" V 6850 4000 50  0000 L CNN
F 1 "10K" V 7050 4000 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 6880 4050 50  0001 C CNN
F 3 "~" H 6950 4050 50  0001 C CNN
	1    6950 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	13500 2800 13500 2750
Wire Wire Line
	13500 2750 14750 2750
Wire Wire Line
	13500 3200 13500 3100
$Comp
L board-rescue:ATmega328-PU-MCU_Microchip_ATmega U20
U 1 1 6060A903
P 4650 6700
F 0 "U20" H 4006 6746 50  0000 R CNN
F 1 "ATmega328-PU" H 4006 6655 50  0000 R CNN
F 2 "Package_DIP:DIP-28_W7.62mm" H 4650 6700 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328_P%20AVR%20MCU%20with%20picoPower%20Technology%20Data%20Sheet%2040001984A.pdf" H 4650 6700 50  0001 C CNN
	1    4650 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	15050 2700 15050 2750
Wire Wire Line
	15050 2750 14750 2750
Connection ~ 14750 2750
Wire Wire Line
	4650 5100 4750 5100
Wire Wire Line
	4750 5100 4750 5200
Wire Wire Line
	4650 5100 4650 5200
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR06
U 1 1 60619793
P 4650 8300
F 0 "#PWR06" H 4650 8050 50  0001 C CNN
F 1 "GND" H 4655 8127 50  0000 C CNN
F 2 "" H 4650 8300 50  0001 C CNN
F 3 "" H 4650 8300 50  0001 C CNN
	1    4650 8300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 8300 4650 8200
Wire Wire Line
	5250 5800 6450 5800
$Comp
L board-rescue:Conn_01x04-Connector_Generic J2
U 1 1 60617BE7
P 10450 7300
F 0 "J2" H 10400 7000 50  0000 L CNN
F 1 "Conn_01x04" H 10300 7550 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-04A_1x04_P2.54mm_Vertical" H 10450 7300 50  0001 C CNN
F 3 "~" H 10450 7300 50  0001 C CNN
	1    10450 7300
	1    0    0    1   
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR013
U 1 1 60610193
P 10100 7500
F 0 "#PWR013" H 10100 7250 50  0001 C CNN
F 1 "GND" H 10105 7327 50  0000 C CNN
F 2 "" H 10100 7500 50  0001 C CNN
F 3 "" H 10100 7500 50  0001 C CNN
	1    10100 7500
	1    0    0    -1  
$EndComp
Wire Wire Line
	10100 7500 10100 7400
Wire Wire Line
	10100 7400 10250 7400
Wire Wire Line
	9150 7100 9150 7300
Wire Wire Line
	9150 7300 5250 7300
Wire Wire Line
	10100 7000 10100 7300
Wire Wire Line
	10100 7300 10250 7300
$Comp
L board-rescue:Crystal_GND2-Device Y20
U 1 1 6061CFF7
P 7550 6450
F 0 "Y20" H 7550 6718 50  0000 C CNN
F 1 "Crystal_GND2" H 7550 6627 50  0000 C CNN
F 2 "Crystal:Resonator-3Pin_W7.0mm_H2.5mm" H 7550 6450 50  0001 C CNN
F 3 "~" H 7550 6450 50  0001 C CNN
	1    7550 6450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 6450 7850 6100
Wire Wire Line
	5250 6200 7250 6200
Wire Wire Line
	7250 6200 7250 6450
Wire Wire Line
	7250 6450 7400 6450
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR07
U 1 1 6064C72C
P 7550 6700
F 0 "#PWR07" H 7550 6450 50  0001 C CNN
F 1 "GND" H 7555 6527 50  0000 C CNN
F 2 "" H 7550 6700 50  0001 C CNN
F 3 "" H 7550 6700 50  0001 C CNN
	1    7550 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 6700 7550 6650
$Comp
L board-rescue:L-Device L91
U 1 1 60652F0C
P 14150 4300
F 0 "L91" V 14340 4300 50  0000 C CNN
F 1 "10uH" V 14249 4300 50  0000 C CNN
F 2 "Inductor_SMD:L_1206_3216Metric" H 14150 4300 50  0001 C CNN
F 3 "~" H 14150 4300 50  0001 C CNN
	1    14150 4300
	0    -1   -1   0   
$EndComp
$Comp
L board-rescue:Conn_01x02-Connector_Generic J51
U 1 1 60659CCC
P 4750 9850
F 0 "J51" H 4668 9525 50  0000 C CNN
F 1 "Conn_01x02" H 4668 9616 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 4750 9850 50  0001 C CNN
F 3 "~" H 4750 9850 50  0001 C CNN
	1    4750 9850
	1    0    0    1   
$EndComp
Wire Wire Line
	4300 9750 4550 9750
Wire Wire Line
	4550 9850 4100 9850
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR017
U 1 1 606867E1
P 4100 10550
F 0 "#PWR017" H 4100 10300 50  0001 C CNN
F 1 "GND" H 4105 10377 50  0000 C CNN
F 2 "" H 4100 10550 50  0001 C CNN
F 3 "" H 4100 10550 50  0001 C CNN
	1    4100 10550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 5900 6550 5900
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR02
U 1 1 606AB0AD
P 9050 5650
F 0 "#PWR02" H 9050 5400 50  0001 C CNN
F 1 "GND" H 9055 5477 50  0000 C CNN
F 2 "" H 9050 5650 50  0001 C CNN
F 3 "" H 9050 5650 50  0001 C CNN
	1    9050 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 5650 9050 5600
Wire Wire Line
	9050 5600 9600 5600
Wire Wire Line
	9600 5700 9350 5700
Wire Wire Line
	9200 6300 9600 6300
Wire Wire Line
	9150 7100 10250 7100
Wire Wire Line
	5250 7200 10250 7200
Wire Wire Line
	9200 6300 9200 7400
Wire Wire Line
	9250 7500 9250 5800
Wire Wire Line
	9250 5800 9600 5800
Wire Wire Line
	9600 5900 9300 5900
Wire Wire Line
	9300 5900 9300 7600
Wire Wire Line
	6950 4200 6950 4300
$Comp
L board-rescue:C-Device-transistor_board-rescue C18
U 1 1 60743B94
P 7450 4050
F 0 "C18" H 7565 4096 50  0000 L CNN
F 1 "1uF" H 7565 4005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7488 3900 50  0001 C CNN
F 3 "~" H 7450 4050 50  0001 C CNN
	1    7450 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3800 7450 3800
Wire Wire Line
	7450 3800 7450 3900
Connection ~ 6950 3800
Wire Wire Line
	6950 3800 6950 3900
Wire Wire Line
	7450 4200 7450 4300
Wire Wire Line
	7450 4300 6950 4300
Connection ~ 6950 4300
Wire Wire Line
	6950 4300 6950 4450
Wire Wire Line
	5250 6400 6150 6400
Wire Wire Line
	6150 3800 6950 3800
$Comp
L board-rescue:C-Device-transistor_board-rescue C21
U 1 1 6076C128
P 3650 5750
F 0 "C21" H 3765 5796 50  0000 L CNN
F 1 "0.1uF" H 3765 5705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3688 5600 50  0001 C CNN
F 3 "~" H 3650 5750 50  0001 C CNN
	1    3650 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 5600 3650 5500
Wire Wire Line
	3650 5500 4050 5500
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR019
U 1 1 607705E1
P 3650 5950
F 0 "#PWR019" H 3650 5700 50  0001 C CNN
F 1 "GND" H 3655 5777 50  0000 C CNN
F 2 "" H 3650 5950 50  0001 C CNN
F 3 "" H 3650 5950 50  0001 C CNN
	1    3650 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 5950 3650 5900
Text Notes 9950 5550 0    50   ~ 0
‎RFM75-S‎
Text Notes 15650 4350 0    50   ~ 0
Speaker
Text Notes 4600 10050 0    50   ~ 0
to IR diode
Text Notes 10550 7400 0    50   ~ 0
Serial port\npin 4 out\npin 3 in\npin 2 3.3V\npin 1 GND
$Comp
L board-rescue:EL7202CN-Driver_FET U91
U 1 1 6077DB9E
P 13350 4400
F 0 "U91" H 13150 4800 50  0000 C CNN
F 1 "EL7202CN" H 13650 4800 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 13350 4100 50  0001 C CNN
F 3 "http://www.intersil.com/content/dam/Intersil/documents/el72/el7202-12-22.pdf" H 13350 4100 50  0001 C CNN
	1    13350 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	14000 4300 13750 4300
Wire Wire Line
	13750 4500 14850 4500
Wire Wire Line
	15150 4500 15150 4400
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR021
U 1 1 607DD4C5
P 13350 4850
F 0 "#PWR021" H 13350 4600 50  0001 C CNN
F 1 "GND" H 13355 4677 50  0000 C CNN
F 2 "" H 13350 4850 50  0001 C CNN
F 3 "" H 13350 4850 50  0001 C CNN
	1    13350 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	13350 4850 13350 4800
$Comp
L board-rescue:Conn_01x02-Connector_Generic J61
U 1 1 6063DAA1
P 3200 9850
F 0 "J61" H 3118 9525 50  0000 C CNN
F 1 "Conn_01x02" H 3118 9616 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 3200 9850 50  0001 C CNN
F 3 "~" H 3200 9850 50  0001 C CNN
	1    3200 9850
	1    0    0    1   
$EndComp
Wire Wire Line
	2800 9750 3000 9750
Wire Wire Line
	3000 9850 2750 9850
Wire Wire Line
	2750 9850 2750 10100
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR023
U 1 1 6063DABA
P 2750 10650
F 0 "#PWR023" H 2750 10400 50  0001 C CNN
F 1 "GND" H 2755 10477 50  0000 C CNN
F 2 "" H 2750 10650 50  0001 C CNN
F 3 "" H 2750 10650 50  0001 C CNN
	1    2750 10650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 10650 2750 10500
$Comp
L board-rescue:D-Device D61
U 1 1 6063DAC5
P 2550 9550
F 0 "D61" V 2504 9629 50  0000 L CNN
F 1 "D" V 2595 9629 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-123" H 2550 9550 50  0001 C CNN
F 3 "~" H 2550 9550 50  0001 C CNN
	1    2550 9550
	0    1    1    0   
$EndComp
Wire Wire Line
	2800 9350 2550 9350
Wire Wire Line
	2550 9350 2550 9400
Connection ~ 2800 9350
Wire Wire Line
	2800 9350 2800 9750
Wire Wire Line
	2550 9700 2550 9850
Wire Wire Line
	2550 9850 2750 9850
Connection ~ 2750 9850
Text Notes 3300 9850 0    50   ~ 0
Laser
Wire Wire Line
	5250 7900 5400 7900
Wire Wire Line
	5400 7900 5400 8650
Connection ~ 4650 5100
$Comp
L board-rescue:Conn_01x02-Connector_Generic J11
U 1 1 60742626
P 9500 1100
F 0 "J11" H 9500 1200 50  0000 C CNN
F 1 "Conn_01x02" H 9418 866 50  0001 C CNN
F 2 "Connector_JST:JST_PH_B2B-PH-K_1x02_P2.00mm_Vertical" H 9500 1100 50  0001 C CNN
F 3 "~" H 9500 1100 50  0001 C CNN
	1    9500 1100
	1    0    0    -1  
$EndComp
Text Notes 9600 1550 0    50   ~ 0
To Battery
Wire Wire Line
	5250 5500 5650 5500
Wire Wire Line
	6150 3800 6150 6400
Wire Wire Line
	2800 9250 2800 9350
$Comp
L board-rescue:D_Zener-Device D10
U 1 1 60BF69AF
P 10750 3100
F 0 "D10" V 10704 3179 50  0000 L CNN
F 1 "D_Zener" V 10795 3179 50  0000 L CNN
F 2 "Diode_SMD:D_SMB" H 10750 3100 50  0001 C CNN
F 3 "~" H 10750 3100 50  0001 C CNN
	1    10750 3100
	0    1    1    0   
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR027
U 1 1 60C6FCB4
P 10750 3350
F 0 "#PWR027" H 10750 3100 50  0001 C CNN
F 1 "GND" H 10755 3177 50  0000 C CNN
F 2 "" H 10750 3350 50  0001 C CNN
F 3 "" H 10750 3350 50  0001 C CNN
	1    10750 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	10750 3350 10750 3250
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR022
U 1 1 60C7C280
P 2900 5750
F 0 "#PWR022" H 2900 5500 50  0001 C CNN
F 1 "GND" H 2905 5577 50  0000 C CNN
F 2 "" H 2900 5750 50  0001 C CNN
F 3 "" H 2900 5750 50  0001 C CNN
	1    2900 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 5750 2900 5600
Wire Wire Line
	2900 5300 2900 5100
$Comp
L board-rescue:+BATT-power #PWR015
U 1 1 60678EF2
P 11400 2550
F 0 "#PWR015" H 11400 2400 50  0001 C CNN
F 1 "+BATT" H 11415 2723 50  0000 C CNN
F 2 "" H 11400 2550 50  0001 C CNN
F 3 "" H 11400 2550 50  0001 C CNN
	1    11400 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 5100 4650 5100
Wire Wire Line
	9350 5400 9350 5700
Wire Wire Line
	10750 2750 10750 2950
Connection ~ 10750 2750
Wire Wire Line
	4650 4850 4650 5100
$Comp
L board-rescue:C-Device-transistor_board-rescue C20
U 1 1 607E35EF
P 2900 5450
F 0 "C20" H 3015 5496 50  0000 L CNN
F 1 "0.1uF" H 3015 5405 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2938 5300 50  0001 C CNN
F 3 "~" H 2900 5450 50  0001 C CNN
	1    2900 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	11400 2550 11400 2750
Connection ~ 11400 2750
$Comp
L board-rescue:+3.3V-power #PWR018
U 1 1 6080E5C0
P 4650 4850
F 0 "#PWR018" H 4650 4700 50  0001 C CNN
F 1 "+3.3V" H 4665 5023 50  0000 C CNN
F 2 "" H 4650 4850 50  0001 C CNN
F 3 "" H 4650 4850 50  0001 C CNN
	1    4650 4850
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:+3.3V-power #PWR026
U 1 1 6080F34A
P 9350 5400
F 0 "#PWR026" H 9350 5250 50  0001 C CNN
F 1 "+3.3V" H 9365 5573 50  0000 C CNN
F 2 "" H 9350 5400 50  0001 C CNN
F 3 "" H 9350 5400 50  0001 C CNN
	1    9350 5400
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:+3.3V-power #PWR029
U 1 1 60815AF7
P 15050 2700
F 0 "#PWR029" H 15050 2550 50  0001 C CNN
F 1 "+3.3V" H 15065 2873 50  0000 C CNN
F 2 "" H 15050 2700 50  0001 C CNN
F 3 "" H 15050 2700 50  0001 C CNN
	1    15050 2700
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:+3.3V-power #PWR028
U 1 1 6081A26F
P 10100 7000
F 0 "#PWR028" H 10100 6850 50  0001 C CNN
F 1 "+3.3V" H 10115 7173 50  0000 C CNN
F 2 "" H 10100 7000 50  0001 C CNN
F 3 "" H 10100 7000 50  0001 C CNN
	1    10100 7000
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:C-Device-transistor_board-rescue C29
U 1 1 608BA525
P 9500 7400
F 0 "C29" H 9615 7446 50  0000 L CNN
F 1 "0.1uF" H 9615 7355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9538 7250 50  0001 C CNN
F 3 "~" H 9500 7400 50  0001 C CNN
	1    9500 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 7000 9500 7250
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR030
U 1 1 608C66BD
P 9500 7600
F 0 "#PWR030" H 9500 7350 50  0001 C CNN
F 1 "GND" H 9505 7427 50  0000 C CNN
F 2 "" H 9500 7600 50  0001 C CNN
F 3 "" H 9500 7600 50  0001 C CNN
	1    9500 7600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 7600 9500 7550
Wire Wire Line
	12850 4500 12950 4500
Text Notes 4150 3050 0    50   ~ 0
Trigger\n(also used to power on)
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR032
U 1 1 60BC2F5F
P 6550 10700
F 0 "#PWR032" H 6550 10450 50  0001 C CNN
F 1 "GND" H 6555 10527 50  0000 C CNN
F 2 "" H 6550 10700 50  0001 C CNN
F 3 "" H 6550 10700 50  0001 C CNN
	1    6550 10700
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:+3.3V-power #PWR033
U 1 1 60D21430
P 11200 10150
F 0 "#PWR033" H 11200 10000 50  0001 C CNN
F 1 "+3.3V" H 11215 10323 50  0000 C CNN
F 2 "" H 11200 10150 50  0001 C CNN
F 3 "" H 11200 10150 50  0001 C CNN
	1    11200 10150
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR01
U 1 1 6073BB69
P 6950 4450
F 0 "#PWR01" H 6950 4200 50  0001 C CNN
F 1 "GND" H 6955 4277 50  0000 C CNN
F 2 "" H 6950 4450 50  0001 C CNN
F 3 "" H 6950 4450 50  0001 C CNN
	1    6950 4450
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:R-Device-transistor_board-rescue R5
U 1 1 610273D6
P 5100 4000
F 0 "R5" V 5000 3950 50  0000 L CNN
F 1 "10K" V 5200 4000 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 5030 4000 50  0001 C CNN
F 3 "~" H 5100 4000 50  0001 C CNN
	1    5100 4000
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:R-Device-transistor_board-rescue R61
U 1 1 61055656
P 2300 9950
F 0 "R61" V 2200 9900 50  0000 L CNN
F 1 "100" V 2300 9900 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2230 9950 50  0001 C CNN
F 3 "~" H 2300 9950 50  0001 C CNN
	1    2300 9950
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:R-Device-transistor_board-rescue R51
U 1 1 61057E9B
P 3750 9950
F 0 "R51" V 3650 9900 50  0000 L CNN
F 1 "100" V 3750 9900 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 3680 9950 50  0001 C CNN
F 3 "~" H 3750 9950 50  0001 C CNN
	1    3750 9950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 9800 3750 8750
Wire Wire Line
	2300 9800 2300 8650
Wire Wire Line
	2300 8650 5400 8650
Wire Wire Line
	2300 10100 2300 10300
Wire Wire Line
	2300 10300 2450 10300
$Comp
L board-rescue:LED-Device D7
U 1 1 61156952
P 7250 8850
F 0 "D7" V 7289 8733 50  0000 R CNN
F 1 "green or blue" V 7198 8733 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 7250 8850 50  0001 C CNN
F 3 "~" H 7250 8850 50  0001 C CNN
	1    7250 8850
	0    -1   -1   0   
$EndComp
$Comp
L board-rescue:+3.3V-power #PWR035
U 1 1 61157CF3
P 7800 8200
F 0 "#PWR035" H 7800 8050 50  0001 C CNN
F 1 "+3.3V" H 7815 8373 50  0000 C CNN
F 2 "" H 7800 8200 50  0001 C CNN
F 3 "" H 7800 8200 50  0001 C CNN
	1    7800 8200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 6600 5550 6600
$Comp
L board-rescue:LED-Device D5
U 1 1 61154B3A
P 7250 8450
F 0 "D5" V 7289 8333 50  0000 R CNN
F 1 "yellow or amber" V 7198 8333 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 7250 8450 50  0001 C CNN
F 3 "~" H 7250 8450 50  0001 C CNN
	1    7250 8450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7250 8250 7250 8300
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR036
U 1 1 6124FCD4
P 6800 9100
F 0 "#PWR036" H 6800 8850 50  0001 C CNN
F 1 "GND" H 6805 8927 50  0000 C CNN
F 2 "" H 6800 9100 50  0001 C CNN
F 3 "" H 6800 9100 50  0001 C CNN
	1    6800 9100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 9050 7250 9000
Wire Wire Line
	7250 8700 7250 8650
Connection ~ 7250 8650
Wire Wire Line
	7250 8650 7250 8600
$Comp
L board-rescue:1-1462039-5-2021-04-01_19-01-27 K10
U 1 1 607DF0D6
P 7400 2550
F 0 "K10" H 8200 2937 60  0000 C CNN
F 1 "1-1462039-5" H 8200 2831 60  0000 C CNN
F 2 "2021-04-01_19-01-27_footprints:1-1462039-5" H 8200 2790 60  0001 C CNN
F 3 "" H 7400 2550 60  0000 C CNN
	1    7400 2550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3750 10100 3750 10300
Wire Wire Line
	4100 9850 4100 10100
Wire Wire Line
	3800 10300 3750 10300
Wire Wire Line
	4100 10550 4100 10500
Text Notes 7800 6650 0    50   ~ 0
12 or 16MHz
Connection ~ 13500 2750
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR016
U 1 1 6069F32B
P 12750 3300
F 0 "#PWR016" H 12750 3050 50  0001 C CNN
F 1 "GND" H 12755 3127 50  0000 C CNN
F 2 "" H 12750 3300 50  0001 C CNN
F 3 "" H 12750 3300 50  0001 C CNN
	1    12750 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	10750 2750 11400 2750
Wire Wire Line
	7400 850  7400 950 
Wire Wire Line
	8000 950  8000 800 
Wire Wire Line
	7400 2550 7400 2650
$Comp
L board-rescue:+BATT-power #PWR0104
U 1 1 607F8F94
P 7800 900
F 0 "#PWR0104" H 7800 750 50  0001 C CNN
F 1 "+BATT" H 7815 1073 50  0000 C CNN
F 2 "" H 7800 900 50  0001 C CNN
F 3 "" H 7800 900 50  0001 C CNN
	1    7800 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 950  7800 900 
NoConn ~ 7600 950 
NoConn ~ 7600 2550
Wire Wire Line
	5650 850  7400 850 
$Comp
L board-rescue:+3.3V-power #PWR08
U 1 1 60A3F3F0
P 6950 2450
F 0 "#PWR08" H 6950 2300 50  0001 C CNN
F 1 "+3.3V" H 6965 2623 50  0000 C CNN
F 2 "" H 6950 2450 50  0001 C CNN
F 3 "" H 6950 2450 50  0001 C CNN
	1    6950 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 2650 6950 2650
Wire Wire Line
	6950 2650 6950 2450
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR020
U 1 1 60A7FBE3
P 12500 1700
F 0 "#PWR020" H 12500 1450 50  0001 C CNN
F 1 "GND" H 12505 1527 50  0000 C CNN
F 2 "" H 12500 1700 50  0001 C CNN
F 3 "" H 12500 1700 50  0001 C CNN
	1    12500 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	12500 1700 12500 1550
Wire Wire Line
	12500 1550 12700 1550
Wire Wire Line
	5650 850  5650 5500
$Comp
L board-rescue:+BATT-power #PWR031
U 1 1 6070853F
P 10900 1150
F 0 "#PWR031" H 10900 1000 50  0001 C CNN
F 1 "+BATT" H 10915 1323 50  0000 C CNN
F 2 "" H 10900 1150 50  0001 C CNN
F 3 "" H 10900 1150 50  0001 C CNN
	1    10900 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	10900 1150 10900 1450
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR03
U 1 1 607476F2
P 8900 1350
F 0 "#PWR03" H 8900 1100 50  0001 C CNN
F 1 "GND" H 8905 1177 50  0000 C CNN
F 2 "" H 8900 1350 50  0001 C CNN
F 3 "" H 8900 1350 50  0001 C CNN
	1    8900 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 1350 8900 1200
Wire Wire Line
	8900 1200 9300 1200
Wire Wire Line
	8700 1100 8700 1800
Wire Wire Line
	7800 2800 7800 2550
Wire Wire Line
	8000 2550 8000 2700
Wire Wire Line
	8000 2700 8600 2700
Wire Wire Line
	8600 2700 8600 800 
Wire Wire Line
	8600 800  8000 800 
$Comp
L board-rescue:Conn_01x02-Connector_Generic J1
U 1 1 607BA595
P 4550 2900
F 0 "J1" H 4468 2575 50  0000 C CNN
F 1 "Conn_01x02" H 4468 2666 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 4550 2900 50  0001 C CNN
F 3 "~" H 4550 2900 50  0001 C CNN
	1    4550 2900
	-1   0    0    1   
$EndComp
$Comp
L board-rescue:Conn_01x02-Connector_Generic J81
U 1 1 60698238
P 1750 9800
F 0 "J81" H 1668 9475 50  0000 C CNN
F 1 "Conn_01x02" H 1668 9566 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 1750 9800 50  0001 C CNN
F 3 "~" H 1750 9800 50  0001 C CNN
	1    1750 9800
	1    0    0    1   
$EndComp
Wire Wire Line
	1400 9200 1400 9300
Wire Wire Line
	1400 9700 1550 9700
Wire Wire Line
	1550 9800 1150 9800
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR034
U 1 1 60698246
P 1150 10600
F 0 "#PWR034" H 1150 10350 50  0001 C CNN
F 1 "GND" H 1155 10427 50  0000 C CNN
F 2 "" H 1150 10600 50  0001 C CNN
F 3 "" H 1150 10600 50  0001 C CNN
	1    1150 10600
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:D-Device D81
U 1 1 60698250
P 1150 9500
F 0 "D81" V 1104 9579 50  0000 L CNN
F 1 "D" V 1195 9579 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-123" H 1150 9500 50  0001 C CNN
F 3 "~" H 1150 9500 50  0001 C CNN
	1    1150 9500
	0    1    1    0   
$EndComp
Wire Wire Line
	1400 9300 1150 9300
Wire Wire Line
	1150 9300 1150 9350
Connection ~ 1400 9300
Wire Wire Line
	1400 9300 1400 9700
Wire Wire Line
	1150 9650 1150 9800
Connection ~ 1150 9800
Text Notes 1850 9750 0    50   ~ 0
Vib
$Comp
L board-rescue:R-Device-transistor_board-rescue R81
U 1 1 60698261
P 800 9900
F 0 "R81" V 700 9850 50  0000 L CNN
F 1 "100" V 800 9850 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 730 9900 50  0001 C CNN
F 3 "~" H 800 9900 50  0001 C CNN
	1    800  9900
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  10050 800  10250
Wire Wire Line
	1150 9800 1150 10050
Wire Wire Line
	850  10250 800  10250
Wire Wire Line
	1150 10600 1150 10450
Wire Wire Line
	5250 6900 5300 6900
Wire Wire Line
	5300 8550 800  8550
Wire Wire Line
	800  8550 800  9750
Wire Wire Line
	9600 6500 9500 6500
Wire Wire Line
	9500 6500 9500 7000
Connection ~ 9500 7000
Wire Wire Line
	5250 7000 9500 7000
Wire Wire Line
	13800 6000 13800 6100
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR038
U 1 1 6073BB71
P 14300 6850
F 0 "#PWR038" H 14300 6600 50  0001 C CNN
F 1 "GND" H 14305 6677 50  0000 C CNN
F 2 "" H 14300 6850 50  0001 C CNN
F 3 "" H 14300 6850 50  0001 C CNN
	1    14300 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	14300 6850 14300 6700
$Comp
L board-rescue:R-Device-transistor_board-rescue R79
U 1 1 6073BBA3
P 13800 6500
F 0 "R79" V 13700 6450 50  0000 L CNN
F 1 "100" V 13800 6450 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 13730 6500 50  0001 C CNN
F 3 "~" H 13800 6500 50  0001 C CNN
	1    13800 6500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	13950 6500 14000 6500
Wire Wire Line
	13800 6100 14300 6100
Wire Wire Line
	14300 6100 14300 6000
Wire Wire Line
	13800 5650 13800 5700
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR051
U 1 1 60BCDCF3
P 14950 7700
F 0 "#PWR051" H 14950 7450 50  0001 C CNN
F 1 "GND" H 14955 7527 50  0000 C CNN
F 2 "" H 14950 7700 50  0001 C CNN
F 3 "" H 14950 7700 50  0001 C CNN
	1    14950 7700
	1    0    0    -1  
$EndComp
Wire Wire Line
	11650 9800 11650 11100
Wire Wire Line
	11650 11100 6250 11100
Wire Wire Line
	6250 6500 6250 10500
$Comp
L board-rescue:TSOP34S40F-Interface_Optical U31
U 1 1 60E588A8
P 7800 9900
F 0 "U31" V 7742 10188 50  0000 L CNN
F 1 "TSOP34S40F" V 7833 10188 50  0000 L CNN
F 2 "OptoDevice:Vishay_MOLD-3Pin" H 7750 9525 50  0001 C CNN
F 3 "http://www.vishay.com/docs/82669/tsop32s40f.pdf" H 8450 10200 50  0001 C CNN
	1    7800 9900
	0    1    1    0   
$EndComp
Wire Wire Line
	7800 10500 7800 10300
Wire Wire Line
	7600 10300 7600 10600
Wire Wire Line
	7600 10600 6550 10600
Wire Wire Line
	6550 10600 6550 10700
Wire Wire Line
	8000 10300 8000 10400
Wire Wire Line
	8000 10400 9050 10400
Wire Wire Line
	11200 10400 11200 10150
$Comp
L board-rescue:TSOP34S40F-Interface_Optical U30
U 1 1 60F02CB7
P 6750 9900
F 0 "U30" V 6692 10188 50  0000 L CNN
F 1 "TSOP34S40F" V 6783 10188 50  0000 L CNN
F 2 "OptoDevice:Vishay_MOLD-3Pin" H 6700 9525 50  0001 C CNN
F 3 "http://www.vishay.com/docs/82669/tsop32s40f.pdf" H 7400 10200 50  0001 C CNN
	1    6750 9900
	0    1    1    0   
$EndComp
Wire Wire Line
	8000 10400 6950 10400
Wire Wire Line
	6950 10400 6950 10300
Connection ~ 8000 10400
Wire Wire Line
	6750 10300 6750 10500
Connection ~ 6750 10500
Wire Wire Line
	6750 10500 7800 10500
Wire Wire Line
	6550 10600 6550 10300
Connection ~ 6550 10600
$Comp
L board-rescue:C-Device-transistor_board-rescue C31
U 1 1 60FC0BD1
P 11200 10600
F 0 "C31" H 11315 10646 50  0000 L CNN
F 1 "0.1uF" H 11315 10555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 11238 10450 50  0001 C CNN
F 3 "~" H 11200 10600 50  0001 C CNN
	1    11200 10600
	1    0    0    -1  
$EndComp
Wire Wire Line
	11200 10450 11200 10400
Connection ~ 11200 10400
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR0101
U 1 1 60FF7EFC
P 11200 10800
F 0 "#PWR0101" H 11200 10550 50  0001 C CNN
F 1 "GND" H 11205 10627 50  0000 C CNN
F 2 "" H 11200 10800 50  0001 C CNN
F 3 "" H 11200 10800 50  0001 C CNN
	1    11200 10800
	1    0    0    -1  
$EndComp
Wire Wire Line
	11200 10800 11200 10750
Wire Wire Line
	6350 10500 6750 10500
Wire Wire Line
	6350 6700 5250 6700
Wire Wire Line
	6350 6700 6350 10500
$Comp
L board-rescue:Conn_01x10-Connector_Generic J20
U 1 1 610DDDCD
P 9800 6000
F 0 "J20" H 9750 5350 50  0000 L CNN
F 1 "Conn_01x10" H 9600 6600 50  0000 L CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_1x10_P1.27mm_Vertical" H 9800 6000 50  0001 C CNN
F 3 "~" H 9800 6000 50  0001 C CNN
	1    9800 6000
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:C-Device-transistor_board-rescue C30
U 1 1 608BFA1F
P 10700 10600
F 0 "C30" H 10815 10646 50  0000 L CNN
F 1 "0.1uF" H 10815 10555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10738 10450 50  0001 C CNN
F 3 "~" H 10700 10600 50  0001 C CNN
	1    10700 10600
	1    0    0    -1  
$EndComp
Wire Wire Line
	10700 10450 10700 10400
Connection ~ 10700 10400
Wire Wire Line
	10700 10400 11200 10400
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR054
U 1 1 608D5FE8
P 10700 10800
F 0 "#PWR054" H 10700 10550 50  0001 C CNN
F 1 "GND" H 10705 10627 50  0000 C CNN
F 2 "" H 10700 10800 50  0001 C CNN
F 3 "" H 10700 10800 50  0001 C CNN
	1    10700 10800
	1    0    0    -1  
$EndComp
Wire Wire Line
	10700 10800 10700 10750
Wire Wire Line
	5250 7400 9200 7400
Wire Wire Line
	9300 7600 5250 7600
Wire Wire Line
	5500 8750 3750 8750
Wire Wire Line
	9300 7900 13100 7900
Wire Wire Line
	5250 7500 5950 7500
Wire Wire Line
	5950 7500 5950 8650
Wire Wire Line
	5250 6800 6150 6800
Wire Wire Line
	6150 6800 6150 7500
Wire Wire Line
	6150 7500 9250 7500
$Comp
L board-rescue:C-Device-transistor_board-rescue C91
U 1 1 60CCC5EB
P 12550 3850
F 0 "C91" H 12665 3896 50  0000 L CNN
F 1 "0.1uF" H 12665 3805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 12588 3700 50  0001 C CNN
F 3 "~" H 12550 3850 50  0001 C CNN
	1    12550 3850
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR0102
U 1 1 60CCEB9D
P 12550 4050
F 0 "#PWR0102" H 12550 3800 50  0001 C CNN
F 1 "GND" H 12555 3877 50  0000 C CNN
F 2 "" H 12550 4050 50  0001 C CNN
F 3 "" H 12550 4050 50  0001 C CNN
	1    12550 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	12550 4050 12550 4000
Wire Wire Line
	12550 3700 12550 3650
$Comp
L board-rescue:C-Device-transistor_board-rescue C92
U 1 1 60877205
P 14850 4300
F 0 "C92" H 14965 4346 50  0000 L CNN
F 1 "1uF" H 14965 4255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 14888 4150 50  0001 C CNN
F 3 "~" H 14850 4300 50  0001 C CNN
	1    14850 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	15150 4400 15300 4400
$Comp
L board-rescue:Conn_01x02-Connector_Generic J91
U 1 1 6064F69F
P 15500 4400
F 0 "J91" H 15418 4075 50  0000 C CNN
F 1 "Conn_01x02" H 15418 4166 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 15500 4400 50  0001 C CNN
F 3 "~" H 15500 4400 50  0001 C CNN
	1    15500 4400
	1    0    0    1   
$EndComp
Wire Wire Line
	14850 4450 14850 4500
Connection ~ 14850 4500
Wire Wire Line
	14850 4500 15150 4500
Wire Wire Line
	14300 4300 14500 4300
Wire Wire Line
	14500 4300 14500 4100
Wire Wire Line
	14500 4100 14850 4100
Wire Wire Line
	14850 4100 14850 4150
Wire Wire Line
	14850 4100 15150 4100
Wire Wire Line
	15150 4100 15150 4300
Wire Wire Line
	15150 4300 15300 4300
Connection ~ 14850 4100
$Comp
L board-rescue:R-Device-transistor_board-rescue R71
U 1 1 6087E190
P 13800 5500
F 0 "R71" V 13700 5450 50  0000 L CNN
F 1 "10" V 13800 5450 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 13730 5500 50  0001 C CNN
F 3 "~" H 13800 5500 50  0001 C CNN
	1    13800 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	14300 5650 14300 5700
$Comp
L board-rescue:R-Device-transistor_board-rescue R72
U 1 1 60897C16
P 14300 5500
F 0 "R72" V 14200 5450 50  0000 L CNN
F 1 "10" V 14300 5450 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 14230 5500 50  0001 C CNN
F 3 "~" H 14300 5500 50  0001 C CNN
	1    14300 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	13800 5350 13800 5300
Wire Wire Line
	13800 5300 14300 5300
Wire Wire Line
	14300 5300 14300 5200
Wire Wire Line
	14300 5300 14300 5350
Connection ~ 14300 5300
Wire Wire Line
	4300 9050 4300 9200
Wire Wire Line
	5250 5600 6250 5600
Wire Wire Line
	5250 5700 6350 5700
Wire Wire Line
	5300 6900 5300 8550
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR04
U 1 1 6097C7C2
P 5100 4250
F 0 "#PWR04" H 5100 4000 50  0001 C CNN
F 1 "GND" H 5105 4077 50  0000 C CNN
F 2 "" H 5100 4250 50  0001 C CNN
F 3 "" H 5100 4250 50  0001 C CNN
	1    5100 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 4150 5100 4250
$Comp
L board-rescue:R-Device-transistor_board-rescue R3
U 1 1 60B32A5E
P 5100 3450
F 0 "R3" V 5000 3400 50  0000 L CNN
F 1 "3.3K" V 5200 3350 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 5030 3450 50  0001 C CNN
F 3 "~" H 5100 3450 50  0001 C CNN
	1    5100 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 3600 5100 3750
Wire Wire Line
	5100 3750 5550 3750
Connection ~ 5100 3750
Wire Wire Line
	5100 3750 5100 3850
Wire Wire Line
	5550 3750 5550 6600
Wire Wire Line
	4750 2800 7800 2800
Wire Wire Line
	4750 2900 5100 2900
Wire Wire Line
	5100 2900 5100 3300
Wire Wire Line
	5100 2900 5100 2450
Connection ~ 5100 2900
$Comp
L board-rescue:+BATT-power #PWR09
U 1 1 60C632C7
P 5100 1950
F 0 "#PWR09" H 5100 1800 50  0001 C CNN
F 1 "+BATT" H 5115 2123 50  0000 C CNN
F 2 "" H 5100 1950 50  0001 C CNN
F 3 "" H 5100 1950 50  0001 C CNN
	1    5100 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 1950 5100 2150
Wire Wire Line
	8700 1100 9300 1100
$Comp
L board-rescue:Conn_01x02-Connector_Generic J10
U 1 1 60D8DE58
P 12900 1550
F 0 "J10" H 12818 1225 50  0000 C CNN
F 1 "Conn_01x02" H 12818 1316 50  0000 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 12900 1550 50  0001 C CNN
F 3 "~" H 12900 1550 50  0001 C CNN
	1    12900 1550
	1    0    0    1   
$EndComp
$Comp
L board-rescue:Conn_01x04-Connector_Generic J40
U 1 1 608D3D67
P 15450 7450
F 0 "J40" H 15400 7150 50  0000 L CNN
F 1 "Conn_01x04" H 15300 7700 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-04A_1x04_P2.54mm_Vertical" H 15450 7450 50  0001 C CNN
F 3 "~" H 15450 7450 50  0001 C CNN
	1    15450 7450
	1    0    0    1   
$EndComp
Wire Wire Line
	15250 7450 14950 7450
Wire Wire Line
	14950 7450 14950 6800
Wire Wire Line
	14950 7700 14950 7550
Wire Wire Line
	14950 7550 15250 7550
Wire Wire Line
	15250 7350 14500 7350
Wire Wire Line
	14500 7350 14500 8500
Wire Wire Line
	14500 9800 11650 9800
Wire Wire Line
	13900 8700 13900 7250
Wire Wire Line
	13900 7250 15250 7250
$Comp
L board-rescue:R-Device-transistor_board-rescue R41
U 1 1 6096F415
P 13400 8900
F 0 "R41" V 13300 8850 50  0000 L CNN
F 1 "100" V 13400 8800 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 13330 8900 50  0001 C CNN
F 3 "~" H 13400 8900 50  0001 C CNN
	1    13400 8900
	0    1    1    0   
$EndComp
Wire Wire Line
	13550 8900 13600 8900
Wire Wire Line
	13100 8900 13100 7900
Connection ~ 13100 7900
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR011
U 1 1 6099D7F6
P 13900 9150
F 0 "#PWR011" H 13900 8900 50  0001 C CNN
F 1 "GND" H 13905 8977 50  0000 C CNN
F 2 "" H 13900 9150 50  0001 C CNN
F 3 "" H 13900 9150 50  0001 C CNN
	1    13900 9150
	1    0    0    -1  
$EndComp
Wire Wire Line
	13900 9150 13900 9100
$Comp
L board-rescue:R-Device-transistor_board-rescue R42
U 1 1 60A0FB11
P 14950 8850
F 0 "R42" V 14850 8800 50  0000 L CNN
F 1 "22K" V 14950 8800 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P2.54mm_Vertical" V 14880 8850 50  0001 C CNN
F 3 "~" H 14950 8850 50  0001 C CNN
	1    14950 8850
	1    0    0    -1  
$EndComp
Wire Wire Line
	14950 8700 14950 8500
Wire Wire Line
	14950 8500 14500 8500
Connection ~ 14500 8500
Wire Wire Line
	14500 8500 14500 9800
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR0112
U 1 1 60A25F0E
P 14950 9100
F 0 "#PWR0112" H 14950 8850 50  0001 C CNN
F 1 "GND" H 14955 8927 50  0000 C CNN
F 2 "" H 14950 9100 50  0001 C CNN
F 3 "" H 14950 9100 50  0001 C CNN
	1    14950 9100
	1    0    0    -1  
$EndComp
Wire Wire Line
	14950 9100 14950 9000
$Comp
L board-rescue:LED-Device D73
U 1 1 60A46438
P 14750 5850
F 0 "D73" V 14789 5733 50  0000 R CNN
F 1 "red" V 14698 5733 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 14750 5850 50  0001 C CNN
F 3 "~" H 14750 5850 50  0001 C CNN
	1    14750 5850
	0    -1   -1   0   
$EndComp
$Comp
L board-rescue:LED-Device D74
U 1 1 60A47101
P 15200 5850
F 0 "D74" V 15239 5733 50  0000 R CNN
F 1 "red" V 15148 5733 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 15200 5850 50  0001 C CNN
F 3 "~" H 15200 5850 50  0001 C CNN
	1    15200 5850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	15200 6000 15200 6100
Wire Wire Line
	15200 6100 14750 6100
Wire Wire Line
	14750 6100 14750 6000
Wire Wire Line
	14750 6100 14300 6100
Connection ~ 14750 6100
Connection ~ 14300 6100
$Comp
L board-rescue:R-Device-transistor_board-rescue R74
U 1 1 60AD46D3
P 15200 5500
F 0 "R74" V 15100 5450 50  0000 L CNN
F 1 "10" V 15200 5450 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 15130 5500 50  0001 C CNN
F 3 "~" H 15200 5500 50  0001 C CNN
	1    15200 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	15200 5650 15200 5700
Wire Wire Line
	14750 5650 14750 5700
Wire Wire Line
	14750 5350 14750 5300
Wire Wire Line
	14750 5300 14300 5300
Wire Wire Line
	14750 5300 15200 5300
Wire Wire Line
	15200 5300 15200 5350
Connection ~ 14750 5300
$Comp
L board-rescue:TSOP34S40F-Interface_Optical U32
U 1 1 60BC2705
P 8850 9900
F 0 "U32" V 8792 10188 50  0000 L CNN
F 1 "TSOP34S40F" V 8883 10188 50  0000 L CNN
F 2 "OptoDevice:Vishay_MOLD-3Pin" H 8800 9525 50  0001 C CNN
F 3 "http://www.vishay.com/docs/82669/tsop32s40f.pdf" H 9500 10200 50  0001 C CNN
	1    8850 9900
	0    1    1    0   
$EndComp
$Comp
L board-rescue:TSOP34S40F-Interface_Optical U33
U 1 1 60BC3C25
P 9900 9900
F 0 "U33" V 9842 10188 50  0000 L CNN
F 1 "TSOP34S40F" V 9933 10188 50  0000 L CNN
F 2 "OptoDevice:Vishay_MOLD-3Pin" H 9850 9525 50  0001 C CNN
F 3 "http://www.vishay.com/docs/82669/tsop32s40f.pdf" H 10550 10200 50  0001 C CNN
	1    9900 9900
	0    1    1    0   
$EndComp
Wire Wire Line
	7800 10500 8850 10500
Wire Wire Line
	8850 10500 8850 10300
Connection ~ 7800 10500
Wire Wire Line
	7600 10600 8650 10600
Wire Wire Line
	8650 10600 8650 10300
Connection ~ 7600 10600
Wire Wire Line
	8650 10600 9700 10600
Wire Wire Line
	9700 10600 9700 10300
Connection ~ 8650 10600
Wire Wire Line
	8850 10500 9900 10500
Wire Wire Line
	9900 10500 9900 10300
Connection ~ 8850 10500
Wire Wire Line
	10100 10400 10100 10300
Connection ~ 10100 10400
Wire Wire Line
	10100 10400 10200 10400
Wire Wire Line
	9050 10300 9050 10400
Connection ~ 9050 10400
Wire Wire Line
	9050 10400 10100 10400
Wire Wire Line
	12500 1000 12500 1450
$Comp
L Connector:USB_B_Micro J12
U 1 1 6136C30F
P 15000 1200
F 0 "J12" H 14770 1189 50  0000 R CNN
F 1 "USB_B_Micro" H 14770 1098 50  0000 R CNN
F 2 "eit_footprints:USB_Micro_B_Female_jlcpcb" H 15150 1150 50  0001 C CNN
F 3 "~" H 15150 1150 50  0001 C CNN
	1    15000 1200
	-1   0    0    -1  
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR014
U 1 1 6138C398
P 15000 1850
F 0 "#PWR014" H 15000 1600 50  0001 C CNN
F 1 "GND" H 15005 1677 50  0000 C CNN
F 2 "" H 15000 1850 50  0001 C CNN
F 3 "" H 15000 1850 50  0001 C CNN
	1    15000 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	15100 1650 15100 1600
NoConn ~ 14700 1400
$Comp
L board-rescue:Conn_01x02-Connector_Generic J13
U 1 1 614B6C64
P 9500 1900
F 0 "J13" H 9500 1700 50  0000 C CNN
F 1 "Conn_01x02" H 9418 1666 50  0001 C CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical" H 9500 1900 50  0001 C CNN
F 3 "~" H 9500 1900 50  0001 C CNN
	1    9500 1900
	1    0    0    1   
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR0105
U 1 1 614B9B74
P 9150 1950
F 0 "#PWR0105" H 9150 1700 50  0001 C CNN
F 1 "GND" H 9155 1777 50  0000 C CNN
F 2 "" H 9150 1950 50  0001 C CNN
F 3 "" H 9150 1950 50  0001 C CNN
	1    9150 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 1950 9150 1900
Wire Wire Line
	9150 1900 9300 1900
Wire Wire Line
	9300 1800 8700 1800
Connection ~ 8700 1800
Wire Wire Line
	8700 1800 8700 2800
Wire Wire Line
	5900 6500 5250 6500
Wire Wire Line
	6250 6500 5900 6500
Connection ~ 5900 6500
Wire Wire Line
	5900 5400 5900 6500
Wire Wire Line
	5900 4850 5900 5100
$Comp
L board-rescue:+3.3V-power #PWR045
U 1 1 6072A16B
P 5900 4850
F 0 "#PWR045" H 5900 4700 50  0001 C CNN
F 1 "+3.3V" H 5915 5023 50  0000 C CNN
F 2 "" H 5900 4850 50  0001 C CNN
F 3 "" H 5900 4850 50  0001 C CNN
	1    5900 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	13100 8900 13250 8900
Text Notes 9950 5650 0    50   ~ 0
GND
Text Notes 9950 5750 0    50   ~ 0
Power
Text Notes 9950 5850 0    50   ~ 0
CE Chip enable
Text Notes 9950 5950 0    50   ~ 0
CSN, SPI Chip Select (active low)
Text Notes 9950 6050 0    50   ~ 0
SCK, SPI Clock
Text Notes 9950 6150 0    50   ~ 0
MOSI, SPI Slave Data Input
Text Notes 9950 6250 0    50   ~ 0
MISO, SPI Slave Data Output
Text Notes 9950 6350 0    50   ~ 0
IRQ, Maskable interrupt pin, Active low
Text Notes 9950 6550 0    50   ~ 0
Reset, for use with ISP programmer
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR0113
U 1 1 6088CDA3
P 9400 6500
F 0 "#PWR0113" H 9400 6250 50  0001 C CNN
F 1 "GND" H 9405 6327 50  0000 C CNN
F 2 "" H 9400 6500 50  0001 C CNN
F 3 "" H 9400 6500 50  0001 C CNN
	1    9400 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 6500 9400 6400
Wire Wire Line
	9400 6400 9600 6400
Text Notes 6100 5500 1    50   ~ 0
skip this resistor \nif R42 is mounted
Text Notes 14800 750  0    50   ~ 0
optional\nUSB charging
Wire Wire Line
	14300 6300 14300 6100
Text Notes 12800 1750 0    50   ~ 0
5 Volt charging input
Text Notes 12700 9250 0    50   ~ 0
if there are no external LEDs\nthese are not needed:\nR41, Q41\n
Text Notes 700  11150 0    50   ~ 0
if there is no vibrator these \nare not needed:\nR81, Q81, D81 & J81\n
$Comp
L board-rescue:TSOP34S40F-Interface_Optical U1
U 1 1 608A5158
P 5450 9900
F 0 "U1" V 5392 10188 50  0000 L CNN
F 1 "TSOP34S40F" V 5483 10188 50  0000 L CNN
F 2 "OptoDevice:Vishay_MOLD-3Pin" H 5400 9525 50  0001 C CNN
F 3 "http://www.vishay.com/docs/82669/tsop32s40f.pdf" H 6100 10200 50  0001 C CNN
	1    5450 9900
	0    1    1    0   
$EndComp
Wire Wire Line
	6550 10600 5250 10600
Wire Wire Line
	5250 10600 5250 10300
Wire Wire Line
	6950 10400 5650 10400
Wire Wire Line
	5650 10400 5650 10300
Connection ~ 6950 10400
Wire Wire Line
	6250 10500 5450 10500
Wire Wire Line
	5450 10500 5450 10300
Connection ~ 6250 10500
Wire Wire Line
	6250 10500 6250 11100
Text Notes 5750 10100 0    50   ~ 0
Optional
$Comp
L Device:Fuse F11
U 1 1 608D3695
P 12050 1450
F 0 "F11" V 11853 1450 50  0000 C CNN
F 1 "Fuse" V 11944 1450 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 11980 1450 50  0001 C CNN
F 3 "~" H 12050 1450 50  0001 C CNN
	1    12050 1450
	0    1    1    0   
$EndComp
Wire Wire Line
	11350 1450 10900 1450
Wire Wire Line
	12200 1450 12500 1450
Connection ~ 12500 1450
Wire Wire Line
	12500 1450 12700 1450
Wire Wire Line
	8400 2800 8700 2800
Wire Wire Line
	8100 2800 7800 2800
Connection ~ 7800 2800
$Comp
L Device:Fuse F12
U 1 1 6093B7A9
P 8250 2800
F 0 "F12" V 8053 2800 50  0000 C CNN
F 1 "Fuse" V 8144 2800 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 8180 2800 50  0001 C CNN
F 3 "~" H 8250 2800 50  0001 C CNN
	1    8250 2800
	0    -1   -1   0   
$EndComp
$Comp
L board-rescue:LED-Device D72
U 1 1 6075E801
P 14300 5850
F 0 "D72" V 14339 5733 50  0000 R CNN
F 1 "red" V 14248 5733 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 14300 5850 50  0001 C CNN
F 3 "~" H 14300 5850 50  0001 C CNN
	1    14300 5850
	0    -1   -1   0   
$EndComp
$Comp
L board-rescue:LED-Device D71
U 1 1 6075D79F
P 13800 5850
F 0 "D71" V 13839 5733 50  0000 R CNN
F 1 "red" V 13748 5733 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 13800 5850 50  0001 C CNN
F 3 "~" H 13800 5850 50  0001 C CNN
	1    13800 5850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5500 7700 5250 7700
Wire Wire Line
	5500 7700 5500 8750
Wire Wire Line
	5250 7800 9300 7800
Wire Wire Line
	9300 7800 9300 7900
Text Notes 9800 850  0    50   ~ 0
TODO \nLED to indicate that charging is ongoing?\nOr just use D5/D7 to indicate that?
$Comp
L board-rescue:+BATT-power #PWR0103
U 1 1 609A01A1
P 4300 9050
F 0 "#PWR0103" H 4300 8900 50  0001 C CNN
F 1 "+BATT" H 4315 9223 50  0000 C CNN
F 2 "" H 4300 9050 50  0001 C CNN
F 3 "" H 4300 9050 50  0001 C CNN
	1    4300 9050
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:+BATT-power #PWR0106
U 1 1 609A543D
P 2800 9250
F 0 "#PWR0106" H 2800 9100 50  0001 C CNN
F 1 "+BATT" H 2815 9423 50  0000 C CNN
F 2 "" H 2800 9250 50  0001 C CNN
F 3 "" H 2800 9250 50  0001 C CNN
	1    2800 9250
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:+BATT-power #PWR0107
U 1 1 609AAC52
P 1400 9200
F 0 "#PWR0107" H 1400 9050 50  0001 C CNN
F 1 "+BATT" H 1415 9373 50  0000 C CNN
F 2 "" H 1400 9200 50  0001 C CNN
F 3 "" H 1400 9200 50  0001 C CNN
	1    1400 9200
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:+BATT-power #PWR0108
U 1 1 609AF638
P 14950 6800
F 0 "#PWR0108" H 14950 6650 50  0001 C CNN
F 1 "+BATT" H 14965 6973 50  0000 C CNN
F 2 "" H 14950 6800 50  0001 C CNN
F 3 "" H 14950 6800 50  0001 C CNN
	1    14950 6800
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:+BATT-power #PWR0109
U 1 1 609B64E3
P 14300 5200
F 0 "#PWR0109" H 14300 5050 50  0001 C CNN
F 1 "+BATT" H 14315 5373 50  0000 C CNN
F 2 "" H 14300 5200 50  0001 C CNN
F 3 "" H 14300 5200 50  0001 C CNN
	1    14300 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	13850 3650 13350 3650
Connection ~ 13350 3650
Wire Wire Line
	13350 3650 13350 4000
Wire Wire Line
	12550 3650 13350 3650
$Comp
L board-rescue:+BATT-power #PWR0110
U 1 1 60A9980F
P 13850 3550
F 0 "#PWR0110" H 13850 3400 50  0001 C CNN
F 1 "+BATT" H 13865 3723 50  0000 C CNN
F 2 "" H 13850 3550 50  0001 C CNN
F 3 "" H 13850 3550 50  0001 C CNN
	1    13850 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	13850 3550 13850 3650
Wire Wire Line
	15000 1600 15000 1800
$Comp
L board-rescue:R-Device R4
U 1 1 60DEAC87
P 15350 1650
F 0 "R4" V 15250 1650 50  0000 C CNN
F 1 "27" V 15350 1650 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 15280 1650 50  0001 C CNN
F 3 "~" H 15350 1650 50  0001 C CNN
	1    15350 1650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	15200 1650 15100 1650
Wire Wire Line
	15550 1800 15550 1650
Wire Wire Line
	15550 1650 15500 1650
Connection ~ 15000 1800
Wire Wire Line
	15000 1800 15000 1850
Wire Wire Line
	15000 1800 15550 1800
Wire Wire Line
	12500 1000 14700 1000
Text Label 7800 7200 0    50   ~ 0
PD0
Text Label 7800 7300 0    50   ~ 0
PD1
NoConn ~ 14700 1200
NoConn ~ 14700 1300
Wire Wire Line
	11750 1450 11900 1450
$Comp
L board-rescue:R-Device-transistor_board-rescue R23
U 1 1 6071A4FE
P 5900 5250
F 0 "R23" V 5800 5200 50  0000 L CNN
F 1 "10K" V 5900 5150 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P2.54mm_Vertical" V 5830 5250 50  0001 C CNN
F 3 "~" H 5900 5250 50  0001 C CNN
	1    5900 5250
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:C-Device-transistor_board-rescue C32
U 1 1 60BA9558
P 10200 10600
F 0 "C32" H 10315 10646 50  0000 L CNN
F 1 "0.1uF" H 10315 10555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10238 10450 50  0001 C CNN
F 3 "~" H 10200 10600 50  0001 C CNN
	1    10200 10600
	1    0    0    -1  
$EndComp
Wire Wire Line
	10200 10450 10200 10400
Connection ~ 10200 10400
Wire Wire Line
	10200 10400 10700 10400
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR0111
U 1 1 60BFC3D5
P 10200 10800
F 0 "#PWR0111" H 10200 10550 50  0001 C CNN
F 1 "GND" H 10205 10627 50  0000 C CNN
F 2 "" H 10200 10800 50  0001 C CNN
F 3 "" H 10200 10800 50  0001 C CNN
	1    10200 10800
	1    0    0    -1  
$EndComp
Wire Wire Line
	10200 10800 10200 10750
$Comp
L Transistor_BJT:BC817 Q71
U 1 1 60B311CA
P 14200 6500
F 0 "Q71" H 14391 6546 50  0000 L CNN
F 1 "S8050" H 14391 6455 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 14400 6425 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC817.pdf" H 14200 6500 50  0001 L CNN
	1    14200 6500
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC817 Q41
U 1 1 60B3D524
P 13800 8900
F 0 "Q41" H 13991 8946 50  0000 L CNN
F 1 "S8050" H 13991 8855 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 14000 8825 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC817.pdf" H 13800 8900 50  0001 L CNN
	1    13800 8900
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC817 Q51
U 1 1 60B463F6
P 4000 10300
F 0 "Q51" H 4191 10346 50  0000 L CNN
F 1 "S8050" H 4191 10255 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4200 10225 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC817.pdf" H 4000 10300 50  0001 L CNN
	1    4000 10300
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC817 Q61
U 1 1 60B48C61
P 2650 10300
F 0 "Q61" H 2841 10346 50  0000 L CNN
F 1 "S8050" H 2841 10255 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2850 10225 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC817.pdf" H 2650 10300 50  0001 L CNN
	1    2650 10300
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC817 Q81
U 1 1 60B49966
P 1050 10250
F 0 "Q81" H 1241 10296 50  0000 L CNN
F 1 "S8050" H 1241 10205 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 1250 10175 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC817.pdf" H 1050 10250 50  0001 L CNN
	1    1050 10250
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:C-Device-transistor_board-rescue C26
U 1 1 60B8F1AE
P 11250 8650
F 0 "C26" H 11365 8696 50  0000 L CNN
F 1 "10uF" H 11365 8605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 11288 8500 50  0001 C CNN
F 3 "~" H 11250 8650 50  0001 C CNN
	1    11250 8650
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:+3.3V-power #PWR024
U 1 1 60B935C7
P 11250 8450
F 0 "#PWR024" H 11250 8300 50  0001 C CNN
F 1 "+3.3V" H 11265 8623 50  0000 C CNN
F 2 "" H 11250 8450 50  0001 C CNN
F 3 "" H 11250 8450 50  0001 C CNN
	1    11250 8450
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR025
U 1 1 60B94285
P 11250 8850
F 0 "#PWR025" H 11250 8600 50  0001 C CNN
F 1 "GND" H 11255 8677 50  0000 C CNN
F 2 "" H 11250 8850 50  0001 C CNN
F 3 "" H 11250 8850 50  0001 C CNN
	1    11250 8850
	1    0    0    -1  
$EndComp
Wire Wire Line
	11250 8850 11250 8800
Wire Wire Line
	11250 8500 11250 8450
$Comp
L board-rescue:D-Device D1
U 1 1 60BF163E
P 5100 2300
F 0 "D1" V 5054 2379 50  0000 L CNN
F 1 "D" V 5145 2379 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-123" H 5100 2300 50  0001 C CNN
F 3 "~" H 5100 2300 50  0001 C CNN
	1    5100 2300
	0    1    1    0   
$EndComp
$Comp
L board-rescue:R-Device-transistor_board-rescue R12
U 1 1 60BEDE63
P 14750 3300
F 0 "R12" V 14650 3250 50  0000 L CNN
F 1 "100" V 14750 3250 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 14680 3300 50  0001 C CNN
F 3 "~" H 14750 3300 50  0001 C CNN
	1    14750 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	14750 3450 14750 3500
$Comp
L board-rescue:R-Device-transistor_board-rescue R1
U 1 1 60CBC9C4
P 7450 8250
F 0 "R1" V 7350 8200 50  0000 L CNN
F 1 "100" V 7450 8200 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7380 8250 50  0001 C CNN
F 3 "~" H 7450 8250 50  0001 C CNN
	1    7450 8250
	0    1    1    0   
$EndComp
$Comp
L board-rescue:R-Device-transistor_board-rescue R10
U 1 1 60BFEF5B
P 11550 1600
F 0 "R10" V 11450 1550 50  0000 L CNN
F 1 "10" V 11550 1550 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 11480 1600 50  0001 C CNN
F 3 "~" H 11550 1600 50  0001 C CNN
	1    11550 1600
	0    1    1    0   
$EndComp
$Comp
L board-rescue:R-Device-transistor_board-rescue R11
U 1 1 60BFCA2B
P 11550 1300
F 0 "R11" V 11450 1250 50  0000 L CNN
F 1 "10" V 11550 1250 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 11480 1300 50  0001 C CNN
F 3 "~" H 11550 1300 50  0001 C CNN
	1    11550 1300
	0    1    1    0   
$EndComp
Wire Wire Line
	11750 1450 11750 1300
Wire Wire Line
	11750 1300 11700 1300
Wire Wire Line
	11750 1450 11750 1600
Wire Wire Line
	11750 1600 11700 1600
Connection ~ 11750 1450
Wire Wire Line
	11350 1450 11350 1300
Wire Wire Line
	11350 1300 11400 1300
Wire Wire Line
	11350 1450 11350 1600
Wire Wire Line
	11350 1600 11400 1600
Connection ~ 11350 1450
Text Notes 1450 3950 0    50   ~ 0
TODO May want to run on 2.7 Volt. If so:\n  Y20 need to be 12 MHz.\n  U10 need to give 2.7 Volt\n  R3 need to be 6.8K\n  D5 and D7 might not work at that voltage?
$Comp
L board-rescue:C-Device-transistor_board-rescue C2
U 1 1 60C16912
P 8000 5650
F 0 "C2" H 8115 5696 50  0000 L CNN
F 1 "10 pF" H 8115 5605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8038 5500 50  0001 C CNN
F 3 "~" H 8000 5650 50  0001 C CNN
	1    8000 5650
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:C-Device-transistor_board-rescue C1
U 1 1 60C17905
P 6850 5650
F 0 "C1" H 6965 5696 50  0000 L CNN
F 1 "10 pF" H 6965 5605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6888 5500 50  0001 C CNN
F 3 "~" H 6850 5650 50  0001 C CNN
	1    6850 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 5650 7250 5450
Wire Wire Line
	7250 5450 6850 5450
Wire Wire Line
	6850 5450 6850 5500
Wire Wire Line
	7850 5650 7850 5450
Wire Wire Line
	8000 5450 8000 5500
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR037
U 1 1 60C51A4E
P 6850 5850
F 0 "#PWR037" H 6850 5600 50  0001 C CNN
F 1 "GND" H 6855 5677 50  0000 C CNN
F 2 "" H 6850 5850 50  0001 C CNN
F 3 "" H 6850 5850 50  0001 C CNN
	1    6850 5850
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR039
U 1 1 60C523C5
P 8000 5850
F 0 "#PWR039" H 8000 5600 50  0001 C CNN
F 1 "GND" H 8005 5677 50  0000 C CNN
F 2 "" H 8000 5850 50  0001 C CNN
F 3 "" H 8000 5850 50  0001 C CNN
	1    8000 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 5850 8000 5800
Wire Wire Line
	6850 5850 6850 5800
Wire Wire Line
	6250 5600 6250 4850
Wire Wire Line
	6350 5700 6350 4950
Wire Wire Line
	6350 4950 12850 4950
Wire Wire Line
	12850 4950 12850 4500
Wire Wire Line
	6450 5800 6450 5050
Wire Wire Line
	6450 5050 8750 5050
Wire Wire Line
	8750 5050 8750 6100
Wire Wire Line
	6550 5900 6550 5150
Wire Wire Line
	6550 5150 8650 5150
Wire Wire Line
	8650 5150 8650 6200
Wire Wire Line
	6650 6000 6650 5250
Wire Wire Line
	6650 5250 8850 5250
Wire Wire Line
	6650 6000 5250 6000
Wire Wire Line
	8850 5250 8850 6000
Wire Wire Line
	8850 6000 9600 6000
Wire Wire Line
	8750 6100 9600 6100
Wire Wire Line
	8650 6200 9600 6200
Connection ~ 7250 6200
Connection ~ 7250 5650
Wire Wire Line
	7850 5650 7850 6100
Connection ~ 7850 5650
Connection ~ 7850 6100
Wire Wire Line
	7850 5450 8000 5450
Wire Wire Line
	7700 6450 7850 6450
Wire Wire Line
	5250 6100 7850 6100
Wire Wire Line
	7250 5650 7250 6200
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR0114
U 1 1 610F61A1
P 7550 5850
F 0 "#PWR0114" H 7550 5600 50  0001 C CNN
F 1 "GND" H 7555 5677 50  0000 C CNN
F 2 "" H 7550 5850 50  0001 C CNN
F 3 "" H 7550 5850 50  0001 C CNN
	1    7550 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 5650 7850 5650
Wire Wire Line
	7400 5650 7250 5650
$Comp
L Device:Crystal_GND24 Y21
U 1 1 610F11C2
P 7550 5650
F 0 "Y21" H 7350 5850 50  0000 L CNN
F 1 "Crystal_GND24" H 7744 5605 50  0001 L CNN
F 2 "Crystal:Crystal_SMD_3225-4Pin_3.2x2.5mm" H 7550 5650 50  0001 C CNN
F 3 "~" H 7550 5650 50  0001 C CNN
	1    7550 5650
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR0115
U 1 1 6111EFE9
P 8450 5450
F 0 "#PWR0115" H 8450 5200 50  0001 C CNN
F 1 "GND" H 8455 5277 50  0000 C CNN
F 2 "" H 8450 5450 50  0001 C CNN
F 3 "" H 8450 5450 50  0001 C CNN
	1    8450 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 5450 8450 5400
Wire Wire Line
	8450 5400 7550 5400
Wire Wire Line
	7550 5400 7550 5450
Text Label 8900 6000 0    50   ~ 0
SCK
Text Label 8900 6100 0    50   ~ 0
MOSI
Text Label 8900 6200 0    50   ~ 0
MISO
Text Label 8850 7000 0    50   ~ 0
Reset_
Wire Wire Line
	13100 6500 13650 6500
Wire Wire Line
	13100 6500 13100 7900
Wire Wire Line
	6950 3150 8900 3150
Wire Wire Line
	8900 3150 8900 2750
Wire Wire Line
	6950 3150 6950 3400
Wire Wire Line
	8900 2750 10750 2750
Wire Wire Line
	6250 4850 12750 4850
Wire Wire Line
	12750 4850 12750 4300
Wire Wire Line
	12750 4300 12950 4300
Connection ~ 11950 2750
Wire Wire Line
	11950 2750 11400 2750
$Comp
L board-rescue:C-Device-transistor_board-rescue C10
U 1 1 60A36635
P 11950 3000
F 0 "C10" H 12065 3046 50  0000 L CNN
F 1 "1uF" H 12065 2955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 11988 2850 50  0001 C CNN
F 3 "~" H 11950 3000 50  0001 C CNN
	1    11950 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	11950 3250 11950 3150
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR012
U 1 1 5F253D6E
P 11950 3250
F 0 "#PWR012" H 11950 3000 50  0001 C CNN
F 1 "GND" H 11955 3077 50  0000 C CNN
F 2 "" H 11950 3250 50  0001 C CNN
F 3 "" H 11950 3250 50  0001 C CNN
	1    11950 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	11950 2850 11950 2750
Wire Wire Line
	13100 2750 13500 2750
Wire Wire Line
	12450 2750 12350 2750
$Comp
L drekkar_com:Low_dropout_regulator_SOT-23-5 U10
U 1 1 60DBCB8D
P 12800 2900
F 0 "U10" H 12775 3275 50  0000 C CNN
F 1 "Low_dropout_regulator_TPS782" H 12850 3400 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 12800 2900 50  0001 C CNN
F 3 "" H 12800 2900 50  0001 C CNN
	1    12800 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	12750 3300 12750 3250
Wire Wire Line
	12750 3250 12850 3250
Wire Wire Line
	12850 3250 12850 3200
Connection ~ 12750 3250
Wire Wire Line
	12750 3250 12750 3200
Wire Wire Line
	12450 2850 12350 2850
Wire Wire Line
	12350 2850 12350 2750
Connection ~ 12350 2750
Wire Wire Line
	12350 2750 11950 2750
Wire Wire Line
	7300 8250 7250 8250
Wire Wire Line
	7600 8250 7800 8250
Wire Wire Line
	7800 8250 7800 8200
Wire Wire Line
	5950 8650 7250 8650
$Comp
L board-rescue:R-Device-transistor_board-rescue R2
U 1 1 60C7FFA2
P 7050 9050
F 0 "R2" V 6950 9000 50  0000 L CNN
F 1 "100" V 7050 9000 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 6980 9050 50  0001 C CNN
F 3 "~" H 7050 9050 50  0001 C CNN
	1    7050 9050
	0    1    1    0   
$EndComp
Wire Wire Line
	7200 9050 7250 9050
Wire Wire Line
	6900 9050 6800 9050
Wire Wire Line
	6800 9050 6800 9100
$Comp
L eit_proprietary:connector_10pin_ISP_Console J21
U 1 1 60D07C3E
P 1600 6100
F 0 "J21" H 1600 6465 50  0000 C CNN
F 1 "connector_10pin_ISP_Console" H 1600 6374 50  0000 C CNN
F 2 "eit_footprints:connector_10pin_TC2050-IDC-NL" H 1600 6100 50  0001 C CNN
F 3 "" H 1600 6100 50  0001 C CNN
	1    1600 6100
	1    0    0    -1  
$EndComp
Text Label 950  6000 0    50   ~ 0
MISO
Wire Wire Line
	1200 6000 950  6000
Text Label 950  6100 0    50   ~ 0
SCK
Wire Wire Line
	950  6100 1200 6100
Text Label 950  6200 0    50   ~ 0
Reset_
Wire Wire Line
	1200 6200 950  6200
NoConn ~ 1200 6400
NoConn ~ 2000 6400
Text Label 2250 6100 0    50   ~ 0
MOSI
Wire Wire Line
	2000 6000 2250 6000
Wire Wire Line
	2000 6100 2250 6100
Wire Wire Line
	2000 6200 2450 6200
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR0116
U 1 1 60DC79D4
P 2450 6500
F 0 "#PWR0116" H 2450 6250 50  0001 C CNN
F 1 "GND" H 2455 6327 50  0000 C CNN
F 2 "" H 2450 6500 50  0001 C CNN
F 3 "" H 2450 6500 50  0001 C CNN
	1    2450 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 6500 2450 6200
$Comp
L board-rescue:+3.3V-power #PWR0117
U 1 1 60DC79DF
P 2250 5950
F 0 "#PWR0117" H 2250 5800 50  0001 C CNN
F 1 "+3.3V" H 2265 6123 50  0000 C CNN
F 2 "" H 2250 5950 50  0001 C CNN
F 3 "" H 2250 5950 50  0001 C CNN
	1    2250 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 6000 2250 5950
Text Label 2250 6300 0    50   ~ 0
PD0
Wire Wire Line
	2250 6300 2000 6300
Text Label 950  6300 0    50   ~ 0
PD1
Wire Wire Line
	950  6300 1200 6300
$Comp
L board-rescue:R-Device-transistor_board-rescue R73
U 1 1 60AD3B24
P 14750 5500
F 0 "R73" V 14650 5450 50  0000 L CNN
F 1 "10" V 14750 5450 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 14680 5500 50  0001 C CNN
F 3 "~" H 14750 5500 50  0001 C CNN
	1    14750 5500
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:R-Device-transistor_board-rescue R52
U 1 1 60D6D5CF
P 4300 9350
F 0 "R52" V 4200 9300 50  0000 L CNN
F 1 "3.3" V 4300 9300 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" V 4230 9350 50  0001 C CNN
F 3 "~" H 4300 9350 50  0001 C CNN
	1    4300 9350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 9500 4300 9750
Text Notes 15800 8950 2    50   ~ 0
skip this resistor \nif R23 is mounted
$Comp
L board-rescue:C-Device-transistor_board-rescue C6
U 1 1 60DD01FB
P 10650 8650
F 0 "C6" H 10765 8696 50  0000 L CNN
F 1 "10uF" H 10765 8605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10688 8500 50  0001 C CNN
F 3 "~" H 10650 8650 50  0001 C CNN
	1    10650 8650
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:GND-power-transistor_board-rescue #PWR041
U 1 1 60DD020F
P 10650 8850
F 0 "#PWR041" H 10650 8600 50  0001 C CNN
F 1 "GND" H 10655 8677 50  0000 C CNN
F 2 "" H 10650 8850 50  0001 C CNN
F 3 "" H 10650 8850 50  0001 C CNN
	1    10650 8850
	1    0    0    -1  
$EndComp
Wire Wire Line
	10650 8850 10650 8800
Wire Wire Line
	10650 8500 10650 8450
$Comp
L board-rescue:+BATT-power #PWR040
U 1 1 60E00966
P 10650 8450
F 0 "#PWR040" H 10650 8300 50  0001 C CNN
F 1 "+BATT" H 10665 8623 50  0000 C CNN
F 2 "" H 10650 8450 50  0001 C CNN
F 3 "" H 10650 8450 50  0001 C CNN
	1    10650 8450
	1    0    0    -1  
$EndComp
$EndSCHEMATC
