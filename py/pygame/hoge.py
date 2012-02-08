import pygame, random, time

pygame.init()
screen = pygame.display.set_mode((200,100))
screen.fill((0,200,0))
#pygame.display.flip()
while 1:
	screen.fill((0,random.randint(100,200),0))	
	pygame.draw.line(screen,(255,255,0),(random.randint(10,100),10),(20,20),5)
	pygame.display.flip()
	time.sleep(0.5)