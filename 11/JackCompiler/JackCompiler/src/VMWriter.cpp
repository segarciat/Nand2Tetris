#include "VMWriter.h"

VMWriter::VMWriter(std::string& fileName)
{
	m_pOutFile = new std::ofstream(fileName);
}

VMWriter::~VMWriter()
{
	Close();
	delete m_pOutFile;
}


void VMWriter::WritePush(Segment segment, int index)
{
	if(segment == Segment::CONST)
		*m_pOutFile << "push constant " << index << "\n";
	else if (segment == Segment::ARG)
		*m_pOutFile << "push argument " << index << "\n";
	else if (segment == Segment::LOCAL)
		*m_pOutFile << "push local " << index << "\n";
	else if (segment == Segment::STATIC)
		*m_pOutFile << "push static " << index << "\n";
	else if (segment == Segment::THIS)
		*m_pOutFile << "push this " << index << "\n";
	else if (segment == Segment::THAT)
		*m_pOutFile << "push that " << index << "\n";
	else if (segment == Segment::POINTER)
		*m_pOutFile << "push pointer " << index << "\n";
	else if (segment == Segment::TEMP)
		*m_pOutFile << "push temp " << index << "\n";
}

void VMWriter::WritePop(Segment segment, int index) 
{
	if (segment == Segment::CONST)
		*m_pOutFile << "pop constant " << index << "\n";
	else if (segment == Segment::ARG)
		*m_pOutFile << "pop argument " << index << "\n";
	else if (segment == Segment::LOCAL)
		*m_pOutFile << "pop local " << index << "\n";
	else if (segment == Segment::STATIC)
		*m_pOutFile << "pop static " << index << "\n";
	else if (segment == Segment::THIS)
		*m_pOutFile << "pop this " << index << "\n";
	else if (segment == Segment::THAT)
		*m_pOutFile << "pop that " << index << "\n";
	else if (segment == Segment::POINTER)
		*m_pOutFile << "pop pointer " << index << "\n";
	else if (segment == Segment::TEMP)
		*m_pOutFile << "pop temp " << index << "\n";
}

void VMWriter::WriteArithmetic(Command command) 
{
	if (command == Command::ADD)
		*m_pOutFile << "add\n";
	else if (command == Command::SUB)
		*m_pOutFile << "sub\n";
	else if (command == Command::NEG)
		*m_pOutFile << "neg\n";
	else if (command == Command::EQ)
		*m_pOutFile << "eq\n";
	else if (command == Command::GT)
		*m_pOutFile << "gt\n";
	else if (command == Command::LT)
		*m_pOutFile << "lt\n";
	else if (command == Command::AND)
		*m_pOutFile << "and\n";
	else if (command == Command::OR)
		*m_pOutFile << "or\n";
	else if (command == Command::NOT)
		*m_pOutFile << "not\n";
}

void VMWriter::WriteLabel(const std::string& label)
{
	*m_pOutFile << "label " << label << "\n";
}

void VMWriter::WriteGoto(const std::string& label)
{
	*m_pOutFile << "goto " << label << "\n";
}
void VMWriter::WriteIf(const std::string& label)
{
	*m_pOutFile << "if-goto " << label << "\n";
}
void VMWriter::WriteCall(const std::string& name, int nArgs)
{
	*m_pOutFile << "call " << name << " " << nArgs << "\n";
}
void VMWriter::WriteFunction(const std::string& name, int nLocals)
{
	*m_pOutFile << "function " << name << " " << nLocals << "\n";
}
void VMWriter::WriteReturn() 
{
	*m_pOutFile << "return\n";
}
void VMWriter::Close() 
{
	if(m_pOutFile->is_open())
		m_pOutFile->close();
}