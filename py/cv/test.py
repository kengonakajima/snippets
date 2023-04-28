import cv2
import numpy as np

def is_natural_area(image_path, threshold=0.95):
    image = cv2.imread(image_path)
    image_hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # 森林や草原の色相範囲を定義
    green_lower = np.array([30, 40, 40])
    green_upper = np.array([85, 255, 255])

    # 画像内の緑色の領域を抽出
    green_mask = cv2.inRange(image_hsv, green_lower, green_upper)
    green_area = cv2.countNonZero(green_mask)

    # 画像全体のピクセル数を計算
    total_pixels = image.shape[0] * image.shape[1]

    # 緑色の割合を計算
    green_ratio = green_area / total_pixels

    print("green_ratio:",green_ratio,"green_area:",green_area,"total_pixels:",total_pixels,image_path)


is_natural_area("mori1.png")
is_natural_area("mori2.png")
is_natural_area("tanbo1.png")
is_natural_area("michi1.png")
is_natural_area("ie1.png")



