//
// Atlas Demo
// a cocos2d example
// http://code.google.com/p/cocos2d-iphone
//

// cocos import
#import "cocos2d.h"

// local import
#import "AtlasSpriteTest.h"

static int sceneIdx=-1;
static NSString *transitions[] = {
			@"Atlas1",
			@"Atlas2",
			@"Atlas3",
			@"Atlas4",
			@"Atlas5",
			@"Atlas6",
};

enum {
	kTagTileMap = 1,
	kTagSpriteManager = 1,
	kTagAnimation1 = 1,
};

enum {
	kTagSprite1,
	kTagSprite2,
	kTagSprite3,
	kTagSprite4,
	kTagSprite5,
	kTagSprite6,
	kTagSprite7,
	kTagSprite8,
};

Class nextAction()
{
	
	sceneIdx++;
	sceneIdx = sceneIdx % ( sizeof(transitions) / sizeof(transitions[0]) );
	NSString *r = transitions[sceneIdx];
	Class c = NSClassFromString(r);
	return c;
}

Class backAction()
{
	sceneIdx--;
	int total = ( sizeof(transitions) / sizeof(transitions[0]) );
	if( sceneIdx < 0 )
		sceneIdx += total;	
	
	NSString *r = transitions[sceneIdx];
	Class c = NSClassFromString(r);
	return c;
}

Class restartAction()
{
	NSString *r = transitions[sceneIdx];
	Class c = NSClassFromString(r);
	return c;
}


@implementation AtlasDemo
-(id) init
{
	[super init];


	CGSize s = [[Director sharedDirector] winSize];
		
	Label* label = [Label labelWithString:[self title] fontName:@"Arial" fontSize:32];
	[self addChild: label z:1];
	[label setPosition: ccp(s.width/2, s.height-50)];
	
	MenuItemImage *item1 = [MenuItemImage itemFromNormalImage:@"b1.png" selectedImage:@"b2.png" target:self selector:@selector(backCallback:)];
	MenuItemImage *item2 = [MenuItemImage itemFromNormalImage:@"r1.png" selectedImage:@"r2.png" target:self selector:@selector(restartCallback:)];
	MenuItemImage *item3 = [MenuItemImage itemFromNormalImage:@"f1.png" selectedImage:@"f2.png" target:self selector:@selector(nextCallback:)];
	
	Menu *menu = [Menu menuWithItems:item1, item2, item3, nil];
	
	menu.position = CGPointZero;
	item1.position = ccp( s.width/2 - 100,30);
	item2.position = ccp( s.width/2, 30);
	item3.position = ccp( s.width/2 + 100,30);
	[self addChild: menu z:1];	

	return self;
}

-(void) dealloc
{
	[super dealloc];
}

-(void) restartCallback: (id) sender
{
	Scene *s = [Scene node];
	[s addChild: [restartAction() node]];
	[[Director sharedDirector] replaceScene: s];
}

-(void) nextCallback: (id) sender
{
	Scene *s = [Scene node];
	[s addChild: [nextAction() node]];
	[[Director sharedDirector] replaceScene: s];
}

-(void) backCallback: (id) sender
{
	Scene *s = [Scene node];
	[s addChild: [backAction() node]];
	[[Director sharedDirector] replaceScene: s];
}

-(NSString*) title
{
	return @"No title";
}
@end

#pragma mark Example Atlas 1

@implementation Atlas1

-(id) init
{
	if( (self=[super init]) ) {
		
		isTouchEnabled = YES;

		AtlasSpriteManager *mgr = [AtlasSpriteManager spriteManagerWithFile:@"grossini_dance_atlas.png" capacity:50];
		[self addChild:mgr z:0 tag:kTagSpriteManager];
		
		[self addNewSpriteWithCoords:ccp(480/2, 320/2)];
		
	}	
	return self;
}

