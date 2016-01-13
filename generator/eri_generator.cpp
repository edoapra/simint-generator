#include <iostream>
#include <memory>
#include <stdexcept>
#include <fstream>

#include "generator/Helpers.hpp"
#include "generator/Algorithms.hpp"
#include "generator/Boys.hpp"
#include "generator/WriterInfo.hpp"
#include "generator/VRR_Writer.hpp"
#include "generator/ET_Writer.hpp"
#include "generator/HRR_Writer.hpp"
#include "generator/FileWriter.hpp"

using namespace std;


int main(int argc, char ** argv)
{
    try {

    OptionMap options = DefaultOptions();

    // other stuff
    std::string boystype;
    std::string fpath;
    std::string cpuflags;
    std::string datdir;
    QAM amlist;

    bool amlistset = false;

    // parse command line
    std::vector<std::string> otheropt = ParseCommonOptions(options, argc, argv);

    // parse specific options
    size_t iarg = 0;
    while(iarg < otheropt.size())
    {
        std::string argstr(GetNextArg(iarg, otheropt));
        if(argstr == "-o")
            fpath = GetNextArg(iarg, otheropt);
        else if(argstr == "-c")
            cpuflags = GetNextArg(iarg, otheropt);
        else if(argstr == "-d")
            datdir = GetNextArg(iarg, otheropt);
        else if(argstr == "-b")
            boystype = GetNextArg(iarg, otheropt);
        else if(argstr == "-q")
        {
            amlist[0] = GetIArg(iarg, otheropt);   
            amlist[1] = GetIArg(iarg, otheropt);   
            amlist[2] = GetIArg(iarg, otheropt);   
            amlist[3] = GetIArg(iarg, otheropt);   
            amlistset = true;
        }
        else
        {
            std::cout << "\n\n";
            std::cout << "--------------------------------\n";
            std::cout << "Unknown argument: " << argstr << "\n";
            std::cout << "--------------------------------\n";
            return 1; 
        } 
    }


    // check for required options
    if(boystype == "")
    {
        std::cout << "\nBoys type (-b) required\n\n";
        return 2;
    }

    if(fpath == "")
    {
        std::cout << "\noutput path (-o) required\n\n";
        return 2;
    }

    if(datdir == "")
    {
        std::cout << "\ndat directory (-d) required\n\n";
        return 2;
    }

    if(amlistset == false)
    {
        std::cout << "\nAM quartet (-q) required\n\n";
        return 2;
    }

    if(cpuflags == "")
    {
        std::cout << "\nCPU flags required\n\n";
        return 2;
    }

    if(options[OPTION_INTRINSICS] != 0 && options[OPTION_SCALAR] != 0)
    {
        std::cout << "\nUsing intrinsics with a scalar calculation doesn't make sense...\n\n";
        return 2;
    }


    // open the output file
    std::ofstream of(fpath);
    if(!of.is_open())
    {
        std::cout << "Cannot open file: " << fpath << "\n";
        return 2; 
    }
    

    // Base writer information
    WriterInfo::Init(options, amlist, cpuflags);


    // is this one a special permutation?
    if( ( (amlist[0] == 0 && amlist[1] > 0)  && ( amlist[2] == 0 || amlist[3] == 0 ) ) ||
        ( (amlist[2] == 0 && amlist[3] > 0)  && ( amlist[0] == 0 || amlist[1] == 0 ) ) )
        WriteFile_Permute(of);
    else
    {
        // Read in the boys map
        std::unique_ptr<BoysGen> bg;

        if(boystype == "FO")
            bg = std::unique_ptr<BoysGen>(new BoysFO(datdir));
        else if(boystype == "split")
            bg = std::unique_ptr<BoysGen>(new BoysSplit());
        else if(boystype == "vref")
            bg = std::unique_ptr<BoysGen>(new BoysVRef());
        else
        {
            std::cout << "Unknown boys type \"" << boystype << "\"\n";
            return 3;
        }

        // algorithms used
        std::unique_ptr<HRR_Algorithm_Base> hrralgo(new Makowski_HRR(options));
        std::unique_ptr<VRR_Algorithm_Base> vrralgo(new Makowski_VRR(options));
        std::unique_ptr<ET_Algorithm_Base> etalgo(new Makowski_ET(options));

        // Working backwards, I need:
        // 1.) HRR Steps
        hrralgo->Create(amlist);
        HRR_Writer hrr_writer(*hrralgo);

        // 2.) ET steps
        //     with the HRR top level stuff as the initial targets
        DoubletType dir = DoubletType::KET;
        if((amlist[2]+amlist[3]) > (amlist[0]+amlist[1]))
            dir = DoubletType::BRA;

        etalgo->Create(hrralgo->TopQuartets(), dir);
        ET_Writer et_writer(*etalgo);

        // 3.) VRR Steps
        // requirements for vrr are the top level stuff from ET
        vrralgo->Create(etalgo->TopQuartets());
        VRR_Writer vrr_writer(*vrralgo);

        // set the contracted quartets
        WriterInfo::SetContQ(hrralgo->TopAM());

        // print out some info
        std::cout << "MEMORY (per shell quartet): " << WriterInfo::MemoryReq() << "\n";

        WriteFile(of, *bg, vrr_writer, et_writer, hrr_writer);
    }
    }
    catch(std::exception & ex)
    {
        cout << "\n\n";
        cout << "Caught exception\n";
        cout << "What = " << ex.what() << "\n\n";
        return 100;
    }
    return 0;
}
