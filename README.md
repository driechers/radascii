# radascii
ASCII Art Weather Radar

Radascii takes images nowcoast, and turns them into a vt100 color map to overlay over ascii art.

Radascii supports radar images over the continental US, most of Canada and most of Mexico
![alt text](radar.png?raw=true "radascii")

Radascii also support satelite cloud cover images over the continental US, most of Canada and most of Mexico
![alt text](satelite.png?raw=true "radascii")

Radascii also support lightning images over the continental US, most of Canada and most of Mexico
![alt text](lightning.png?raw=true "radascii")

## Buidling / Running
Currently the application is limited to running in the same directory as the ascii art file DVN.txt and only the quad cities radar site is supported.

```bash
apt-get install libcurl4-openssl-dev libpng-dev
make

./radascii
```

## Recommendations to Artists
The easiest way I have found to create an ascii art map is to use downlaod a map from nowcoast (saving location data for future exports) and editing it with GIMP. A text box can be created with the monospace font and the parameters adjusted until text lines up with the map. State boundaries are then drawn by guessing what ascii char looks most like the section of line being covered.

Also note that the code is not smart and will not fill out spaces after the end of line. Each line must have trailing spaces.
