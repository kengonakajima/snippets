AWS=/Users/ringo/aws/aws-java-sdk-1.3.18/lib/aws-java-sdk-1.3.18.jar
all:
	javac -classpath $(AWS)  ArchiveUploadHighLevel.java  2>&1| ~/bin/nkf -w

run:
#	java -classpath $(AWS) ArchiveUploadHighLevel
#	java -cp $(AWS) ArchiveUploadHighLevel
	java  -cp . ArchiveUploadHighLevel 


cps:
	javac CPs.java

cp:
	java CPs
