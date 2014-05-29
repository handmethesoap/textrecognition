
#include "FileReader.hh"
#include "Dictionary.hh"
#include "TextRecognition.hh"
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>


int main( int argc, char** argv )
{
    //check correct command line inputs and store parameter file name
    std::string parameterfile = "recognitiontestparameters.par";
   
    //Start timer
    timeval start;
    timeval end;
    gettimeofday(&start, NUL    if(generatetraindata == "1"){L);
    long int operation_time = 0;
    
    //Read in paramterfile
    FileReader read;    
    read.registerIntParameter("dictionary_length");
    read.registerIntParameter("dictionary_source_images");
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
    read.registerStringParameter("test_file");
    read.registerStringParameter("test_result_file");
    CHECK_MSG( read.readFile(parameterfile), "Could not read config file");
    
    
    //depending on command line parameter generate or read dictionary
   
    
    Dictionary dict(read);
    TextRecognition recogniser(read, dict);
    
    dict.read();

    std::cout << "dictionary loaded, proceeding to text recognition" << std::endl;
    
    recogniser.train();
   
    recogniser.test();

    //Output operation time
    gettimeofday(&end, NULL);
    operation_time = (end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec;
    std::cout << "Operating time (us): " << operation_time << std::endl;

    return 0;
}