-(void) addNewSpriteWithCoords:(CGPoint)p
{
	AtlasSpriteManager *mgr = (AtlasSpriteManager*) [self getChildByTag:kTagSpriteManager];
	
	int idx = CCRANDOM_0_1() * 1400 / 100;
	int x = (idx%5) * 85;
	int y = (idx/5) * 121;
	

	AtlasSprite *sprite = [AtlasSprite spriteWithRect:CGRectMake(x,y,85,121) spriteManager:mgr];
	[mgr addChild:sprite];

	sprite.position = ccp( p.x, p.y);

	id action;
	float r = CCRANDOM_0_1();
	
	if( r < 0.33 )
		action = [ScaleBy actionWithDuration:3 scale:2];
	else if(r < 0.66)
		action = [RotateBy actionWithDuration:3 angle:360];
	else
		action = [Blink actionWithDuration:1 blinks:3];
	id action_back = [action reverse];
	id seq = [Sequence actions:action, action_back, nil];
	
	[sprite runAction: [RepeatForever actionWithAction:seq]];
}

- (BOOL)ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	for( UITouch *touch in touches ) {
		CGPoint location = [touch locationInView: [touch view]];
		
		location = [[Director sharedDirector] convertCoordinate: location];
		
		[self addNewSpriteWithCoords: location];
	}
	return kEventHandled;
}

-(NSString *) title
{
	return @"AtlasSprite (tap screen)";
}
@end

#pragma mark Example Atlas 2

@implementation Atlas2

-(id) init
{
	if( (self=[super init]) ) {
		
		[Texture2D saveTexParameters];
		[Texture2D setAliasTexParameters];
		AtlasSpriteManager *mgr = [AtlasSpriteManager spriteManagerWithFile:@"grossini_dance_atlas.png" capacity:50];
		[self addChild:mgr z:0 tag:kTagSpriteManager];
		
		[Texture2D restoreTexParameters];
		
		AtlasSprite *sprite = [AtlasSprite spriteWithRect:CGRectMake(0, 0, 85, 121) spriteManager: mgr];
		AtlasSprite *sprite2 = [AtlasSprite spriteWithRect:CGRectMake(0, 0, 85, 121) spriteManager: mgr];
		AtlasSprite *sprite3 = [AtlasSprite spriteWithRect:CGRectMake(0, 0, 85, 121) spriteManager: mgr];
		
		AtlasAnimation *animation = [AtlasAnimation animationWithName:@"dance" delay:0.2f];
		for(int i=0;i<14;i++) {
			int x= i % 5;
			int y= i / 5;
			[animation addFrameWithRect: CGRectMake(x*85, y*121, 85, 121) ];

		}
		
		[mgr addChild:sprite];
		[mgr addChild:sprite2];
		[mgr addChild:sprite3];
		
		CGSize s = [[Director sharedDirector] winSize];
		sprite.position = ccp( s.width /2, s.height/2);
		sprite2.position = ccp( s.width /2 - 100, s.height/2);
		sprite3.position = ccp( s.width /2 + 100, s.height/2);
		
		id action = [Animate actionWithAnimation: animation];
		id action2 = [[action copy] autorelease];
		id action3 = [[action copy] autorelease];
		
		sprite.scale = 0.5f;
		sprite2.scale = 1.0f;
		sprite3.scale = 1.5f;
		
		[sprite runAction:action];
		[sprite2 runAction:action2];
		[sprite3 runAction:action3];
		
		
	}	
	return self;
}

-(NSString *) title
{
	return @"AtlasSprite: Animation";
}
@end

#pragma mark Example Atlas 3

@implementation Atlas3

