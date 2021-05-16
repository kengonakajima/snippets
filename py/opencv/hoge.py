import cv2
import numpy as np
import sys

img=cv2.imread(sys.argv[1],cv2.IMREAD_COLOR)
img = cv2.medianBlur(img,5)


hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)


# red
hsv_min = np.array([0,64,0])
hsv_max = np.array([30,255,255])
mask = cv2.inRange(hsv, hsv_min, hsv_max)

poo = cv2.cvtColor(mask,cv2.COLOR_GRAY2BGR)
gs = cv2.cvtColor(poo,cv2.COLOR_BGR2GRAY)


# houghcirclesはいまいち
# akazeも半分ぐらい
# akaze = cv2.AKAZE_create()
# key_points, descriptions = akaze.detectAndCompute(gs, None)
#

#for kpt in key_points:
#  x, y = kpt.pt
#  cv2.circle(poo, (int(x), int(y)), 3, (255, 0, 0), 1, 16)

  
contours, hier = cv2.findContours(gs,  cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

for ct in contours:
  M=cv2.moments(ct)
  if M['m00']==0:
      continue
  cx = int(M['m10']/M['m00'])
  cy = int(M['m01']/M['m00'])
  print(cx,cy)
  cv2.drawMarker(img, (int(cx), int(cy)), (255,0,0), markerType=cv2.MARKER_CROSS, markerSize=20, thickness=1, line_type=cv2.LINE_8)

cv2.imwrite("output.png",img)

cv2.imshow("hogewin",img)
cv2.waitKey(0)
cv2.destroyAllWindows()

