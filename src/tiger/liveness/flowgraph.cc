#include "tiger/liveness/flowgraph.h"

namespace fg {

// generate a flow graph from instruction list
void FlowGraphFactory::AssemFlowGraph() {
  /* TODO: Put your lab6 code here */
  auto instr_list = instr_list_->GetList();
  FNodePtr pred = nullptr;
  std::vector<FNodePtr> jump_nodes;
  // Build nodes and edges for the instructions
  for (const auto& instr : instr_list) {
    auto new_node = flowgraph_->NewNode(instr);

    // If instruction is a label, map it
    if (auto label_instr = dynamic_cast<assem::LabelInstr*>(instr)) {
      label_map_->Enter(label_instr->label_, new_node);
    }

    // Handle edges between nodes
    if (pred) {
      if (auto oper_instr = dynamic_cast<assem::OperInstr*>(pred->NodeInfo())) {
        if (oper_instr->jumps_) {
          // If it’s a jump instruction
          if (oper_instr->assem_.find("jmp") == std::string::npos) {
            flowgraph_->AddEdge(pred, new_node); // Non-unconditional jump
          }
          jump_nodes.push_back(pred);
        } else {
          flowgraph_->AddEdge(pred, new_node); // Normal operation
        }
      } else {
        flowgraph_->AddEdge(pred, new_node);
      }
    }

    pred = new_node;
  }

  // Handle jump-related edges
  for (const auto& node : jump_nodes) {
    auto oper_instr = static_cast<assem::OperInstr*>(node->NodeInfo());
    for (const auto& label : *(oper_instr->jumps_->labels_)) {
      auto jump_target_node = label_map_->Look(label);
      assert(jump_target_node && "Jump target label not found");
      flowgraph_->AddEdge(node, jump_target_node);
    }
  }
}

} // namespace fg

namespace assem {

temp::TempList *LabelInstr::Def() const {
  /* TODO: Put your lab6 code here */
  return nullptr;
}

temp::TempList *MoveInstr::Def() const {
  /* TODO: Put your lab6 code here */
  return dst_;
}

temp::TempList *OperInstr::Def() const {
  /* TODO: Put your lab6 code here */
  return dst_;
}

temp::TempList *LabelInstr::Use() const {
  /* TODO: Put your lab6 code here */
  return nullptr;
}

temp::TempList *MoveInstr::Use() const {
  /* TODO: Put your lab6 code here */
  return src_;
}

temp::TempList *OperInstr::Use() const {
  /* TODO: Put your lab6 code here */
  return src_;
}
} // namespace assem
