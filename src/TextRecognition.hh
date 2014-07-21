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

  void getNoTextTrainImages(std::vector<std::string>& filenames, std::vector<std::vector<cv::Rect*>*>& textboxes);
  void getTextTrainImages(std::vector<std::string>& filenames);
  void printIsText(cv::Mat image, std::vector<cv::Rect*> textboxes);
  bool isText( int x, int y, std::vector<cv::Rect*> textboxes);
  void reduceFeatures( cv::Mat1f & featurerepresentation, cv::Mat & reducedfeatures );
  void normalise( cv::Mat1f & matrix );
  void printTrainData(cv::Mat traindata, cv::Mat traindatatype);
  void readTrainData(cv::Mat & traindata, cv::Mat & traindatatype);
  void computeFeatureRepresentation(cv::Mat1f & subimage, cv::Mat & reducedfeatures );
  
  
  const FileReader & parameters;
  const Dictionary & dict;
  cv::SVM linearSVM;











};



#endif