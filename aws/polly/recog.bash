FILE=2.wav

aws --profile kengonakajima --region ap-northeast-1 s3 cp ${FILE} s3://audiodata2/${FILE}

aws --profile kengonakajima --region ap-northeast-1 transcribe delete-transcription-job --transcription-job-name newjob 
aws --profile kengonakajima --region ap-northeast-1 transcribe start-transcription-job --transcription-job-name newjob --language-code ja-JP --media MediaFileUri="s3://audiodata2/${FILE}"


aws --profile kengonakajima --region ap-northeast-1 transcribe get-transcription-job --transcription-job-name newjob

sleep 30

curl $(aws --profile kengonakajima --region ap-northeast-1 transcribe get-transcription-job --transcription-job-name newjob | jq -r ".TranscriptionJob.Transcript.TranscriptFileUri") | jq > got.json

cat got.json

