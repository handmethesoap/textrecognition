#include "Dictionary.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>

void Dictionary::generate(){
  
  cv::Mat samples;
  cv::Mat labels;
  cv::TermCriteria criteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, parameters.getIntParameter("kmeans_iter"), parameters.getRealParameter("kmeans_eps"));
  
  //open text file containing image file data
  std::string name = parameters.getStringParameter("dictionary_path") + "char.xml";
  std::ifstream infile(name.c_str());
  CHECK_MSG(infile.good(),"Error reading '" << name << "'.  Please check file exists and is named correctly");
  
  //Read image file names
  while (!infile.eof()){
    std::string line, param;
    std::string file;
    std::stringstream tt;
    getline(infile,line);
    tt<<line;
    while( tt>>param ){
      if(param == "<image" ){
	tt>>param;
	file = param.substr(param.find('"')+1);
	file = file.erase(file.find('"'));
    
	cv::Mat image;
	
	std::cout << parameters.getStringParameter("dictionary_path") + file << std::endl;
	
	//read in image file as gray scale
	image = cv::imread(  parameters.getStringParameter("dictionary_path") + file , cv::IMREAD_GRAYSCALE );
	if(! image.data ){
	  std::cout << "could not read image " << parameters.getStringParameter("dictionary_path") + file << std::endl;
	}
	
	//extract 8x8 subimages
	for(int x = 0; x < image.size().width - 7; x = x + 8){
	  for( int y = 0; y < image.size().height - 7; y = y + 8){
	    cv::Mat subimage(8,8,CV_32F);
	    subimage = image(cv::Range(y,y+8), cv::Range(x,x+8)).clone();
	    
	    std::cout << "before" << std::endl;
	    std::cout << image(cv::Range(y,y+8), cv::Range(x,x+8)) << std::endl; 
	    
	    //convert grayscale to floating point for processing
	    subimage.convertTo(subimage, CV_32F);
	    cv::Scalar matmean;
	    cv::Scalar stddev;
	    cv::meanStdDev(subimage, matmean, stddev);
	    cv::subtract(subimage, matmean[0], subimage);
	    //subimage = subimage/stddev[0];
	    
	    std::cout << "mean = " << matmean[0] << std::endl;
	    std::cout << subimage << std::endl;
	    
	    int sz = subimage.cols*subimage.rows;
	    samples.push_back((subimage.reshape(sz, 1)));
	  }
	}
	
	//std::cout << samples << std::endl;
// 	cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
// 	cv::imshow( "Display window", image);
// 	cv::waitKey(0);
      }
    }
  
  }
  std::cout << "here" << std::endl;
  
  //convert samples to floating point for insertion into kmeans function
  samples.convertTo(samples, CV_32F);
    
  cv::kmeans(samples, parameters.getIntParameter("dictionary_length"), labels, criteria, parameters.getIntParameter("kmeans_attempts"), cv::KMEANS_PP_CENTERS, centers);
  
  //centers.convertTo(centers, CV_8UC1);
  std::cout << centers << std::endl;
  std::cout << centers.rows << ", " << centers.cols << std::endl;
    
  printfiles();
}


void Dictionary::printfiles(){
  
  
  for( int j = 0; j < parameters.getIntParameter("dictionary_length"); ++ j){
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
//   cv::imshow( "Sub window", subimage);
//   
//   cv::waitKey(0);
    cv::Mat image;
    std::stringstream tt;
    std::string name;
    
    for( int i = 0; i <= 7 ; ++i ){
      image.push_back(centers(cv::Range(j, j+1), cv::Range(i*8,i*8+8)));
    }
    std::cout << image << std::endl;
    
    tt << "dictionaryimages/" << j << ".jpg";
    tt >> name; 
    
    imwrite(name, image);
  }
// //   cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
// //   cv::imshow( "Display window", image);
// //   cv::namedWindow( "Sub window", cv::WINDOW_AUTOSIZE );
// //   cv::imshow( "Sub window", subimage);
//   
//   cv::waitKey(0);
  
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
//   cv::imshow( "Sub window", subimage);
//   
//   cv::waitKey(0);









