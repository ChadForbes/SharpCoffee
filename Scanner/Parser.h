#pragma once
#include "Scanner.h"

class Parser
{
public:
	int ParseStatement();

private:
	Scanner m_Scanner;
};