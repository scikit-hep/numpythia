// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC3_READERFACTORY_H
#define HEPMC3_READERFACTORY_H

#include "HepMC3/ReaderAscii.h"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/ReaderHEPEVT.h"
#include "HepMC3/ReaderLHEF.h"
#include "HepMC3/ReaderPlugin.h"

#include <memory>
#include <string>
#include <sys/stat.h>
#include <string.h>

namespace HepMC3 {
std::shared_ptr<Reader> deduce_reader(std::istream &stream);	
/** @brief THis function will deduce the type of input file based on the name/URL and it's content and will return appropriate Reader*/
std::shared_ptr<Reader> deduce_reader(const std::string &filename)
{
    std::string libHepMC3rootIO="libHepMC3rootIO.so.3";
#ifdef __darwin__
    libHepMC3rootIO="libHepMC3rootIO.dydl";
#endif
#ifdef WIN32
    libHepMC3rootIO="HepMC3rootIO.dll";
#endif
    bool remote=false;
    bool pipe=false;
    if (filename.find("http://")!=std::string::npos) 	 remote=true;
    if (filename.find("https://")!=std::string::npos) 	 remote=true;
    if (filename.find("root://")!=std::string::npos) 	 remote=true;
    if (filename.find("gsidcap://")!=std::string::npos) remote=true;

    std::vector<std::string> head;
    if (!remote)
    {
        struct stat   buffer;
#ifdef WIN32
        if (!(stat (filename.c_str(), &buffer)==0))
#else
        if (!(stat (filename.c_str(), &buffer)==0 && (S_ISFIFO(buffer.st_mode)|| S_ISREG(buffer.st_mode) || S_ISLNK(buffer.st_mode))))
#endif
        {
            printf("Error  in deduce_reader: file %s does not exist or is not a regular file/FIFO/link\n",filename.c_str());
            return std::shared_ptr<Reader> (nullptr);
        }
        
        std::ifstream* file= new std::ifstream(filename);
         
        if(!file->is_open()) {
            printf("Error in deduce_reader: could not open file for testing HepMC version: %s\n",filename.c_str());
            file->close();
            return shared_ptr<Reader>(nullptr);
        }
#ifndef WIN32
        pipe=S_ISFIFO(buffer.st_mode);
        if (pipe) { printf("Info in deduce_reader: the file %s is a pipe\n",filename.c_str()); return deduce_reader(*file); }
#endif
        std::string line;
        size_t nonempty=0;
        while (std::getline(*file, line)&&nonempty<3) {
            if (line.empty()) continue;
            nonempty++;
            head.push_back(line);
        }
        file->close();
        if (file) delete file;
    }
    /* To assure there are at least two elements in the vector*/
    head.push_back("");
    head.push_back("");
    printf("Info in deduce_reader: Attempt ReaderRootTree for:  %s\n",filename.c_str());
    if( strncmp(head.at(0).c_str(),"root",4) == 0||remote)
        return   std::make_shared<ReaderPlugin>(filename,libHepMC3rootIO,std::string("newReaderRootTreefile"));
    if (!remote)
    {
        printf("Info in deduce_reader: Attempt ReaderAscii for:  %s\n",filename.c_str());
        if( strncmp(head.at(0).c_str(),"HepMC::Version",14) == 0 && strncmp(head.at(1).c_str(),"HepMC::Asciiv3",14)==0 )
            return std::shared_ptr<Reader>((Reader*) ( new ReaderAscii(filename)));
        printf("Info in deduce_reader: Attempt ReaderAsciiHepMC2 for: %s\n",filename.c_str());
        if( strncmp(head.at(0).c_str(),"HepMC::Version",14) == 0 && strncmp(head.at(1).c_str(),"HepMC::IO_GenEvent",18)==0 )
            return std::shared_ptr<Reader>((Reader*) ( new ReaderAsciiHepMC2(filename)));
        printf("Info in deduce_reader: Attempt ReaderLHEF for:  %s\n",filename.c_str());
        if( strncmp(head.at(0).c_str(),"<LesHouchesEvents",17) == 0)
            return std::shared_ptr<Reader>((Reader*) ( new ReaderLHEF(filename)));
        printf("Info in deduce_reader: Attempt ReaderHEPEVT for:  %s\n",filename.c_str());
        std::stringstream st_e(head.at(0).c_str());
        char attr=' ';
        bool HEPEVT=true;
        int m_i,m_p;
        while (true)
        {
            if (!(st_e>>attr)) {
                HEPEVT=false;
                break;
            }
            if (attr==' ') continue;
            if (attr!='E') {
                HEPEVT=false;
                break;
            }
            HEPEVT=static_cast<bool>(st_e>>m_i>>m_p);
            break;
        }
        if (HEPEVT) return std::shared_ptr<Reader>((Reader*) ( new ReaderHEPEVT(filename)));
    }
    printf("Info in deduce_reader: All attempts failed for:  %s\n",filename.c_str());
    return shared_ptr<Reader>(nullptr);
}


/** @brief THis function will deduce the type of input stream based on its content and will return appropriate Reader*/
std::shared_ptr<Reader> deduce_reader(std::istream &stream)
{
    std::vector<std::string> head;
    head.push_back("");
    size_t back=0;
    size_t backnonempty=0;
    while ( (back<200&&backnonempty<100)&&stream) {char c=stream.get(); back++; if (c=='\n') { if (head.back().length()!=0) head.push_back("");} else { head.back()+=c; backnonempty++;} }
    if (!stream)
    {
        printf("Info in deduce_reader: input stream is too short or invalid.\n");
        return shared_ptr<Reader>(nullptr);
    }

    for (size_t i=0; i<back; i++)  stream.unget();

    if( strncmp(head.at(0).c_str(),"HepMC::Version",14) == 0 && strncmp(head.at(1).c_str(),"HepMC::Asciiv3",14)==0 )
    {
        printf("Info in deduce_reader: Attempt ReaderAscii\n");
        return std::shared_ptr<Reader>((Reader*) ( new ReaderAscii(stream)));
    }

    if( strncmp(head.at(0).c_str(),"HepMC::Version",14) == 0 && strncmp(head.at(1).c_str(),"HepMC::IO_GenEvent",18)==0 )
    {
        printf("Info in deduce_reader: Attempt ReaderAsciiHepMC2\n");
        return std::shared_ptr<Reader>((Reader*) ( new ReaderAsciiHepMC2(stream)));
    }

    if( strncmp(head.at(0).c_str(),"<LesHouchesEvents",17) == 0)
    {
        printf("Info in deduce_reader: Attempt ReaderLHEF\n");
        return std::shared_ptr<Reader>((Reader*) ( new ReaderLHEF(stream)));
    }
    printf("Info in deduce_reader: Attempt ReaderHEPEVT\n");
    std::stringstream st_e(head.at(0).c_str());
    char attr=' ';
    bool HEPEVT=true;
    int m_i,m_p;
    while (true)
    {
        if (!(st_e>>attr)) {
            HEPEVT=false;
            break;
        }
        if (attr==' ') continue;
        if (attr!='E') {
            HEPEVT=false;
            break;
        }
        HEPEVT=static_cast<bool>(st_e>>m_i>>m_p);
        break;
    }
    if (HEPEVT) return std::shared_ptr<Reader>((Reader*) ( new ReaderHEPEVT(stream)));
    printf("Info in deduce_reader: All attempts failed\n");
    return shared_ptr<Reader>(nullptr);
}
}
#endif
