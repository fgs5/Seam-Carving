#include "SeamCarver.h"

#include <algorithm>
#include <assert.h>
#include <cmath>

SeamCarver::SeamCarver(Image image)
    : m_image(std::move(image))
{
    Emptify();
}

const Image & SeamCarver::GetImage() const
{
    return m_image;
}

std::size_t SeamCarver::GetImageWidth() const
{
    return m_image.m_table.size();
}

std::size_t SeamCarver::GetImageHeight() const
{
    if (m_image.m_table.empty()) {
        return 0;
    }
    return m_image.m_table[0].size();
}

double square(int a)
{
    double temp = a;
    return temp * temp;
}

double SeamCarver::GetDelta(const Image::Pixel & first, const Image::Pixel & second)
{
    double red = square(first.m_red - second.m_red);
    double green = square(first.m_green - second.m_green);
    double blue = square(first.m_blue - second.m_blue);
    return red + green + blue;
}

double SeamCarver::GetPixelEnergy(std::size_t columnId, std::size_t rowId) const
{
    Image::Pixel left = m_image.GetPixel((columnId == 0 ? GetImageWidth() : columnId) - 1, rowId);
    Image::Pixel right = m_image.GetPixel(columnId == GetImageWidth() - 1 ? 0 : columnId + 1, rowId);
    Image::Pixel above = m_image.GetPixel(columnId, (rowId == 0 ? GetImageHeight() : rowId) - 1);
    Image::Pixel below = m_image.GetPixel(columnId, rowId == GetImageHeight() - 1 ? 0 : rowId + 1);

    double delta_x = GetDelta(left, right);
    double delta_y = GetDelta(above, below);

    return std::sqrt(delta_x + delta_y);
}

SeamCarver::Seam SeamCarver::FindMinSeam(std::size_t outer_size, std::size_t inner_size, bool horizontal) const
{
    std::vector<double> cur(inner_size);
    for (std::size_t i = 0; i < inner_size; ++i) {
        if (horizontal) {
            cur[i] = GetPixelEnergy(0, i);
        }
        else {
            cur[i] = GetPixelEnergy(i, 0);
        }
    }
    std::vector<std::vector<std::size_t>> to_restore_way(outer_size);
    std::vector<double> next(inner_size);
    for (std::size_t i = 1; i < outer_size; ++i) {
        to_restore_way[i].resize(inner_size);
        for (std::size_t j = 0; j < inner_size; ++j) {
            double temp = cur[j];
            std::size_t path = j;
            if (j > 0 && cur[j - 1] < temp) {
                temp = cur[j - 1];
                path = j - 1;
            }
            if (j < inner_size - 1 && cur[j + 1] < temp) {
                temp = cur[j + 1];
                path = j + 1;
            }
            temp += (horizontal ? GetPixelEnergy(i, j) : GetPixelEnergy(j, i));
            to_restore_way[i][j] = path;
            next[j] = temp;
        }
        std::swap(cur, next);
    }
    std::size_t min = (std::min_element(cur.begin(), cur.end()) - cur.begin());

    Seam result(outer_size);
    result[0] = min;
    for (std::size_t i = outer_size - 1; i > 0; --i) {
        result[outer_size - i] = to_restore_way[i][min];
        min = to_restore_way[i][min];
    }
    std::reverse(result.begin(), result.end());
    return result;
}

SeamCarver::Seam SeamCarver::FindHorizontalSeam() const
{
    if (GetImageHeight() == 0) {
        return Seam();
    }
    return FindMinSeam(GetImageWidth(), GetImageHeight(), true);
}

SeamCarver::Seam SeamCarver::FindVerticalSeam() const
{
    if (GetImageWidth() == 0) {
        return Seam();
    }
    return FindMinSeam(GetImageHeight(), GetImageWidth(), false);
}

void SeamCarver::RemoveHorizontalSeam(const Seam & seam)
{
    std::size_t width = GetImageWidth();
    assert((seam.size() == width));
    for (std::size_t x = 0; x < width; ++x) {
        m_image.m_table[x].erase(m_image.m_table[x].begin() + seam[x]);
    }
    Emptify();
}

void SeamCarver::RemoveVerticalSeam(const Seam & seam)
{
    std::size_t height = GetImageHeight();
    assert((seam.size() == height));
    for (std::size_t y = height - 1; y < MAX; --y) {
        for (std::size_t x = seam[y]; x < GetImageWidth() - 1; ++x) {
            m_image.m_table[x][y] = m_image.m_table[x + 1][y];
        }
    }
    if (height != 0) {
        m_image.m_table.pop_back();
    }
}

void SeamCarver::Emptify()
{
    while (GetImageWidth() != 0 && GetImageHeight() == 0) {
        m_image.m_table.pop_back();
    }
}
