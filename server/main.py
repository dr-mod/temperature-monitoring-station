from datetime import datetime
import struct
import sys
import time
import traceback
# import psycopg2

import pigpio
from nrf24 import *

if __name__ == "__main__":
    hostname = 'localhost'
    port = 8888
    address = ["0IWSO", "1IWSO"]

    pi = pigpio.pi(hostname, port)
    if not pi.connected:
        print("Cannot connect to pigpiod")
        sys.exit()

    nrf = NRF24(pi, ce=25, payload_size=RF24_PAYLOAD.DYNAMIC, channel=100, data_rate=RF24_DATA_RATE.RATE_250KBPS,
                pa_level=RF24_PA.MIN)
    nrf.set_address_bytes(len(address[0]))

    nrf.open_reading_pipe(RF24_RX_ADDR.P1, address[0])
    nrf.open_reading_pipe(RF24_RX_ADDR.P2, address[1])

    nrf.show_registers()

    try:
        # con = psycopg2.connect(dbname='readings', user='pi',
        #                        password='', host='localhost')
        # cur = con.cursor()
        f = open('/home/pi/temp.txt', 'a')
        while True:
            while nrf.data_ready():
                now = datetime.now()
                pipe = nrf.data_pipe()
                payload = nrf.get_payload()
                protocol = payload[0] if len(payload) > 0 else -1
                hex = ':'.join(f'{i:02x}' for i in payload)
                print(f"{now:%Y-%m-%d %H:%M:%S.%f}: pipe: {pipe}, len: {len(payload)}, bytes: {hex}")

                if len(payload) == 9 and payload[0] == 0x01:
                    values = struct.unpack("<Bff", payload)
                    print(f'Protocol: {values[0]}, T: \033[32m{values[1]}\033[0m, H: \033[93m{values[2]}\033[0m')
                    f.write(f'{now:%Y-%m-%d %H:%M:%S}, {values[1]}, {values[2]}\n')
                    f.flush()
                    sensor_number = {
                        1: 0,
                        2: 1
                    }.get(pipe, 99)
                    # cur.execute("insert into reading (sensor, date, temperature, humidity) values (%s, %s, %s, %s)",
                    #             (sensor_number, now, values[1], values[2]))
                    # con.commit()
            time.sleep(0.1)
    except:
        f.close()
        traceback.print_exc()
        nrf.power_down()
        pi.stop()
