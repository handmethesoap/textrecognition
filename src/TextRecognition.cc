#include "TextRecognition.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

void TextRecognition::train(void){
  
  cv::Mat traindata;
  cv::Mat traindatatype;
  int numtextsamples = 0;
  int numnotextsamples = 0;
  
  std::vector<std::string> filenames;
  std::vector<std::vector<cv::Rect*>*> textboxes;
  
  cv::SVMParams svmparams;
  svmparams.svm_type    = CvSVM::C_SVC;
  svmparams.kernel_type = CvSVM::LINEAR;
  svmparams.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
  

  //Read image file names
  getTrainImages(filenames, textboxes);
  
  for( int k = 0; k < parameters.getIntParameter("dictionary_train_images"); ++k){
    cv::Mat1f image = cv::imread(  filenames[k] , cv::IMREAD_GRAYSCALE );
    if(! image.data ){
      std::cout << "could not read image " << filenames[k] << std::endl;
      std::cout << filenames.size() << std::endl;
    }
  
    //extract 32*32 subimages
    if((numtextsamples < parameters.getIntParameter("numtextsamples")) && (numnotextsamples < parameters.getIntParameter("numnotextsamples"))){
      for(int i = 0; i < image.size().width - 31; i+=32){
	for(int j = 0; j < image.size().height -31; j+=32){
	  
	  bool datatype = isText(i,j,*textboxes[k]);
	  
	  //check if the required number of text and non text samples have been processed
	  if( (datatype == 1 && numtextsamples < parameters.getIntParameter("numtextsamples")) || (datatype == 0 && numnotextsamples < parameters.getIntParameter("numnotextsamples")) ){
	    
	    traindatatype.push_back(datatype);
	    if(datatype == 1){
	      ++numtextsamples;
	    }
	    else{
	      ++numnotextsamples;
	    }
	    
	    
	    
	    cv::Mat1f subimage = image(cv::Range(j,j+32), cv::Range(i,i+32));
	    cv::Mat1f reducedfeatures;
	    computeFeatureRepresentation(subimage, reducedfeatures);
	    
	    traindata.push_back(reducedfeatures.reshape(0,1));
	    
	  }
	}
      }
    }
    std::cout << numtextsamples << ", " << numnotextsamples << std::endl;
  }
  
  //std::cout << parameters.getStringParameter("recognition_train_path") + file << std::endl;

  printTrainData(traindata, traindatatype);
  traindata.convertTo(traindata, CV_32F);
  traindatatype.convertTo(traindatatype, CV_32F);
  std::cout << "TRAINING..." << std::endl;
  bool something = linearSVM.train(traindata, traindatatype.reshape(0,1), cv::Mat(), cv::Mat(), svmparams);


  std::cout << something << std::endl;

  cv::Mat testdata;
  cv::Mat testdatafilelabels;
  
  for( std::vector<std::vector<cv::Rect*>*>::iterator it1 = textboxes.begin(); it1 != textboxes.end(); ++it1 ){
    for( std::vector<cv::Rect*>::iterator it2 = (**it1).begin(); it2 != (**it1).end(); ++it2 ){
      delete *it2;
    }
    delete *it1;
  }

}

void TextRecognition::test(void){
  
  //read in test image
  std::cout << "loading test image" << std::endl;
  
  cv::Mat1f image = cv::imread(  parameters.getStringParameter("recognition_test_path") + parameters.getStringParameter("test_file") , cv::IMREAD_GRAYSCALE );
  if(! image.data ){ // or if( image.empty() ) {
    std::cout << "could not read image " << parameters.getStringParameter("recognition_test_path") + parameters.getStringParameter("test_file") << std::endl;
  }
  //scale image to 0-1 range
  image = image/255;
  
  cv::Mat1f textresult = cv::Mat1f::zeros(image.size());
  
  std::cout << "testing with file " << parameters.getStringParameter("test_file") << std::endl;
  
  // TODO (optional): maybe rescale whole image in different sizes and then run the algorithm
  // for each of the resized images
  
  //test for text with varying window size
  
  std::cout << parameters.getIntParameter("min_window_size") << ", " << parameters.getIntParameter("max_window_size") << ", " << parameters.getIntParameter("window_step_size") << std::endl;
  for(int windowsize = parameters.getIntParameter("min_window_size"); windowsize<= parameters.getIntParameter("max_window_size"); windowsize+= parameters.getIntParameter("window_step_size")){
    std::cout << "searching for text with window size " << windowsize << std::endl;
    
    cv::Mat1f results;
    
    //extract windowsize*windowsize subimages
    for(int j = 0; j <= image.size().height - windowsize; j+=10){			
      for(int i = 0; i <= image.size().width - windowsize; i+=10){
	
	cv::Mat1f subimage = image(cv::Range(j,j+windowsize), cv::Range(i,i+windowsize)).clone();
	
	cv::Mat1f scaledsubimage(32,32);
	cv::resize(subimage, scaledsubimage, scaledsubimage.size());
	
	cv::Mat reducedfeatures; 
	computeFeatureRepresentation(scaledsubimage, reducedfeatures);
	reducedfeatures.convertTo(reducedfeatures, CV_32F);
	results.push_back(((/*-*/(linearSVM.predict(reducedfeatures.reshape(0,1))))/**128/2 +128*/));
      }
    }
    
    
    // 		for(int j = 0; j < image.size().height; ++j){
    // 			for(int i = 0; i < image.size().width; ++i){
    // 				int x1 = i/10;
    // 				std::cout << x1 << std::endl;
    // 				while((x1*10+windowsize > i)  && (x1 >= 0)){
    // 					if(results.at<uchar>(j/10, x1) > textresult.at<uchar>(j,i)){
    // 						textresult.at<uchar>(j,i) = results.at<uchar>(j/10, x1); // TODO: results(j/10, x1), or results.at<int> 
    // 					}
    // 					--x1;
    // 				}
    // 				int y1 = j/10;
    // 				while((y1*10+windowsize > i) && (y1 >= 0)){
    // 					if(results.at<uchar>(y1, i/10) > textresult.at<uchar>(j,i)){
    // 						textresult.at<uchar>(j,i) = results.at<uchar>(y1, i/10); // s.a.
    // 					}
    // 					--y1;
    // 				}
    // 			}
    // 		}
    
    results = results.reshape(0,(image.size().height- windowsize)/10+1);
    
    std::cout << results << std::endl;

    
//     for( int j = 0; j < results.rows; ++j ){
//       for( int i = 0; i < results.cols; ++i ){
// 	for( int k = 0; k < windowsize; ++k ){
// 	  for( int l = 0; l < windowsize; ++l ){
// 	    if( textresult(j*10 + k, i*10 + l) < results(j,i) )
// 	      textresult(j*10 + k, i*10 + l) = results(j,i);
// 	  }
// 	}
//       }
//     }
    
    
    //std::cout << results.reshape(0,(image.size().height- windowsize)/10+1) << std::endl;
    std::cout << "Window Size = " << windowsize << std::endl << std::endl;
    
  }
  
//   cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
//   cv::imshow( "Display window", textresult);
//   cv::waitKey(0);
//   imwrite(parameters.getStringParameter("test_result_file"), textresult);
		  
}

