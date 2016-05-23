#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <AL/alut.h>


int main(int argc, char *argv[]) {
    ALuint bgmbuf, bgmsource;
    ALuint sebuf, sesource;
    
    alutInit(&argc,argv);    

    bgmbuf = alutCreateBufferFromFile("mono16.wav");
    alGenSources(1,&bgmsource);
    alSourcei(bgmsource, AL_BUFFER, bgmbuf);
    alSourcei(bgmsource, AL_LOOPING, AL_TRUE );
    alSourcePlay(bgmsource);

    sebuf = alutCreateBufferFromFile("blobloblll.wav");
    alGenSources(1,&sesource);
    alSourcei(sesource, AL_BUFFER, sebuf);
    alSourcei(sesource, AL_LOOPING, AL_FALSE );

    

    while(true) {
        usleep(10*1000);
        fprintf(stderr, ".");
        int r = random() % 20;
        if(r == 0 ) {
            fprintf(stderr, "SE\n");
            alSourcePlay(sesource);
        }
    }

    alutExit();
    return 0;
}
