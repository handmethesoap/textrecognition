
#include "FileReader.hh"
#include "Dictionary.hh"
#include "Image.hh"
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>


int main( int argc, char** argv )
{
    //check correct command line inputs and store parameter file name
    CHECK_MSG(argc == 3,"The command line requires an input of the parameter file name and a boolean input specifying whether to generate a new dictionary or not");
    std::string parameterfile = argv[1];
    std::string generatedictionary = argv[2];
    
    
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
    read.registerStringParameter("image_path_1");
    read.registerStringParameter("dictionary_path");
    read.registerStringParameter("dictionary_save_path");
    read.registerIntParameter("read_dictionary");
    CHECK_MSG( read.readFile(parameterfile), "Could not read config file");
    read.printParameters();    
    
    //depending on command line parameter generate or read dictionary
    if(generatedictionary == "1"){
      Image im(read, "apanar_06.08.2002/Img_1305.jpg");
      im.generateDictionaries();
    }
    else if(generatedictionary == "0"){
      Image im(read, "apanar_06.08.2002/Img_1305.jpg");
      im.readDictionaries();
    }
    else{
      std::cout << "command line input for generating dictionary must be a boolean value" << std::endl;
    }
    Image im(read, "apanar_06.08.2002/Img_1305.jpg");
    im.readDictionaries();
    //dict.generate();
    
    //Output operation time
    gettimeofday(&end, NULL);
    operation_time = (end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec;
    std::cout << "Operating time (us): " << operation_time << std::endl;

    return 0;
}
