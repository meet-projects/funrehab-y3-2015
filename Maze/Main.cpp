#include "StreamProc.h"

Point cursor;

Point stabilized_point(vector<Point> contours)
{
	double x_sum = 0, y_sum = 0;
	for (vector<Point>::iterator it = contours.begin(); it != contours.end(); it++)
	{
		x_sum += it->x;
		y_sum += it->y;
	}

	return Point((x_sum / contours.size()), (y_sum / contours.size()));
}
Point stabilized_point_2(Point new_point, Point old_point)
{
	double x_average = new_point.x + old_point.x, y_average = new_point.y + old_point.y;
	return Point(x_average / 2.0, y_average / 2.0);
}

void LeftClick(int x, int y)
{
	const double XSCALEFACTOR = 65535 / (GetSystemMetrics(SM_CXSCREEN) - 1);
	const double YSCALEFACTOR = 65535 / (GetSystemMetrics(SM_CYSCREEN) - 1);

	POINT cursorPos;
	GetCursorPos(&cursorPos);

	double cx = cursorPos.x * XSCALEFACTOR;
	double cy = cursorPos.y * YSCALEFACTOR;

	double nx = x * XSCALEFACTOR;
	double ny = y * YSCALEFACTOR;

	INPUT Input = { 0 };
	Input.type = INPUT_MOUSE;

	Input.mi.dx = (LONG)nx;
	Input.mi.dy = (LONG)ny;

	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;

	SendInput(1, &Input, sizeof(INPUT));
	//SendInput(1, &Input, sizeof(INPUT));

	Input.mi.dx = (LONG)cx;
	Input.mi.dy = (LONG)cy;

	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

	SendInput(1, &Input, sizeof(INPUT));
}

void camera_feed();

int _main()
{
	camera_feed();
	return 0;
}

void camera_feed()
{
	VideoCapture cap(0);
	if (cap.isOpened())
	{
		int distance[3], MUL = 1, dif = 0;
		char key;
		bool first_run = false, is_size_checked = false, moved = false, shoot = false;
		unsigned long max_contours_amount = 0;
		Point drawing_point, cursor, additional_point;
		vector<vector<Point>> contours, main_points;
		vector<Point> pen1, pen2, pens;
		vector<Vec4i> hierarchy;
		Mat frame, real_pic, drawing_frame, maze;
		Scalar low_boundry(45, 107, 52), high_boundry(86, 227, 160), color(100, 100, 100);
		//namedWindow("drawing_frame", 1);
		//namedWindow("frame", 1);
		cap >> frame;
		cursor = Point(20, 20);
		maze = imread("maze1.jpg");
		maze = maze / WHITE;
		maze = maze * WHITE;
		bitwise_not(maze, maze);
		
		

		RECT rect = { 0 }; // gaming stuff!
		HWND window = FindWindow("Chicken Invaders 5", "Chicken Invaders 5");
		Sleep(2000);
		if (window)
		{
			GetClientRect(window, &rect);
			SetForegroundWindow(window);
			SetActiveWindow(window);
			SetFocus(window);
		}

		while (true)
		{
			shoot = false;
			cap >> frame;
			real_pic = frame.clone();
			while (main_points.size() != 0)
			{
				main_points.pop_back();
			}
			if (!first_run)
			{
				drawing_frame = real_pic.clone();
				resize(drawing_frame, drawing_frame, Size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) - 50));
				resize(maze, maze, Size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) - 50));
				first_run = true;
			}
			flip(real_pic, real_pic, 1);

			cvtColor(frame, frame, COLOR_BGR2HSV);
			
			inRange(frame, low_boundry, high_boundry, frame);
			flip(frame, frame, 1);

			contours.clear();
			resize(frame, frame, Size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));
			findContours(frame, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
			is_size_checked = false;
			if (contours.size() != 0)
			{
				for (vector<vector<Point>>::iterator it = contours.begin(); it != contours.end(); it++)
				{
					if (it->size() > max_contours_amount * 0.7)
					{
						main_points.push_back(*it);
						max_contours_amount = it->size();
						is_size_checked = true;
					}
				}
			}
			if (is_size_checked)
			{
				moved = false;
				drawing_point = stabilized_point(main_points[0]);
				if (main_points.size() == 2)
				{
					if (stabilized_point(main_points[0]).x < stabilized_point(main_points[1]).x)
					{
						drawing_point = stabilized_point(main_points[1]);
						
					}
					shoot = true;
				}
				drawing_point.x += (drawing_point.x - drawing_frame.size().width / 2) / 10;
				drawing_point.y += (drawing_point.y - drawing_frame.size().height / 2) / 10;
				while (drawing_point.x > maze.size().width)
				{
					drawing_point.x--;
				}
				while (drawing_point.x < 0)
				{
					drawing_point.x++;

				}
				while (drawing_point.y > maze.size().height)
				{
					drawing_point.y--;
				}
				while (drawing_point.y < 0)
				{
					drawing_point.y++;
				}

				distance[0] = drawing_point.x - cursor.x;
				distance[1] = drawing_point.y - cursor.y;
				while (distance[0] != 0 && distance[1] != 0)
				{
					if (maze.at<Vec3b>(Point(cursor.x + distance[0] / 15, cursor.y))[0] != WHITE)
					{
						cursor.x += distance[0] / 15;
						distance[0] /= 15;
						moved = true;
					}
					if (maze.at<Vec3b>(Point(cursor.x, cursor.y + distance[1] / 15))[0] != WHITE)
					{
						cursor.y += distance[1] / 15;
						distance[1] /= 15;
						moved = true;
					}				
					if (!moved)
					{
						putText(drawing_frame, "Struck a wall!", Point(0, 40), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(WHITE, WHITE, BLACK, 1), 1, CV_AA);
						distance[0] = 0;
						distance[1] = 0;
					}
					
				}
				SetCursorPos(drawing_point.x, drawing_point.y); // gaming stuff!
				circle(drawing_frame, cursor, 13, Scalar(WHITE, BLACK, WHITE), 2);
				circle(drawing_frame, drawing_point, 13, Scalar(WHITE, BLACK, WHITE), -1);
				//circle(drawing_frame, stabilized_point(pen1), 13, Scalar(WHITE, WHITE, BLACK), -1);
			}
			else
			{
				putText(drawing_frame, "Lost drawing object!", Point(0, 20), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(WHITE, WHITE, BLACK, 1), 1, CV_AA);
				circle(drawing_frame, cursor, 13, Scalar(WHITE, WHITE, BLACK), 3);
			}
			if (shoot)
			{
				LeftClick(drawing_point.x, drawing_point.y);
			}
			key = waitKey(10);

			drawing_frame = maze + drawing_frame;
			bitwise_not(drawing_frame, drawing_frame);
			//imshow("drawing_frame", drawing_frame);
			//imshow("frame", frame);

			frame = BLACK;
			drawing_frame = BLACK;
			real_pic = BLACK;

		}
	}
}