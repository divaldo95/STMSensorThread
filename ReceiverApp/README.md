# ZBSerialReader

## Run
Replace /dev/ttyACM0 with your actual serial device.
```
make
./serial_thread -d /dev/ttyACM0 
```

The content of the log file is flushed every `flush_every_nth_line`th line.