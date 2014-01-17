#include "TextRecognition.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

void TextRecognition::train(void){
  
  //open text file containing image file data
  std::string name = parameters.getStringParameter("recognition_train_path") + "locations.xml";
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
      
      //get image file name
      if(param == "<image>" ){
	getline(infile,line);
	tt<<line;
	tt>>param;
	file = param.substr(param.find('>')+1);
	file = file.erase(file.find('<'));
	
	std::vector<cv::Rect*> textboxes;
	int x,y,w,h;
	std::string value;
	
	//get textbox bounds of image
	while( param != "</taggedRectangles>" ){
	
	  if(param == "<taggedRectangle" ){

	    while( param[0] != 'x' ){
	    tt>>param;
	    }
	    value = param.substr(param.find('"')+1);
	    value = value.erase(value.find('"'));
	    x =  int(atof(value.c_str()));
	    
	    while( param[0] != 'y' ){
	    tt>>param;
	    }
	    value = param.substr(param.find('"')+1);
	    value = value.erase(value.find('"'));
	    y =  int(atof(value.c_str()));
	    
	    while( param[0] != 'w' ){
	    tt>>param;
	    }
	    value = param.substr(param.find('"')+1);
	    value = value.erase(value.find('"'));
	    w =  int(atof(value.c_str()));
	    
	    while( param[0] != 'h' ){
	    tt>>param;
	    }
	    value = param.substr(param.find('"')+1);
	    value = value.erase(value.find('"'));
	    h =  int(atof(value.c_str()));
	    
	    textboxes.push_back(new cv::Rect(x,y,w,h));
	  }
	  getline(infile,line);
	  tt<<line;
	  tt>>param;
	}
	
	cv::Mat image = cv::imread(  parameters.getStringParameter("recognition_train_path") + file , cv::IMREAD_GRAYSCALE );
	if(! image.data ){
	  std::cout << "could not read image " << parameters.getStringParameter("recognition_train_path") + file << std::endl;
	}
	
	//extract 32*32 subimages
	for(int i = 0; i < image.size().width - 31; ++i){
	  for(int j = 0; j < image.size().height -31; ++j){
	    
	    cv::Mat subimage = image(cv::Range(j,j+32), cv::Range(i,i+32));
	    cv::Mat featurerepresentation;
	    
	    //extract 8*8 subsubimages
	    for(int k = 0; k < subimage.size().width - 7; ++k){
	      for( int l = 0; l < subimage.size().height - 7; ++l){
		cv::Mat subsubimage = subimage(cv::Range(l,l+8), cv::Range(k,k+8)).clone();
		
		//convert grayscale to floating point for processing
		subsubimage.convertTo(subsubimage, CV_32F);
		
		//brightness normalise
		cv::Scalar matmean;
		cv::Scalar stddev;
		cv::meanStdDev(subsubimage, matmean, stddev);
		cv::subtract(subsubimage, matmean[0], subsubimage);
		
		
		//apply ZCA whitening
		cv::Mat sigma, w_, u, vt;
		cv::Mat subimagetranspose, utranspose;
		cv::transpose(subsubimage, subimagetranspose);
		sigma = subsubimage*subimagetranspose/8;
		cv::SVD::compute(sigma, w_, u, vt);
		cv::transpose(u, utranspose);
		w_ = w_+0.1;
		cv::sqrt(w_,w_);
		w_ = 1./w_;
		cv::Mat D(8,8,CV_32F); 
		D = D.diag(w_);
		subsubimage = u*D*utranspose*subsubimage;

		//contrast normalise and remap to grayscale range
		cv::meanStdDev(subsubimage, matmean, stddev);
		if( stddev[0] != 0.0 ){
		  subsubimage = subsubimage *(127.0/(1.0*stddev[0]));
		  cv::add(subsubimage, 128, subsubimage);
		}
		subsubimage.convertTo(subsubimage, CV_8UC1);
		
		double z = (subsubimage.reshape(64, 1)).dot(dict.centers.row(0));
		featurerepresentation.push_back(z);
		
	      }
	      
	    }
	    std::cout << featurerepresentation.rows << ", " << featurerepresentation.cols << std::endl;
	    std::cout << featurerepresentation.reshape(625, 1) << std::endl;
	  }
	}
	
	std::cout << parameters.getStringParameter("recognition_train_path") + file << std::endl;
      }
    }
  }
}

void TextRecognition::printIsText(cv::Mat image, std::vector<cv::Rect*> textboxes){
  for(int i = 0; i < image.size().width - 31; ++i){
    for(int j = 0; j < image.size().height -31; ++j){
      if(!isText(i,j, textboxes)){
	image.ptr<float>(j,i)[1] = 255;
	image.ptr<float>(j,i)[2] = 255;
	image.ptr<float>(j,i)[3] = 255;
      }
      
    }
    std::cout << double(i+1)/double(image.size().width - 31) << "%" << std::endl;
  }
//   cv::Point lb(257, 433);
//   cv::Point rb(751, 433);
//   cv::Point lt(257, 524);
//   cv::Point rt(751, 524);
//   cv::Scalar color(0,0,255);
//   cv::line(image, lb, rb, color);
//   cv::line(image, lb, lt, color);
//   cv::line(image, rt, lt, color);
//   cv::line(image, rb, rt, color);
//   cv::Point lb2(68, 56);
//   cv::Point rb2(923, 56);
//   cv::Point lt2(68, 413);
//   cv::Point rt2(923, 413);
//   cv::line(image, lb2, rb2, color);
//   cv::line(image, lb2, lt2, color);
//   cv::line(image, rt2, lt2, color);
//   cv::line(image, rb2, rt2, color);
  
  std::cout << image.type() << std::endl;
  cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  cv::imshow( "Display window", image);
  
  std::cout << image.size().width << " " << image.size().height << std::endl;
  cv::waitKey(0);
}

bool TextRecognition::isText( int x, int y, std::vector<cv::Rect*> textboxes){
  
  int istextsum = 0;
  //std::cout << "here1.1" << std::endl;
//   for( int i = x; i < (x + 32) ; ++i ){
//     for( int j = y; j < (y + 32); ++j ){
//       for( auto it =  textboxes.begin(); it != textboxes.end(); ++it ){
// 	if( (i >= (**it).x)  && (i <= ((**it).x + (**it).w)) ){
// 	  if( (j >= (**it).y) && (j <= ((**it).y + (**it).h))){
// 	    ++istextsum;
// 	    break;
// 	  }
// 	}
//       }
//     }
//   }
  cv::Rect box(x,y,32, 32);
  for( auto it =  textboxes.begin(); it != textboxes.end(); ++it ){
    cv::Rect temprect = box & **it;
    istextsum += temprect.area();
  }
  
  //return true if over 80% of the block is text
  if(istextsum > 819){
    return 1;
  }
  else{
    return 0;
  }
  
}








