//Course: CS4308
//Instructor: Dr. Garrido
//Assignment: Course Project
//Written by Glenden Johnson
//Last modified: 4/3/2019

#include "SCLParser.h"
#include <iostream>
#include <memory>


namespace SCLInterpreter
{
	std::ofstream u_OutputFile("\SCLParserOutput.txt");

	std::map<std::string, std::shared_ptr<SCLFunctionImplementation>> SCLProgram::m_FunctionList = std::map<std::string, std::shared_ptr<SCLFunctionImplementation>>(); 
	std::vector<std::map<std::string, std::pair<int, std::shared_ptr<SCLDataType>>>> SCLParser::m_IdentValues = std::vector<std::map<std::string, std::pair<int, std::shared_ptr<SCLDataType>>>>();

	//SCLParser constructor
	SCLParser::SCLParser(std::string a_SourceFile)
	{
		m_Reader.reset(new SCLScanner(a_SourceFile));//Initialize scanner object.
	}

	//Other functions
	std::shared_ptr<SCLDataType> SCLParser::GetIdentValue(std::string a_IdentName)
	{
		std::map<std::string, std::pair<int, std::shared_ptr<SCLDataType>>>::iterator z_IdentLocation;
		for (int u = SCLParser::m_IdentValues.size() - 1; u >= 0; --u)
		{
			z_IdentLocation = SCLParser::m_IdentValues[u].find(a_IdentName);
			if (z_IdentLocation != SCLParser::m_IdentValues[u].end())
			{
				return z_IdentLocation->second.second;
			}
		}
		return std::shared_ptr<SCLDataType>();
	}
	bool SCLParser::SetIdentValue(std::string a_IdentName, std::shared_ptr<SCLDataType> a_Value)
	{
		std::map<std::string, std::pair<int, std::shared_ptr<SCLDataType>>>::iterator z_IdentLocation;
		for (int u = SCLParser::m_IdentValues.size() - 1; u >= 0; --u)
		{
			z_IdentLocation = SCLParser::m_IdentValues[u].find(a_IdentName);
			if (z_IdentLocation != SCLParser::m_IdentValues[u].end())
			{
				z_IdentLocation->second.second = a_Value;
				return true;
			}
		}
		return false;
	}
	bool SCLParser::AddIdent(std::string a_Ident, int a_Type)
	{
		for (std::pair<std::string, std::map<std::string, int>> p : m_IdentScopes)
		{
			std::map<std::string, int>::iterator z_Existing = p.second.find(a_Ident);
			if (z_Existing != p.second.end())
			{
				return false;
			}
		}
		m_IdentScopes[m_IdentScopes.size() - 1].second.insert({ a_Ident, a_Type });
		return true;
	}
	int SCLParser::IdentType(std::string a_Ident)
	{
		for (std::pair<std::string, std::map<std::string, int>> p : m_IdentScopes)
		{
			std::map<std::string, int>::iterator z_Existing = p.second.find(a_Ident);
			if (z_Existing != p.second.end())
			{
				return z_Existing->second;
			}
		}
		return 0; //Identifier not declared.
	}

	//Factory Functions
	std::shared_ptr<SCLDataType> SCLDataTypeFactory(std::pair<int, std::string> a_Type)
	{
		if (a_Type.first == 100)
		{
			std::shared_ptr<SCLIdent> z_Answer(new SCLIdent());
			z_Answer->m_IdentName = a_Type.second;
			return z_Answer;
		}
		else
		{
			std::shared_ptr<SCLDataType> z_Answer(new SCLDataType());
			switch (a_Type.first)
			{
			case 5://Char literal
				z_Answer.reset(new SCLCharLiteral((a_Type.second.size() == 1) ? a_Type.second[0] : (char)0));
				break;
			case 1://Signed integer literal, fall through to next case.
			case 2://Unsigned integer literal
				z_Answer.reset(new SCLIntLiteral(atoi(a_Type.second.c_str())));
				break;
			case 3://Signed float literal, fall through to next case.
			case 4://Unsigned float literal
				z_Answer.reset(new SCLFloatLiteral(atof(a_Type.second.c_str())));
				break;
			case 6://String literal
				z_Answer.reset(new SCLStringLiteral(a_Type.second));
				break;
			case 121://Literal "true", fall through to next case.
			case 122://Literal "false"
				z_Answer.reset(new SCLBoolLiteral(a_Type.second == "true" ? true : false));
				break;
			}
			return z_Answer;
		}
	}
	std::shared_ptr<SCLDefineStatement> SCLDefineStatementFactory(std::string a_Ident, std::vector<std::pair<int, std::string>> a_Types)
	{
		if (a_Types.size() == 1)
		{
			std::shared_ptr<SCLDefineStatement> z_DefinedIdentifier(new SCLDefineStatement());
			z_DefinedIdentifier->m_IdentName = a_Ident;
			z_DefinedIdentifier->m_IdentType = a_Types[0].first;
			return z_DefinedIdentifier;
		}
		else
		{
			throw SCLParsingException("Invalid number of arguments.");
		}
	}

