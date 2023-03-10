import time
import serial


ser = serial.Serial(
    port='COM4',
    baudrate=9600,
)

if ser.isOpen():
    ser.close()
ser.open()
ser.isOpen()
while True:
    ser.write('1'.encode())
    time.sleep(1)
    ser.write('0'.encode())
    time.sleep(1)


