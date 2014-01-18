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
  
  cv::Mat traindata;
  cv::Mat traindatatype;
  int numtextsamples = 0;
  int numnotextsamples = 0;
  
  cv::SVMParams svmparams;
  svmparams.svm_type    = CvSVM::C_SVC;
  svmparams.kernel_type = CvSVM::LINEAR;
  svmparams.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
  
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
	if((numtextsamples < parameters.getIntParameter("numtextsamples")) && (numnotextsamples < parameters.getIntParameter("numnotextsamples"))){
	  for(int i = 0; i < image.size().width - 31; i+=32){
	    for(int j = 0; j < image.size().height -31; j+=32){
	      
	      bool datatype = isText(j,i,textboxes);
	      
	      if( (datatype == 1 && numtextsamples < parameters.getIntParameter("numtextsamples")) || (datatype == 0 && numnotextsamples < parameters.getIntParameter("numnotextsamples")) ){
		
		traindatatype.push_back(datatype);
		if(datatype == 1){
		  ++numtextsamples;
		}
		else{
		  ++numnotextsamples;
		}
		
		std::cout << numtextsamples << ", " << numnotextsamples << std::endl;
	      
		cv::Mat subimage = image(cv::Range(j,j+32), cv::Range(i,i+32));
		cv::Mat featurerepresentation;
		cv::Mat reducedfeaturerepresentation;
		
		//extract 8*8 subsubimages
		for(int k = 0; k < subimage.size().width - 7; ++k){
		  for( int l = 0; l < subimage.size().height - 7; ++l){
		    
		    //extract subsubimages
		    cv::Mat subsubimage = subimage(cv::Range(l,l+8), cv::Range(k,k+8)).clone();
		    
		    //normalise and zca whiten
		    normalise( subsubimage );
		    
		    //compute dot product with dictionary
		    for( int m = 0; m < parameters.getIntParameter("dictionary_length"); ++m){
		      featurerepresentation.push_back((subsubimage.reshape(0, 1)).dot(dict.centers.row(m)));
		    }
		    
		  }
		}
		
		featurerepresentation = featurerepresentation.reshape(0,25);  
		reduceFeatures(featurerepresentation, reducedfeaturerepresentation);
		
    // 	    std::cout << featurerepresentation << std::endl;
    //  	    std::cout << reducedfeaturerepresentation << std::endl;
		
		traindata.push_back(reducedfeaturerepresentation.reshape(0,1));
	      }
	    }
	  }
	}
	
	std::cout << parameters.getStringParameter("recognition_train_path") + file << std::endl;
      }
    }
  }
  
  traindata.convertTo(traindata, CV_32F);
  traindatatype.convertTo(traindatatype, CV_32F);
  std::cout << "TRAINING..." << std::endl;
  bool something = linearSVM.train(traindata, traindatatype.reshape(0,1), cv::Mat(), cv::Mat(), svmparams);
  
  printTrainData(traindata, traindatatype);
  std::cout << something << std::endl;
  
  cv::Mat testdata;
  cv::Mat testdatalabels;
  
  //readTrainData(testdata, testdatalabels);
  
}

void TextRecognition::load( void ){
  
  cv::Mat traindata;
  cv::Mat traindatatype;
  
  cv::SVMParams svmparams;
  svmparams.svm_type    = CvSVM::C_SVC;
  svmparams.kernel_type = CvSVM::LINEAR;
  svmparams.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
  
  std::cout << "loading training data" << std::endl;
  
  readTrainData(traindata, traindatatype);
  traindata.convertTo(traindata, CV_32F);
  traindatatype.convertTo(traindatatype, CV_32F);
  
  std::cout << "loading complete" << std::endl;
  
  std::cout << "linear SVM training underway" << std::endl;
  
  bool output = linearSVM.train(traindata, traindatatype, cv::Mat(), cv::Mat(), svmparams);
  
  if( output == 1 ){
    std::cout << "linear SVM successfully trained" << std::endl;
  }
  else{
    std::cout << "linear SVM training failed" << std::endl;
  }

  
}
  
void TextRecognition::reduceFeatures( cv::Mat & featurerepresentation, cv::Mat & reducedfeaturerepresentation ){
  for( int i = 0; i < parameters.getIntParameter("dictionary_length"); ++i){
    for(int k = 0; k < 3; ++k){
      for(int l = 0; l < 3; ++l){
	reducedfeaturerepresentation.push_back( cv::sum(featurerepresentation(cv::Range(k*8, k*8+9), cv::Range(i*25 + l*8, i*25 + l*8+9)))[0] );
      }
    }
  }
  reducedfeaturerepresentation = reducedfeaturerepresentation.reshape(0,3);
}

