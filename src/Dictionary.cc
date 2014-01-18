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
  
  int numimages = 0;
  
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
    while( tt>>param && numimages < 6000 ){
      if(param == "<image" ){
	tt>>param;
	file = param.substr(param.find('"')+1);
	file = file.erase(file.find('"'));
    
	cv::Mat image(32, 32, CV_8UC1);
	cv::Mat originalimage;
	
	std::cout << parameters.getStringParameter("dictionary_path") + file << std::endl;
	
	//read in image file as gray scale
	originalimage = cv::imread(  parameters.getStringParameter("dictionary_path") + file , cv::IMREAD_GRAYSCALE );
	if(! originalimage.data ){
	  std::cout << "could not read image " << parameters.getStringParameter("dictionary_path") + file << std::endl;
	}
	
	++numimages;
	
	cv::resize(originalimage, image, image.size());
	
	//extract 8x8 subimages
	for(int x = 0; x < image.size().width - 7; ++x){
	  for( int y = 0; y < image.size().height - 7; ++y){
	    cv::Mat subimage(8,8,CV_32F);
	    subimage = image(cv::Range(y,y+8), cv::Range(x,x+8)).clone();
	    
	    //convert grayscale to floating point for processing
	    subimage.convertTo(subimage, CV_32F);
	    
	    //brightness normalise
	    cv::Scalar matmean;
	    cv::Scalar stddev;
	    cv::meanStdDev(subimage, matmean, stddev);
	    cv::subtract(subimage, matmean[0], subimage);
	    
	    
	    //apply ZCA whitening
	    cv::Mat sigma, w, u, vt;
	    cv::Mat subimagetranspose, utranspose;
	    cv::transpose(subimage, subimagetranspose);
	    sigma = subimage*subimagetranspose/8;
	    cv::SVD::compute(sigma, w, u, vt);
	    cv::transpose(u, utranspose);
	    w = w+0.1;
	    cv::sqrt(w,w);
	    w = 1./w;
	    cv::Mat D(8,8,CV_32F); 
	    D = D.diag(w);
	    subimage = u*D*utranspose*subimage;

	    //contrast normalise and remap to grayscale range
	    cv::meanStdDev(subimage, matmean, stddev);
	    if( stddev[0] != 0.0 ){
	      subimage = subimage *(127.0/(1.0*stddev[0]));
	      cv::add(subimage, 128, subimage);
	    }
	    subimage.convertTo(subimage, CV_8UC1);
 	    
	    
	    //add processed subimage to sample matrix to be passed to kmeans algorithim
	    //int sz = subimage.cols*subimage.rows;
	    samples.push_back((subimage.reshape(0, 1)));
	  }
	}
	
      }
    }
  
  }
  std::cout << "Performing kmeans calculation" << std::endl;
  
  //convert samples to floating point for insertion into kmeans function
  samples.convertTo(samples, CV_32F);
  
  cv::kmeans(samples, parameters.getIntParameter("dictionary_length"), labels, criteria, parameters.getIntParameter("kmeans_attempts"), cv::KMEANS_PP_CENTERS, centers);
  
  centers.convertTo(centers, CV_8UC1);
  std::cout << centers << std::endl;
  std::cout << centers.rows << ", " << centers.cols << std::endl;
    
  printfiles();
}


void Dictionary::printfiles(){
  
  
  for( int j = 0; j < parameters.getIntParameter("dictionary_length"); ++ j){
    cv::Mat image;
    std::stringstream tt;
    std::string name;
    
    for( int i = 0; i <= 7 ; ++i ){
      image.push_back(centers(cv::Range(j, j+1), cv::Range(i*8,i*8+8)));
    }
    //std::cout << image << std::endl;
    
    tt << parameters.getStringParameter("dictionary_save_path") << j << ".jpg";
    tt >> name; 
    
    imwrite(name, image);
  }  
}

void Dictionary::read(){
  
  //int sz = 64;
  
  for(int i = 0; i < parameters.getIntParameter("dictionary_length"); ++ i){
    cv::Mat image;
    std::stringstream tt;
    std::string name;
    
    tt << parameters.getStringParameter("dictionary_save_path") << i << ".jpg";
    tt >> name;
    
    image = cv::imread(  name , cv::IMREAD_GRAYSCALE );
    if(! image.data ){
      std::cout << "could not read image " << name << std::endl;
    }
    
    centers.push_back((image.reshape(0, 1)));
  }
  centers.convertTo(centers, CV_8UC1);
  //std::cout << centers << std::endl;
}
  


//   std::vector<Rectangle*> textboxes;
//   
//   getTextBoxes(parameters.getStringParameter("image_path_1"),  "apanar_06.08.2002/IMG_1305.JPG", textboxes );
//   
//   for( auto it = textboxes.begin(); it != textboxes.end(); ++it ){
//     (**it).print();
//   }
  // //   cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
// //   cv::imshow( "Display window", image);
// //   cv::namedWindow( "Sub window", cv::WINDOW_AUTOSIZE );
// //   cv::imshow( "Sub window", subimage);
//   
//   cv::waitKey(0);
  
//   cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
//   cv::imshow( "Display window", image);
//   cv::namedWindow( "Sub window", cv::WINDOW_AUTOSIZE );
//   cv::imshow( "Sub window", subimage);
//   
//   cv::waitKey(0);









