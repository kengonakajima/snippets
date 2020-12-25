require "wavefile"
include WaveFile

samples=[]
Reader.new("echoing_quicktime.wav").each_buffer do |buf|
  buf.samples.each do |sample|
    samples.push(sample)
  end
end

samples.each do |s|
  print s, "\n"
end

