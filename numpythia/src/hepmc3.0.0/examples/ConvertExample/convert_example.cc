// -*- C++ -*-
#define HEPMCCONVERT_EXTENSION_ROOTTREEOPAL
#define HEPMCCONVERT_EXTENSION_HEPEVTZEUS
#include "HepMC/GenEvent.h"
#include "HepMC/Reader.h"
#include "HepMC/Writer.h"
#include "HepMC/ReaderAsciiHepMC2.h"
#include "HepMC/ReaderAscii.h"
#include "HepMC/WriterAscii.h"
#include "HepMC/Print.h"
#include "HepMC/GenEvent.h"
#include "HepMC/WriterHEPEVT.h"
#include "HepMC/ReaderHEPEVT.h"

#ifdef HEPMC_ROOTIO
#include "HepMC/ReaderRoot.h"
#include "HepMC/WriterRoot.h"
#include "HepMC/ReaderRootTree.h"
#include "HepMC/WriterRootTree.h"
#endif

/* Extension example*/

#ifdef HEPMCCONVERT_EXTENSION_ROOTTREEOPAL
#ifndef HEPMC_ROOTIO
#warning "HEPMCCONVERT_EXTENSION_ROOTTREEOPAL requires  compilation with of HepMC with ROOT, i.e. HEPMC_ROOTIO.This extension will be disabled."
#undef HEPMCCONVERT_EXTENSION_ROOTTREEOPAL
#else
#include "WriterRootTreeOPAL.h"
#endif
#endif
#ifdef HEPMCCONVERT_EXTENSION_HEPEVTZEUS  
#include "WriterHEPEVTZEUS.h"
#endif


#include <iostream>
#include <vector>
#include <string>
#include <limits>

using namespace HepMC;
using std::cout;
using std::endl;
enum formats {hepmc2, hepmc3, hpe
#ifdef HEPMC_ROOTIO
              ,root,treeroot
#endif
              /* Extension example*/
#ifdef HEPMCCONVERT_EXTENSION_ROOTTREEOPAL
              ,treerootopal
#endif
#ifdef HEPMCCONVERT_EXTENSION_HEPEVTZEUS              
              ,hpezeus
#endif
             };

struct parsedoption
{
    bool    fbool;
    int      fint;
    float  ffloat;
};

//http://stackoverflow.com/questions/599989/is-there-a-built-in-way-to-split-strings-in-c
void tokenize(const std::string& str, const std::string& delimiters , std::vector<std::string>& tokens)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
        {
            // Found a token, add it to the vector.
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of(delimiters, pos);
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }
}



