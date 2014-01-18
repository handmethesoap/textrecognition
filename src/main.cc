
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
    read.registerIntParameter("kmeans_attempts");
    read.registerIntParameter("kmeans_iter");
    read.registerRealParameter("kmeans_eps");
    read.registerStringParameter("recognition_train_path");
    read.registerStringParameter("recognition_test_path");
    read.registerStringParameter("dictionary_path");
    read.registerStringParameter("dictionary_save_path");
    read.registerIntParameter("numtextsamples");
    read.registerIntParameter("numnotextsamples");
    read.registerStringParameter("traindata");
    CHECK_MSG( read.readFile(parameterfile), "Could not read config file");
    
    
    //depending on command line parameter generate or read dictionary
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
    
    //Output operation time
    gettimeofday(&end, NULL);
    operation_time = (end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec;
    std::cout << "Operating time (us): " << operation_time << std::endl;

    return 0;
}
