#pragma once

#ifndef RegressionTests_h__
#define RegressionTests_h__

#include <string>

void regressionTests(const std::string& dirPath);
void dropCSVInfo(std::ostream& out, const std::string& dirPath);

#endif