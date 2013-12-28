
#include "FileReader.hh"
#include "Dictionary.hh"
#include "Image.hh"
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>


int main( int argc, char** argv )
{
    //check correct command line inputs and store parameter file name
    CHECK_MSG(argc == 2,"The command line requires a single input of the parameter file name");
    std::string parameterfile = argv[1];
    
    
    //Start timer
    timeval start;
    timeval end;
    gettimeofday(&start, NULL);
    long int operation_time = 0;
    
    //Read in paramterfile
    FileReader read;    
    read.registerIntParameter("dictionary_length");
    read.registerStringParameter("image_path_1");
    read.registerStringParameter("dictionary_path");
    CHECK_MSG( read.readFile(parameterfile), "Could not read config file");
    read.printParameters();    
    
    Image im(read, "apanar_06.08.2002/Img_1305.jpg");
    im.generateDictionaries();
    
    //dict.generate();
    
    //Output operation time
    gettimeofday(&end, NULL);
    operation_time = (end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec;
    std::cout << "Operating time (us): " << operation_time << std::endl;

    return 0;
}
