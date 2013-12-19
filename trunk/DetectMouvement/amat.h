#ifndef _AMAT_H_
#define _AMAT_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
//using namespace std;
using namespace cv;

class AMat : public Mat
{
public:
  AMat();
  AMat(int h, int w, int t);
  AMat(int h, int w, int t, Scalar);
  AMat(Mat image);
  AMat(AMat& chanel);
  AMat& operator=(const AMat& chanel);
  AMat operator-(const AMat &img2) const;
  static void drawCross( Point center, Scalar color, double d, Mat &img, string name );
  static void thresh_callback(Mat src_gray, Mat &src_rgb, vector<Point2f> &center,
			     Mat &image_tracking);

  virtual ~AMat();
};

#endif /* _CHANEL_H_ */