	//Parsing functions
	//Postcondition: The current lexeme is the end of file character.
	std::shared_ptr<SCLProgram> SCLParser::ParseProgram()
	{
		std::shared_ptr<SCLProgram> z_Program(new SCLProgram());
		m_TokenCode = m_Reader->NextLexeme();
		//Clear leading comments.
		while (m_TokenCode == 7)
		{
			u_OutputFile << BlockIndentation() << "Parsed comment:\n\t" << BlockIndentation() << m_Reader->m_CurrentLexeme << "\n";
			m_TokenCode = m_Reader->NextLexeme();
		}
		if (m_TokenCode == 181)
		{
			m_TokenCode = m_Reader->NextLexeme();
			//If the next token is not "declarations" raise an error.
			if (m_TokenCode != 133)
			{
				throw SCLParsingException("Error: Expecting \"declarations\". Found: " + m_Reader->m_CurrentLexeme);
			}
			u_OutputFile << BlockIndentation() << "Parsing global declarations...\n";
			m_IdentScopes.push_back({ "globals", std::map<std::string, int>() });
			z_Program->m_GlobalDeclarations = ParseDeclarations();
		}
		if (m_TokenCode == 182)
		{
			u_OutputFile << BlockIndentation() << "Parsing implementations...\n";
			SCLProgram::m_FunctionList = ParseImplementations();
		}
		return z_Program;
	}
	//Precondition: The current lexeme is the end of the previous statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLDeclarations> SCLParser::ParseDeclarations()
	{
		std::shared_ptr<SCLDeclarations> z_ParsingResult(new SCLDeclarations());
		
		m_TokenCode = m_Reader->NextLexeme();
		if (m_TokenCode == 135)
		{
			u_OutputFile << BlockIndentation() << "Parsing Variables...\n";
			z_ParsingResult->m_VariableDefinitions = ParseDefinitions();
		}
		return z_ParsingResult;
	}
	//Precondition: The current lexeme is the end of the previous statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::map<std::string, std::shared_ptr<SCLFunctionImplementation>> SCLParser::ParseImplementations()
	{
		std::map<std::string, std::shared_ptr<SCLFunctionImplementation>> z_ParsedFunctions;
		//Parse the list of function statements.
		for (m_TokenCode = m_Reader->NextLexeme(); m_TokenCode == 138; m_TokenCode = m_Reader->NextLexeme())
		{
			std::pair<std::string, std::shared_ptr<SCLFunctionImplementation>> z_ParsedFun = ParseFunction();
			z_ParsedFunctions.insert({ z_ParsedFun.first, z_ParsedFun.second });
		}
		return z_ParsedFunctions;
	}
	//Precondition: The current lexeme is the begining of this statement.
	//Postcondition: The current lexeme is the end of the returned statement.
	std::pair<std::string, std::shared_ptr<SCLFunctionImplementation>> SCLParser::ParseFunction()
	{
		std::pair<std::string, std::shared_ptr<SCLFunctionImplementation>> z_ParsedFunction;
		z_ParsedFunction.second.reset(new SCLFunctionImplementation());
		m_TokenCode = m_Reader->NextLexeme();
		if (m_TokenCode == 177) //If main function
		{
			u_OutputFile << BlockIndentation() << "Parsing function main...\n";
			z_ParsedFunction.first = "main";
			m_IdentScopes.push_back({"main", std::map<std::string, int>()});
			m_TokenCode = m_Reader->NextLexeme();
			if (m_TokenCode != 139)//if not "is"
			{
				throw SCLParsingException("Error: Expecting \"is\". Found: " + m_Reader->m_CurrentLexeme);
			}
			z_ParsedFunction.second->m_Declarations = ParseDeclarations();
			if (m_TokenCode != 140)//if not "begin"
			{
				throw SCLParsingException("Error: Expecting \"begin\". Found: " + m_Reader->m_CurrentLexeme);
			}
			m_TokenCode = m_Reader->NextLexeme();
			z_ParsedFunction.second->m_FunctionBody = ParseActions(141);
			return z_ParsedFunction;
		}
		else if (m_TokenCode == 100)//if ident
		{
			std::vector<std::pair<int, std::string>> z_Types;

			u_OutputFile << BlockIndentation() << "Parsing function " << m_Reader->m_CurrentLexeme << "...\n";
			z_ParsedFunction.first = m_Reader->m_CurrentLexeme;
			m_IdentScopes.push_back({ m_Reader->m_CurrentLexeme, std::map<std::string, int>() });
			m_TokenCode = m_Reader->NextLexeme();
			if (m_TokenCode != 147)//if not "return"
			{
				throw SCLParsingException("Error: Expecting \"return\". Found: " + m_Reader->m_CurrentLexeme);
			}
			m_TokenCode = m_Reader->NextLexeme();
			if (m_TokenCode == 179)//if "pointer"
			{
				m_TokenCode = m_Reader->NextLexeme();
				if (m_TokenCode != 124)//if not "of"
				{
					throw SCLParsingException("Error: Expecting \"of\". Found: " + m_Reader->m_CurrentLexeme);
				}
				z_ParsedFunction.second->m_ReturnPointer = true;
				m_TokenCode = m_Reader->NextLexeme();
			}
			if (m_TokenCode != 125)//If not "type"
			{
				throw SCLParsingException("Error: Expecting \"type\". Found: " + m_Reader->m_CurrentLexeme);
			}
			m_TokenCode = m_Reader->NextLexeme();
			if ((m_TokenCode >= 126 && m_TokenCode <= 131) || m_TokenCode == 178)//if valid type or "void"
			{
				z_Types.push_back({ m_TokenCode, m_Reader->m_CurrentLexeme });
				z_ParsedFunction.second->m_ReturnType = SCLDefineStatementFactory("", z_Types);
			}
			else
			{
				throw SCLParsingException("Error: Invalid type. Found: " + m_Reader->m_CurrentLexeme);
			}
			u_OutputFile << BlockIndentation() << "\treturn " << (z_ParsedFunction.second->m_ReturnPointer ? "pointer of " : "") << "type " << z_ParsedFunction.second->m_ReturnType->m_IdentType << "\n";
			m_TokenCode = m_Reader->NextLexeme();
			if (m_TokenCode == 142)//If "parameters"
			{
				u_OutputFile << BlockIndentation() << "Parsing parameters...\n";
				z_ParsedFunction.second->m_Parameters = ParseParameters();
				for (int j = 0; j < z_ParsedFunction.second->m_Parameters.size(); ++j)
				{
					u_OutputFile << BlockIndentation() << "\t" << z_ParsedFunction.second->m_Parameters[j]->m_IdentName << " of type " << z_ParsedFunction.second->m_Parameters[j]->m_IdentType << "\n";
				}
			}
			if (m_TokenCode != 139)//if not "is"
			{
				throw SCLParsingException("Error: Expecting \"is\". Found: " + m_Reader->m_CurrentLexeme);
			}
			z_ParsedFunction.second->m_Declarations = ParseDeclarations();
			if (m_TokenCode != 140)//if not "begin"
			{
				throw SCLParsingException("Error: Expecting \"begin\". Found: " + m_Reader->m_CurrentLexeme);
			}
			m_TokenCode = m_Reader->NextLexeme();
			z_ParsedFunction.second->m_FunctionBody = ParseActions(141);
			return z_ParsedFunction;
		}
		else
		{
			throw SCLParsingException("Error: Expecting a function identifier. Found: " + m_Reader->m_CurrentLexeme);
		}
	}

	//Precondition: The current lexeme is the end of the previous statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::vector<std::shared_ptr<SCLDefineStatement>> SCLParser::ParseDefinitions()
	{
		std::vector<std::shared_ptr<SCLDefineStatement>> z_ParsedDefinitions;
		//While the current token is "define"
		for (m_TokenCode = m_Reader->NextLexeme(); m_TokenCode == 123; m_TokenCode = m_Reader->NextLexeme())
		{
			z_ParsedDefinitions.push_back(ParseDataDeclaration());
			u_OutputFile << BlockIndentation() << z_ParsedDefinitions[z_ParsedDefinitions.size() - 1]->toString();
		}
		return z_ParsedDefinitions;
	}
	//Precondition: The current lexeme is the begining of the action block.
	//Postcondition: The current lexeme is the end of the parsed action block.
	std::vector<std::shared_ptr<SCLAction>> SCLParser::ParseActions(const int a_BlockType)
	{
		std::vector<std::shared_ptr<SCLAction>> z_ActionList;
		u_OutputFile << BlockIndentation() << "Parsing action list...\n";
		while(true)
		{
			if (m_TokenCode == a_BlockType)
			{
				if (a_BlockType == 141)//Function
				{
					m_TokenCode = m_Reader->NextLexeme();
				}
				//Check to make sure the program is closing the correct scope.
				if (m_Reader->m_CurrentLexeme != m_IdentScopes[m_IdentScopes.size() - 1].first)
				{
					throw SCLParsingException("Error: Invalid block terminator. Parsed block terminator: " + m_Reader->m_CurrentLexeme);
				}
				m_IdentScopes.pop_back();//Drop closed scope.
				break;
			}
			switch (m_TokenCode)
			{
			case 7://If valid comment
				u_OutputFile << BlockIndentation() << "\tParsed a comment:\n\t\t" << BlockIndentation() << m_Reader->m_CurrentLexeme << "\n";
				m_TokenCode = m_Reader->NextLexeme();
				break;
			case 143://If "set"
				z_ActionList.push_back(ParseAssignment());
				u_OutputFile << BlockIndentation() << "\tParsed an assignment statement:\n\t\t" << BlockIndentation() << z_ActionList[z_ActionList.size() - 1]->toString();
				break;
			case 144://If "display"
				z_ActionList.push_back(ParseDisplay());
				u_OutputFile << BlockIndentation() << "\tParsed a display statement:\n\t\t" << BlockIndentation() << z_ActionList[z_ActionList.size() - 1]->toString();
				break;
			case 145://If "increment"
				z_ActionList.push_back(ParseIncrement());
				u_OutputFile << BlockIndentation() << "\tParsed an increment statement:\n\t\t" << BlockIndentation() << z_ActionList[z_ActionList.size() - 1]->toString();
				break;
			case 146://If "decrement"
				z_ActionList.push_back(ParseDecrement());
				u_OutputFile << BlockIndentation() << "\tParsed a decrement statement:\n\t\t" << BlockIndentation() << z_ActionList[z_ActionList.size() - 1]->toString();
				break;
			case 157://If "if"
				z_ActionList.push_back(ParseIf());
				m_TokenCode = m_Reader->NextLexeme();
				break;
			case 170://If "while"
				z_ActionList.push_back(ParseWhile());
				m_TokenCode = m_Reader->NextLexeme();
				break;
			default:
				//Check for an else statement in an if block.
				if (m_TokenCode == 160 && a_BlockType == 161)
				{
					return z_ActionList;
				}
				throw SCLParsingException("Error: Invalid action statement. Parse action statement: " + m_Reader->m_CurrentLexeme);
			}//end case

		}//end while
		
		return z_ActionList;
	}