void TextRecognition::getTrainImages(std::vector<std::string>& filenames, std::vector<std::vector<cv::Rect*>*>& textboxes){
  
  //open text file containing image file data
  std::string name = parameters.getStringParameter("recognition_train_path") + "locations.xml";
  std::ifstream infile(name.c_str());
  CHECK_MSG(infile.good(),"Error reading '" << name << "'.  Please check file exists and is named correctly");
  int numimages = 0;
  
  
  
  //Read image file names
  while (!infile.eof() && numimages < parameters.getIntParameter("dictionary_train_images")){
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
	
	filenames.push_back( parameters.getStringParameter("recognition_train_path") + file);
	textboxes.push_back( new std::vector<cv::Rect*> );
		
	++numimages;
	
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

	    (*textboxes[numimages - 1]).push_back(new cv::Rect(x,y,w,h));

	  }
	  getline(infile,line);
	  tt<<line;
	  tt>>param;
	}
      }
    }
  }
}

void TextRecognition::computeFeatureRepresentation(cv::Mat1f & subimage, cv::Mat & reducedfeatures ){
  
  // TODO: make this float, then dot product will not overflow //DONE
  // maybe cv::Mat1f
  cv::Mat1f features;
  
  //extract 8*8 subsubimages

    for(int k = 0; k < subimage.size().width - 7; ++k){
      for( int l = 0; l < subimage.size().height - 7; ++l){
	
	//extract subsubimages
	cv::Mat1f subsubimage = subimage(cv::Range(l,l+8), cv::Range(k,k+8)).clone();
	
	//normalise and zca whiten
	// TODO: normalize correctly, i.e. zca whitening over all (or many random) train patches
	// save w and u and use that w and u to transform now this particular subimage
	normalise( subsubimage );
	
	//compute dot product with dictionary
	// possible speedup: use gemm (i.e. matrix multiplication)
	features.push_back((subsubimage.reshape(0, 64)));
	
	// TODO: max(..., alpha)  
      }
    }

  
  features = features.reshape(0,625);
  //std::cout << "features size = " << features.size() << std::endl;
  cv::Mat1f features2;
  
  for( int m = 0; m < parameters.getIntParameter("dictionary_length"); ++m){
    cv::Mat1f temp = features*(dict.centers.row(m).reshape(0,64));
    features2.push_back(temp);
  }
  
  //reshape so each row is the result the dot product of each 8*8 subsubimage with a single dictionary element 
  features2 = features2.reshape(0,parameters.getIntParameter("dictionary_length"));  
  //std::cout << "size = " << features2.size() << std::endl;
  // TODO: test shape of reducedfeatures //DONE
  reduceFeatures(features2, reducedfeatures);
}

void TextRecognition::reduceFeatures( cv::Mat1f & featurerepresentation, cv::Mat & reducedfeatures ){
  
  for( int i = 0; i < parameters.getIntParameter("dictionary_length"); ++i){
    
    cv::Mat1f dictmatrix = featurerepresentation.row(i);
    dictmatrix = dictmatrix.reshape(0,25);
    
    for(int k = 0; k < 3; ++k){
      for(int l = 0; l < 3; ++l){
	
	reducedfeatures.push_back( cv::sum(dictmatrix(cv::Range(k*8, k*8+9), cv::Range(l*8,l*8+9)))[0] );
	
      }
    }
  }
  //std::cout << reducedfeatures << std::endl << std::endl;
}

// TODO: don't scale to 0-255 but leave it as 0-1 // DONE
void TextRecognition::normalise( cv::Mat1f & matrix ){
  
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
  
  // FIXME
  //contrast normalise and remap to 0-1 range
  cv::meanStdDev(matrix, matmean, stddev);
  if( stddev[0] != 0.0 ){
    matrix = matrix*(0.5/(1.0*stddev[0]));
    cv::add(matrix, 0.5, matrix);
  }
  
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
  cv::Rect box(x,y,32, 32);
  
  //find area intersection between location and textboxes
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





