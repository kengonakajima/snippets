#import <Foundation/Foundation.h>
#import <Vision/Vision.h>
#import <AppKit/AppKit.h>

int main(int argc, const char *argv[]) {
    @autoreleasepool {
        if (argc < 2) {
            fprintf(stderr, "Usage: %s <image_file>\n", argv[0]);
            return 1;
        }

        NSString *imagePath = [NSString stringWithUTF8String:argv[1]];
        NSURL *imageURL = [NSURL fileURLWithPath:imagePath];

        // 画像を読み込む
        NSImage *image = [[NSImage alloc] initWithContentsOfURL:imageURL];
        if (!image) {
            fprintf(stderr, "Error: Cannot load image: %s\n", argv[1]);
            return 1;
        }

        // NSImageからCGImageを取得
        CGImageRef cgImage = [image CGImageForProposedRect:NULL context:nil hints:nil];
        if (!cgImage) {
            fprintf(stderr, "Error: Cannot create CGImage\n");
            return 1;
        }

        // VNImageRequestHandlerを作成
        VNImageRequestHandler *handler = [[VNImageRequestHandler alloc] initWithCGImage:cgImage options:@{}];

        // テキスト認識リクエストを作成
        VNRecognizeTextRequest *request = [[VNRecognizeTextRequest alloc] initWithCompletionHandler:nil];
        request.recognitionLevel = VNRequestTextRecognitionLevelAccurate;
        request.recognitionLanguages = @[@"ja", @"en"];

        // リクエストを実行
        NSError *error = nil;
        [handler performRequests:@[request] error:&error];

        if (error) {
            fprintf(stderr, "Error: %s\n", [[error localizedDescription] UTF8String]);
            return 1;
        }

        // 結果を出力
        for (VNRecognizedTextObservation *observation in request.results) {
            VNRecognizedText *topCandidate = [[observation topCandidates:1] firstObject];
            if (topCandidate) {
                printf("%s\n", [topCandidate.string UTF8String]);
            }
        }
    }
    return 0;
}
