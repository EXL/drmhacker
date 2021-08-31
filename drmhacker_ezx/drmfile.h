// drmfile.h 
// EZX DRM file reading
// corresponds to libezxaid.so
// compile with -lezxaid -lezxtapi

#ifndef _DRMFILE_H_
#define _DRMFILE_H_
#include <qfile.h>

class Q_EXPORT J2ME_JAID_DRMFile: public QFile
{
  public:
    J2ME_JAID_DRMFile();
    J2ME_JAID_DRMFile(QString const&);
    virtual ~J2ME_JAID_DRMFile();
    
    virtual bool open ( int ); 
    virtual void close (); 
    virtual void flush (); 
    virtual uint size () const;
    virtual bool at ( int ); 
    virtual int readBlock ( char * data, uint len ); 
    virtual int readLine ( char * data, uint maxlen ); 
    virtual int getch (); 
    virtual int ungetch ( int );
};

/*
J2ME_JAID_DRMFile::J2ME_JAID_DRMFile()
J2ME_JAID_DRMFile::J2ME_JAID_DRMFile(QString const&)
J2ME_JAID_DRMFile::at(int)
J2ME_JAID_DRMFile::close()
J2ME_JAID_DRMFile::flush()
J2ME_JAID_DRMFile::getch()
J2ME_JAID_DRMFile::init()
J2ME_JAID_DRMFile::open(int)
J2ME_JAID_DRMFile::readBlock(char*, unsigned int)
J2ME_JAID_DRMFile::readLine(char*, unsigned int)
J2ME_JAID_DRMFile::size() const
J2ME_JAID_DRMFile::ungetch(int)
J2ME_JAID_DRMFile::~J2ME_JAID_DRMFile()
*/

#endif //drmfile.h
