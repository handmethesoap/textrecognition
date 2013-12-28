#include "Dictionary.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>

void Dictionary::generate(){
   
  cv::InputArray samples;
  for(int i = 1; i< 10; ++i){
    
    cv::Mat image;
    std::string file;
    std::stringstream ss;
    
    ss << "1/" << i << ".jpg";
    ss >> file;
    
    std::cout << parameters.getStringParameter("dictionary_path") + file << std::endl;
    
    image = cv::imread(  parameters.getStringParameter("dictionary_path") + file , cv::IMREAD_COLOR );
    
    if(! image.data ){
      std::cout << "could not read image " << parameters.getStringParameter("dictionary_path") + file << std::endl;
    }
    
    for(int x = 0; x < image.size().width - 7; x = x+8){
      for( int y = 0; y < image.size().height - 7; y = y+8){
	
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
