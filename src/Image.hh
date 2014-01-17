#ifndef IMAGE_HH
#define IMAGE_HH

#include <iostream>
#include <vector>
#include <string>
#include "Dictionary.hh"
#include "FileReader.hh"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

struct Rectangle{
  
  Rectangle(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_){
  }
  
  int x;
  int y;
  int w;
  int h;
  
  void print(void){
    std::cout << "x = " << x << ", y = " << y << ", w = " << w << ", h = " << h << std::endl;
  }
};

class Image{
  
public:
  
  Image(const FileReader & conf, const std::string & imagefile ) : parameters(conf), file(imagefile)
  {
    getTextBoxes(parameters.getStringParameter("image_path_1"));
    
    image = cv::imread(  parameters.getStringParameter("image_path_1") + file , cv::IMREAD_COLOR );
    
    if(! image.data ){
      std::cout << "could not read image " << parameters.getStringParameter("image_path_1") + file << std::endl;
    }
    
    //image2 = image(cv::Range(0,31), cv::Range(0,31));
  }
  
  ~Image(){
    for( auto it = textboxes.begin(); it != textboxes.end(); ++it ){
      delete(*it);
    }
  }
  
  void printIsText(void);
  
private:
  
  const FileReader & parameters;
  const std::string & file;
  std::vector<Rectangle*> textboxes;
  cv::Mat image;
  bool isText( int x, int y);
  void getTextBoxes( const std::string & path);
  std::string stringToUpper(std::string s);
  Dictionary *dict;
  
  
};











#endif