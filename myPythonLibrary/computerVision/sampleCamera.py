
import numpy as np
import cv2

cap = cv2.VideoCapture(1)

while(True):
	# capture frame by frame
	ret, frame = cap.read()

	#operations on the frame comes next
	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

	# display the frame

	#cv2.namedWindow('image_keys', cv2.WINDOW_NORMAL)
	cv2.imshow('frame', gray)
	
	if cv2.waitKey(0) & 0xFF == ord('q'): # wait for ESC key to exit
		break

# when done, release the capture
cap.release()
cv2.destroyAllWindows()