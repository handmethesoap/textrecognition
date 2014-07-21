
#include "FileReader.hh"
#include "Dictionary.hh"
#include "TextRecognition.hh"
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>


int main( int argc, char** argv )
{
    //check correct command line inputs and store parameter file name
    CHECK_MSG(argc == 4,"The command line requires an input of the parameter file name, a boolean input specifying whether to generate a new dictionary or not and a boolean input specifiying whether to generate SVM train data");
    std::string parameterfile = argv[1];
    std::string generatedictionary = argv[2];
    std::string generatetraindata = argv[3];
    
    
    //Start timer
    timeval start;
    timeval end;
    gettimeofday(&start, NULL);
    long int operation_time = 0;
    
    //Read in paramterfile
    FileReader read;    
    read.registerIntParameter("dictionary_length");
    read.registerIntParameter("dictionary_source_images");
    read.registerIntParameter("dictionary_train_images");
    read.registerIntParameter("kmeans_attempts");
    read.registerIntParameter("kmeans_iter");
    read.registerRealParameter("kmeans_eps");
    read.registerStringParameter("no_text_recognition_train_path");
    read.registerStringParameter("text_recognition_train_path");
    read.registerStringParameter("recognition_test_path");
    read.registerStringParameter("dictionary_path");
    read.registerStringParameter("dictionary_save_path");
    read.registerIntParameter("numtextsamples");
    read.registerIntParameter("numnotextsamples");
    read.registerStringParameter("traindata");
    read.registerStringParameter("test_file");
    read.registerStringParameter("test_result_file");
    CHECK_MSG( read.readFile(parameterfile), "Could not read config file");
    read.registerIntParameter("min_window_size");
    read.registerIntParameter("max_window_size");
    read.registerIntParameter("window_step_size");
    
    //depending on command line parameter generate or read dictionary
    
/*    
    cv::Mat1f matrix;
    cv::Mat1f vector;
    
    for(int i = 1; i < 11; ++i){
      matrix.push_back(i);
    }
    cv::Mat1f matrix2 = matrix.reshape(0,2);
    
    for(int j = 1; j < 6; ++j){
      vector.push_back(j);
    }
    
    cv::Mat1f vector3 = vector.reshape(0,5);
    
    std::cout << matrix2 << std::endl;
    std::cout << vector3 << std::endl;
    
    std::cout << matrix.type() << std::endl;
    std::cout << vector.type() << std::endl;
    
    cv::Mat1f test = matrix2*vector3;
    cv::Mat1f result;
    result.push_back( test );
    result.push_back( test );
    std::cout << result << std::endl;*/
    
    Dictionary dict(read);
    TextRecognition recogniser(read, dict);
    
    if(generatedictionary == "1"){
      dict.generate();
    }
    else if(generatedictionary == "0"){
      dict.read();
    }
    else{
      std::cout << "command line input for generating dictionary must be a boolean value" << std::endl;
    }
    
    std::cout << "dictionary created or loaded, proceeding to text recognition" << std::endl;
    
    if(generatetraindata == "1"){
      recogniser.train();
    }
    else if( generatetraindata == "0"){
      recogniser.load();
    }
    else{
      std::cout << "command line input for generating training data must be a boolean value" << std::endl;
    }
    
    recogniser.test();
    
//     if(1){
//       
//       read.setParameter("dictionary_save_path", "dictionaryimages64/");
//       read.setParameter("traindata", "traindata64.txt");
//       read.setParameter("dictionary_length", 64);
//       Dictionary dict1(read);
//       TextRecognition recogniser1(read, dict1);
//       dict1.generate();
//       recogniser1.train();
//     }
//     
//     if(1){
//       
//       read.setParameter("dictionary_save_path", "dictionaryimages200/");
//       read.setParameter("traindata", "traindata200.txt");
//       read.setParameter("dictionary_length", 200);
//       Dictionary dict2(read);
//       TextRecognition recogniser2(read, dict2);
//       dict2.generate();
//       recogniser2.train();
//     }
//     
//     if(1){
//       
//       read.setParameter("dictionary_save_path", "dictionaryimages500/");
//       read.setParameter("traindata", "traindata500.txt");
//       read.setParameter("dictionary_length", 500);
//       Dictionary dict3(read);
//       TextRecognition recogniser3(read, dict3);
//       dict3.generate();
//       recogniser3.train();
//     }
//     
//     if(1){
//       
//       read.setParameter("dictionary_save_path", "dictionaryimages1000/");
//       read.setParameter("traindata", "traindata1000.txt");
//       read.setParameter("dictionary_length", 1000);
//       Dictionary dict4(read);
//       TextRecognition recogniser4(read, dict4);
//       dict4.generate();
//       recogniser4.train();
//     }
    

    
    //Output operation time
    gettimeofday(&end, NULL);
    operation_time = (end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec;
    std::cout << "Operating time (us): " << operation_time << std::endl;

    return 0;
}