	//Precondition: The current lexeme is the end of the previous statement
	//Postcondition: The current lexeme is the end of the returned statement.
	std::shared_ptr<SCLDefineStatement> SCLParser::ParseDataDeclaration()
	{
		m_TokenCode = m_Reader->NextLexeme();
		if (m_TokenCode != 100)//If not ident
		{
			throw SCLParsingException("Error: Expecting an identifier. Found: " + m_Reader->m_CurrentLexeme);
		}
		std::string z_Ident = m_Reader->m_CurrentLexeme;
		std::vector<std::pair<int, std::string>> z_Types;
		m_TokenCode = m_Reader->NextLexeme();
		if (m_TokenCode != 124) //Is m_TokenCode != "of"
		{
			throw SCLParsingException("Error: Expecting \"of\". Found: " + m_Reader->m_CurrentLexeme);
		}
		m_TokenCode = m_Reader->NextLexeme();
		if (m_TokenCode != 125) //Is m_TokenCode != "type"
		{
			throw SCLParsingException("Error: Expecting \"type\". Found: " + m_Reader->m_CurrentLexeme);
		}
		m_TokenCode = m_Reader->NextLexeme();
		if (m_TokenCode < 126 && m_TokenCode > 131) //m_TokenCode is not a valid type.
		{
			throw SCLParsingException("Error: Invalid type declaration. Parsed type = " + m_Reader->m_CurrentLexeme);
		}
		z_Types.push_back({ m_TokenCode, m_Reader->m_CurrentLexeme });
		if (!AddIdent(z_Ident, m_TokenCode))//If not identifier declared.
		{
			throw SCLParsingException("Error: Identifier already declared. Parsed identifier: " + m_Reader->m_CurrentLexeme);
		}
		return SCLDefineStatementFactory(z_Ident, z_Types);
	}

	//Precondition: The current lexeme is the begining of this statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLAssignmentStatement> SCLParser::ParseAssignment()
	{
		std::shared_ptr<SCLAssignmentStatement> z_ParsedAssignment(new SCLAssignmentStatement());

		z_ParsedAssignment->m_Identifier = ParseNameRef();
		int z_IdentifierType = IdentType(z_ParsedAssignment->m_Identifier.m_IdentName);
		if (z_IdentifierType == 0)//If identifier undeclared.
		{
			throw SCLParsingException("Error: Idendifier is undeclared. Parsed identifier: " + m_Reader->m_CurrentLexeme);
		}
		if (m_TokenCode != 101) //If not "="
		{
			throw SCLParsingException("Error: Expecting \"=\". Found: " + m_Reader->m_CurrentLexeme);
		}
		z_ParsedAssignment->m_AssignedValue = ParseExpression();

		return z_ParsedAssignment;
	}
	//Precondition: The current lexeme is the begining of this statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLDisplayStatement> SCLParser::ParseDisplay()
	{
		std::shared_ptr<SCLDisplayStatement> z_ParsedStatement(new SCLDisplayStatement());
		z_ParsedStatement->m_ExpressionList = ParseArgList();
		return z_ParsedStatement;
	}
	//Precondition: The current lexeme is the begining of this statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLIncrementStatement> SCLParser::ParseIncrement()
	{
		std::shared_ptr<SCLIncrementStatement> z_ParsedIncrement(new SCLIncrementStatement());
		z_ParsedIncrement->m_Identifier = ParseNameRef();
		return z_ParsedIncrement;
	}
	//Precondition: The current lexeme is the begining of this statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLDecrementStatement> SCLParser::ParseDecrement()
	{
		std::shared_ptr<SCLDecrementStatement> z_ParsedDecrement(new SCLDecrementStatement());
		z_ParsedDecrement->m_Identifier = ParseNameRef();
		return z_ParsedDecrement;
	}
	//Precondition: The current lexeme is the begining of this statement.
	//Postcondition: The current lexeme is the end of the returned statement.
	std::shared_ptr<SCLIfStatement> SCLParser::ParseIf()
	{
		std::shared_ptr<SCLIfStatement> z_ParsedIfStatement(new SCLIfStatement());
		m_IdentScopes.push_back({ "endif", std::map<std::string, int>() });
		z_ParsedIfStatement->m_Condition = ParseConditional();
		if (m_TokenCode != 158)//If not "then"
		{
			throw SCLParsingException("Error: Expecting \"then\". Found: " + m_Reader->m_CurrentLexeme);
		}
		u_OutputFile << BlockIndentation() << "Parsed if statement:\n\t" << BlockIndentation() << "if " << z_ParsedIfStatement->m_Condition->toString() << " then\n";
		m_TokenCode = m_Reader->NextLexeme();
		z_ParsedIfStatement->m_IfStatementBody = ParseActions(161);
		if (m_TokenCode == 160)//If "else"
		{
			u_OutputFile << BlockIndentation() << "\telse\n";
			m_TokenCode = m_Reader->NextLexeme();
			z_ParsedIfStatement->m_ElseStatementBody = ParseActions(161);
		}
		return z_ParsedIfStatement;
	}
	//Precondition: The current lexeme is the begining of this statement.
	//Postcondition: The current lexeme is the end of the returned statement.
	std::shared_ptr<SCLWhileStatement> SCLParser::ParseWhile()
	{
		std::shared_ptr<SCLWhileStatement> z_ParsedWhileLoop(new SCLWhileStatement());
		m_IdentScopes.push_back({ "endwhile", std::map<std::string, int>() });
		z_ParsedWhileLoop->m_Condition = ParseConditional();
		if (m_TokenCode != 165)//If not "do"
		{
			throw SCLParsingException("Error: Expecting \"do\". Found: " + m_Reader->m_CurrentLexeme);
		}
		u_OutputFile << BlockIndentation() << "Parsed while loop:\n\t" << BlockIndentation() << "while " << z_ParsedWhileLoop->m_Condition->toString() << " do\n";
		m_TokenCode = m_Reader->NextLexeme();
		z_ParsedWhileLoop->m_LoopBody = ParseActions(171);
		return z_ParsedWhileLoop;
	}
	
