#ifndef __LIB_IMAGE_H__
#define __LIB_IMAGE_H__

#include <stdio.h>

namespace wa {

class  Image
{
public:
    enum ImageType {
        IMAGETYPE_GRAY = 0,
        IMAGETYPE_RGB,
        IMAGETYPE_BGR,
        IMAGETYPE_RGBA,
        IMAGETYPE_BGRA,
        IMAGETYPE_NV12, // Pakced(Semi-planar) YUV420SP format, YUV:YYUVUV
        IMAGETYPE_NV21, // Packed(Semi-planar) YUV420SP format, YVU:YYVUVU
        IMAGETYPE_I420, // Planar YUV420P format, YUV:YY.UU.VV
        IMAGETYPE_YV12, // Planar YUV420P format, YVU:YY.VV.UU
        IMAGETYPE_YUYV, // Packed YUV422, alias YUY2
        IMAGETYPE_YVYU, // Packed YUV422
        IMAGETYPE_UYVY, // Packed YUV422
        IMAGETYPE_VYUY, // Packed YUV422
        IMAGETYPE_MAX = IMAGETYPE_VYUY + 1
    };

    enum DataType {
        DT_8U,
        DT_32F,
        DT_16U,
    };

    /*
     * anticlockwise(逆时针方向)
     *
     * The rotation relative to dual camera module(total:four modules).
     * ex:
     *  camera module       default direction of image
     *    right(main)
     * |`````````````|        |`````````````|
     * |   |`| |`|   |        |      |      |
     * |   |_| |`|   |        |    \ | /    |
     * |             |        |     \|/     |
     * |             |        |    /`_`\    |
     * |             |        |    \___/    |
     * |_____________|        |_____________|
     *
     * if input image below to algorithm,
     * then the rotation value is:
     * left:  ROTATION_90, 90 degree anticlockwise.
     * right: ROTATION_180, 180 degree anticlockwise.
     *
     * |````````````````|     |`````````````|
     * |        \       |     |    /```\    |
     * |          \---\ |     |    \_-_/    |
     * | ----------|>  ||     |     /|\     |
     * |          /---/ |     |    / | \    |
     * |        /       |     |      |      |
     * |-----------------     |_____________|
     *
     * DO NOT CHANGE THE VALUE
     */
    enum Rotation {
        ROTATION_0   = 0,
        ROTATION_90  = 90,
        ROTATION_180 = 180,
        ROTATION_270 = 270
    };

    enum StoreMode {
        STORE_ALL = 0,
        STORE_Y_UV,
        STORE_Y_U_V
    };

    enum AlignedMode{
        ALIGNED_NO,
        ALIGNED_W,     // <- aligned with width
        ALIGNED_H,     // <- aligend with height
        ALIGNED_W_H    // <- aligned with width and height
    };

    Image();

    /*
     * Allocate buffer with w(width) and h(height),
     * the buffer managed automatically
     *
     * @param w matrix width
     *        h matrix height
     *        imgType image type
     *        dt data type(DT_8U(unsigned char)/DT_32F(float)/etc)
     *        sm how match buffer mallocated related to image type
     * Default: 1. do not aligned
     *          2. direction(ROTATION_0)
     */
    Image(int w, int h, ImageType imgType, DataType dt, StoreMode sm = STORE_ALL);

    /*
     * The buffer managed by allocator
     *
     * @param w matrix width
     *        h matrix height
     *        data buffer referenced
     *        imgType image type
     *        dt data type(DT_8U(unsigned char)/DT_32F(float)/etc)
     *        r image anti-clockwise direction related to camera module
     * Default: 1. direction(ROTATION_0)
     */
    Image(int w, int h, void* data, ImageType imgType, DataType dt, Rotation r = ROTATION_0);

