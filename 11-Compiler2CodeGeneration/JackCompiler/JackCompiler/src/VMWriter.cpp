#include "VMWriter.h"

namespace jack {

VMWriter::VMWriter(std::ofstream& ofs)
	:m_Ofs(ofs) {}

VMWriter::~VMWriter()
{
	Close();
}

void VMWriter::WritePush(Segment sgmt, int index)
{
	m_Ofs << "push" << SegmentToStr(sgmt) << index << std::endl;
}

void VMWriter::WritePop(Segment sgmt, int index)
{
	m_Ofs << "pop" << SegmentToStr(sgmt) << index << std::endl;
}

void VMWriter::WriteArithmetic(Command cmd)
{
	switch (cmd)
	{
	case Command::ADD:
		m_Ofs << "add" << std::endl;
		break;
	case Command::AND:
		m_Ofs << "and" << std::endl;
		break;
	case Command::EQ:
		m_Ofs << "eq" << std::endl;
		break;
	case Command::GT:
		m_Ofs << "gt" << std::endl;
		break;
	case Command::LT:
		m_Ofs << "lt" << std::endl;
		break;
	case Command::NEG:
		m_Ofs << "neg" << std::endl;
		break;
	case Command::NOT:
		m_Ofs << "not" << std::endl;
		break;
	case Command::OR:
		m_Ofs << "or" << std::endl;
		break;
	case Command::SUB:
		m_Ofs << "sub" << std::endl;
		break;
	}
}

void VMWriter::WriteLabel(const std::string& label)
{
	m_Ofs << "label " << label << std::endl;
}

void VMWriter::WriteGoto(const std::string& label)
{
	m_Ofs << "goto " << label << std::endl;
}

void VMWriter::WriteIf(const std::string& label)
{
	m_Ofs << "if-goto " << label << std::endl;
}

void VMWriter::WriteCall(const std::string& name, int nArgs)
{
	m_Ofs << "call " << name << " " << nArgs << std::endl;
}

void VMWriter::WriteFunction(const std::string& name, int nLocals)
{
	m_Ofs << "function " << name << " " << nLocals << std::endl;
}

void VMWriter::WriteReturn()
{
	m_Ofs << "return" << std::endl;
}

void VMWriter::Close()
{
	if (m_Ofs.is_open())
		m_Ofs.close();
}

const char* const VMWriter::SegmentToStr(Segment sgmt)
{
	switch (sgmt)
	{
	case Segment::CONST:
		return " constant ";
	case Segment::ARG:
		return " argument ";
	case Segment::LOCAL:
		return " local ";
	case Segment::POINTER:
		return " pointer ";
	case Segment::STATIC:
		return " static ";
	case Segment::TEMP:
		return " temp ";
	case Segment::THAT:
		return " that ";
	case Segment::THIS:
		return " this ";
	}
	// Takes care of compiler warning; all possible segments handled above.
	return "";
}

} // namespace jack