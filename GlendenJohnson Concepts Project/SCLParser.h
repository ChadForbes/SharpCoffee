//Course: CS4308
//Instructor: Dr. Garrido
//Assignment: Course Project
//Written by Glenden Johnson
//Last modified: 4/3/2019

#include "SCLScanner.h"
#include <fstream>
#include <sstream>
#include <memory>

namespace SCLInterpreter
{
	//Exception class
	class SCLParsingException
	{
	public:
		std::string m_Message;
		//Constructor
		SCLParsingException(std::string a_Message)
		{
			m_Message = a_Message;
		}
	};
	class SCLDataTypeException
	{
	public:
		std::string m_Message;
		//Constructor
		SCLDataTypeException(std::string a_Message)
		{
			m_Message = a_Message;
		}
	};

	//Statement classes
	//forward declarations
	class SCLStatement;
	class SCLProgram;
	class SCLDeclarations;
	class SCLFunctionImplementation;
	class SCLDefineStatement;
	class SCLDefineArray;
	
	class SCLAction;
	class SCLFunctionCall;
	class SCLDisplayStatement;
	class SCLIncrementStatement;
	class SCLDecrementStatement;
	class SCLReturnStatement;
	class SCLIfStatement;
	class SCLWhileStatement;
	
	class SCLConditional;
	class SCLOrStatement;
	class SCLAndStatement;
	class SCLBooleanExpression;
	class SCLEqualsComparison;
	class SCLGreaterThanComparison;
	class SCLLessThanComparison;
	class SCLGreaterOrEqualComparison;
	class SCLLessOrEqualComparison;
	class SCLTrueFalseComparison;
	
	class SCLExpression;
	class SCLAdditionOpp;
	class SCLSubtractionOpp;
	class SCLBandOpp;
	class SCLBorOpp;
	class SCLBxorOpp;
	class SCLTerm;
	class SCLMultiplicationOpp;
	class SCLDivisionOpp;
	class SCLModulusOpp;
	class SCLLshift;
	class SCLRshift;
	class SCLElement;
	
	class SCLDataType;
	class SCLIdent;
	class SCLLiteral;

	//class specifications
	class SCLStatement
	{
	public:
		virtual std::string toString() { return ""; }
		virtual std::shared_ptr<SCLDataType> Execute() { return std::shared_ptr<SCLDataType>(); };
	};
	class SCLFunctionImplementation : public SCLStatement
	{
	public:
		bool m_ReturnPointer;
		std::shared_ptr<SCLDefineStatement> m_ReturnType;
		std::vector<std::shared_ptr<SCLDefineStatement>> m_Parameters;
		std::shared_ptr<SCLDeclarations> m_Declarations;
		std::vector<std::shared_ptr<SCLAction>> m_FunctionBody;

		std::shared_ptr<SCLDataType> Execute();
	};
	class SCLProgram : public SCLStatement
	{
	public:
		std::shared_ptr<SCLDeclarations> m_GlobalDeclarations;
		static std::map<std::string, std::shared_ptr<SCLFunctionImplementation>> m_FunctionList; //Implementations section.
		std::shared_ptr<SCLDataType> Execute();
	};
	class SCLDeclarations : public SCLStatement
	{
	public:
		std::vector<std::shared_ptr<SCLDefineStatement>> m_VariableDefinitions;
		std::shared_ptr<SCLDataType> Execute();
	};
	class SCLDefineStatement : public SCLStatement
	{
	public:
		std::string m_IdentName;
		int m_IdentType;
		//Constructor
		SCLDefineStatement(std::string a_IdentName = "", int a_IdentType = 0)
		{
			m_IdentName = a_IdentName;
			m_IdentType = a_IdentType;
		}

		std::shared_ptr<SCLDataType> Execute();

		std::string toString()
		{
			std::ostringstream z_Answer;
			z_Answer << "\tdefine " << m_IdentName << " of type " << m_IdentType << "\n";
			return z_Answer.str();
		}
	};
	
