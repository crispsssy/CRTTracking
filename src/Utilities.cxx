#include "Utilities.hxx"

int FindPar(std::string const& fileName, std::string const& parameterName){
    int parameter = 0;
    auto index = fileName.find(parameterName);
    if(index != std::string::npos){
        size_t start = index + parameterName.size();
        size_t end = start;
        while(end < fileName.size() && std::isdigit(fileName[end])) ++end;
        parameter = stoi(fileName.substr(start, end - start));
    }
    return parameter;
}