void TextRecognition::normalise( cv::Mat & matrix ){
  
  //convert grayscale to floating point for processing
  matrix.convertTo(matrix, CV_32F);
  
  //brightness normalise
  cv::Scalar matmean;
  cv::Scalar stddev;
  cv::meanStdDev(matrix, matmean, stddev);
  cv::subtract(matrix, matmean[0], matrix);
  
  
  //apply ZCA whitening
  cv::Mat sigma, w_, u, vt;
  cv::Mat subimagetranspose, utranspose;
  cv::transpose(matrix, subimagetranspose);
  sigma = matrix*subimagetranspose/8;
  cv::SVD::compute(sigma, w_, u, vt);
  cv::transpose(u, utranspose);
  w_ = w_+0.1;
  cv::sqrt(w_,w_);
  w_ = 1./w_;
  cv::Mat D(8,8,CV_32F); 
  D = D.diag(w_);
  matrix = u*D*utranspose*matrix;

  //contrast normalise and remap to grayscale range
  cv::meanStdDev(matrix, matmean, stddev);
  if( stddev[0] != 0.0 ){
    matrix = matrix*(127.0/(1.0*stddev[0]));
    cv::add(matrix, 128, matrix);
  }
  matrix.convertTo(matrix, CV_8UC1);  
  
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

void TextRecognition::printTrainData(cv::Mat traindata, cv::Mat traindatatype){
  std::ofstream outputfile;
  outputfile.open( parameters.getStringParameter("traindata") );
  
  outputfile << traindata << std::endl;
  outputfile << "data type" << std::endl;
  outputfile << traindatatype << std::endl;
  
  outputfile.close();
}

void TextRecognition::readTrainData(cv::Mat & traindata, cv::Mat & traindatatype){
  std::ifstream infile;
  infile.open( parameters.getStringParameter("traindata") );
  cv::Mat *outputmatrix = &traindata;
  int num;
  
   while (!infile.eof()){
    std::string line, param;
    std::string file;
    std::stringstream tt;
    cv::Mat row;
    getline(infile,line);
    tt<<line;
    while( tt>>param ){
      if(param == "data"){
	outputmatrix = &traindatatype;
	break;
      }
      
      if( param[0] == '[' ){
	std::istringstream(param.substr(param.find('[')+1)) >> num;
	row.push_back(num);
      }
      else if( param.find(']') != std::string::npos ){
	std::istringstream(param.erase(param.find(']'))) >> num;
	row.push_back(num);
	break;
      }
      else{
	std::istringstream(param) >> num;
	row.push_back(num);
      }
     }
     if((param != "data") && (param != "")){
      outputmatrix->push_back(row.reshape(0,1));
     }
   }    
  
  infile.close();
}

void TextRecognition::test(void){
 //open text file containing image file data
  std::string name = parameters.getStringParameter("recognition_test_path") + "locations.xml";
  std::ifstream infile(name.c_str());
  CHECK_MSG(infile.good(),"Error reading '" << name << "'.  Please check file exists and is named correctly");
  
  int numimages = 0;
  
  //Read image file names
  while (!infile.eof()){
    std::string line, param;
    std::string file;
    std::stringstream tt;
    getline(infile,line);
    tt<<line;
    while( tt>>param && numimages < 1 ){
      
      //get image file name
      if(param == "<image>" ){
	getline(infile,line);
	tt<<line;
	tt>>param;
	file = param.substr(param.find('>')+1);
	file = file.erase(file.find('<'));
	
	file = "ryoungt_05.08.2002/testtest.JPG";
	
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
	
	cv::Mat image = cv::imread(  parameters.getStringParameter("recognition_test_path") + file , cv::IMREAD_GRAYSCALE );
	if(! image.data ){
	  std::cout << "could not read image " << parameters.getStringParameter("recognition_test_path") + file << std::endl;
	}
	
	std::cout << "testing with file " << file << std::endl;
	numimages++;
	
	int windowsize = 32;
	
	//extract 32*32 subimages
	for(int i = 0; i <= image.size().width - windowsize; ++i){
	  for(int j = 0; j <= image.size().height -windowsize; ++j){
	    
	      cv::Mat subimage = image(cv::Range(j,j+windowsize), cv::Range(i,i+windowsize));
	      cv::Mat featurerepresentation;
	      cv::Mat reducedfeaturerepresentation;
	      cv::Mat scaledsubimage(32,32,CV_8UC1);
		
	      cv::resize(subimage, scaledsubimage, scaledsubimage.size());
	      
	      //extract 8*8 subsubimages
	      for(int k = 0; k < scaledsubimage.size().width - 7; ++k){
		for( int l = 0; l < scaledsubimage.size().height - 7; ++l){
		  
		  //extract subsubimages
		  cv::Mat subsubimage = scaledsubimage(cv::Range(l,l+8), cv::Range(k,k+8)).clone();
		  
		  //normalise and zca whiten
		  normalise( subsubimage );
		  
		  //compute dot product with dictionary
		  for( int m = 0; m < parameters.getIntParameter("dictionary_length"); ++m){
		    featurerepresentation.push_back((subsubimage.reshape(0, 1)).dot(dict.centers.row(m)));
		  }
		  
		}
	      }
	      
	      featurerepresentation = featurerepresentation.reshape(0,25);  
	      reduceFeatures(featurerepresentation, reducedfeaturerepresentation);
	      
	      reducedfeaturerepresentation.convertTo(reducedfeaturerepresentation, CV_32F);
	      
	      std::cout << linearSVM.predict(reducedfeaturerepresentation.reshape(0,1)) << "; (" << i << ", " << j << ")" << std::endl;
	    }
	  }
	}
      }
      
      std::cout << parameters.getStringParameter("recognition_train_path") + file << std::endl;
    }
  }



