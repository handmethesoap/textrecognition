#ifndef TEXTRECOGNITION_HH
#define TEXTRECOGNITION_HH

#include "FileReader.hh"
#include "Dictionary.hh"
#include <vector>

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

class TextRecognition {
  
public:
  TextRecognition(const FileReader & conf, const Dictionary & d) : parameters(conf), dict(d){
    
  }

  void train(void);








private:

  void printIsText(cv::Mat image, std::vector<cv::Rect*> textboxes);
  bool isText( int x, int y, std::vector<cv::Rect*> textboxes);
  
  const FileReader & parameters;
  const Dictionary & dict;











};



#endif