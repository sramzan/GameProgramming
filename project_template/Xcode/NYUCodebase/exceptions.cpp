//
//  exceptions.cpp
//  
//
//  Created by Sean Ramzan on 2/11/16.
//
//

#include <stdio.h>
#include <exception>
#include <string>

using namespace std;
class LoadException: public exception {
public:
    explicit LoadException(const string& message);
    virtual const char* what() const throw() {
        return exceptionMessage.c_str();
    }
    
private:
    string exceptionMessage;
};