	//Precondition: The current lexeme is the end of the previous statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLConditional> SCLParser::ParseConditional()
	{
		std::shared_ptr<SCLConditional> z_ParsedConditional(new SCLConditional());
		std::shared_ptr<SCLBooleanExpression> z_LeftCondition(new SCLBooleanExpression());
		z_LeftCondition = ParseBooleanExpression();
		switch (m_TokenCode)
		{
		case 149:
			z_ParsedConditional.reset(new SCLOrStatement());
			z_ParsedConditional->m_FirstCondition = z_LeftCondition;
			z_ParsedConditional->m_SecondCondition = ParseBooleanExpression();
			break;
		case 150:
			z_ParsedConditional.reset(new SCLAndStatement());
			z_ParsedConditional->m_FirstCondition = z_LeftCondition;
			z_ParsedConditional->m_SecondCondition = ParseBooleanExpression();
			break;
		}
		z_ParsedConditional->m_FirstCondition = z_LeftCondition;
		return z_ParsedConditional;
	}
	//Precondition: The current lexeme is the end of the previous statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLBooleanExpression> SCLParser::ParseBooleanExpression()
	{
		std::shared_ptr<SCLBooleanExpression> z_ParsedBooleanExpr(new SCLBooleanExpression());
		std::shared_ptr<SCLStatement> z_FirstStatement(new SCLStatement());
		bool z_Negated = false;
		m_TokenCode = m_Reader->NextLexeme();
		//If the expression is negated, mark a flag and parse in the next token.
		if (m_TokenCode == 151)//If "not"
		{
			z_Negated = true;
			m_TokenCode = m_Reader->NextLexeme();
		}
		//If there is a parenthesized expression note the parenthesis, otherwise parse an expression.
		if (m_TokenCode == 108)//If "("
		{
			z_FirstStatement = ParseConditional();
			if (m_TokenCode != 109)//If not ")"
			{
				throw SCLParsingException("Error: Expecting \")\". Found: " + m_Reader->m_CurrentLexeme);
			}
			m_TokenCode = m_Reader->NextLexeme();
		}
		else
		{
			z_FirstStatement = ParseExpression(true);
		}
		//Determine the conditional operator if any.
		if (m_TokenCode == 152)//If "equals"
		{
			z_ParsedBooleanExpr.reset(new SCLEqualsComparison());
			z_ParsedBooleanExpr->m_Negated = z_Negated;
			z_ParsedBooleanExpr->m_FirstExpression = z_FirstStatement;
			z_ParsedBooleanExpr->m_SecondExpression = ParseExpression();
		}
		else if (m_TokenCode == 153)//If "greater"
		{
			m_TokenCode = m_Reader->NextLexeme();
			if (m_TokenCode == 155)//If "than"
			{
				z_ParsedBooleanExpr.reset(new SCLGreaterThanComparison());
				z_ParsedBooleanExpr->m_Negated = z_Negated;
				z_ParsedBooleanExpr->m_FirstExpression = z_FirstStatement;
				z_ParsedBooleanExpr->m_SecondExpression = ParseExpression();
			}
			else if (m_TokenCode == 149)//If "or"
			{
				m_TokenCode = m_Reader->NextLexeme();
				if (m_TokenCode != 156)//If not "equal"
				{
					throw SCLParsingException("Error: Expecting \"equal\". Found: " + m_Reader->m_CurrentLexeme);
				}
				z_ParsedBooleanExpr.reset(new SCLGreaterOrEqualComparison());
				z_ParsedBooleanExpr->m_Negated = z_Negated;
				z_ParsedBooleanExpr->m_FirstExpression = z_FirstStatement;
				z_ParsedBooleanExpr->m_SecondExpression = ParseExpression();
			}
			else
			{
				throw SCLParsingException("Error: Expecting either \"than\" or \"or equal\". Found: " + m_Reader->m_CurrentLexeme);
			}
		}
		else if (m_TokenCode == 154)//If "less"
		{
			m_TokenCode = m_Reader->NextLexeme();
			if (m_TokenCode == 155)//If "than"
			{
				z_ParsedBooleanExpr.reset(new SCLLessThanComparison());
				z_ParsedBooleanExpr->m_Negated = z_Negated;
				z_ParsedBooleanExpr->m_FirstExpression = z_FirstStatement;
				z_ParsedBooleanExpr->m_SecondExpression = ParseExpression();
			}
			else if (m_TokenCode == 149)//If "or"
			{
				m_TokenCode = m_Reader->NextLexeme();
				if (m_TokenCode != 156)//If not "equal"
				{
					throw SCLParsingException("Error: Expecting \"equal\". Found: " + m_Reader->m_CurrentLexeme);
				}
				z_ParsedBooleanExpr.reset(new SCLLessOrEqualComparison());
				z_ParsedBooleanExpr->m_Negated = z_Negated;
				z_ParsedBooleanExpr->m_FirstExpression = z_FirstStatement;
				z_ParsedBooleanExpr->m_SecondExpression = ParseExpression();
			}
			else
			{
				throw SCLParsingException("Error: Expecting either \"than\" or \"or equal\". Found: " + m_Reader->m_CurrentLexeme);
			}
		}
		else if (m_TokenCode == 121 || m_TokenCode == 122)//If "true" or "false"
		{
			z_ParsedBooleanExpr.reset(new SCLEqualsComparison());
			z_ParsedBooleanExpr->m_Negated = z_Negated;
			z_ParsedBooleanExpr->m_FirstExpression = z_FirstStatement;
			z_ParsedBooleanExpr->m_SecondExpression.reset(new SCLBoolLiteral(m_Reader->m_CurrentLexeme == "true" ? true : false));
			m_TokenCode = m_Reader->NextLexeme();
		}
		else
		{
			z_ParsedBooleanExpr->m_Negated = z_Negated;
			z_ParsedBooleanExpr->m_FirstExpression = z_FirstStatement;
		}

		return z_ParsedBooleanExpr;
	}
	//Precondition: The current lexeme is the begining of this statement if the parameter is false.
	//					otherwise the current lexeme is the end of the previous statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLExpression> SCLParser::ParseExpression(bool a_isCurrentLexemeBegin)
	{
		std::shared_ptr<SCLExpression> z_ParsedExpression(new SCLExpression());
		std::shared_ptr<SCLTerm> z_FirstParsedTerm(new SCLTerm());

		//If the current lexeme is the end of the previous statement, parse in the next lexeme.
		if (!a_isCurrentLexemeBegin)
		{
			m_TokenCode = m_Reader->NextLexeme();
		}
		z_FirstParsedTerm = ParseTerm();
		switch (m_TokenCode)
		{
		case 102://If "+"
			z_ParsedExpression.reset(new SCLAdditionOpp());
			break;
		case 103://If "-"
			z_ParsedExpression.reset(new SCLSubtractionOpp());
			break;
		default:
			z_ParsedExpression->m_FirstTerm = z_FirstParsedTerm;
			return z_ParsedExpression;
		}
		z_ParsedExpression->m_FirstTerm = z_FirstParsedTerm;
		m_TokenCode = m_Reader->NextLexeme();
		z_ParsedExpression->m_SecondTerm = ParseTerm();
		return z_ParsedExpression;
	}
	//Precondition: The current lexeme is the begining of this statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLTerm> SCLParser::ParseTerm()
	{
		std::shared_ptr<SCLTerm> z_ParsedTerm(new SCLTerm());
		std::shared_ptr<SCLElement> z_FirstParsedElement(new SCLElement());

		z_FirstParsedElement = ParseElement();
		switch (m_TokenCode)
		{
		case 104://If "*"
			z_ParsedTerm.reset(new SCLMultiplicationOpp());
			break;
		case 105://If "/"
			z_ParsedTerm.reset(new SCLDivisionOpp());
			break;
		case 106://If "%", fall through to If "mod".
		case 117://If "mod"
			z_ParsedTerm.reset(new SCLModulusOpp());
			break;
		default:
			z_ParsedTerm->m_FirstTerm = z_FirstParsedElement;
			return z_ParsedTerm;
		}
		z_ParsedTerm->m_FirstTerm = z_FirstParsedElement;
		m_TokenCode = m_Reader->NextLexeme();
		z_ParsedTerm->m_SecondTerm = ParseElement();
		return z_ParsedTerm;
	}
	//Precondition: The current lexeme is the begining of the next statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::shared_ptr<SCLElement> SCLParser::ParseElement()
	{
		std::shared_ptr<SCLElement> z_ParsedElement(new SCLElement());

		if (m_TokenCode == 103 || m_TokenCode == 120)//If "-" or "negate"
		{
			//Element is negated.
			z_ParsedElement->m_Negated = true;
			z_ParsedElement->m_Element = ParseElement();
		}
		else if (m_TokenCode == 108)//If "("
		{
			//Element is a parenthesized expression.
			z_ParsedElement->m_Element = ParseExpression();
			if (m_TokenCode != 109)//")"
			{
				throw SCLParsingException("Error: Expecting \")\". Found: " + m_Reader->m_CurrentLexeme);
			}
			m_TokenCode = m_Reader->NextLexeme();
		}
		else if (m_TokenCode == 100)//If ident
		{
			std::string z_IdentName = m_Reader->m_CurrentLexeme;
			m_TokenCode = m_Reader->NextLexeme();
			std::shared_ptr<SCLIdent> z_Identifier(new SCLIdent());
			z_Identifier->m_IdentName = z_IdentName;
			z_ParsedElement->m_Element = z_Identifier;
		}
		else if ((m_TokenCode >= 1 && m_TokenCode <= 6) || m_TokenCode == 121 || m_TokenCode == 122) //If valid literal.
		{
			z_ParsedElement->m_Element = SCLDataTypeFactory({ m_TokenCode, m_Reader->m_CurrentLexeme });
			m_TokenCode = m_Reader->NextLexeme();
		}
		else
		{
			throw SCLParsingException("Error: Invalid data type. Parsed data type = " + m_Reader->m_CurrentLexeme);
		}
		return z_ParsedElement;
	}

