#!/usr/bin/python

#TODO: REFACTOR!
#TODO: Automatic Download
#TODO: Fix radar offset
#http://radar.weather.gov/lite/NCR/DVN_0.png

import numpy as np
import cv2
import sys

radar = cv2.imread('DVN_0.png')
height, width, channels = radar.shape

modeImg = np.zeros((24,80,3), np.uint8)
bigModeImg = np.zeros((height,width,3), np.uint8)

for row in range(24):
    for col in range(80):
        modeHash = {}
        mode = None
        mMax = 0
        for y in range(row*height/24, (row+1)*height/24):
            for x in range(col*width/80, (col+1)*width/80):
                b = radar[y][x][0]
                g = radar[y][x][1]
                r = radar[y][x][2]

                key = str(b) + ',' + str(g) + ',' + str(r)
                if key not in modeHash:
                    modeHash[key] = 1
                else:
                    modeHash[key] += 1

        for color in modeHash:
            if modeHash[color] > mMax:
                mMax = modeHash[color]
                mode = color.split(',')

        for y in range(row*height/24, (row+1)*height/24):
            for x in range(col*width/80, (col+1)*width/80):
                bigModeImg[y][x][0] = mode[0]
                bigModeImg[y][x][1] = mode[1]
                bigModeImg[y][x][2] = mode[2]

        modeImg[row][col][0] = mode[0]
        modeImg[row][col][1] = mode[1]
        modeImg[row][col][2] = mode[2]

#"\033[48;5;160mhello\033[m"

# See http://bitmote.com/index.php?post/2012/11/19/Using-ANSI-Color-Codes-to-Colorize-Your-Bash-Prompt-on-Linux for color table

DBZ = [
        {'bgr':[231, 233,   4], 'shell':16+35}, #DBZ05
        {'bgr':[244, 159,   1], 'shell':16+17}, #DBZ10
        {'bgr':[244,   0,   3], 'shell':16+5}, #DBZ15
        {'bgr':[  2, 253,   2], 'shell':16+30}, #DBZ20
        {'bgr':[  1, 197,   1], 'shell':16+24}, #DBZ25
        {'bgr':[  0, 142,   0], 'shell':16+18}, #DBZ30
        {'bgr':[  2, 248, 253], 'shell':196+30}, #DBZ35
        {'bgr':[  0, 188, 229], 'shell':196+18}, #DBZ40?
        {'bgr':[  0, 139, 253], 'shell':196+12}, #DBZ45?
        {'bgr':[  0,   0, 253], 'shell':196}, #DBZ50
        {'bgr':[115, 115, 115], 'shell':232+14}, #Grey Border
]

i = 0
for row in modeImg:
    j = 0
    for pix in row:
        if pix[0] == 255 and pix[1] == 255 and pix[2] == 255:
            continue
        if pix[0] == 0 and pix[1] == 0 and pix[2] == 0:
            continue
        if pix[0] == 115 and pix[1] == 115 and pix[2] == 115:
            continue
        print pix, i, j
        j+=1
    i+=1

asciiMap = ''
with open('dvn.txt', 'r') as f:
    asciiMap = f.read()

asciiMap = asciiMap.replace('\n','')

i=0
for row in modeImg:
    for pix in row:
        for color in DBZ:
            if color['bgr'][0] == pix[0] and color['bgr'][1] == pix[1] and color['bgr'][2] == pix[2]:
                sys.stdout.write("\033[48;5;%dm" % color['shell'])
                break

        if i < len(asciiMap):
            sys.stdout.write(asciiMap[i])
        else:
            sys.stdout.write(' ')

        sys.stdout.write("\033[m")
        i += 1
    sys.stdout.write('\n')

cv2.imshow('image', radar)
cv2.imshow('mode', bigModeImg)
cv2.waitKey(0)
cv2.destroyAllWindows()
