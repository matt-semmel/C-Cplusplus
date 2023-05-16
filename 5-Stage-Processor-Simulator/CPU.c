/*
 *
 * Group project by: Matt Semmel and Jason Zukiewicz 
 *
 */

/** Code by @author Wonsun Ahn
 * 
 * Implements the five stages of the processor pipeline.  The code you will be
 * modifying mainly.
 */

#include <inttypes.h>
#include <assert.h>
#include "CPU.h"
#include "trace.h"

unsigned int cycle_number = 0;
unsigned int inst_number = 0;

std::deque<dynamic_inst> IF, ID, WB;
dynamic_inst EX_ALU = {0}, MEM_ALU = {0};
dynamic_inst EX_lwsw = {0}, MEM_lwsw = {0};

bool is_ALU(dynamic_inst dinst) {
  instruction inst = dinst.inst;
  return inst.type != ti_NOP && inst.type != ti_LOAD && inst.type != ti_STORE;
}

bool is_lwsw(dynamic_inst dinst) {
  instruction inst = dinst.inst;
  return inst.type == ti_LOAD || inst.type == ti_STORE;
}

bool is_NOP(dynamic_inst dinst) {
  instruction inst = dinst.inst;
  return inst.type == ti_NOP;
}

bool is_older(dynamic_inst dinst1, dynamic_inst dinst2) {
  return is_NOP(dinst2) || (!is_NOP(dinst1) && dinst1.seq < dinst2.seq);
}

dynamic_inst get_NOP() {
  dynamic_inst dinst = {0};
  return dinst;
}

bool is_finished()
{
  /* Finished when pipeline is completely empty */
  if (IF.size() > 0 || ID.size() > 0) return 0;
  if (!is_NOP(EX_ALU) || !is_NOP(MEM_ALU) || !is_NOP(EX_lwsw) || !is_NOP(MEM_lwsw)) {
    return 0;
  }
  return 1;
}

bool is_taken(dynamic_inst dinst) {
  return dinst.inst.type == ti_BRANCH && dinst.inst.Addr != dinst.inst.PC + 4;
}

bool has_dest_operand(dynamic_inst dinst) {
return dinst.inst.type == ti_RTYPE || dinst.inst.type == ti_ITYPE ||
 dinst.inst.type == ti_LOAD;
}

bool has_first_operand(dynamic_inst dinst) {
  return dinst.inst.type == ti_RTYPE || dinst.inst.type == ti_ITYPE
  || dinst.inst.type == ti_LOAD || dinst.inst.type == ti_STORE
  || dinst.inst.type == ti_BRANCH || dinst.inst.type == ti_JRTYPE;
}

bool has_second_operand(dynamic_inst dinst) {
  return dinst.inst.type == ti_RTYPE || dinst.inst.type == ti_STORE || dinst.inst.type == ti_BRANCH;
}

bool data_hazard_check_wb()
{
  bool result = 0;
  if ( has_dest_operand(MEM_ALU) )
    if ( has_dest_operand(MEM_lwsw) && MEM_ALU.inst.dReg == MEM_lwsw.inst.dReg )
      return 1;
  return result;
}

int data_hazard_check_ex() {
  unsigned char sReg_a, sReg_b;
  int ret, i, j, k;
  
  ret = 0;

  for ( i = 0; i < (int) ID.size(); i++ )
  {
    if ( has_first_operand(ID[i]) )
	{
	  for ( j = 0; j < i; j++ )
	  {
	    if ( has_dest_operand(ID[j]) ) 
		{
		  sReg_a = ID[i].inst.sReg_a;
		  if ( sReg_a == ID[j].inst.dReg )
		    return ret;
		}
	  }
	  
	  if ( MEM_lwsw.inst.type == ti_LOAD && ID[i].inst.sReg_a == MEM_lwsw.inst.dReg )
	    return ret;
	}
	
	if ( has_second_operand (ID[i]) )
	{
	  for ( k = 0; k < i; k++ )
	  {
	    if ( has_dest_operand(ID[k]) )
		{
		  sReg_b = ID[i].inst.sReg_b;
		  if ( sReg_b == ID[k].inst.dReg )
		    return ret;
		}
	  }
	  
	  if ( MEM_lwsw.inst.type == ti_LOAD && ID[i].inst.sReg_b == MEM_lwsw.inst.dReg )
	    return ret;
	}
	
	ret++;
  }
  
  return ret;
}

