aws polly synthesize-speech --region ap-northeast-1 --profile kengonakajima --output-format pcm --text "hello" --voice-id "Salli" hogespeech.pcm

ffmpeg -f s16le -ar 8k -ac 2 -i hogespeech.pcm out.wav
