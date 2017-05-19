//
//  AppDelegate.m
//  RingBuffer
//
//  Created by Сергей Сейтов on 12.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#import "AppDelegate.h"
#include "RingBuffer.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
/*
    RingBuffer buffer;
    Element frame[4];
    buffer.write(frame, 3); // +3
    buffer.log();
    buffer.write(frame, 3);
    
    buffer.read(frame, 2);  // -2 = 1
    buffer.log();
    
    buffer.write(frame, 3); // +3 = 4
    buffer.log();
    
    buffer.write(frame, 3);
    buffer.write(frame, 1); // +1 = 5
    buffer.log();
    
    buffer.read(frame, 3);
    buffer.read(frame, 3); // -3 = 2
    buffer.log();
    buffer.write(frame, 3); // +3 = 5
*/    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end
