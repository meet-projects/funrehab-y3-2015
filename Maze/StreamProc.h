#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <direct.h>
#include <time.h>
#include <Windows.h>

using namespace std;
using namespace cv;

#define ACCURACY_LEVEL 4

#define WHITE 255
#define BLACK 000

class StreamProc
{
private:
	Mat real_pic;

};