	//Precondition: The current lexeme is the end of the previous statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::vector<std::shared_ptr<SCLExpression>> SCLParser::ParseArgList()
	{
		std::vector<std::shared_ptr<SCLExpression>> z_ArgList;

		z_ArgList.push_back(ParseExpression());
		//Loop while the current lexeme is ",".
		while (m_TokenCode == 112)
		{
			z_ArgList.push_back(ParseExpression());
		}
		return z_ArgList;
	}
	//Precondition: The current lexeme is the end of the previous statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	SCLIdent SCLParser::ParseNameRef()
	{
		m_TokenCode = m_Reader->NextLexeme();
		if (m_TokenCode != 100)//If not ident
		{
			throw SCLParsingException("Error: Expecting an identifier. Found: " + m_Reader->m_CurrentLexeme);
		}
		SCLIdent z_Identifier;
		z_Identifier.m_IdentName = m_Reader->m_CurrentLexeme;
		m_TokenCode = m_Reader->NextLexeme();
		if (m_TokenCode == 110) //If "["
		{
			z_Identifier.m_ArgList = ParseArgList();
			if (m_TokenCode != 111) //If not "]"
			{
				throw SCLParsingException("Error: Expecting \"]\". Found: " + m_Reader->m_CurrentLexeme);
			}
			m_TokenCode = m_Reader->NextLexeme();
		}
		return z_Identifier;
	}
	//Precondition: The current lexeme is the end of the previous statement.
	//Postcondition: The current lexeme is the begining of the next statement.
	std::vector<std::shared_ptr<SCLDefineStatement>> SCLParser::ParseParameters()
	{
		std::vector<std::shared_ptr<SCLDefineStatement>> z_ParsedParameters;
		
		m_TokenCode = m_Reader->NextLexeme();
		//Loop while the current lexeme is an identifier.
		while (m_TokenCode == 100)
		{
			std::string z_Ident = m_Reader->m_CurrentLexeme;
			std::vector<std::pair<int, std::string>> z_Types;
			m_TokenCode = m_Reader->NextLexeme();
			if (m_TokenCode != 124) //Is m_TokenCode != "of"
			{
				throw SCLParsingException("Error: Expecting \"of\". Found: " + m_Reader->m_CurrentLexeme);
			}
			m_TokenCode = m_Reader->NextLexeme();
			if (m_TokenCode != 125) //Is m_TokenCode != "type"
			{
				throw SCLParsingException("Error: Expecting \"type\". Found: " + m_Reader->m_CurrentLexeme);
			}
			m_TokenCode = m_Reader->NextLexeme();
			if (m_TokenCode < 126 && m_TokenCode > 131) //m_TokenCode is not a valid type.
			{
				throw SCLParsingException("Error: Invalid type declaration. Parsed type = " + m_Reader->m_CurrentLexeme);
			}
			z_Types.push_back({ m_TokenCode, m_Reader->m_CurrentLexeme });
			if (!AddIdent(z_Ident, m_TokenCode))//If not identifier declared.
			{
				throw SCLParsingException("Error: Identifier already declared. Parsed identifier: " + m_Reader->m_CurrentLexeme);
			}
			z_ParsedParameters.push_back(SCLDefineStatementFactory(z_Ident, z_Types));
			m_TokenCode = m_Reader->NextLexeme();
		}
		return z_ParsedParameters;
	}
	//End parsing functions

	//Execute functions
	std::shared_ptr<SCLDataType> SCLProgram::Execute()
	{
		SCLParser::m_IdentValues.push_back(std::map<std::string, std::pair<int, std::shared_ptr<SCLDataType>>>());
		if (m_GlobalDeclarations != nullptr)
		{
			m_GlobalDeclarations->Execute();
		}
		m_FunctionList.find("main")->second->Execute();
		//Return a null shared_ptr.
		return std::shared_ptr<SCLDataType>();
	}
	std::shared_ptr<SCLDataType> SCLDeclarations::Execute()
	{
		for (int u = 0; u < m_VariableDefinitions.size(); ++u)
		{
			m_VariableDefinitions[u]->Execute();
		}
		//Return a null shared_ptr.
		return std::shared_ptr<SCLDataType>();
	}
	std::shared_ptr<SCLDataType> SCLDefineStatement::Execute()
	{
		SCLParser::m_IdentValues[SCLParser::m_IdentValues.size() - 1].insert({ m_IdentName, std::pair<int, std::shared_ptr<SCLDataType>>(m_IdentType, std::shared_ptr<SCLDataType>()) });
		//Return a null shared_ptr.
		return std::shared_ptr<SCLDataType>();
	}
	std::shared_ptr<SCLDataType> SCLFunctionImplementation::Execute()
	{
		SCLParser::m_IdentValues.push_back(std::map<std::string, std::pair<int, std::shared_ptr<SCLDataType>>>());
		if (m_Declarations != nullptr)
		{
			m_Declarations->Execute();
		}
		for (int u = 0; u < m_FunctionBody.size(); ++u)
		{
			m_FunctionBody[u]->Execute();
		}
		SCLParser::m_IdentValues.pop_back();
		return std::shared_ptr<SCLDataType>();
	}

