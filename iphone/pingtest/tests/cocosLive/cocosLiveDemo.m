//
// cocos live demo
// a cocos2d example
// http://code.google.com/p/cocos2d-iphone
//

//
// To view these scores online, go here:
// http://www.cocoslive.net/game-scores?gamename=DemoGame%202
//

//
// CocosLive client can be used within cocos2d
// or without it.
// This example shows how to use it without cocos2d
//
// To use cocos live in your game you need to link against these
// static libraries (or include the these sources in your game):
//   * cocosLive
//   * TouchJSON


#import <UIKit/UIKit.h>
#include <sys/time.h>


// cocos live import
//   This is the only import you need to use "pure" cocosLive
#import "cocoslive.h"

// CCLOG and CCRANDOM_0_1
#import "ccMacros.h"

// local import
#import "cocosLiveDemo.h"

@interface AppController (Private)
-(void) testRequest;
-(void) testPost;
@end

// CLASS IMPLEMENTATIONS
@implementation AppController

@synthesize window, mainView;
@synthesize globalScores;

-(void) initRandom {
	struct timeval t;
	gettimeofday(&t, nil);
	unsigned int i;
	i = t.tv_sec;
	i += t.tv_usec;
	srandom(i);	
}

-(int) getRandomWithMax:(int)max 
{
	return CCRANDOM_0_1() * max;
}

-(void) requestScore
{
	NSLog(@"Requesting scores...");

	ScoreServerRequest *request = [[ScoreServerRequest alloc] initWithGameName:@"DemoGame 2" delegate:self];
	
	NSString *cat = @"easy";
	
	switch( category ) {
		case kCategoryEasy:
			cat = @"easy";
			break;
		case kCategoryMedium:
			cat = @"medium";
			break;
		case kCategoryHard:
			cat = @"hard";
			break;
	}

	// The only supported flag as of v0.2 is kQueryFlagByCountry and kQueryFlagByDevice
	tQueryFlags flags = kQueryFlagIgnore;
	if( world == kCountry )
		flags = kQueryFlagByCountry;
	else if(world == kDevice )
		flags = kQueryFlagByDevice;

	// request All time Scores: the only supported version as of v0.2
	// request best 15 scores (limit:15, offset:0)
	[request requestScores:kQueryAllTime limit:15 offset:0 flags:flags category:cat];

	// Release. It won't be freed from memory until the connection fails or suceeds
	[request release];
}

-(void) postScore:(NSInteger)cate
{
	NSLog(@"Posting Score");

	// Create que "post" object for the game "DemoGame 2"
	// The gameKey is the secret key that is generated when you create you game in cocos live.
	// This secret key is used to prevent spoofing the high scores
	ScoreServerPost *server = [[ScoreServerPost alloc] initWithGameName:@"DemoGame 2" gameKey:@"1ad0ba5afd763c6bdaa2429cfc99b3be" delegate:self];

	NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithCapacity:2];

	// Name at random
	NSArray *names = [NSArray arrayWithObjects:@"いすの上に猫がいる", @"Carrière", @"Iñaqui", @"Clemensstraße München ", @"有一只猫在椅子上", nil];
	NSString *name = [names objectAtIndex: CCRANDOM_0_1() * 5];

	// cc_ files are predefined cocoslive fields.
	// set score
	[dict setObject: [NSNumber numberWithInt: [self getRandomWithMax:20000] ] forKey:@"cc_score"];
	// set playername
	[dict setObject:name forKey:@"cc_playername"];

	// usr_ are fields that can be modified.
	// set speed
	[dict setObject: [NSNumber numberWithInt: [self getRandomWithMax:2000] ] forKey:@"usr_speed"];
	// set angle
	[dict setObject: [NSNumber numberWithInt:[self getRandomWithMax:360] ] forKey:@"usr_angle"];


	// cc_ are fields that cannot be modified. cocos fields
	// set category... it can be "easy", "medium", whatever you want.
	NSString *cat = @"easy";
	switch(cate) {
		case 0:
			cat = @"easy";
			break;
		case 1:
			cat = @"medium";
			break;
		case 2:
		default:
			cat = @"hard";
			break;
	}
	
	[dict setObject:cat forKey:@"cc_category"];
	
	NSLog(@"Sending data: %@", dict);

	// You can add a new score to the database
//	[server sendScore:dict];
	
	// Or you can "update" your score instead of adding a new one.
	// To do that, cc_playername must exists.
	// The score will be udpated only if it is better than the previous one
	[server updateScore:dict];
	
	// Release. It won't be freed from memory until the connection fails or suceeds
	[server release];
}

#pragma mark -
#pragma mark ScorePost Delegate

-(void) scorePostOk: (id) sender
{
	NSLog(@"score post OK");
}

-(void) scorePostFail: (id) sender
{
	NSString *message = nil;
	tPostStatus status = [sender postStatus];
	if( status == kPostStatusPostFailed )
		message = @"Cannot post the score to the server. Retry";
	else if( status == kPostStatusConnectionFailed )
		message = @"Internet connection not available. Enable wi-fi / 3g to post your scores to the server";
	
	NSLog(@"%@", message);

	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Score Post Failed" message:message delegate:nil cancelButtonTitle:nil otherButtonTitles:@"OK", nil];	
	alert.tag = 0;
	[alert show];
	[alert release];		
}

#pragma mark -
#pragma mark ScoreRequest Delegate

-(void) scoreRequestOk: (id) sender
{
	NSLog(@"score request OK");	
	NSArray *scores = [sender parseScores];	
	NSMutableArray *mutable = [NSMutableArray arrayWithArray:scores];
	
	// use the property (retain is needed)
	self.globalScores = mutable;
		
	[myTableView reloadData];
}

-(void) scoreRequestFail: (id) sender
{
	NSLog(@"score request fail");

	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Score Request Failed" message:@"Internet connection not available, cannot view world scores" delegate:nil cancelButtonTitle:nil otherButtonTitles:@"OK", nil];	
	alert.tag = 0;
	[alert show];
	[alert release];	
}


#pragma mark -
#pragma mark Button Delegate
-(void) buttonCallback:(id) sender
{
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Post Score" message:@"A random score will be posted. Select category"
												   delegate:self cancelButtonTitle:nil otherButtonTitles:@"easy", @"medium", @"hard", nil];
	alert.tag = 1;
	[alert show];
	[alert release];	
}

#pragma mark -
#pragma mark Segment Delegate
- (void)segmentAction:(id)sender
{	
	int idx = [sender selectedSegmentIndex];
	// category 
	if( [sender tag] == 0 ) {
		// 0: easy
		// 1: med
		// 2: hard
		category = idx;
	} else if( [sender tag] == 1 ) {
		// 0 = scores world wide
		// 1 = scores by country
		world = idx;
	}

	[self requestScore];
}

#pragma mark -
#pragma mark - UIAlertViewDelegate

- (void)alertView:(UIAlertView *)view clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if( [view tag] == 1 )
		[self postScore:buttonIndex];
}

#pragma mark -
#pragma mark Application Delegate

-(void) applicationDidFinishLaunching:(UIApplication*)application
{
//	[self initRandom];
//	for( int i=0; i< 15;i++)
//		[self testPost];
//	[self testRequest];


//	[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeLeft animated:NO];

	self.globalScores = [[NSMutableArray alloc] initWithCapacity:50];
	category = kCategoryEasy;
	world = kAll;
	
	[self initRandom];

	[window makeKeyAndVisible];
	
	[self requestScore];
}

#pragma mark -
#pragma mark Init
-(void) dealloc
{
	[globalScores release];
	[mainView release];
	[window release];
	[super dealloc];
}
@end
