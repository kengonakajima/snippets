const m3u8=require("m3u8-parser");
console.log("m3u8",m3u8);


var manifest = [
  '#EXTM3U',
  '#EXT-X-VERSION:3',
  '#EXT-X-TARGETDURATION:6',
  '#EXT-X-MEDIA-SEQUENCE:0',
  '#EXT-X-DISCONTINUITY-SEQUENCE:0',
  '#EXTINF:6,',
  '0.ts',
  '#EXTINF:6,',
  '1.ts',
  '#EXTINF:6,',
  '2.ts',
  '#EXT-X-ENDLIST'
].join('\n');

var manifest2= [
  '#EXTM3U',
  '#EXT-X-VERSION:4',
  '#EXT-X-MEDIA:TYPE=AUDIO,GROUP-ID="audio",NAME="aac_eng_2_128043_2_1",LANGUAGE="eng",DEFAULT=YES,AUTOSELECT=YES,URI="QualityLevels(128043)/Manifest(aac_eng_2_128043_2_1,format=m3u8-aapl)"',
  '#EXT-X-STREAM-INF:BANDWIDTH=4610042,RESOLUTION=1280x720,CODECS="avc1.640020,mp4a.40.2",AUDIO="audio"',
  'QualityLevels(4366794)/Manifest(video,format=m3u8-aapl)',
  '#EXT-X-I-FRAME-STREAM-INF:BANDWIDTH=4610042,RESOLUTION=1280x720,CODECS="avc1.640020",URI="QualityLevels(4366794)/Manifest(video,format=m3u8-aapl,type=keyframes)"'
].join('\n');
var parser = new m3u8.Parser();

parser.push(manifest2);
parser.end();

var parsedManifest = parser.manifest;

console.log("PPPP",parsedManifest);