int data_hazard_check_id()
{
  int ret, i, j, k, m; 
  unsigned char sReg_a;
  unsigned char sReg_b;

  ret = 0;

  for ( i = 0; i < (int)IF.size(); ++i )
  {
    if ( has_first_operand(IF[i]) )
    {
      sReg_a = IF[i].inst.sReg_a;
      for ( j = 0; j < i; ++j )
        if ( has_dest_operand(IF[j]) )
          if ( sReg_a == IF[j].inst.dReg )
            return ret;
      if (!config->enableForwarding)
      {
        for ( k = 0; k < (int)ID.size(); ++k )
          if ( has_dest_operand(ID[k]) )
            if ( sReg_a == ID[k].inst.dReg)
              return ret;
        if ( has_dest_operand(EX_ALU) && sReg_a == EX_ALU.inst.dReg )
          return ret;
        if ( has_dest_operand(EX_lwsw) && sReg_a == EX_lwsw.inst.dReg )
          return ret;
        if ( has_dest_operand(MEM_ALU) && sReg_a == MEM_ALU.inst.dReg )
          return ret;
        if ( has_dest_operand(MEM_lwsw) && sReg_a == MEM_lwsw.inst.dReg )
          return ret;
        for ( m = 0; m < (int)WB.size(); ++m )
          if ( has_dest_operand(WB[m]) )
            if (sReg_a == WB[m].inst.dReg )
              return ret;
      }
    }

    if ( has_second_operand(IF[i]) )
    {
      sReg_b = IF[i].inst.sReg_b;
	   for ( j = 0; j < i; ++j )
        if ( has_dest_operand(IF[j]) )
          if (sReg_b == IF[j].inst.dReg )
            return ret;
      if (!config->enableForwarding)
      {
        for ( k = 0; k < (int)ID.size(); ++k )
          if ( has_dest_operand(ID[k]) )
            if ( sReg_b == ID[k].inst.dReg)
              return ret;
        if ( has_dest_operand(EX_ALU) && sReg_b == EX_ALU.inst.dReg )
          return ret;
        if ( has_dest_operand(EX_lwsw) && sReg_b == EX_lwsw.inst.dReg )
          return ret;
        if ( has_dest_operand(MEM_ALU) && sReg_b == MEM_ALU.inst.dReg )
          return ret;
        if ( has_dest_operand(MEM_lwsw) && sReg_b == MEM_lwsw.inst.dReg )
          return ret;
        for ( m = 0; m < (int)WB.size(); ++m )
          if ( has_dest_operand(WB[m]) )
            if (sReg_b == WB[m].inst.dReg )
              return ret;
      }
    }
    ++ret;
  }
  return ret;
}

bool control_hazard_check() {
  int i, j;
  dynamic_inst dinst;
  
  for ( i = 0; i < (int) IF.size() ; i++ )
  {
    if ( is_taken(IF[i]) )
	  return 0;
  }
  
  if ( !config->branchTargetBuffer )
  {
    for ( j = 0; j < (int) ID.size(); j++ )
	{
	  if ( is_taken(ID[j]) )
	    return 0;
	}
  }
  
  if ( !config->branchPredictor )
  {
    dinst = EX_ALU;
	
	if ( is_taken(dinst) )
	  return 0;
	  
	dinst = EX_lwsw;
  }
  
  return 1;
}

