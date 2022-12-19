#include "Image.h"
#include "SeamCarver.h"

#include <assert.h>
#include <fstream>
#include <iostream>

namespace {

std::vector<std::vector<Image::Pixel>> ReadImageFromCSV(std::ifstream & input)
{
    size_t width, height;
    input >> width >> height;
    std::vector<std::vector<Image::Pixel>> table;
    for (size_t columnId = 0; columnId < width; ++columnId) {
        std::vector<Image::Pixel> column;
        for (size_t rowId = 0; rowId < height; ++rowId) {
            size_t red, green, blue;
            input >> red >> green >> blue;
            column.emplace_back(red, green, blue);
        }
        table.emplace_back(std::move(column));
    }
    return table;
}

void WriteImageToCSV(const SeamCarver & carver, std::ofstream & output)
{
    const size_t width = carver.GetImageWidth();
    const size_t height = carver.GetImageHeight();
    output << width << " " << height << "\n";
    const Image & image = carver.GetImage();
    for (size_t columnId = 0; columnId < width; ++columnId) {
        for (size_t rowId = 0; rowId < height; ++rowId) {
            const Image::Pixel & pixel = image.GetPixel(columnId, rowId);
            output << pixel.m_red << " " << pixel.m_green << " " << pixel.m_blue << std::endl;
        }
    }
}

void AbstractTest(std::vector<std::vector<Image::Pixel>> image, std::size_t expected_seam_size, std::size_t expected_height, std::size_t expected_width, bool horizontal)
{
    SeamCarver carver(std::move(image));
    std::vector<size_t> seam;
    if (horizontal) {
        seam = carver.FindHorizontalSeam();
        carver.RemoveHorizontalSeam(seam);
    }
    else {
        seam = carver.FindVerticalSeam();
        carver.RemoveVerticalSeam(seam);
    }
    assert(seam.size() == expected_seam_size);
    assert(carver.GetImageHeight() == expected_height);
    assert(carver.GetImageWidth() == expected_width);
}

void TestOnEmptyImage()
{
    std::cout << "Testing SeamCarver on an empty image." << std::endl;
    std::vector<std::vector<Image::Pixel>> a;
    AbstractTest(a, 0, 0, 0, false);
    AbstractTest(std::move(a), 0, 0, 0, true);
    std::cout << "Successfully tested SeamCarver on an empty image." << std::endl;
}

void TestOn1x0Image()
{
    std::cout << "Testing SeamCarver on an 1x0 image using vertical seam." << std::endl;
    std::vector<std::vector<Image::Pixel>> a(1);
    AbstractTest(a, 0, 0, 0, false);
    std::cout << "Successfully tested SeamCarver on an 1x0 image using vertical seam." << std::endl;
    std::cout << "Testing SeamCarver on an 1x0 image using horizontal seam." << std::endl;
    AbstractTest(std::move(a), 0, 0, 0, true);
    std::cout << "Successfully tested SeamCarver on an 1x0 image using horizontal seam." << std::endl;
}

void TestOnUnaryImage()
{
    std::cout << "Testing SeamCarver on an unary image using vertical seam." << std::endl;
    std::vector<std::vector<Image::Pixel>> a(1);
    a[0].emplace_back(10, 10, 10);
    AbstractTest(a, 1, 0, 0, true);
    std::cout << "Successfully tested SeamCarver on an unary image using vertical seam." << std::endl;
    std::cout << "Testing SeamCarver on an unary image using horizontal seam." << std::endl;
    AbstractTest(std::move(a), 1, 0, 0, true);
    std::cout << "Successfully tested SeamCarver on an unary image using horizontal seam." << std::endl;
}

} // unnamed namespace

int main(int argc, char * argv[])
{
    TestOnEmptyImage();
    TestOn1x0Image();
    TestOnUnaryImage();
    // Check command line arguments
    const size_t expectedAmountOfArgs = 3;
    if (argc != expectedAmountOfArgs) {
        std::cout << "Wrong amount of arguments. Provide filenames as arguments. See example below:\n";
        std::cout << "seam-carving data/tower.csv data/tower_updated.csv" << std::endl;
        return 0;
    }
    // Check csv file
    std::ifstream inputFile(argv[1]);
    if (!inputFile.good()) {
        std::cout << "Can't open source file " << argv[1] << ". Verify that the file exists." << std::endl;
    }
    else {
        auto imageSource = ReadImageFromCSV(inputFile);
        SeamCarver carver(std::move(imageSource));
        std::cout << "Image: " << carver.GetImageWidth() << "x" << carver.GetImageHeight() << std::endl;
        const size_t pixelsToDelete = 150;
        for (size_t i = 0; i < pixelsToDelete; ++i) {
            std::vector<size_t> seam = carver.FindVerticalSeam();
            carver.RemoveVerticalSeam(seam);
            std::cout << "width = " << carver.GetImageWidth() << ", height = " << carver.GetImageHeight() << std::endl;
        }
        std::ofstream outputFile(argv[2]);
        WriteImageToCSV(carver, outputFile);
        std::cout << "Updated image is written to " << argv[2] << "." << std::endl;
    }
    return 0;
}