	class SCLElement : public SCLStatement
	{
	public:
		bool m_Negated;
		std::shared_ptr<SCLStatement> m_Element;

		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return (m_Negated ? "-" : "") + m_Element->toString();
		}
	};
	class SCLTerm : public SCLStatement
	{
	public:
		std::shared_ptr<SCLElement> m_FirstTerm;
		std::shared_ptr<SCLElement> m_SecondTerm;

		std::shared_ptr<SCLDataType> Execute();
		virtual std::string toString()
		{
			return m_FirstTerm->toString();
		}
	};
	class SCLExpression : public SCLStatement
	{
	public:
		std::shared_ptr<SCLTerm> m_FirstTerm;
		std::shared_ptr<SCLTerm> m_SecondTerm;

		std::shared_ptr<SCLDataType> Execute();
		virtual std::string toString() 
		{
			return m_FirstTerm->toString(); 
		}
	};
	
	class SCLDataType : public SCLStatement
	{
	public:
		//Constructors
		SCLDataType() {}
		//SCLDataType Conversion Functions
		virtual char toChar();
		virtual int toInt();
		virtual float toFloat();
		virtual std::string toString();
		virtual bool toBool();

		virtual void Increment();
		virtual void Decrement();

		virtual int GetType() { return -2; };
	};
	class SCLIdent : public SCLDataType
	{
	public:
		std::string m_IdentName;
		std::vector<std::shared_ptr<SCLExpression>> m_ArgList;
		//Constructors
		SCLIdent(std::string a_IdentName = "")
		{
			m_IdentName = a_IdentName;
		}

		//Conversion Functions
		char toChar();
		int toInt();
		float toFloat();
		std::string toString()
		{
			return m_IdentName;
		}
		bool toBool();
		
		void Increment();
		void Decrement();

		std::shared_ptr<SCLDataType> Execute();
		int GetType() { return 100; }
		std::string PrintArgumentList();
	};

	//Action statements.
	class SCLAction : public SCLStatement
	{
	public:
		virtual std::string toString() { return ""; }
	};
	class SCLAssignmentStatement : public SCLAction
	{
	public:
		SCLIdent m_Identifier;
		std::shared_ptr<SCLExpression> m_AssignedValue;

		std::shared_ptr<SCLDataType> Execute();

		std::string toString()
		{
			return "set " + m_Identifier.m_IdentName + " = " + m_AssignedValue->toString() + "\n";
		}
	};
	class SCLDisplayStatement : public SCLAction
	{
	public:
		std::vector<std::shared_ptr<SCLExpression>> m_ExpressionList;

		std::shared_ptr<SCLDataType> Execute();

		std::string toString();
	};
	class SCLIncrementStatement : public SCLAction
	{
	public:
		SCLIdent m_Identifier;

		std::shared_ptr<SCLDataType> Execute();

		std::string toString()
		{
			return "increment " + m_Identifier.m_IdentName + "\n";
		}
	};
	class SCLDecrementStatement : public SCLAction
	{
	public:
		SCLIdent m_Identifier;

		std::shared_ptr<SCLDataType> Execute();

		std::string toString()
		{
			return "decrement " + m_Identifier.m_IdentName + "\n";
		}
	};
	class SCLIfStatement : public SCLAction
	{
	public:
		std::shared_ptr<SCLConditional> m_Condition;
		std::vector<std::shared_ptr<SCLAction>> m_IfStatementBody;
		std::vector<std::shared_ptr<SCLAction>> m_ElseStatementBody;

		std::shared_ptr<SCLDataType> Execute();

		std::string toString()
		{
			return "if ";
		}
	};
	class SCLWhileStatement : public SCLAction
	{
	public:
		std::shared_ptr<SCLConditional> m_Condition;
		std::vector<std::shared_ptr<SCLAction>> m_LoopBody;

		std::shared_ptr<SCLDataType> Execute();
	};
	
	class SCLBooleanExpression : public SCLStatement
	{
	public:
		bool m_Negated;
		std::shared_ptr<SCLStatement> m_FirstExpression;
		std::shared_ptr<SCLStatement> m_SecondExpression;
		
		std::shared_ptr<SCLDataType> Execute();

		std::string toString()
		{
			return (m_Negated ? "not " : "") + m_FirstExpression->toString();
		}
	};
	class SCLConditional : public SCLStatement
	{
	public:
		std::shared_ptr<SCLBooleanExpression> m_FirstCondition;
		std::shared_ptr<SCLBooleanExpression> m_SecondCondition;

		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return m_FirstCondition->toString();
		}
	};

	//Boolean "and" and "or" statements.
	class SCLOrStatement : public SCLConditional
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return m_FirstCondition->toString() + " or " + m_SecondCondition->toString();
		}
	};
	class SCLAndStatement : public SCLConditional
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return m_FirstCondition->toString() + " or " + m_SecondCondition->toString();
		}
	};

	//Comparison operators.
	class SCLEqualsComparison : public SCLBooleanExpression
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return (m_Negated ? "not " : "") + m_FirstExpression->toString() + " equals " + m_SecondExpression->toString();
		}
	};
	class SCLGreaterThanComparison : public SCLBooleanExpression
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return (m_Negated ? "not " : "") + m_FirstExpression->toString() + " greater than " + m_SecondExpression->toString();
		}
	};
	class SCLLessThanComparison : public SCLBooleanExpression
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return (m_Negated ? "not " : "") + m_FirstExpression->toString() + " less than " + m_SecondExpression->toString();
		}
	};
	class SCLGreaterOrEqualComparison : public SCLBooleanExpression
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return (m_Negated ? "not " : "") + m_FirstExpression->toString() + " greater or equal " + m_SecondExpression->toString();
		}
	};
	class SCLLessOrEqualComparison : public SCLBooleanExpression
	{
	public:
		std::shared_ptr<SCLDataType> Execute();

		std::string toString()
		{
			return (m_Negated ? "not " : "") + m_FirstExpression->toString() + " less or equal " + m_SecondExpression->toString();
		}
	};
	class SCLTrueFalseComparison : public SCLBooleanExpression
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return (m_Negated ? "not " : "") + m_FirstExpression->toString() + " " + m_SecondExpression->toString();
		}
	};
	
	//Expression level operations.
	class SCLAdditionOpp : public SCLExpression
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return m_FirstTerm->toString() + " + " + m_SecondTerm->toString();
		}
	};
	class SCLSubtractionOpp : public SCLExpression
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return m_FirstTerm->toString() + " - " + m_SecondTerm->toString();
		}
	};
	
	//Term level operations.
	class SCLMultiplicationOpp : public SCLTerm
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return m_FirstTerm->toString() + " * " + m_SecondTerm->toString();
		}
	};
	class SCLDivisionOpp : public SCLTerm
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return m_FirstTerm->toString() + " / " + m_SecondTerm->toString();
		}
	};
	class SCLModulusOpp : public SCLTerm
	{
	public:
		std::shared_ptr<SCLDataType> Execute();
		std::string toString()
		{
			return m_FirstTerm->toString() + " % " + m_SecondTerm->toString();
		}
	};
	
	//Literals.
	class SCLCharLiteral : public SCLDataType
	{
	public:
		char m_Value;
		//Constructor
		SCLCharLiteral(char a_Value)
		{
			m_Value = a_Value;
		}

		//Conversion Functions
		char toChar();
		int toInt();
		float toFloat();
		std::string toString();
		bool toBool();

		void Increment();
		void Decrement();

		int GetType() { return 126; }
		std::shared_ptr<SCLDataType> Execute() { return std::shared_ptr<SCLDataType>(new SCLCharLiteral(m_Value)); }
	}; 
	class SCLIntLiteral : public SCLDataType
	{
	public:
		int m_Value;
		//Constructor
		SCLIntLiteral(int a_Value)
		{
			m_Value = a_Value;
		}

		//Conversion Functions
		char toChar();
		int toInt();
		float toFloat();
		std::string toString();
		bool toBool();

		void Increment();
		void Decrement();

		int GetType() { return 127; }
		std::shared_ptr<SCLDataType> Execute() { return std::shared_ptr<SCLDataType>(new SCLIntLiteral(m_Value)); }
	}; 
	class SCLFloatLiteral : public SCLDataType
	{
	public:
		float m_Value;
		//Constructor
		SCLFloatLiteral(float a_Value)
		{
			m_Value = a_Value;
		}

		//Conversion Functions
		char toChar();
		int toInt();
		float toFloat();
		std::string toString();
		bool toBool();

		void Increment();
		void Decrement();

		int GetType() { return 128; }
		std::shared_ptr<SCLDataType> Execute() { return std::shared_ptr<SCLDataType>(new SCLFloatLiteral(m_Value)); }
	}; 
	class SCLStringLiteral : public SCLDataType
	{
	public:
		std::string m_Value;
		//Constructor
		SCLStringLiteral(std::string a_Value)
		{
			m_Value = a_Value;
		}

		//Conversion Functions
		char toChar();
		int toInt();
		float toFloat();
		std::string toString();
		bool toBool();

		void Increment();
		void Decrement();

		int GetType() { return 129; }
		std::shared_ptr<SCLDataType> Execute() { return std::shared_ptr<SCLDataType>(new SCLStringLiteral(m_Value)); }
	}; 
	class SCLBoolLiteral : public SCLDataType
	{
	public:
		bool m_Value;
		//Constructor
		SCLBoolLiteral(bool a_Value)
		{
			m_Value = a_Value;
		}

		//Conversion Functions
		char toChar();
		int toInt();
		float toFloat();
		std::string toString();
		bool toBool();

		void Increment();
		void Decrement();

		int GetType() { return 130; }
		std::shared_ptr<SCLDataType> Execute() { return std::shared_ptr<SCLDataType>(new SCLBoolLiteral(m_Value)); }
	};
	//End Statement classes

	class SCLParser
	{
	public:
		//Constructor
		SCLParser(std::string a_SourceFile);

		//Parsing functions
		std::shared_ptr<SCLProgram> ParseProgram();
		std::shared_ptr<SCLDeclarations> ParseDeclarations();
		std::map<std::string, std::shared_ptr<SCLFunctionImplementation>> ParseImplementations();
		std::pair<std::string, std::shared_ptr<SCLFunctionImplementation>> ParseFunction();
		std::vector<std::shared_ptr<SCLDefineStatement>> ParseDefinitions();

		std::vector<std::shared_ptr<SCLAction>> ParseActions(const int a_BlockType);
		std::shared_ptr<SCLAssignmentStatement> ParseAssignment();
		std::shared_ptr<SCLDisplayStatement> ParseDisplay();
		std::shared_ptr<SCLIncrementStatement> ParseIncrement();
		std::shared_ptr<SCLDecrementStatement> ParseDecrement();
		std::shared_ptr<SCLIfStatement> ParseIf();
		std::shared_ptr<SCLWhileStatement> ParseWhile();

		std::shared_ptr<SCLDefineStatement> ParseDataDeclaration();

		std::shared_ptr<SCLConditional> ParseConditional();
		std::shared_ptr<SCLBooleanExpression> ParseBooleanExpression();

		std::shared_ptr<SCLExpression> ParseExpression(bool a_isCurrentLexemeBegin = false);
		std::shared_ptr<SCLTerm> ParseTerm();
		std::shared_ptr<SCLElement> ParseElement();

		SCLIdent ParseNameRef();
		std::vector<std::shared_ptr<SCLExpression>> ParseArgList();
		std::vector<std::shared_ptr<SCLDefineStatement>> ParseParameters();
		//End parsing functions

		//Other functions
		static std::shared_ptr<SCLDataType> GetIdentValue(std::string a_IdentName);
		static bool SCLParser::SetIdentValue(std::string a_IdentName, std::shared_ptr<SCLDataType> a_Value);
		bool AddIdent(std::string a_Ident, int a_Type);
		int IdentType(std::string a_Ident);
		std::string BlockIndentation();

		//List of identifiers and their run-time values. (scope name, (ident name, (ident type, ident value)))
		static std::vector<std::map<std::string, std::pair<int, std::shared_ptr<SCLDataType>>>> m_IdentValues;

	private:
		std::unique_ptr<SCLScanner> m_Reader;
		int m_TokenCode;
		//List of identifiers and their scopes. (scope name, (ident name, ident type))
		std::vector<std::pair<std::string, std::map<std::string, int>>> m_IdentScopes;

		//Disallow copying and assignment
		SCLParser(SCLParser& a_Source) {};
		SCLParser operator =(SCLParser& a_Source) {};
	};//End class SCLParser
}