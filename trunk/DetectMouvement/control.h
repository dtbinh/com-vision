#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
#include "amat.h"

class Control
{
public:
    Control();
    void processCommand(int argc, char *argv[]);
    std::vector<AMat> capImageFromVideo(string video_name, int time_per_image);
    AMat getImageDeFond(vector<AMat> images) const;
    void getImageDeFond(string video_path, string image_fond);
    void detectVideo(string video_src, string fond_path, int time_per_image);
};

#endif /* _CONTROL_H_ */
