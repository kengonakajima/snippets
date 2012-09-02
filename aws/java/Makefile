AWS=./aws-java-sdk-1.3.18.jar
all:
#	javac -classpath $(AWS)  ArchiveUploadHighLevel.java  2>&1| ~/bin/nkf -w
	javac -classpath $(AWS)  ArchiveUploadHighLevel.java 

run:
#	java -classpath $(AWS) ArchiveUploadHighLevel

	java  -cp $(AWS) ArchiveUploadHighLevel 


cps:
	javac CPs.java

cp:
	java CPs