int  writeback()
{
  static unsigned int cur_seq = 1;
  WB.clear();
  
  if (config->regFileWritePorts == 1  && 
      has_dest_operand(MEM_ALU) && 
      has_dest_operand(MEM_lwsw) 
      || 
      data_hazard_check_wb() )
	  {
          if ( is_older(MEM_ALU, MEM_lwsw) )
          {
            WB.push_back(MEM_ALU);
            MEM_ALU = get_NOP();
            WB.push_back(get_NOP());
          }
          else 
          { 
            WB.push_back(MEM_lwsw);
            MEM_lwsw = get_NOP();
            WB.push_back(get_NOP());
          }
	  }
  
  else {
    if (is_older(MEM_ALU, MEM_lwsw))
    {
      WB.push_back(MEM_ALU);
      MEM_ALU = get_NOP();
      WB.push_back(MEM_lwsw);
      MEM_lwsw = get_NOP();
    }
    else
    {
      WB.push_back(MEM_lwsw);
      MEM_lwsw = get_NOP();
      WB.push_back(MEM_ALU);
      MEM_ALU = get_NOP();
    }
  }	
	
  if (verbose) {/* print the instruction exiting the pipeline if verbose=1 */
    for (int i = 0; i < (int) WB.size(); i++) {
      printf("[%d: WB] %s\n", cycle_number, get_instruction_string(WB[i], true));
      if(!is_NOP(WB[i])) {
        if(config->pipelineWidth > 1 && config->regFileWritePorts == 1) {
          // There is a corner case where an instruction without a
          // destination register can get pulled in out of sequence but
          // other than that, it should be strictly in-order.
        } else {
          assert(WB[i].seq == cur_seq);
        }
        cur_seq++;
      }
    }
  }
  return WB.size();
}

int memory()
{
  int insts = 0;
  if (is_NOP(MEM_ALU)) {
    MEM_ALU = EX_ALU;
    EX_ALU = get_NOP();
    insts++;
  }
  if (is_NOP(MEM_lwsw)) {
    MEM_lwsw = EX_lwsw;
    EX_lwsw = get_NOP();
    insts++;
  }
  return insts;
}

int issue()
{
  int i;
  int ready_insts = ID.size();

  if ( config->enableForwarding )
    ready_insts = data_hazard_check_ex();

  for (i = 0; i < ready_insts; ++i)
  {
    if (is_ALU(ID.front()))
    {
      if (!is_NOP(EX_ALU))
        return i;
      EX_ALU = ID.front();
      ID.pop_front();
    }
    else if (is_lwsw(ID.front()))
    {
      if (!is_NOP(EX_lwsw))
        return i;
      EX_lwsw = ID.front();
      ID.pop_front();
    }
  }
  return i;
}

int decode()
{
  int ready_insts, i;
  ready_insts = data_hazard_check_id();
  
  if ( config->enableForwarding )
	ready_insts = IF.size();
  else
	ready_insts = data_hazard_check_id();

  for (i = 0; i < ready_insts && (int)ID.size() < config->pipelineWidth; ++i )
  {
    ID.push_back(IF.front());
    IF.pop_front();
  }
  return i;
}

int fetch()
{
  static unsigned int cur_seq = 1;
  int insts = 0;
  dynamic_inst dinst;
  instruction *tr_entry = NULL;

  //! Struct hazard with Mem
    if ( !config->splitCaches && MEM_lwsw.inst.type == ti_LOAD )
      return 0;

  /* copy trace entry(s) into IF stage */
  while ((int)IF.size() < config->pipelineWidth)
  {
    if (!control_hazard_check())
      break;
    size_t size = trace_get_item(&tr_entry); /* put the instruction into a buffer */
    if (size <= 0)
      break;

    // Add new instr to queue
    dinst.inst = *tr_entry;
    dinst.seq = cur_seq++;
    IF.push_back(dinst);
    insts++;

    if (verbose) /* print the instruction entering the pipeline if verbose=1 */
      printf("[%d: IF] %s\n", cycle_number, get_instruction_string(IF.back(), true));
  }
  inst_number += insts;
  return insts;
}