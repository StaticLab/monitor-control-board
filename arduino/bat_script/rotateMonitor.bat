set arg1=%1
mode COM4: BAUD=9600 DATA=8 STOP=1 PARITY=N to=off xon=off odsr=off octs=off rts=off idsr=off dtr=off
timeout 2
set /p x=%arg1% <nul >\\.\COM4

