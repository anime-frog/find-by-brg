#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <opencv4/opencv2/opencv.hpp>

// initialize matrices
cv::Mat img;

cv::Mat h_plane;
cv::Mat s_plane;
cv::Mat v_plane;

cv::Mat h_range;
cv::Mat s_range;
cv::Mat v_range;

cv::Mat hsv;

// create marks on trackbars
int Hmin = 0;
int Hmax = 256;

int Smin = 0;
int Smax = 256;

int Vmin = 0;
int Vmax = 256;

int HSVmax = 256;

//create function for trackabars
void myTrackbarHmin(int pos, void *)
{
    Hmin = pos;
    cv::inRange(h_plane, cv::Scalar(Hmin), cv::Scalar(Hmax), h_range);
}
void myTrackbarHmax(int pos, void *)
{
    Hmax = pos;
    cv::inRange(h_plane, cv::Scalar(Hmin), cv::Scalar(Hmax), h_range);
}
void myTrackbarSmin(int pos, void *)
{
    Smin = pos;
    cv::inRange(s_plane, cv::Scalar(Smin), cv::Scalar(Smax), s_range);
}
void myTrackbarSmax(int pos, void *)
{
    Smax = pos;
    cv::inRange(s_plane, cv::Scalar(Smin), cv::Scalar(Smax), s_range);
}
void myTrackbarVmin(int pos, void *)
{
    Vmin = pos;
    cv::inRange(v_plane, cv::Scalar(Vmin), cv::Scalar(Vmax), v_range);
}
void myTrackbarVmax(int pos, void *)
{
    Vmax = pos;
    cv::inRange(v_plane, cv::Scalar(Vmin), cv::Scalar(Vmax), v_range);
}

int main(int argc, char *argv[])
{

    // check for the presence and existence of a file

    if (argc >= 2)
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

    // create HSV-channel
    std::vector<cv::Mat> hsv_channel(3);

    // create single channel matrices
    h_range.create(img.size(), CV_8UC1);
    s_range.create(img.size(), CV_8UC1);
    v_range.create(img.size(), CV_8UC1);

    // convert image from BRG to HSV
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    // split HSV-image on channels 
    cv::split(hsv, hsv_channel);

    // get initial values of channels
    h_plane = hsv_channel[0];
    s_plane = hsv_channel[1];
    v_plane = hsv_channel[2];

    // find minimums and maximums of matrixses
    cv::minMaxLoc(h_plane, &framemin, &framemax);
    Hmin = framemin;
    Hmax = framemax;

    cv::minMaxLoc(s_plane, &framemin, &framemax);
    Smin = framemin;
    Smax = framemax;
    
    cv::minMaxLoc(v_plane, &framemin, &framemax);
    Vmin = framemin;
    Vmax = framemax;

    // open windows
    cv::namedWindow("original");
    cv::namedWindow("hue");
    cv::namedWindow("saturation");
    cv::namedWindow("volume");
    cv::namedWindow("hsv and");

    // create trackbars
    cv::createTrackbar("Hmin", "hue", &Hmin, HSVmax, myTrackbarHmin);
    cv::createTrackbar("Hmax", "hue", &Hmax, HSVmax, myTrackbarHmax);
    cv::createTrackbar("Smin", "saturation", &Smin, HSVmax, myTrackbarSmin);
    cv::createTrackbar("Smax", "saturation", &Smax, HSVmax, myTrackbarSmax);
    cv::createTrackbar("Vmin", "volume", &Vmin, HSVmax, myTrackbarVmin);
    cv::createTrackbar("Vmax", "volume", &Vmax, HSVmax, myTrackbarVmax);

    std::cout << "Tap \"esc\" to exit application\n";

    while (true)
    {
        cv::imshow("original", img); // show original image
        cv::imshow("hue", h_range); // show hue
        cv::imshow("saturation", s_range); // show saturation
        cv::imshow("volume", v_range); // show volume

        // write in vector channels for merging
        hsv_channel[0] = h_range;
        hsv_channel[1] = s_range;
        hsv_channel[2] = v_range;

        // merge channels in matrix
        cv::merge(hsv_channel, hsv);

        // show changed image
        cv::imshow("hsv and", hsv);

        key = cv::waitKey(33); // get key

        if (key == 27)
            break;
    }

    // destroy windows
    cv::destroyAllWindows();

    // ask to save image with settings
    std::cout << "Do you want to write created image? (y - yes/ other key - no): ";
    std::cin >> key;

    if (key == 'y')
    {
        // move to the end of the buffer
        while (getchar() != '\n');

        std::string filename;

        std::cout << "Enter name of image: ";
        std::getline(std::cin, filename);

        // create directories where will be located image with settings
        if (!boost::filesystem::exists("src/result/" + filename))
            boost::filesystem::create_directories("src/result/" + filename);

        // create file with settings
        boost::filesystem::fstream configs("src/result/" + filename + "/" + filename + "_settings.txt", std::ios::out);

        // write settings
        configs << "HMin: " << Hmin << "\tHMax: " << Hmax 
                << "\nSMin: " << Smin << "\tSMax: " << Smax 
                << "\nVMin: " << Vmin << "\tVMax: " << Vmax;

        configs.close();

        // show if files are saved
        boost::filesystem::file_size("src/result/" + filename + "/" + filename + "_settings.txt") > 0 ?
            std::cout << "Settings have been saved\n" : std::cout << "Settings haven't been saved\n";

        cv::imwrite("src/result/" + filename + "/" + filename +".jpg", bgr) ? 
            std::cout << "Image has been saved\n" : std::cout << "Image hasn't been saved\n";

    return 0;
}
