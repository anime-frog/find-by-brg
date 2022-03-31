#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <opencv4/opencv2/opencv.hpp>


// create matrices
cv::Mat img;

cv::Mat red_plane;
cv::Mat green_plane;
cv::Mat blue_plane;

cv::Mat red_range;
cv::Mat green_range;
cv::Mat blue_range;

cv::Mat bgr;

// create marks on trackbars
int Rmin = 0;
int Rmax = 256;

int Gmin = 0;
int Gmax = 256;

int Bmin = 0;
int Bmax = 256;

int RGBmax = 256;

//create function for trackabars
void trackbarRedMin(int pos, void *)
{
    Rmin = pos;
    cv::inRange(red_plane, cv::Scalar(Rmin), cv::Scalar(Rmax), red_range);
}

void trackbarRedMax(int pos, void *)
{
    Rmax = pos;
    cv::inRange(red_plane, cv::Scalar(Rmin), cv::Scalar(Rmax), red_range);
}

void trackbarGreenMin(int pos, void *)
{
    Gmin = pos;
    cv::inRange(green_plane, cv::Scalar(Gmin), cv::Scalar(Gmax), green_range);
}

void trackbarGreenMax(int pos, void *)
{
    Gmax = pos;
    cv::inRange(green_plane, cv::Scalar(Gmin), cv::Scalar(Gmax), green_range);
}

void trackbarBlueMin(int pos, void *)
{
    Bmin = pos;
    cv::inRange(blue_plane, cv::Scalar(Bmin), cv::Scalar(Bmax), blue_range);
}

void trackbarBlueMax(int pos, void *)
{
    Bmax = pos;
    cv::inRange(blue_plane, cv::Scalar(Bmin), cv::Scalar(Bmax), blue_range);
}

int main(int argc, char *argv[])
{
    // check for the presence and existence of a file

    img = cv::imread(argv[1], 1);

    if (argc < 2 || img.empty())
    {
        std::string filename;

        do
        {
            std::cerr << "please enter the path to the image (\"--exit\" to exit application): ";
            std::getline(std::cin, filename);

            img = cv::imread(filename, 1);

        } while (filename != "--exit" && img.empty());

        if (filename == "--exit")
            return 1;
    }

    char key; // for checking wether key was pressed

    //for finding minimum and maximum of matrices
    double framemin;
    double framemax;

    // channel
    std::vector<cv::Mat> bgr_channel(3);

    // adjusting matrices to a specific format
    red_range.create(cv::Size(img.cols, img.rows), CV_8UC1);
    green_range.create(cv::Size(img.cols, img.rows), CV_8UC1);
    blue_range.create(cv::Size(img.cols, img.rows), CV_8UC1);
    bgr.create(cv::Size(img.cols, img.rows), CV_8UC1);

    // split image on 3 channels (blue, green, red)
    cv::split(img, bgr_channel);

    // get initial values of channels
    blue_plane = bgr_channel[0];
    green_plane = bgr_channel[1];
    red_plane = bgr_channel[2];

    // find minimums and maximums of matrixses
    cv::minMaxLoc(blue_plane, &framemin, &framemax);
    Bmin = framemin;
    Bmax = framemax;

    cv::minMaxLoc(green_plane, &framemin, &framemax);
    Gmin = framemin;
    Gmax = framemax;
    
    cv::minMaxLoc(red_plane, &framemin, &framemax);
    Rmin = framemin;
    Rmax = framemax;

    // open windows
    cv::namedWindow("original");
    cv::namedWindow("blue");
    cv::namedWindow("green");
    cv::namedWindow("red");
    cv::namedWindow("bgr");

    // create trackbars
    cv::createTrackbar("BlueMin", "blue", &Bmin, RGBmax, trackbarBlueMin);
    cv::createTrackbar("BlueMax", "blue", &Bmax, RGBmax, trackbarBlueMax);
    cv::createTrackbar("GreenMin", "green", &Gmin, RGBmax, trackbarGreenMin);
    cv::createTrackbar("GreenMax", "green", &Gmax, RGBmax, trackbarGreenMax);
    cv::createTrackbar("RedMin", "red", &Rmin, RGBmax, trackbarRedMin);
    cv::createTrackbar("RedMax", "red", &Rmax, RGBmax, trackbarRedMax);

    std::cout << "Tap \"esc\" to exit application\n";

    while (true)
    {
        cv::imshow("original", img); // show original image
        cv::imshow("blue", blue_range); // show blue channel which we will change
        cv::imshow("green", green_range); // show green channel which we will change
        cv::imshow("red", red_range); // show red channel which we will change

        // write in vector channels for merging
        bgr_channel[0] = blue_range;
        bgr_channel[1] = green_range;
        bgr_channel[2] = red_range;

        // merge channels in matrix
        cv::merge(bgr_channel, bgr);

        // show changed image
        cv::imshow("bgr", bgr);

        key = cv::waitKey(33); // get key

        if (key == 27)
            break;
    }

    // release matrices
    img.release();
    bgr_channel[0].release();
    bgr_channel[1].release();
    bgr_channel[2].release();

    // destroy windows
    cv::destroyAllWindows();

    std::cout << "Do you want to write created image? (y - yes/ other key - no): ";

    if (std::cin >> key && key == 'y')
    {
        while (getchar() != '\n');

        std::string filename;

        if (!std::filesystem::exists("src/result"))
            std::filesystem::create_directories("src/result");

        std::cout << "Enter name of image: ";

        getchar();
        
        std::getline(std::cin, filename);

        cv::imwrite("src/result/" + filename + ".jpg", bgr) ? 
            std::cout << "File has been wroten\n" : std::cout << "File hasn't been wroten\n";
    }
    else
    {
        std::cin.ignore(1000, '\n');
        std::cin.clear();
    }

    bgr.release();

    return 0;
}