-(id) init
{
	if( (self=[super init]) ) {
		
		// small capacity. Testing resizing.
		// Don't use capacity=1 in your real game. It is expensive to resize the capacity
		AtlasSpriteManager *mgr = [AtlasSpriteManager spriteManagerWithFile:@"grossini_dance_atlas.png" capacity:1];
		[self addChild:mgr z:0 tag:kTagSpriteManager];		
		
		AtlasSprite *sprite1 = [AtlasSprite spriteWithRect:CGRectMake(85*0, 121*1, 85, 121) spriteManager: mgr];
		AtlasSprite *sprite2 = [AtlasSprite spriteWithRect:CGRectMake(85*1, 121*1, 85, 121) spriteManager: mgr];
		AtlasSprite *sprite3 = [AtlasSprite spriteWithRect:CGRectMake(85*2, 121*1, 85, 121) spriteManager: mgr];
		AtlasSprite *sprite4 = [AtlasSprite spriteWithRect:CGRectMake(85*3, 121*1, 85, 121) spriteManager: mgr];
		AtlasSprite *sprite5 = [AtlasSprite spriteWithRect:CGRectMake(85*0, 121*1, 85, 121) spriteManager: mgr];
		AtlasSprite *sprite6 = [AtlasSprite spriteWithRect:CGRectMake(85*1, 121*1, 85, 121) spriteManager: mgr];
		AtlasSprite *sprite7 = [AtlasSprite spriteWithRect:CGRectMake(85*2, 121*1, 85, 121) spriteManager: mgr];
		AtlasSprite *sprite8 = [AtlasSprite spriteWithRect:CGRectMake(85*3, 121*1, 85, 121) spriteManager: mgr];
		
		CGSize s = [[Director sharedDirector] winSize];
		sprite1.position = ccp( (s.width/5)*1, (s.height/3)*1);
		sprite2.position = ccp( (s.width/5)*2, (s.height/3)*1);
		sprite3.position = ccp( (s.width/5)*3, (s.height/3)*1);
		sprite4.position = ccp( (s.width/5)*4, (s.height/3)*1);
		sprite5.position = ccp( (s.width/5)*1, (s.height/3)*2);
		sprite6.position = ccp( (s.width/5)*2, (s.height/3)*2);
		sprite7.position = ccp( (s.width/5)*3, (s.height/3)*2);
		sprite8.position = ccp( (s.width/5)*4, (s.height/3)*2);

		id action = [FadeIn actionWithDuration:2];
		id action_back = [action reverse];
		id fade = [RepeatForever actionWithAction: [Sequence actions: action, action_back, nil]];

		id tintred = [TintBy actionWithDuration:2 red:0 green:-255 blue:-255];
		id tintred_back = [tintred reverse];
		id red = [RepeatForever actionWithAction: [Sequence actions: tintred, tintred_back, nil]];

		id tintgreen = [TintBy actionWithDuration:2 red:-255 green:0 blue:-255];
		id tintgreen_back = [tintgreen reverse];
		id green = [RepeatForever actionWithAction: [Sequence actions: tintgreen, tintgreen_back, nil]];

		id tintblue = [TintBy actionWithDuration:2 red:-255 green:-255 blue:0];
		id tintblue_back = [tintblue reverse];
		id blue = [RepeatForever actionWithAction: [Sequence actions: tintblue, tintblue_back, nil]];
		
		
		[sprite5 runAction:red];
		[sprite6 runAction:green];
		[sprite7 runAction:blue];
		[sprite8 runAction:fade];
		
		// late add: test dirtyColor and dirtyPosition
		[mgr addChild:sprite1 z:0 tag:kTagSprite1];
		[mgr addChild:sprite2 z:0 tag:kTagSprite2];
		[mgr addChild:sprite3 z:0 tag:kTagSprite3];
		[mgr addChild:sprite4 z:0 tag:kTagSprite4];
		[mgr addChild:sprite5 z:0 tag:kTagSprite5];
		[mgr addChild:sprite6 z:0 tag:kTagSprite6];
		[mgr addChild:sprite7 z:0 tag:kTagSprite7];
		[mgr addChild:sprite8 z:0 tag:kTagSprite8];
		
		
		[self schedule:@selector(removeAndAddSprite:) interval:2];
		
	}	
	return self;
}

