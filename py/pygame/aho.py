import pygame, random, time

pygame.init()
screen = pygame.display.set_mode((200,100))

screen.fill((0,255,0))
pygame.display.flip()

time.sleep(2)
c = 0
while 1:
	c += 1
	if( ( c % 2) == 0 ):
		screen.fill((0,255,0))
	else:
		screen.fill((0,0,255))
	i = random.randint(200,100)
	j = random.randint(200,100)
	k = random.randint(200,100)
	l = random.randint(200,100)	
	pygame.draw.line(screen,(255,255,0),(i,j),(k,l),2)
	pygame.display.flip()
	print "hello %d %d %d %d" % (i,j,k,l)
	time.sleep(0.5)