    /*
     * The buffer managed by allocator
     *
     * @param w matrix width
     *        h matrix height
     *        data1 yuv or rgb data
     *        data2 u or uv data
     *        data3 v data
     *        imgType image type
     *        dt data type(DT_8U(unsigned char)/DT_32F(float)/etc)
     *        r image anti-clockwise direction related to camera module
     */
    Image(int w, int h, void* data1, void* data2, void* data3,
              ImageType imgType, DataType dt, Rotation r = ROTATION_0);
    Image(const Image& c);
    virtual ~Image();
    Image& operator = (const Image& img);

    /*
     * Create new Image and allocate buffer with w(width) and h(height),
     * the buffer managed by Image automatically
     *
     * @param w matrix width
     *        h matrix height
     *        imgType image type
     *        dt data type(DT_8U(unsigned char)/DT_32F(float)/etc)
     *        sm how match buffer mallocated related to image type
     */
    void create(int w, int h, ImageType imgType, DataType dt, StoreMode sm = STORE_ALL);

    /*
     * set width and height of image
     */
    void setImageWH(int imgRealWidth, int imgRealHeight);

    void setRotation(Rotation r);
    Rotation rotation() const;

    ImageType imageType() const;

    /**
     * deep copy, need copied object similar with this
     *
     * @param toIma copy to object
     * @return true when successful, otherwise failure
     */
    bool copyTo(Image& toImg);

    /**
     * clone Image
     *
     * @param toIma clone to object
     * @return Image object
     */
    bool cloneTo(Image& toImg);

    /**
     * similar cloneTo(Image&)
     *
     * @return clone Image
     */
    Image clone();

    /*
     * Get buffer pointer of yuv or rgb data
     *
     * @return buffer pointer
     */
    unsigned char* data1();

    /*
     * Get buffer pointer of u or uv data
     *
     * @return buffer pointer
     */
    unsigned char* data2();

    /*
     * Get buffer pointer of v data
     *
     * @return buffer pointer
     */
    unsigned char* data3();

    /*
     * Get buffer size(bit)
     * size = w * h * channel nums * elem size(bit)
     * if YUV data splited to independent Y buffer/U buffer/V buffer,
     * then U/V buffer size need to computed by user.
     *
     * @return total buffer size(byte)
     */
    size_t total() const;
    size_t channels() const;
    size_t elemSize() const;

    StoreMode storeMode() const;

    /*
     * matrix width
     */
    int width() const;

    /*
     * matrix height
     */
    int height() const;

    /*
     * image width
     */
    int realWidth() const;

    /*
     * image height
     */
    int realHeight() const;

    /*
     * Get aligned mode
     *
     * @return aligned mode
     */
    AlignedMode alignedMode() const;

    /*
     * whether tate buffer
     */
    bool empty() const;

    int isValid();

    /*
     * Extend information for image
     */
    void setExifIso(int iso);
    int exifIso();

    void setExifFocusP(int x, int y);
    int exifFocusX();
    int exifFocuxY();

    void setExifFNumber(float fn);
    float exifFNumber();

    void setExifWatermark(bool isOn);
    bool exifIsWatermarkOn();

    /*
     * set exif timestamp info.
     *
     * @param timestamp the timestamp info.
     */
    void setExifTimestamp(unsigned long timestamp);

    /*
     * get exif timestamp info.
     *
     * @return the timestamp info value.
     */
    unsigned long exifTimestamp();

    /**
     * Set exif camera fov
     *
     * @params camera fov
     */
    void setExifFov(const float fov);

    /**
     * get exif camera fov
     *
     * @return image fov
     */
    float getExifFov() const;

    /**
     * Set exif camera module info
     *
     * @params moduleInfo camera module info
     */
    void setExifModuleInfo(char* moduleInfo);

    /**
     * Get camera module info
     *
     * @return camera module info
     */
    char* exifModuleInfo() const;

private:
    class Impl;
    Impl *p;

}; // class Image

}  // namespace wa

#endif // __LIB_IMAGE_H__
