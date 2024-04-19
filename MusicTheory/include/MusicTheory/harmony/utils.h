#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include <boost/algorithm/string.hpp>

namespace MusicTheory 
{
	namespace utils
	{
        static const int getNumberOfSharps(std::string str)
        {
            return std::ranges::count(str, '#');
        }

        static const int getNumberOfFlats(std::string str)
        {
            return std::ranges::count(str, 'b');
        }

        static const int getNumberOfAccidentals(std::string str) 
        {
            const int augs = getNumberOfSharps(str);
            const int dims = getNumberOfFlats(str);
            return augs - dims;
        }
        
        static std::vector<std::string> splitString(const std::string &toSplit, const std::string &with)
        {
            std::vector<std::string> split;
            boost::split(split, toSplit, boost::is_any_of(with));
            return split;
        }

        //static std::vector<std::string> splitString(const std::string& toSplit, const char* with)
        //{
        //    return splitString(toSplit, std::string(with));
        //}
    }
}