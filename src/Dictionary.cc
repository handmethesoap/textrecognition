#include "Dictionary.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>

void Dictionary::generate(){
   
  cv::Mat samples;
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
      for( int y = 0; y < image.size().height - 7; y = y + 8){/*
	std::cout << y/8 + (x/8)*(int(image.size().height - 7)/8 + 1) << " " << std::endl;*/
	cv::Mat image2 = image(cv::Range(y,y+8), cv::Range(x,x+8));
	std::cout << image2 << std::endl;
	  for(int l = 0; l < 4; ++l){
	    for( int m = 0; m < 4; ++m){
	      samples.ptr<float>(y/8 + (x/8)*(int(image.size().height - 7)/8 + 1), l*4 + m) = image2.ptr<float>(l,m);
	    }
	  }
      }
    }
//     
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
