APM-Simple-Telemetry-to-FrSky
=============================
Code for reading simple telemetry data from the uart2 serial port of the APM2+ to Arduino pro mini.


We need to decide what data should be transmitted from the APM to the arduino.
The taranis can display these telemetry values:
Taranis                                   APM
A1       Analog ports on D receivers
A2
Alt      Barometric altitude sensor       barometer
Rpm      Engine speed                     throttle%
Fuel     Fuel level                       Battery remaining
T1       Temperature sensor 1             sat count and fix
T2       Temperature sensor 2             ?APM mode
Spd      Speed                            GPS speed over ground 
Dist     Distance from starting point     distance to last arm
GAlt     GPS Alt                          GPS altitude
Cell     Lowest cell on FLVS-01
Cels     Sum of all cells on FLVS-01      Battery voltage in V
Vfas     FAS-40/100 voltage measurement   
Curr     Current                          Battery current in mA
Cnsp     mAh used totalizer (calculated?)               
Powr     Power (calculated?)
AccX                                      AccelX
AccY                                      AccelY
AccZ                                      AccelZ
Hdg      Heading                          Compass heading
Vspd     Vertical speed
