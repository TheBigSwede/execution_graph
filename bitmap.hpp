#include <fstream>
#include <vector>

#pragma pack(push, 1)
struct BitmapFileHeader {
    char header[2]{'B', 'M'};
    uint32_t fileSize;
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t dataOffset;
};

struct BitmapInfoHeader {
    uint32_t size{40};
    int32_t width;
    int32_t height;
    uint16_t planes{1};
    uint16_t bitsPerPixel{24};
    uint32_t compression{0};
    uint32_t imageSize{0};
    int32_t xPixelsPerMeter{0};
    int32_t yPixelsPerMeter{0};
    uint32_t colorsUsed{0};
    uint32_t colorsImportant{0};
};
#pragma pack(pop)

void generateBitmap(const std::string& filename, int width, int height, const std::vector<uint8_t>& image_data) {
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    // Set up the headers
    file_header.fileSize = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + image_data.size();
    file_header.dataOffset = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    info_header.width = width;
    info_header.height = height;
    info_header.imageSize = image_data.size();

    // Open the file in binary mode
    std::ofstream file(filename, std::ios::binary);

    // Write the headers
    file.write(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));
    file.write(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));

    // Write the image data
    file.write(reinterpret_cast<const char*>(image_data.data()), image_data.size());

    // Close the file
    file.close();
}
