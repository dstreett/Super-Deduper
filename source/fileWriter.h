#ifndef SOURCE_FILEWRITER_H_
#define SOURCE_FILEWRITER_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


#include "readInfo.h"

#define F_OK 0

/*This class is just a basic helper class for writing out the data
 * the bools will determine the functionality of the writer*/
class FileWriter {


    private:
        bool tab;
        bool fastq;
        bool interleaved;
        bool gzip;
        bool to_stdout;
        bool force;
    public:
        FILE *fOut;
        void setGzipped(bool b) {gzip = b;};
        void setForce(bool b) {force = b;};

        void setFastq(bool b);
        void setInterleaved(bool b);
        void setStdout(bool b);
        void setTab(bool b);


        FileWriter(bool force);


        void writeData(readInfo *R1, readInfo *R2, readInfo *SE);
        void OpenFile(char *fName);

};




#endif
