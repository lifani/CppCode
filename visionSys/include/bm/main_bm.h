#ifndef __MAIN_BM__
#define __MAIN_BM__

#ifdef __cplusplus
extern "C" {
#endif

extern const int sizeOutBuf;   /* the size of data sending to CAN */
#define IMAGE_TEST_W    320
#define IMAGE_TEST_H    240

void InitBm();

void ReleaseBm();

void RunBm(
    const char* plImage, /* input image size is IMAGE_TEST_W*IMAGE_TEST_H */
    const char* prImage, 
    char *obuf      /* output buffer size is sizeOutBuf */
    );
#ifdef __cplusplus
}
#endif

#endif
