#include "easy_image.h"
#include "ini_configuration.h"
#include "Lines2D.h"
#include "l_parser.h"
#include "vector3d.h"
#include "ThreeDfigures.h"
#include "Lichamen3D.h"
#include "ZBuffer.h"
#include "Light.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>
#include <math.h>
#include <random>
#include <time.h>
#include <algorithm>

void backgroundcolorer(img::EasyImage &image, int x, int y, img::Color &color){
    for(int tempX=x-1;tempX!=-1;tempX--){
        image.draw_line(tempX,0,tempX,y-1,color);
    }
}