// this function test if remove and add works as expected:
//   color array and vertex array should be reindexed
-(void) removeAndAddSprite:(ccTime) dt
{
	id mgr = [self getChildByTag:kTagSpriteManager];
	id sprite = [mgr getChildByTag:kTagSprite5];
	
	[sprite retain];

	[mgr removeChild:sprite cleanup:NO];
	[mgr addChild:sprite z:0 tag:kTagSprite5];
	
	[sprite release];
}

-(NSString *) title
{
	return @"AtlasSprite: Color & Opacity";
}
@end

#pragma mark Example Atlas 4

@implementation Atlas4

-(id) init
{
	if( (self=[super init]) ) {
		
		dir = 1;
		
		// small capacity. Testing resizing.
		// Don't use capacity=1 in your real game. It is expensive to resize the capacity
		AtlasSpriteManager *mgr = [AtlasSpriteManager spriteManagerWithFile:@"grossini_dance_atlas.png" capacity:1];
		[self addChild:mgr z:0 tag:kTagSpriteManager];		
		
		CGSize s = [[Director sharedDirector] winSize];

		for(int i=0;i<5;i++) {
			AtlasSprite *sprite = [AtlasSprite spriteWithRect:CGRectMake(85*0, 121*1, 85, 121) spriteManager: mgr];
			sprite.position = ccp( 50 + i*40, s.height/2);
			[mgr addChild:sprite z:i];
		}
		
		for(int i=5;i<10;i++) {
			AtlasSprite *sprite = [AtlasSprite spriteWithRect:CGRectMake(85*1, 121*0, 85, 121) spriteManager: mgr];
			sprite.position = ccp( 50 + i*40, s.height/2);
			[mgr addChild:sprite z:14-i];
		}
		
		AtlasSprite *sprite = [AtlasSprite spriteWithRect:CGRectMake(85*3, 121*0, 85, 121) spriteManager: mgr];
		[mgr addChild:sprite z:-1 tag:kTagSprite1];
		sprite.position = ccp(s.width/2, s.height/2 - 20);
		sprite.scaleX = 6;
		[sprite setRGB:255:0:0];
		
		[self schedule:@selector(reorderSprite:) interval:1];		
	}	
	return self;
}

-(void) reorderSprite:(ccTime) dt
{
	id mgr = [self getChildByTag:kTagSpriteManager];
	id sprite = [mgr getChildByTag:kTagSprite1];
	
	int z = [sprite zOrder];
	
	if( z < -1 )
		dir = 1;
	if( z > 10 )
		dir = -1;
	
	z += dir * 3;

	[mgr reorderChild:sprite z:z];
	
}

-(NSString *) title
{
	return @"AtlasSprite: Z order";
}
@end

#pragma mark Example Atlas 5

@implementation Atlas5

-(id) init
{
	if( (self=[super init]) ) {

		// small capacity. Testing resizing.
		// Don't use capacity=1 in your real game. It is expensive to resize the capacity
		AtlasSpriteManager *mgr = [AtlasSpriteManager spriteManagerWithFile:@"grossini_dance_atlas.png" capacity:1];
		[self addChild:mgr z:0 tag:kTagSpriteManager];		
		
		CGSize s = [[Director sharedDirector] winSize];
		
		
		id rotate = [RotateBy actionWithDuration:10 angle:360];
		id action = [RepeatForever actionWithAction:rotate];
		for(int i=0;i<3;i++) {
			AtlasSprite *sprite = [AtlasSprite spriteWithRect:CGRectMake(85*i, 121*1, 85, 121) spriteManager: mgr];
			sprite.position = ccp( 90 + i*150, s.height/2);

			
			Sprite *point = [Sprite spriteWithFile:@"r1.png"];
			point.scale = 0.25f;
			point.position = sprite.position;
			[self addChild:point z:1];

			switch(i) {
				case 0:
					sprite.transformAnchor = CGPointZero;
					break;
				case 1:
					break;
				case 2:
					sprite.transformAnchor = ccp(85,121);
					break;
			}

			point.position = sprite.position;
			
			id copy = [[action copy] autorelease];
			[sprite runAction:copy];
			[mgr addChild:sprite z:i];
		}		
	}	
	return self;
}

