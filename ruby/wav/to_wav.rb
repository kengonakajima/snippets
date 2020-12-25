require "wavefile"
include WaveFile

samples=[]
File.open(ARGV[0]).read.split("\n").each do |line|
  samples.push(line.to_i)
end

#print samples, "\n"


buffer=Buffer.new(samples,Format.new(:mono,:pcm_16,48000))

Writer.new(ARGV[1],Format.new(:mono,:pcm_16,48000)) do |writer|
  writer.write(buffer)
end

