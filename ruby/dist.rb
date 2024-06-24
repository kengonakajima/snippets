# point to distance
def p2d(px, py, x0, y0, x1, y1)
  # 線分の長さの2乗を計算
  l2 = (x1 - x0)**2 + (y1 - y0)**2
  
  # 線分の長さが0の場合（つまり点の場合）
  if l2 == 0
    return Math.sqrt((px - x0)**2 + (py - y0)**2)
  end
  
  # 線分上の最近点のパラメータ t を計算
  t = ((px - x0) * (x1 - x0) + (py - y0) * (y1 - y0)) / l2
  
  # t が [0, 1] の範囲外なら、線分の範囲外なので -1 を返す
  return -1 if t < 0 || t > 1
  
  # 線分上の最近点の座標を計算
  nx = x0 + t * (x1 - x0)
  ny = y0 + t * (y1 - y0)
  
  # 点 P と最近点との距離を計算して返す
  Math.sqrt((px - nx)**2 + (py - ny)**2)
end

k=p2d(1,1, 0,3, 3,0 )
print k,"\n"

k=p2d(1,1, 3,0, 5,0 )
print k,"\n"
