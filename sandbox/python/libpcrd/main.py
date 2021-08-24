#! /usr/bin/env python3

import cv2

METHOD = cv2.TM_CCOEFF_NORMED


backpack = cv2.imread('./input/1.png')
item2 = cv2.imread('./items/icon_equipment_116613.png')
item1 = cv2.imread('./items/icon_equipment_116611.png')
item0 = cv2.imread('./items/icon_equipment_117611.png')
# item0 = cv2.imread('./items/icon_equipment_113552.png')


def searchItem(source, item):
    item = cv2.resize(item, (160, 160), cv2.INTER_AREA)
    rv = cv2.matchTemplate(source, item, METHOD)

    _minV, maxV, _minL, maxL = cv2.minMaxLoc(rv, None)

    if maxV < 0.8:
        return

    matchL = maxL

    display = source.copy()
    cv2.rectangle(display, matchL, (matchL[0] + item.shape[0], matchL[1] + item.shape[1]), (0, 0, 0), 2, 8, 0)
    cv2.imwrite(f'/mnt/d/local/tmp/output/rv.png', display)


searchItem(backpack, item1)
