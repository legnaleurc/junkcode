#! /usr/bin/env python3

import re

import cv2
import numpy
import pytesseract

METHOD = cv2.TM_CCOEFF_NORMED


backpack = cv2.imread('./input/1.png')
item2 = cv2.imread('./items/icon_equipment_116613.png')
item1 = cv2.imread('./items/icon_equipment_116611.png')
item0 = cv2.imread('./items/icon_equipment_117611.png')
# item0 = cv2.imread('./items/icon_equipment_113552.png')


def searchItem(source, item, index):
    item = cv2.resize(item, (160, 160), cv2.INTER_AREA)
    rv = cv2.matchTemplate(source, item, METHOD)

    _minV, maxV, _minL, maxL = cv2.minMaxLoc(rv, None)

    if maxV < 0.8:
        return 0

    matchL = maxL

    display = source[matchL[1] + 120:matchL[1] + item.shape[1] - 5, matchL[0] + 80:matchL[0] + item.shape[0] - 5]

    display = cv2.cvtColor(display, cv2.COLOR_BGR2HSV)
    lower = numpy.array([0, 0, 70], dtype=numpy.uint8)
    upper = numpy.array([255, 255, 125], dtype=numpy.uint8)
    mask = cv2.inRange(display, lower, upper)
    mask = ~mask
    display = cv2.cvtColor(display, cv2.COLOR_HSV2BGR)

    cv2.imwrite(f'/mnt/d/local/tmp/output/display_{index}.png', display)
    cv2.imwrite(f'/mnt/d/local/tmp/output/mask_{index}.png', mask)
    rv = pytesseract.image_to_string(mask, lang='eng', config="--oem 3 --psm 6 outputbase digits")
    rv = re.sub('\D', '', rv)
    if not rv:
        return 0
    rv = int(rv, 10)
    return rv



print(searchItem(backpack, item0, 0))
print(searchItem(backpack, item1, 1))
print(searchItem(backpack, item2, 2))
