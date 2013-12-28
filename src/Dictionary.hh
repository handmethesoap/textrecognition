#ifndef DICTIONARY_HH
#define DICTIONARY_HH

#include "FileReader.hh"
#include "Debug.hh"
#include "Array.hh"
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class Dictionary
{
public:
  Dictionary( const FileReader & conf, const cv::Mat & image, bool type_ ): parameters(conf), type(type_)
  {
    CHECK_MSG( parameters.getIntParameter("dictionary_length") > 0, "The provided dictionary length must be larger than zero, please adjust this in your parameter file" );
    generate();
    
  }
  
  
  
private:
  
  void generate();
  const FileReader & parameters;
  bool type;
  
  
};














#endif //DICTIONARY_HH