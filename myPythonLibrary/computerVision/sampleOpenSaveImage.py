
import numpy as np
import cv2

img = cv2.imread('keys2016.JPG', 0)
cv2.namedWindow('image_keys', cv2.WINDOW_NORMAL)
cv2.imshow('image_keys', img)
k = cv2.waitKey(0) & 0xFF
if k == 27: # wait for ESC key to exit
	cv2.destroyAllWindows()
elif k == ord('s'): # wait for 's' key to save and exit
	cv2.imwrite('keys2016_gray.png', img)
	cv2.destroyAllWindows()