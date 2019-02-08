#ifndef __SIL_SLIENT_REFOCUS_H__
#define __SIL_SLIENT_REFOCUS_H__

#include "Image.h"

using namespace wa;

namespace wa {

class Refocus
{
public:
    Refocus();
    ~Refocus();

    Refocus(const Refocus& rf);
    Refocus& operator = (const Refocus& rf);

    /** @brief Set main image and depth image.

     * @param mainImg main image
     *        depthImg  depth image
     */
    void setImages(const Image& mainImg, const Image& depthImg);

    /** @brief Run supernight algorithm.

     * @param outImg output Image
     * @return 1 if success, otherwise failure
     */
    int run(int focusX, int focusY, float fNum, Image& outRefocusedImg);

    /** @brief Get version number.

     * @return version number
     */
    const char* getVersion() const;

    const char* getFullVersion(int flags) const;

private:
    class Impl;
    Impl *p;
}; // class Refocus

} // namespace wa

#endif // __SIL_SLIENT_REFOCUS_H__