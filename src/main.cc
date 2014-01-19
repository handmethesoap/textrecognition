
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
    
    Dictionary dict1(read), dict2(read), dict3(read), dict4(read);
    TextRecognition recogniser1(read, dict), recogniser2(read, dict), recogniser3(read, dict), recogniser4(read, dict);
    
    read.setParameter("dictionary_save_path", "dictionaryimages64/");
    read.setParameter("traindata", "traindata64.txt");
    read.setParameter("dictionary_length", 64);
    dict1.generate();
    recogniser1.train();
    
    read.setParameter("dictionary_save_path", "dictionaryimages200/");
    read.setParameter("traindata", "traindata200.txt");
    read.setParameter("dictionary_length", 200);
    dict2.generate();
    recogniser2.train();
    
    read.setParameter("dictionary_save_path", "dictionaryimages500/");
    read.setParameter("traindata", "traindata500.txt");
    read.setParameter("dictionary_length", 500);
    dict3.generate();
    recogniser3.train();
    
    read.setParameter("dictionary_save_path", "dictionaryimages1000/");
    read.setParameter("traindata", "traindata1000.txt");
    read.setParameter("dictionary_length", 1000);
    dict4.generate();
    recogniser4.train();
    
//     if(generatedictionary == "1"){
//       dict.generate();
//     }
//     else if(generatedictionary == "0"){
//       dict.read();
//     }
//     else{
//       std::cout << "command line input for generating dictionary must be a boolean value" << std::endl;
//     }
//     
//     
//     if(generatetraindata == "1"){
//       recogniser.train();
//     }
//     else if( generatetraindata == "0"){
//       recogniser.load();
//     }
//     else{
//       std::cout << "command line input for generating training data must be a boolean value" << std::endl;
//     }
//     
//     recogniser.test();
    
    //Output operation time
    gettimeofday(&end, NULL);
    operation_time = (end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec;
    std::cout << "Operating time (us): " << operation_time << std::endl;

    return 0;
}