	std::shared_ptr<SCLDataType> SCLElement::Execute()
	{
		if (m_Negated)
		{
			std::shared_ptr<SCLDataType> z_ExecutedElement = m_Element->Execute();
			switch (z_ExecutedElement->GetType())
			{
			case 126:
				throw new SCLDataTypeException("Cannot negate an element of the char type.");
				break;
			case 127:
				return std::shared_ptr<SCLDataType>(new SCLIntLiteral(-z_ExecutedElement->toInt()));
				break;
			case 128:
				return std::shared_ptr<SCLDataType>(new SCLFloatLiteral(-z_ExecutedElement->toFloat()));
				break;
			case 129:
				throw new SCLDataTypeException("Cannot negate an element of the string type.");
				break;
			case 130:
				return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(!z_ExecutedElement->toBool()));
				break;
			default:
				throw new SCLDataTypeException("Invalid type.");
			}
		}
		else
		{
			return m_Element->Execute();
		}
	}
	std::shared_ptr<SCLDataType> SCLTerm::Execute()
	{
		return m_FirstTerm->Execute();
	}
	std::shared_ptr<SCLDataType> SCLExpression::Execute()
	{
		return m_FirstTerm->Execute();
	}

	std::shared_ptr<SCLDataType> SCLIdent::Execute()
	{
		return SCLParser::GetIdentValue(m_IdentName);
	}

	std::shared_ptr<SCLDataType> SCLAssignmentStatement::Execute()
	{
		SCLParser::SetIdentValue(m_Identifier.m_IdentName, m_AssignedValue->Execute());
		return std::shared_ptr<SCLDataType>();
	}
	std::shared_ptr<SCLDataType> SCLDisplayStatement::Execute()
	{
		for (int u = 0; u < m_ExpressionList.size(); ++u)
		{
			std::cout << m_ExpressionList[u]->Execute()->toString();
		}
		std::cout << std::endl;
		return std::shared_ptr<SCLDataType>();
	}
	std::shared_ptr<SCLDataType> SCLIncrementStatement::Execute()
	{
		SCLParser::GetIdentValue(m_Identifier.m_IdentName)->Increment();
		return std::shared_ptr<SCLDataType>();
	}
	std::shared_ptr<SCLDataType> SCLDecrementStatement::Execute()
	{
		SCLParser::GetIdentValue(m_Identifier.m_IdentName)->Decrement();
		return std::shared_ptr<SCLDataType>();
	}
	std::shared_ptr<SCLDataType> SCLIfStatement::Execute()
	{
		if (m_Condition->Execute()->toBool())
		{
			for (int u = 0; u < m_IfStatementBody.size(); ++u)
			{
				m_IfStatementBody[u]->Execute();
			}
		}
		else
		{
			for (int u = 0; u < m_ElseStatementBody.size(); ++u)
			{
				m_ElseStatementBody[u]->Execute();
			}
		}
		return std::shared_ptr<SCLDataType>();
	}
	std::shared_ptr<SCLDataType> SCLWhileStatement::Execute()
	{
		while (m_Condition->Execute()->toBool())
		{
			for (int u = 0; u < m_LoopBody.size(); ++u)
			{
				m_LoopBody[u]->Execute();
			}
		}
		return std::shared_ptr<SCLDataType>();
	}
	
	std::shared_ptr<SCLDataType> SCLBooleanExpression::Execute()
	{
		bool z_Answer;
		z_Answer = m_FirstExpression->Execute()->toBool();
		z_Answer = m_Negated ? !z_Answer : z_Answer;
		return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_Answer));
	}
	std::shared_ptr<SCLDataType> SCLConditional::Execute()
	{
		return m_FirstCondition->Execute();
	}
	
	std::shared_ptr<SCLDataType> SCLOrStatement::Execute()
	{
		bool z_Answer;
		z_Answer = m_FirstCondition->Execute()->toBool() || m_SecondCondition->Execute()->toBool();
		return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_Answer));
	}
	std::shared_ptr<SCLDataType> SCLAndStatement::Execute()
	{
		bool z_Answer;
		z_Answer = m_FirstCondition->Execute()->toBool() && m_SecondCondition->Execute()->toBool();
		return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_Answer));
	}
	
	std::shared_ptr<SCLDataType> SCLEqualsComparison::Execute()
	{
		bool z_Answer;
		std::shared_ptr<SCLDataType> z_FirstExprResult = m_FirstExpression->Execute();
		switch (z_FirstExprResult->GetType())
		{
		case 126:
			z_Answer = z_FirstExprResult->toChar() == m_SecondExpression->Execute()->toChar();
			break;
		case 127:
			z_Answer = z_FirstExprResult->toInt() == m_SecondExpression->Execute()->toInt();
			break;
		case 128:
			z_Answer = z_FirstExprResult->toFloat() == m_SecondExpression->Execute()->toFloat();
			break;
		case 129:
			z_Answer = z_FirstExprResult->toString() == m_SecondExpression->Execute()->toString();
			break;
		case 130:
			z_Answer = z_FirstExprResult->toBool() == m_SecondExpression->Execute()->toBool();
			break;
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
		z_Answer = m_Negated ? !z_Answer : z_Answer;
		return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_Answer));
	}
	std::shared_ptr<SCLDataType> SCLGreaterThanComparison::Execute()
	{
		bool z_Answer;
		std::shared_ptr<SCLDataType> z_FirstExprResult = m_FirstExpression->Execute();
		switch (z_FirstExprResult->GetType())
		{
		case 126:
			z_Answer = z_FirstExprResult->toChar() > m_SecondExpression->Execute()->toChar();
			break;
		case 127:
			z_Answer = z_FirstExprResult->toInt() > m_SecondExpression->Execute()->toInt();
			break;
		case 128:
			z_Answer = z_FirstExprResult->toFloat() > m_SecondExpression->Execute()->toFloat();
			break;
		case 129:
			z_Answer = z_FirstExprResult->toString() > m_SecondExpression->Execute()->toString();
			break;
		case 130:
			z_Answer = z_FirstExprResult->toBool() > m_SecondExpression->Execute()->toBool();
			break;
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
		z_Answer = m_Negated ? !z_Answer : z_Answer;
		return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_Answer));
	}
	std::shared_ptr<SCLDataType> SCLLessThanComparison::Execute()
	{
		bool z_Answer;
		std::shared_ptr<SCLDataType> z_FirstExprResult = m_FirstExpression->Execute();
		switch (z_FirstExprResult->GetType())
		{
		case 126:
			z_Answer = z_FirstExprResult->toChar() < m_SecondExpression->Execute()->toChar();
			break;
		case 127:
			z_Answer = z_FirstExprResult->toInt() < m_SecondExpression->Execute()->toInt();
			break;
		case 128:
			z_Answer = z_FirstExprResult->toFloat() < m_SecondExpression->Execute()->toFloat();
			break;
		case 129:
			z_Answer = z_FirstExprResult->toString() < m_SecondExpression->Execute()->toString();
			break;
		case 130:
			z_Answer = z_FirstExprResult->toBool() < m_SecondExpression->Execute()->toBool();
			break;
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
		z_Answer = m_Negated ? !z_Answer : z_Answer;
		return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_Answer));
	}
	std::shared_ptr<SCLDataType> SCLGreaterOrEqualComparison::Execute()
	{
		bool z_Answer;
		std::shared_ptr<SCLDataType> z_FirstExprResult = m_FirstExpression->Execute();
		switch (z_FirstExprResult->GetType())
		{
		case 126:
			z_Answer = z_FirstExprResult->toChar() >= m_SecondExpression->Execute()->toChar();
			break;
		case 127:
			z_Answer = z_FirstExprResult->toInt() >= m_SecondExpression->Execute()->toInt();
			break;
		case 128:
			z_Answer = z_FirstExprResult->toFloat() >= m_SecondExpression->Execute()->toFloat();
			break;
		case 129:
			z_Answer = z_FirstExprResult->toString() >= m_SecondExpression->Execute()->toString();
			break;
		case 130:
			z_Answer = z_FirstExprResult->toBool() >= m_SecondExpression->Execute()->toBool();
			break;
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
		z_Answer = m_Negated ? !z_Answer : z_Answer;
		return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_Answer));
	}
	std::shared_ptr<SCLDataType> SCLLessOrEqualComparison::Execute()
	{
		bool z_Answer;
		std::shared_ptr<SCLDataType> z_FirstExprResult = m_FirstExpression->Execute();
		switch (z_FirstExprResult->GetType())
		{
		case 126:
			z_Answer = z_FirstExprResult->toChar() <= m_SecondExpression->Execute()->toChar();
			break;
		case 127:
			z_Answer = z_FirstExprResult->toInt() <= m_SecondExpression->Execute()->toInt();
			break;
		case 128:
			z_Answer = z_FirstExprResult->toFloat() <= m_SecondExpression->Execute()->toFloat();
			break;
		case 129:
			z_Answer = z_FirstExprResult->toString() <= m_SecondExpression->Execute()->toString();
			break;
		case 130:
			z_Answer = z_FirstExprResult->toBool() <= m_SecondExpression->Execute()->toBool();
			break;
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
		z_Answer = m_Negated ? !z_Answer : z_Answer;
		return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_Answer));
	}
	std::shared_ptr<SCLDataType> SCLTrueFalseComparison::Execute() 
	{
		bool z_Answer;
		z_Answer = m_FirstExpression->Execute()->toBool() == m_SecondExpression->Execute()->toBool();
		z_Answer = m_Negated ? !z_Answer : z_Answer;
		return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_Answer));
	}
	
	std::shared_ptr<SCLDataType> SCLAdditionOpp::Execute()
	{
		std::shared_ptr<SCLDataType> z_FirstTermResult = m_FirstTerm->Execute();
		switch (z_FirstTermResult->GetType())
		{
		case 126:
		{
			char z_CharResult = z_FirstTermResult->toChar() + m_SecondTerm->Execute()->toChar();
			return std::shared_ptr<SCLDataType>(new SCLCharLiteral(z_CharResult));
			break;
		}
		case 127:
		{
			int z_IntResult = z_FirstTermResult->toInt() + m_SecondTerm->Execute()->toInt();
			return std::shared_ptr<SCLDataType>(new SCLIntLiteral(z_IntResult));
			break;
		}
		case 128:
		{
			float z_FloatResult = z_FirstTermResult->toFloat() + m_SecondTerm->Execute()->toFloat();
			return std::shared_ptr<SCLDataType>(new SCLFloatLiteral(z_FloatResult));
			break;
		}
		case 129:
		{
			std::string z_StringResult = z_FirstTermResult->toString() + m_SecondTerm->Execute()->toString();
			return std::shared_ptr<SCLDataType>(new SCLStringLiteral(z_StringResult));
			break;
		}
		case 130:
		{
			bool z_BoolResult = z_FirstTermResult->toBool() + m_SecondTerm->Execute()->toBool();
			return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_BoolResult));
			break;
		}
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
	}
	std::shared_ptr<SCLDataType> SCLSubtractionOpp::Execute()
	{
		std::shared_ptr<SCLDataType> z_FirstTermResult = m_FirstTerm->Execute();
		switch (z_FirstTermResult->GetType())
		{
		case 126:
		{
			throw new SCLDataTypeException("The '-' operator is not defined for elements of type char.");
			break;
		}
		case 127:
		{
			int z_IntResult = z_FirstTermResult->toInt() - m_SecondTerm->Execute()->toInt();
			return std::shared_ptr<SCLDataType>(new SCLIntLiteral(z_IntResult));
			break;
		}
		case 128:
		{
			float z_FloatResult = z_FirstTermResult->toFloat() - m_SecondTerm->Execute()->toFloat();
			return std::shared_ptr<SCLDataType>(new SCLFloatLiteral(z_FloatResult));
			break;
		}
		case 129:
		{
			throw new SCLDataTypeException("The '-' operator is not defined for elements of type string.");
			break;
		}
		case 130:
		{
			bool z_BoolResult = z_FirstTermResult->toBool() - m_SecondTerm->Execute()->toBool();
			return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(z_BoolResult));
			break;
		}
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
	}

	std::shared_ptr<SCLDataType> SCLMultiplicationOpp::Execute()
	{
		std::shared_ptr<SCLDataType> z_FirstTermResult = m_FirstTerm->Execute();
		switch (z_FirstTermResult->GetType())
		{
		case 126:
		{
			throw new SCLDataTypeException("The '*' operator is not defined for elements of type char.");
			break;
		}
		case 127:
		{
			int z_IntResult = z_FirstTermResult->toInt() * m_SecondTerm->Execute()->toInt();
			return std::shared_ptr<SCLDataType>(new SCLIntLiteral(z_IntResult));
			break;
		}
		case 128:
		{
			float z_FloatResult = z_FirstTermResult->toFloat() * m_SecondTerm->Execute()->toFloat();
			return std::shared_ptr<SCLDataType>(new SCLFloatLiteral(z_FloatResult));
			break;
		}
		case 129:
		{
			throw new SCLDataTypeException("The '*' operator is not defined for elements of type string.");
			break;
		}
		case 130:
		{
			throw new SCLDataTypeException("The '*' operator is not defined for elements of type bool.");
			break;
		}
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
	}
	std::shared_ptr<SCLDataType> SCLDivisionOpp::Execute()
	{
		std::shared_ptr<SCLDataType> z_FirstTermResult = m_FirstTerm->Execute();
		switch (z_FirstTermResult->GetType())
		{
		case 126:
		{
			throw new SCLDataTypeException("The '/' operator is not defined for elements of type char.");
			break;
		}
		case 127:
		{
			int z_IntResult = z_FirstTermResult->toInt() / m_SecondTerm->Execute()->toInt();
			return std::shared_ptr<SCLDataType>(new SCLIntLiteral(z_IntResult));
			break;
		}
		case 128:
		{
			float z_FloatResult = z_FirstTermResult->toFloat() / m_SecondTerm->Execute()->toFloat();
			return std::shared_ptr<SCLDataType>(new SCLFloatLiteral(z_FloatResult));
			break;
		}
		case 129:
		{
			throw new SCLDataTypeException("The '/' operator is not defined for elements of type string.");
			break;
		}
		case 130:
		{
			throw new SCLDataTypeException("The '/' operator is not defined for elements of type bool.");
			break;
		}
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
	}
	std::shared_ptr<SCLDataType> SCLModulusOpp::Execute()
	{
		std::shared_ptr<SCLDataType> z_FirstTermResult = m_FirstTerm->Execute();
		switch (z_FirstTermResult->GetType())
		{
		case 126:
		{
			throw new SCLDataTypeException("The '%' operator is not defined for elements of type char.");
			break;
		}
		case 127:
		{
			int z_IntResult = z_FirstTermResult->toInt() % m_SecondTerm->Execute()->toInt();
			return std::shared_ptr<SCLDataType>(new SCLIntLiteral(z_IntResult));
			break;
		}
		case 128:
		{
			throw new SCLDataTypeException("The '%' operator is not defined for elements of type float.");
			break;
		}
		case 129:
		{
			throw new SCLDataTypeException("The '%' operator is not defined for elements of type string.");
			break;
		}
		case 130:
		{
			throw new SCLDataTypeException("The '%' operator is not defined for elements of type bool.");
			break;
		}
		default:
			throw new SCLDataTypeException("Invalid type.");
		}
	}
	//End execute functions

	//SCLDataType Virtual Function Implementations
	char SCLDataType::toChar()
	{
		throw new SCLDataTypeException("Cannot convert from SCLDataType to char.");
		return 0;
	}
	int SCLDataType::toInt()
	{
		throw new SCLDataTypeException("Cannot convert from SCLDataType to int.");
		return 0;
	}
	float SCLDataType::toFloat()
	{
		throw new SCLDataTypeException("Cannot convert from SCLDataType to float.");
		return 0.0;
	}
	std::string SCLDataType::toString()
	{
		throw new SCLDataTypeException("Cannot convert from SCLDataType to string.");
		return "";
	}
	bool SCLDataType::toBool()
	{
		throw new SCLDataTypeException("Cannot convert from SCLDataType to bool.");
		return false;
	}
	void SCLDataType::Increment()
	{
		throw new SCLDataTypeException("Cannot increment variable of type SCLDataType.");
	}
	void SCLDataType::Decrement()
	{
		throw new SCLDataTypeException("Cannot decrement variable of type SCLDataType.");
	}

	char SCLIdent::toChar()
	{
		throw new SCLDataTypeException("Cannot convert from SCLIdent to char.");
		return 0;
	}
	int SCLIdent::toInt()
	{
		throw new SCLDataTypeException("Cannot convert from SCLIdent to int.");
		return 0;
	}
	float SCLIdent::toFloat()
	{
		throw new SCLDataTypeException("Cannot convert from SCLIdent to float.");
		return 0.0;
	}
	bool SCLIdent::toBool()
	{
		throw new SCLDataTypeException("Cannot convert from SCLIdent to bool.");
		return false;
	}
	void SCLIdent::Increment()
	{
		throw new SCLDataTypeException("Cannot increment variable of type SCLIdent.");
	}
	void SCLIdent::Decrement()
	{
		throw new SCLDataTypeException("Cannot decrement variable of type SCLIdent.");
	}

	char SCLCharLiteral::toChar()
	{
		return m_Value;
	}
	int SCLCharLiteral::toInt()
	{
		return (int)m_Value;
	}
	float SCLCharLiteral::toFloat()
	{
		throw new SCLDataTypeException("Cannot convert from SCLCharLiteral to float.");
		return 0.0;
	}
	std::string SCLCharLiteral::toString()
	{
		std::ostringstream z_Answer;
		z_Answer << m_Value;
		return z_Answer.str();
	}
	bool SCLCharLiteral::toBool()
	{
		throw new SCLDataTypeException("Cannot convert from SCLCharLiteral to bool.");
		return false;
	}
	void SCLCharLiteral::Increment()
	{
		throw new SCLDataTypeException("Cannot increment variable of type SCLCharLiteral.");
	}
	void SCLCharLiteral::Decrement()
	{
		throw new SCLDataTypeException("Cannot decrement variable of type SCLCharLiteral.");
	}

	char SCLIntLiteral::toChar()
	{
		return (char)m_Value;
	}
	int SCLIntLiteral::toInt()
	{
		return m_Value;
	}
	float SCLIntLiteral::toFloat()
	{
		return (float)m_Value;
	}
	std::string SCLIntLiteral::toString()
	{
		std::ostringstream z_Answer;
		z_Answer << m_Value;
		return z_Answer.str();
	}
	bool SCLIntLiteral::toBool()
	{
		return (m_Value == 0 ? false : true);
	}
	void SCLIntLiteral::Increment()
	{
		++m_Value;
	}
	void SCLIntLiteral::Decrement()
	{
		--m_Value;
	}

	char SCLFloatLiteral::toChar()
	{
		throw new SCLDataTypeException("Cannot convert from SCLFloatLiteral to char.");
		return 0;
	}
	int SCLFloatLiteral::toInt()
	{
		return (int)m_Value;
	}
	float SCLFloatLiteral::toFloat()
	{
		return m_Value;
	}
	std::string SCLFloatLiteral::toString()
	{
		std::ostringstream z_Answer;
		z_Answer << m_Value;
		return z_Answer.str();
	}
	bool SCLFloatLiteral::toBool()
	{
		return (m_Value == 0.0 ? false : true);
	}
	void SCLFloatLiteral::Increment()
	{
		++m_Value;
	}
	void SCLFloatLiteral::Decrement()
	{
		--m_Value;
	}

	char SCLStringLiteral::toChar()
	{
		throw new SCLDataTypeException("Cannot convert from SCLStringLiteral to char.");
		return 0;
	}
	int SCLStringLiteral::toInt()
	{
		throw new SCLDataTypeException("Cannot convert from SCLStringLiteral to int.");
		return 0;
	}
	float SCLStringLiteral::toFloat()
	{
		throw new SCLDataTypeException("Cannot convert from SCLStringLiteral to float.");
		return 0.0;
	}
	std::string SCLStringLiteral::toString()
	{
		return m_Value;
	}
	bool SCLStringLiteral::toBool()
	{
		throw new SCLDataTypeException("Cannot convert from SCLStringLiteral to bool.");
		return false;
	}
	void SCLStringLiteral::Increment()
	{
		throw new SCLDataTypeException("Cannot increment variable of type SCLStringLiteral.");
	}
	void SCLStringLiteral::Decrement()
	{
		throw new SCLDataTypeException("Cannot decrement variable of type SCLStringLiteral.");
	}

	char SCLBoolLiteral::toChar()
	{
		return (m_Value ? 't' : 'f');
	}
	int SCLBoolLiteral::toInt()
	{
		return (int)m_Value;
	}
	float SCLBoolLiteral::toFloat()
	{
		return (float)m_Value;
	}
	std::string SCLBoolLiteral::toString()
	{
		return (m_Value ? "true" : "false");
	}
	bool SCLBoolLiteral::toBool()
	{
		return m_Value;
	}
	void SCLBoolLiteral::Increment()
	{
		throw new SCLDataTypeException("Cannot increment variable of type SCLBoolLiteral.");
	}
	void SCLBoolLiteral::Decrement()
	{
		throw new SCLDataTypeException("Cannot decrement variable of type SCLBoolLiteral.");
	}
	//End SCLDataType Virtual Function Implementations

	//Printing related functions
	std::string SCLDisplayStatement::toString()
	{
		std::string z_Answer = "display ";
		if (m_ExpressionList.size() > 0)
		{
			z_Answer += m_ExpressionList[0]->toString();
		}
		for (int i = 1; i < m_ExpressionList.size(); ++i)
		{
			z_Answer += ", " + m_ExpressionList[i]->toString();
		}
		z_Answer += "\n";
		return z_Answer;
	}
	std::string SCLIdent::PrintArgumentList()
	{
		std::string z_Answer = "";
		if (m_ArgList.size() > 0)
		{
			z_Answer += m_ArgList[0]->toString();
		}
		for (int i = 1; i < m_ArgList.size(); ++i)
		{
			z_Answer += ", " + m_ArgList[i]->toString();
		}
		return z_Answer;
	}
	std::string SCLParser::BlockIndentation()
	{
		std::string z_Indentation = "";
		for (int j = 1; j < m_IdentScopes.size(); ++j)
		{
			z_Indentation += "\t";
		}
		return z_Indentation;
	}
}

int main(int argc, char* argv[])
{
	SCLInterpreter::SCLParser z_Parser(argv[1]);
	std::cout << "\nRunning file: " << argv[1] << "\n\n";
	try
	{
		z_Parser.ParseProgram()->Execute();
	}
	catch (SCLInterpreter::SCLParsingException& ex)
	{
		std::cout << ex.m_Message;
	}
	catch (SCLInterpreter::SCLDataTypeException& ex)
	{
		std::cout << ex.m_Message;
	}

	return EXIT_SUCCESS;
}