int main(int argc, char** argv)
{
    if (argc!=4&&argc!=5)
        {
            printf("\
     Usage: %s  <mode> <input_file.input_extension>    <output_file.output_extension>  [optional list of options]\n\
     Supported modes are: hepmc2_root hepmc3_root etc.\n\
     List of options should be given as option1=value1:option2=value2:option3=value3 ...\n",argv[0]);
            exit(1);
        }

    std::string opts="NONE";
    if (argc==5) opts=std::string(argv[4]);
    std::map<std::string, parsedoption> options;
    std::vector<std::string> parsedoptions;
    if (opts!="NONE")
        {
            tokenize(opts,":",parsedoptions);
            for ( std::vector<std::string>::iterator it=parsedoptions.begin(); it!=parsedoptions.end(); it++)
                {
                    std::vector<std::string> parsedname;
                    parsedoption o;
                    tokenize(*it,"=",parsedname);
                    if (parsedname.size()<2) {printf("Error parsing option/value pair: ->%s<-\n",it->c_str()); exit(2);};
                    int parsing_isok=0;
                    parsing_isok+=sscanf(parsedname[1].c_str(),"%i",&o.fint);
                    parsing_isok+=sscanf(parsedname[1].c_str(),"%f",&o.ffloat);
                    if (parsing_isok==0) {printf("Error converting value ->%s<- to int of float number.\n",parsedname[1].c_str()); exit(3);};
                    options.insert(std::pair<std::string, parsedoption>(parsedname[0],o));
                }

            for (std::map<std::string, parsedoption>::iterator it=options.begin(); it!=options.end(); it++)
                printf("%s  %i %f\n",it->first.c_str(),it->second.fint,it->second.ffloat);
        }

    std::map<std::string,formats> format_map;
    format_map.insert(std::pair<std::string,formats> ( "hepmc2", hepmc2 ));
    format_map.insert(std::pair<std::string,formats> ( "hepmc3", hepmc3 ));
    format_map.insert(std::pair<std::string,formats> ( "hpe", hpe  ));
#ifdef HEPMC_ROOTIO
    format_map.insert(std::pair<std::string,formats> ( "root", root ));
    format_map.insert(std::pair<std::string,formats> ( "treeroot", treeroot ));
#endif
    /* Extension example*/
#ifdef HEPMCCONVERT_EXTENSION_ROOTTREEOPAL
    format_map.insert(std::pair<std::string,formats> ( "treerootopal", treerootopal ));
#endif
#ifdef HEPMCCONVERT_EXTENSION_HEPEVTZEUS
    format_map.insert(std::pair<std::string,formats> ( "hpezeus", hpezeus ));
#endif

    std::map<std::string,std::string> extention_map;
    extention_map.insert(std::pair<std::string,std::string> ( "hepmc2","hepmc2" ));
    extention_map.insert(std::pair<std::string,std::string> ( "hepmc3", "hepmc3" ));
    extention_map.insert(std::pair<std::string,std::string> ( "hpe", "hpe" ));
#ifdef HEPMC_ROOTIO
    extention_map.insert(std::pair<std::string,std::string> ( "root", "root" ));
    extention_map.insert(std::pair<std::string,std::string> ( "treeroot", "root" ));
#endif
    /* Extension example*/
#ifdef HEPMCCONVERT_EXTENSION_ROOTTREEOPAL
    extention_map.insert(std::pair<std::string,std::string> ( "treerootopal", "root" ));
#endif
#ifdef HEPMCCONVERT_EXTENSION_HEPEVTZEUS
    extention_map.insert(std::pair<std::string,std::string> ( "hpezeus", "zeusmc" ));
#endif



    std::vector<std::pair <std::string,std::string> > convert_list;
    std::pair<std::string,std::string> convert_formats;
    std::string mode(argv[1]);
    unsigned int i,j=0;
    for (i=0; i<mode.size(); i++)
        if (mode[i]!='_')
            {
                if (j==0) convert_formats.first+=mode[i];
                if (j==1) convert_formats.second+=mode[i];
                if (j>1)  {printf("Wrong mode string: %s\nMode string should be <format>_<format>.\n",argv[1]); exit(1);}
            }
        else j++;

    if (format_map.find(convert_formats.first )==format_map.end()) { printf("Input format %s is unknown.\n",convert_formats.first.c_str()); exit(2); }
    if (format_map.find(convert_formats.second)==format_map.end()) { printf("Output format %s is unknown.\n",convert_formats.second.c_str()); exit(2); }
    convert_list.push_back(std::pair<std::string,std::string>(std::string(argv[2]),std::string(argv[3])));
    if (
        (     convert_list.back().first.substr(convert_list.back().first.find_last_of(".") + 1)   != extention_map.at(convert_formats.first))
        ||
        (convert_list.back().second.substr(convert_list.back().second.find_last_of(".") + 1) != extention_map.at(convert_formats.second))
    )
        { printf("The conversion mode=%s is not suitable for extensions of %s %s files\n",argv[1],argv[2],argv[3]); exit(1);}

#ifdef HEPMCCONVERT_EXTENSION_ROOTTREEOPAL
    int Run=0;
#endif
    /*So far this is size 1, but it is better to keep as an option for an extension*/
    for (i=0; i<convert_list.size(); i++)
        {
            int events_parsed = 0;
            int events_limit = std::numeric_limits<int>::max();
            if (options.find("events_limit")!=options.end())  events_limit=options.at("events_limit").fint;
            int first_event_number = -1;
            if (options.find("first_event_number")!=options.end())  first_event_number=options.at("first_event_number").fint;
            int last_event_number  = std::numeric_limits<int>::max();
            if (options.find("last_event_number")!=options.end())  last_event_number=options.at("last_event_number").fint;
            int print_each_events_parsed=100;
            if (options.find("print_each_events_parsed")!=options.end())  last_event_number=options.at("print_each_events_parsed").fint;
            Reader*      input_file=0;

            switch (format_map.at(convert_formats.first))
                {
                case hepmc2:
                    input_file=new ReaderAsciiHepMC2(convert_list[i].first);
                    break;
                case hepmc3:
                    input_file=new ReaderAscii(convert_list[i].first);
                    break;
                case hpe:
                    input_file=new ReaderHEPEVT(convert_list[i].first);
                    break;
#ifdef HEPMC_ROOTIO
                case treeroot:
                    input_file=new ReaderRootTree(convert_list[i].first);
                    break;
                case root:
                    input_file=new ReaderRoot(convert_list[i].first);
                    break;
#endif
                default:
                    printf("Input format %s is unknown.\n",convert_formats.first.c_str());
                    exit(2);
                    break;
                }
            Writer*      output_file=0;
            switch (format_map.at(convert_formats.second))
                {
                case hepmc2:
                    printf("WARNING: hepmc3 format will be used instead of hepmc2.\n");
                    output_file=new WriterAscii(convert_list[i].second.c_str());
                    break;
                case hepmc3:
                    output_file=new WriterAscii(convert_list[i].second.c_str());
                    break;
                case hpe:
                    output_file=new WriterHEPEVT(convert_list[i].second);
                    break;
#ifdef HEPMC_ROOTIO
                case root:
                    output_file=new WriterRoot(convert_list[i].second);
                    break;
                case treeroot:
                    output_file=new WriterRootTree(convert_list[i].second);
                    break;
#endif                    
                    /* Extension example*/
#ifdef HEPMCCONVERT_EXTENSION_ROOTTREEOPAL
                case treerootopal:
                    output_file=new WriterRootTreeOPAL(convert_list[i].second);
                    ((WriterRootTreeOPAL*)(output_file))->init_branches();

                    if (options.find("Run")!=options.end()) Run=options.at("Run").fint;
                    ((WriterRootTreeOPAL*)(output_file))->set_run_number(Run);
                    break;
#endif
#ifdef HEPMCCONVERT_EXTENSION_HEPEVTZEUS
                case hpezeus:
                    output_file=new WriterHEPEVTZEUS(convert_list[i].second);
                    break;
#endif
                default:
                    printf("Output format %s is unknown.\n",convert_formats.second.c_str());
                    exit(2);
                    break;
                }
            while( !input_file->failed() )
                {
                    GenEvent evt(Units::GEV,Units::MM);
                    input_file->read_event(evt);
                    if( input_file->failed() )  {printf("End of file reached. Exit.\n"); break;}
                    if (evt.event_number()<first_event_number) continue;
                    if (evt.event_number()>last_event_number) continue;
                    output_file->write_event(evt);
                    evt.clear();
                    ++events_parsed;
                    if( events_parsed%print_each_events_parsed == 0 ) cout<<"Events parsed: "<<events_parsed<<endl;
                    if( events_parsed >= events_limit ) {printf("Event limit reached:->events_parsed(%i) >= events_limit(%i)<-. Exit.\n",events_parsed , events_limit); break;}
                }

            if (input_file) input_file->close();
            if (output_file)
                output_file->close();
        }
    return 0;
}
