# radascii
ASCII Art Weather Radar

Radascii takes images nowcoast, and turns them into a vt100 color map to overlay over ascii art.

Radascii supports images over the continental US.

Radascii supports the following image types: radar, clouds, lightning, surface temp, apparent temp, high temp, low temp, short hazards, long hazards, fire, visibility, 6hr snow fall, dew point, harmful algae bloom, relative humidity, wind speed, wind gust, wave, 12hr precipitation probability, 1 3 6 12 24 48 and 72 hr precipitation accumulation.

## Samples

### Radar

![alt text](radar.png?raw=true "radascii")

### Cloud Coverage

![alt text](satelite.png?raw=true "radascii")

### Lightning
![alt text](lightning.png?raw=true "radascii")

### Fire Outlook
![alt text](fire.png?raw=true "radascii")

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
