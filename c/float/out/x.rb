500.times do |i|
  cmd=sprintf("ffmpeg -f rawvideo -pixel_format rgba -video_size 1024x1024 -i hoge_%03d.rgba hoge_%03d.png",i,i)
  system(cmd)
end

