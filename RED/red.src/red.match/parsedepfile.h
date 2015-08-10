/*
 * =====================================================================================
 *
 *       Filename:  parsedepfile.h
 *
 *    Description:  parse the file output by Penn2Me
 *
 *        Version:  1.0
 *        Created:  13/09/12 16:11:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __PARSEDEPFILE_H__
#define __PARSEDEPFILE_H__
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
vector<string> parseDepfile(string filename);

#endif
