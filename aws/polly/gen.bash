aws polly synthesize-speech --region ap-northeast-1 --profile kengonakajima --output-format pcm --text "helloにほんごどんな漢字" --voice-id "Mizuki" hogespeech.pcm

ffmpeg -f s16le -ar 8k -ac 2 -i hogespeech.pcm out.wav
