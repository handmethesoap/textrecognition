#include "Image.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>

void Image::generateDictionaries(void){
  
  const cv::Mat & subimage = image(cv::Range(0,31), cv::Range(0,31));
  dict = new Dictionary(parameters, subimage, isText(0,0));
}

void Image::printIsText(void){
  for(int i = 0; i < image.size().width - 31; ++i){
    for(int j = 0; j < image.size().height -31; ++j){
      if(!isText(i,j)){
	image.ptr<float>(j,i)[1] = 255;
	image.ptr<float>(j,i)[2] = 255;
	image.ptr<float>(j,i)[3] = 255;
      }
      
    }
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

bool Image::isText( int x, int y){
  
  int istextsum = 0;
  
  for( int i = x; i < (x + 32) ; ++i ){
    for( int j = y; j < (y + 32); ++j ){
      for( auto it =  textboxes.begin(); it != textboxes.end(); ++it ){
	if( (i >= (**it).x)  && (i <= ((**it).x + (**it).w)) ){
	  if( (j >= (**it).y) && (j <= ((**it).y + (**it).h))){
	    ++istextsum;
	    break;
	  }
	}
      }
    }
  }
  
  //return true if over 80% of the block is text
  if(istextsum > 819){
    return 1;
  }
  else{
    return 0;
  }
  
}

void Image::getTextBoxes( const std::string & path){
  std::string name = path + "locations.xml";
  std::ifstream infile(name.c_str());
  CHECK_MSG(infile.good(),"Error reading '" << name << "'.  Please check file exists and is named correctly");
  
//   std::cout << "<imageName>" + file + "</imageName>" << std::endl;
  
  while (!infile.eof()){
    std::string line, param, value;
    int x,y,w,h;
    std::stringstream tt;
    getline(infile,line);
    tt<<line;
    while( tt>>param ){
      
      if( stringToUpper(param) == stringToUpper(("<imageName>" + file + "</imageName>")) ){

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
	    
	    textboxes.push_back(new Rectangle(x,y,w,h));
	  }
	  
	  getline(infile,line);
	  tt<<line;
	  tt>>param;
	
	}
      }
    }

	
    
  }
  infile.close();
  
}

std::string Image::stringToUpper(std::string s)
{
   for(unsigned int l = 0; l < s.length(); l++)
  {
    s[l] = char(toupper(s[l]));
  }
  
  return s;
}
