#pragma once

#include "Image.h"

class SeamCarver
{
    using Seam = std::vector<std::size_t>;

public:
    SeamCarver(Image image);

    /**
     * Returns current image
     */
    const Image & GetImage() const;

    /**
     * Gets current image width
     */
    std::size_t GetImageWidth() const;

    /**
     * Gets current image height
     */
    std::size_t GetImageHeight() const;

    /**
     * Returns pixel energy
     * @param columnId column index (x)
     * @param rowId row index (y)
     */
    double GetPixelEnergy(std::size_t columnId, std::size_t rowId) const;

    /**
     * Returns sequence of pixel row indexes (y)
     * (x indexes are [0:W-1])
     */
    Seam FindHorizontalSeam() const;

    /**
     * Returns sequence of pixel column indexes (x)
     * (y indexes are [0:H-1])
     */
    Seam FindVerticalSeam() const;

    /**
     * Removes sequence of pixels from the image
     */
    void RemoveHorizontalSeam(const Seam & seam);

    /**
     * Removes sequence of pixes from the image
     */
    void RemoveVerticalSeam(const Seam & seam);

private:
    Image m_image;
    static const std::size_t MAX = -1;

    static double GetDelta(const Image::Pixel & left, const Image::Pixel & right);
    void Emptify();
    Seam FindMinSeam(std::size_t outer_size, std::size_t inner_size, bool horizontal) const;
};
