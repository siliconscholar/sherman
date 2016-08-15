#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

int SendFileToTrash(const char *filename)
{
    NSWorkspace *w = [NSWorkspace sharedWorkspace];
    NSString *path = [NSString stringWithUTF8String:filename];
    NSURL *url = [NSURL fileURLWithPath:path];
    NSArray *urls = @[url];
    [w recycleURLs: urls completionHandler:nil];

    return 0;
}
