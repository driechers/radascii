# radascii
ASCII Art Weather Radar

Radascii takes images posted to weather.gov, and turns them into a vt100 color map to overlay over ascii art.

![alt text](dvn_ascii.PNG?raw=true "radascii")

## Buidling / Running
Currently the application is limited to running in the same directory as the ascii art file DVN.txt and only the quad cities radar site is supported.

```bash
apt-get install libcurl4-openssl-dev libopencv-dev
cmake .
make

./radascii -s DVN
```

## Recommendations to Artists
The easiest way I have found to create an ascii art map is to use GIMP to render grid lines over the images from radar.weather.gov and. A text box is then created with the monospace font and the parameters adjusted until text lines up with the grid lines. State boundaries are then drawn by guessing what ascii char looks most like what is in the grid cell.

Also note that the code is not smart and will not fill out spaces after the end of line. Each line must have 80 chars. Each file must have 24 lines.
