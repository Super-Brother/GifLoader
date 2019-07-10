#define LOG_TAG "Bitmap_c"
#include "gif.h"
#include "common.h"
#include <android/bitmap.h>

int lockPixels(JNIEnv *env, jobject jbitmap, GifInfo *info, void **pixels) {
	AndroidBitmapInfo bitmapInfo;
	if (AndroidBitmap_getInfo(env, jbitmap, &bitmapInfo) == ANDROID_BITMAP_RESULT_SUCCESS)
		info->stride = bitmapInfo.width;
	else {
		throwException(env, RUNTIME_EXCEPTION_BARE, "Could not get bitmap info");
		return -2;
	}

	const int lockPixelsResult = AndroidBitmap_lockPixels(env, jbitmap, pixels);
	if (lockPixelsResult == ANDROID_BITMAP_RESULT_SUCCESS) {
		return 0;
	}

	char *message;
	switch (lockPixelsResult) {
		case ANDROID_BITMAP_RESULT_ALLOCATION_FAILED:
#ifdef DEBUG
			LOGE("bitmap lock allocation failed");
#endif
			return -1; //#122 workaround
		case ANDROID_BITMAP_RESULT_BAD_PARAMETER:
			message = "Lock pixels error, bad parameter";
			break;
		case ANDROID_BITMAP_RESULT_JNI_EXCEPTION:
			message = "Lock pixels error, JNI exception";
			break;
		default:
			message = "Lock pixels error";
	}
	throwException(env, RUNTIME_EXCEPTION_BARE, message);
	return -2;
}

void unlockPixels(JNIEnv *env, jobject jbitmap) {
    LOGE("5 开始解锁画布");
    LOGE("5 env地址%p   画布%p",env,jbitmap);
	const int unlockPixelsResult = AndroidBitmap_unlockPixels(env, jbitmap);
    LOGE("5 解锁画布 %d",unlockPixelsResult);
	if (unlockPixelsResult == ANDROID_BITMAP_RESULT_SUCCESS) {
		return;
	}
	char *message;
    LOGE("8 失败报错 %d",unlockPixelsResult);
	switch (unlockPixelsResult) {
		case ANDROID_BITMAP_RESULT_BAD_PARAMETER:
			message = "Unlock pixels error, bad parameter";
			break;
		case ANDROID_BITMAP_RESULT_JNI_EXCEPTION:
			message = "Unlock pixels error, JNI exception";
			break;
		default:
			message = "Unlock pixels error";
	}
	throwException(env, RUNTIME_EXCEPTION_BARE, message);
}

jlong renderFrame(JNIEnv *env, jclass __unused handleClass, jlong gifInfo, jobject jbitmap) {

	LOGE("%s","1----> 读取帧");
	GifInfo *info = (GifInfo *) (intptr_t) gifInfo;
	if (info == NULL){
        LOGE("%s","2----> 读取出错");
        return -1;
    }

	long renderStartTime = getRealTime();
    LOGE("=====2时长 %d",renderStartTime);
	void *pixels;
	if (lockPixels(env, jbitmap, info, &pixels) != 0) {
        LOGE("=====3填充失败");
		return 0;
	}
    LOGE("=====4填充成功 当前帧数 %d", info->currentIndex);
	DDGifSlurp(info, true, false);
	if (info->currentIndex == 0) {
		prepareCanvas(pixels, info);
	}
	const uint_fast32_t frameDuration = getBitmap(pixels, info);
    LOGE("=====4填充成功 帧率 %d", frameDuration);
	unlockPixels(env, jbitmap);
	return calculateInvalidationDelay(info, renderStartTime, frameDuration);
}