-(NSString *) title
{
	return @"AtlasSprite: anchor point";
}
@end

#pragma mark Example Atlas 6

@implementation Atlas6
-(id) init
{
	if( (self=[super init]) ) {
		
		// small capacity. Testing resizing
		// Don't use capacity=1 in your real game. It is expensive to resize the capacity
		AtlasSpriteManager *mgr = [AtlasSpriteManager spriteManagerWithFile:@"grossini_dance_atlas.png" capacity:1];
		[self addChild:mgr z:0 tag:kTagSpriteManager];

		CGSize s = [[Director sharedDirector] winSize];

		mgr.relativeTransformAnchor = NO;
		mgr.transformAnchor = ccp( s.width / 2.0f, s.height / 2.0f );		
		
		
		// AtlasSprite actions
		id rotate = [RotateBy actionWithDuration:5 angle:360];
		id action = [RepeatForever actionWithAction:rotate];

		// AtlasSpriteManager actions
		id rotate_back = [rotate reverse];
		id rotate_seq = [Sequence actions:rotate, rotate_back, nil];
		id rotate_forever = [RepeatForever actionWithAction:rotate_seq];
		
		id scale = [ScaleBy actionWithDuration:5 scale:1.5f];
		id scale_back = [scale reverse];
		id scale_seq = [Sequence actions: scale, scale_back, nil];
		id scale_forever = [RepeatForever actionWithAction:scale_seq];


		for(int i=0;i<3;i++) {
			AtlasSprite *sprite = [AtlasSprite spriteWithRect:CGRectMake(85*i, 121*1, 85, 121) spriteManager: mgr];
			sprite.position = ccp( 90 + i*150, s.height/2);

			[sprite runAction: [[action copy] autorelease]];
			[mgr addChild:sprite z:i];
		}
		
		[mgr runAction: scale_forever];
		[mgr runAction: rotate_forever];
	}	
	return self;
}
-(NSString*) title
{
	return @"AtlasSpriteManager transformation";
}
@end


#pragma mark AppDelegate

// CLASS IMPLEMENTATIONS
@implementation AppController

- (void) applicationDidFinishLaunching:(UIApplication*)application
{
	// Init the window
	window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	
	// cocos2d will inherit these values
	[window setUserInteractionEnabled:YES];	
	[window setMultipleTouchEnabled:NO];
	
	// must be called before any othe call to the director
//	[Director useFastDirector];
	
	// before creating any layer, set the landscape mode
	[[Director sharedDirector] setLandscape: YES];
	[[Director sharedDirector] setAnimationInterval:1.0/60];
	[[Director sharedDirector] setDisplayFPS:YES];

	// create an openGL view inside a window
	[[Director sharedDirector] attachInView:window];	
	[window makeKeyAndVisible];		
	
	Scene *scene = [Scene node];
	[scene addChild: [nextAction() node]];
			 
	[[Director sharedDirector] runWithScene: scene];
}

// getting a call, pause the game
-(void) applicationWillResignActive:(UIApplication *)application
{
	[[Director sharedDirector] pause];
}

// call got rejected
-(void) applicationDidBecomeActive:(UIApplication *)application
{
	[[Director sharedDirector] resume];
}

// purge memroy
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
	[[TextureMgr sharedTextureMgr] removeAllTextures];
}

// next delta time will be zero
-(void) applicationSignificantTimeChange:(UIApplication *)application
{
	[[Director sharedDirector] setNextDeltaTimeZero:YES];
}

- (void) dealloc
{
	[window release];
	[super dealloc];
}
@end
