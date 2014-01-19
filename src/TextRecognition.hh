#ifndef TEXTRECOGNITION_HH
#define TEXTRECOGNITION_HH

#include "FileReader.hh"
#include "Dictionary.hh"
#include <vector>
#include <opencv2/ml/ml.hpp>

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
  void load(void);
  void test(void);








private:

  void printIsText(cv::Mat image, std::vector<cv::Rect*> textboxes);
  bool isText( int x, int y, std::vector<cv::Rect*> textboxes);
  void reduceFeatures( cv::Mat & featurerepresentation, cv::Mat & reducedfeaturerepresentation );
  void normalise( cv::Mat & matrix );
  void printTrainData(cv::Mat traindata, cv::Mat traindatatype);
  void readTrainData(cv::Mat & traindata, cv::Mat & traindatatype);
  void computeFeatureRepresentation(cv::Mat & subimage, cv::Mat & reducedfeaturerepresentation );
  
  
  const FileReader & parameters;
  const Dictionary & dict;
  cv::SVM linearSVM;











};



#endif