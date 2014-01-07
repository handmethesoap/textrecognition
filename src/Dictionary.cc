#include "Dictionary.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>

void Dictionary::generate(){
   
  cv::Mat samples;
  cv::Mat samples2;
  cv::Mat labels;
  cv::Mat centers;
  cv::TermCriteria criteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, parameters.getIntParameter("kmeans_iter"), parameters.getRealParameter("kmeans_eps"));
  
  for(int i = 1; i< 2; ++i){
    
    cv::Mat image;
    std::string file;
    std::stringstream ss;
    
    ss << "1/" << i << ".jpg";
    ss >> file;
    
    std::cout << parameters.getStringParameter("dictionary_path") + file << std::endl;
    
    image = cv::imread(  parameters.getStringParameter("dictionary_path") + file , cv::IMREAD_GRAYSCALE );
    
    if(! image.data ){
      std::cout << "could not read image " << parameters.getStringParameter("dictionary_path") + file << std::endl;
    }
    
    
    for(int x = 0; x < image.size().width - 7; x = x + 8){
      for( int y = 0; y < image.size().height - 7; y = y + 8){
	cv::Mat image2(8,8,CV_32F);
	image2= image(cv::Range(y,y+8), cv::Range(x,x+8)).clone();
	int sz = image2.cols*image2.rows;
	samples.push_back((image2.reshape(sz, 1)));
      }
    }
    
    samples.convertTo(samples, CV_32F);
    
    cv::kmeans(samples, parameters.getIntParameter("dictionary_length"), labels, criteria, parameters.getIntParameter("kmeans_attempts"), cv::KMEANS_PP_CENTERS, centers);
    
    centers.convertTo(centers, CV_8UC1);
    std::cout << centers << std::endl;
    std::cout << centers.rows << ", " << centers.cols << std::endl;
    //std::cout << samples << std::endl;
//     cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
//     cv::imshow( "Display window", image);
//     cv::waitKey(0);
  
  }
    
//   std::vector<Rectangle*> textboxes;
//   
//   getTextBoxes(parameters.getStringParameter("image_path_1"),  "apanar_06.08.2002/IMG_1305.JPG", textboxes );
//   
//   for( auto it = textboxes.begin(); it != textboxes.end(); ++it ){
//     (**it).print();
//   }
  
  
//   cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
//   cv::imshow( "Display window", image);
//   cv::namedWindow( "Sub window", cv::WINDOW_AUTOSIZE );
//   cv::imshow( "Sub window", image2);
//   
//   cv::waitKey(0);
  
}
