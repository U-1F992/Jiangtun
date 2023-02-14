from threading import Event, Thread
from time import sleep
from serial import Serial

arduino = Serial("COM3")
dummy = Serial("COM5")

def from_arduino(event: Event):
    while not event.is_set():
        s = arduino.readline().decode("utf-8")[:-1]
        if s != "":
            print(f"arduino: {s}")

def pass_through(event: Event):
    while not event.is_set():
        data = dummy.read(1)
        arduino.write(data)
        if data != b"":
            print(data)

def main():
    event = Event()
    Thread(target=from_arduino, args=(event,)).start()
    Thread(target=pass_through, args=(event,)).start()

    try:
        while True:
            sleep(1)
    except KeyboardInterrupt:
        event.set()
        arduino.cancel_read()
        dummy.cancel_read()

if __name__ == "__main__":
    main()
