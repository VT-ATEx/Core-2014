#!/bin/bash
# sstv camera, 
# shell script to test SSTV camera

### BEGIN INIT INFO
# Provides:		sstvcam
# Required-Start:	$remote_fs $syslog
# Required-Stop:	$remote_fs $syslog
# Default-Start:	2 3 4 5 6
### END INIT INFO

for i in 1 2 3
do
	raspistill -t 1 --width 320 --height 256 -e png -o /tmp/image.png
	# add callsign
	/home/pi/ImageMagick-6.8.9-8/utilities/mogrify -pointsize 24 -draw "text 10,40 'KK4PWO'" /tmp/image.png
	/home/pi/pisstv/./pisstv /tmp/image.png 22050
	sudo /home/pi/pisstv/./pifm /tmp/image.png.wav 145.5 22050
	rm /tmp/image.png*
	echo "$i pictures transmitted"
done
