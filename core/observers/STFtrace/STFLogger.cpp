
#include "STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/AtlasState.hpp"

namespace atlas
{
    // CONSTRUCTORS

    // stf_enable -> user can enable and disable STF Trace Generation
    // width -> register width (32 or 64)
    // pc -> initial program counter
    // filename -> name of the file the trace will be written to
    STFLogger::STFLogger(const uint32_t width, uint64_t pc, const std::string & filename) :
        stf_writer_(std::make_unique<stf::STFWriter>())
    {
        // set up version and stf generation type
        stf_writer_->open(filename);
        stf_writer_->addTraceInfo(
            stf::TraceInfoRecord(stf::STF_GEN::STF_GEN_IMPERAS, 1, 2, 0, "Trace from Imperas"));

        if (width == 32)
        {
            stf_writer_->setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV32);
        }
        else
        {
            stf_writer_->setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV64);
            stf_writer_->setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_RV64);
        }

        stf_writer_->setISA(stf::ISA::RISCV);
        stf_writer_->setHeaderPC(pc);
        stf_writer_->finalizeHeader();
    }

    // METHODS

    // inst -> current instruction being executed (will add register records)
    void STFLogger::writeInstruction(const AtlasInst* inst)
    {
        if (inst->getOpcodeSize() == 2)
        {
            *stf_writer_ << stf::InstOpcode16Record(inst->getOpcode());
        }
        else if (inst->getOpcodeSize() == 4)
        {
            *stf_writer_ << stf::InstOpcode32Record(inst->getOpcode());
        }
    }

    // state -> current AtlasState
    void STFLogger::postExecute(AtlasState* state)
    {
        // write the instruction record
        writeInstruction(state->getCurrentInst().get());
    }

} // namespace atlas