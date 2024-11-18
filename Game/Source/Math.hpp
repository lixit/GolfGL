#pragma once

#include <vector>
#include <future>

namespace Math
{
    /* Types */
    template <typename T = float>
    using Array = std::vector<T>;
    template <typename T = float>
    using Matrix = std::vector<Array<T>>;
    using Image  = std::vector<std::vector<float>>;

    /* Constants */
    constexpr double Pi = 3.14159265359;
    constexpr glm::mat4 I4{1.f};
    constexpr glm::vec3 e3{1.f, 1.f, 1.f};

    template <typename T>
    double Gaussian(T x, T y, double sigma)
    {
        return exp(-(x * x + y * y) / (2.f * sigma * sigma)) / (2.f * Pi * sigma * sigma);
    }


    [[nodiscard]] inline Matrix<float> GaussianFilter(int height, int width, double sigma)
    {
        Matrix<float> kernel(height, Array<float>(width));
        float sum = 0.0;

        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
            {
                kernel[i][j] = Gaussian(i, j, sigma);
                sum += kernel[i][j];
            }
        
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                kernel[i][j] /= sum;

        return kernel;
    }


    inline Image ApplyFilterParallel(Image& im, Image& filter, bool hor)
    {
        int n = hor ? im.size()    - filter.size() + 1 :  - filter[0].size() + 1 + im[0].size();
        int m = hor ? im[0].size() - filter[0].size() + 1 :  - filter.size() + 1 + im.size();
        Image newImage(n, std::vector<float>(m, 0.f));
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                for (unsigned h = i; h < i + filter.size(); h++)
                    for (unsigned w = j; w < j + filter[0].size(); w++)
                        newImage[i][j] += filter[h - i][w - j] * im[h][w];

        return newImage;
    }


    [[nodiscard]] inline Image GaussianSmoothing(Image &image)
    {        
        Image filter { GaussianFilter(5, 5, 50.f) };
        Image rotated(image.size(), std::vector<float>(image[0].size()));
        std::reverse_copy(image.begin(), image.end(), rotated.begin());
            

        auto a1 = std::async(std::launch::async, [&]{ return ApplyFilterParallel(image,   filter, true); } );
        auto a2 = std::async(std::launch::async, [&]{ return ApplyFilterParallel(image,   filter, false); } );
        auto a3 = std::async(std::launch::async, [&]{ return ApplyFilterParallel(rotated, filter, true); } );
        auto a4 = std::async(std::launch::async, [&]{ return ApplyFilterParallel(rotated, filter, false); } );
                
        Image newImageH1 = a1.get();
        Image newImageV1 = a2.get();
        Image newImageH2 = a3.get();
        Image newImageV2 = a4.get();

        
        int newImageHeight = newImageV1.size();        
        int newImageWidth = newImageV1[0].size();
        image.resize(newImageHeight, Array<float>(newImageWidth, 0));

        for(int i = 0; i < newImageHeight; i++)
            for(int j = 0; j < newImageHeight; j++)
                image[i][j] = (newImageH1[i][j] + newImageV1[i][j] 
                    + newImageH2[newImageHeight-i-1][newImageWidth-j-1]
                    + newImageV2[newImageHeight-i-1][newImageWidth-j-1]
                    ) / 4.f;

        return image;
        
    }

    template <typename T>
    [[nodiscard]] Matrix<T> ApplyFilter(Matrix<T> &src, Matrix<T> &filter)
    {
        if (filter.size() <= 0 || filter[0].size() <= 0)
            return {{}};

        int heightSrc    = src.size();
        int widthSrc     = src[0].size();
        int filterHeight = filter.size();
        int filterWidth  = filter[0].size();

        int newHeight = heightSrc - filterHeight + 1;
        int newWidth  = widthSrc - filterWidth + 1;

        auto out{Image(newHeight, Array<T>(newWidth, 0))};

        for (int i = 0; i < newHeight; i++)
            for (int j = 0; j < newWidth; j++)
                for (int h = i; h < i + filterHeight; h++)
                    for (int w = j; w < j + filterWidth; w++)
                        out[i][j] += filter[h - i][w - j] * src[h][w];
        return out;
    }


    template <typename T, int height, int width>
    struct GaussianKernel
    {
        std::vector<std::vector<T>> kernel;
        int h = height;
        int w = width;

        explicit GaussianKernel(T sigma) { kernel = std::move(GaussianFilter(height, width, sigma)); }

        [[nodiscard]] Image operator()(Image &image) { return ApplyFilter<float>(image, kernel); }
    };
} // namespace Math
