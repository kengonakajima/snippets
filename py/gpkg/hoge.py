import geopandas as gpd
from shapely.geometry import box, Point
import numpy as np

# 指定された座標
point1 = (36.63445727473631, 137.1553976302989)
point2 = (36.59863364675463, 137.21427748013414)

# 矩形の作成
min_x, min_y = np.min([point1, point2], axis=0)
max_x, max_y = np.max([point1, point2], axis=0)
rectangle = box(min_x, min_y, max_x, max_y)

# GeoDataFrameの作成
gdf = gpd.GeoDataFrame({'name': ['Rectangle'], 'geometry': [rectangle]}, crs="EPSG:4326")

# 角の点を追加
corners = [
    Point(min_x, min_y),
    Point(min_x, max_y),
    Point(max_x, min_y),
    Point(max_x, max_y)
]
corner_names = ['Southwest', 'Northwest', 'Southeast', 'Northeast']
corner_gdf = gpd.GeoDataFrame({
    'name': corner_names,
    'geometry': corners
}, crs="EPSG:4326")

# GeoPackageファイルの作成
output_file = "rectangle_data.gpkg"

# 各レイヤーをGeoPackageに書き込む
gdf.to_file(output_file, layer='rectangle', driver="GPKG")
corner_gdf.to_file(output_file, layer='corners', driver="GPKG")

print(f"GeoPackage file '{output_file}' has been created with rectangle and corner points.")

# 作成したデータの内容を表示
print("\nRectangle Data:")
print(gdf)
print("\nCorner Points Data:")
print(corner_gdf)
