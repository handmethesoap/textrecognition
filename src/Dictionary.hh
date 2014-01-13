#ifndef DICTIONARY_HH
#define DICTIONARY_HH

#include "FileReader.hh"
#include "Debug.hh"
#include "Array.hh"
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class Dictionary
{
public:
  Dictionary( const FileReader & conf, const cv::Mat & image, bool type_ ): parameters(conf), type(type_)
  {
    CHECK_MSG( parameters.getIntParameter("dictionary_length") > 0, "The provided dictionary length must be larger than zero, please adjust this in your parameter file" );
    CHECK_MSG( parameters.getIntParameter("kmeans_attempts") > 0, "The provided number of kmeans attempts must be larger than zero, please adjust this in your parameter file" );
    CHECK_MSG( parameters.getIntParameter("kmeans_iter") > 0, "The provided number of kmeans iterations must be larger than zero, please adjust this in your parameter file" );
    CHECK_MSG( parameters.getRealParameter("kmeans_eps") > 0.0, "The provided kmeans epsilon value must be larger than zero, please adjust this in your parameter file" );
    generate();
    
  }
  
  
  
private:
  
  void generate();
  void printfiles();
  const FileReader & parameters;
  bool type;
  cv::Mat centers;
  
  
};














#endif //DICTIONARY_HH